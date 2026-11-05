#include "flash_task.h"
#include "watchdog_task.h"
#include "system_init.h"
#include "norflash.h"
#include <string.h>

/* ============================================================================
 * 分区设计
 *   Meta 区：第 0、1 扇区（A/B 双扇区版本链 + GC 轮转）
 *   Log  区：第 2~5 扇区（当前未使用，预留）
 *
 * Meta 写入策略（追加式版本链）：
 *   写：每次保存 = active 扇区末尾追加一条新版本，不擦不动旧的
 *   读：扫到扇区末尾，取 seq 最大的有效记录
 *   GC：active 写满 → 最新 1 条搬去替补 → 擦旧主力 → 换角色
 *   擦除频率：一条记录 8+5=13B，4096/13 ≈ 315 个版本，约 315 次保存才擦 1 次
 * ==========================================================================*/

extern osMutexId_t SPIMutexHandle;
extern osMutexId_t MetaDataMutexHandle;
extern osMessageQueueId_t FlashCmdQueueHandle;

/* ---------------- RAM 正本 & 写入游标 ---------------- */
static MetaData_t g_meta;
static uint32_t g_meta_active = META_SECTOR_A;
static uint16_t g_meta_write_off = 0U;
static uint32_t g_meta_seq = 0U;

/* ---- 对外访问器：RAM 正本只有一个，谁改它都必须走这扇门 ---- */
void Flash_MetaLock(void) { osMutexAcquire(MetaDataMutexHandle, osWaitForever); }
void Flash_MetaUnlock(void) { osMutexRelease(MetaDataMutexHandle); }
MetaData_t *Flash_MetaGet(void) { return &g_meta; }

/* ---------------- 工具 ---------------- */
static uint32_t sec_addr(uint32_t sector)
{
    return sector * FLASH_SECTOR_SIZE;
}

/* seed：前一段的累计 CRC，首次调用传 0xFF */
static uint8_t crc8_calc(uint8_t seed, const uint8_t *p, uint32_t n)
{
    uint8_t c = seed;
    while (n--)
    {
        c ^= *p++;
        for (int i = 0; i < 8; i++)
            c = (c & 0x80U) ? (uint8_t)((c << 1) ^ 0x31U) : (uint8_t)(c << 1);
    }
    return c;
}

/* CRC 覆盖 len(1)+seq(4)+payload(len 字节) —— 头部参与计算（跳过 magic/crc 自身） */
static uint8_t tlv_crc_of(const TlvHdr_t *h, const uint8_t *payload)
{
    uint8_t c = crc8_calc(0xFFU, (const uint8_t *)&h->len, 5U);  /* ← 补 0xFFU */
    return crc8_calc(c, payload, h->len);
}

static void meta_scan_sector(uint32_t sector, meta_scan_t *info, MetaData_t *out_payload)
{
    memset(info, 0, sizeof(*info));
    uint16_t off = 0U;
    static TlvHdr_t h;
    static uint8_t payload[TLV_PAYLOAD_MAX];

    while (off + sizeof(h) <= FLASH_SECTOR_SIZE)
    {
        norflash_read((uint8_t *)&h, sec_addr(sector) + off, sizeof(h));
        if (h.magic != TLV_MAGIC)
            break; /* 撞到擦除区/脏数据 */
        if (h.len > TLV_PAYLOAD_MAX)
            break; /* 头损坏，防御 */
        if (h.len < sizeof(MetaData_t))
            break; /* 长度不够，防御 */

        norflash_read(payload, sec_addr(sector) + off + sizeof(h), h.len);
        if (tlv_crc_of(&h, payload) != h.crc)
            break; /* 半截写，到此为止 */

        if (h.seq > info->max_seq)
        {
            info->max_seq = h.seq;
            info->has_valid = 1;
            if (out_payload)
                memcpy(out_payload, payload, sizeof(MetaData_t));
        }
        off += (uint16_t)(sizeof(h) + h.len);
    }
    info->end_off = off;
}

static void meta_gc_locked(void)
{
    static TlvHdr_t h;
    static uint8_t payload[TLV_PAYLOAD_MAX];
    uint16_t best_off = 0U;
    uint16_t best_len = 0U;
    uint32_t best_seq = 0U;
    uint8_t found = 0;

    uint16_t off = 0U;
    while (off + sizeof(h) <= FLASH_SECTOR_SIZE)
    {
        norflash_read((uint8_t *)&h, sec_addr(g_meta_active) + off, sizeof(h));
        if (h.magic != TLV_MAGIC)
            break;
        norflash_read(payload, sec_addr(g_meta_active) + off + sizeof(h), h.len);
        if (tlv_crc_of(&h, payload) == h.crc && h.seq > best_seq)
        {
            best_seq = h.seq;
            best_off = off;
            best_len = h.len; /* 记录长度现场存下 */
            found = 1;
        }
        off += (uint16_t)(sizeof(h) + h.len);
    }
    if (!found)
    {
        g_meta_write_off = 0U;
        return;
    }

    const uint16_t rec_len = (uint16_t)(sizeof(TlvHdr_t) + best_len);
    static uint8_t recbuf[TLV_PAYLOAD_MAX + sizeof(TlvHdr_t)];
    norflash_read(recbuf, sec_addr(g_meta_active) + best_off, rec_len);

    const uint32_t backup =
        (g_meta_active == META_SECTOR_A) ? META_SECTOR_B : META_SECTOR_A;
    norflash_erase_sector(backup);
    Watchdog_Checkin(WDG_TASK_FLASH);
    norflash_write(recbuf, sec_addr(backup), rec_len);
    norflash_erase_sector(g_meta_active);
    Watchdog_Checkin(WDG_TASK_FLASH);

    g_meta_active = backup;
    g_meta_write_off = rec_len;
}

static void handle_write_meta(void)
{
    static uint8_t buf[sizeof(TlvHdr_t) + sizeof(MetaData_t)];
    TlvHdr_t *h = (TlvHdr_t *)buf;
    MetaData_t *p = (MetaData_t *)(buf + sizeof(TlvHdr_t));

    *p = g_meta; /* 单结构体方案：同类型赋值，编译器逐成员拷贝 */

    h->magic = TLV_MAGIC;
    h->len = (uint8_t)sizeof(MetaData_t);
    h->seq = g_meta_seq + 1U;
    h->crc = tlv_crc_of(h, buf + sizeof(TlvHdr_t));

    const uint16_t need = (uint16_t)(sizeof(TlvHdr_t) + h->len);

    osMutexAcquire(SPIMutexHandle, osWaitForever);
    if (g_meta_write_off + need > FLASH_SECTOR_SIZE)
    {
        meta_gc_locked();
    }
    norflash_write(buf, sec_addr(g_meta_active) + g_meta_write_off, need);
    g_meta_write_off += need;
    g_meta_seq = h->seq;
    osMutexRelease(SPIMutexHandle);
}

static void init_meta(void)
{
    meta_scan_t a, b;
    static MetaData_t pa, pb;

    osMutexAcquire(SPIMutexHandle, osWaitForever);
    meta_scan_sector(META_SECTOR_A, &a, &pa);
    meta_scan_sector(META_SECTOR_B, &b, &pb);
    osMutexRelease(SPIMutexHandle);

    const uint8_t use_a = a.has_valid && (!b.has_valid || a.max_seq >= b.max_seq);
    const meta_scan_t *win = use_a ? &a : &b;
    const MetaData_t *pw = use_a ? &pa : &pb;

    if (win->has_valid)
    {
        osMutexAcquire(MetaDataMutexHandle, osWaitForever);
        *(&g_meta) = *pw; /* 结构体赋值，替代逐字段五连 */
        osMutexRelease(MetaDataMutexHandle);
        g_meta_seq = win->max_seq;
    }
    else
    {
        osMutexAcquire(MetaDataMutexHandle, osWaitForever);
        memset(&g_meta, 0, sizeof(g_meta));
        osMutexRelease(MetaDataMutexHandle);
        g_meta_seq = 0U;
    }

    g_meta_active = use_a ? META_SECTOR_A : META_SECTOR_B;
    g_meta_write_off = win->end_off;
}

/* ---------------- 任务入口 ---------------- */
void Flash_Task_Entry(void *argument)
{
    (void)argument;
    FlashCmdType_t type;
    System_Init_WaitReady();

    init_meta(); /* 上电加载：恢复 g_meta、seq、active 扇区和追加位置 */

    for (;;)
    {
        /* 1000ms 超时到了就喂狗 */
        osStatus_t status = osMessageQueueGet(FlashCmdQueueHandle, &type, NULL, 1000U);
        if (status != osOK)
        {
            Watchdog_Checkin(WDG_TASK_FLASH);
            continue;
        }

        switch (type)
        {
        case FLASH_CMD_WRITE_META:
            handle_write_meta();
            break;
        default:
            break;
        }
    }
}
