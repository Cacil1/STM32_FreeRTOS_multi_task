#ifndef __FLASH_TASK_H
#define __FLASH_TASK_H

#include <stdint.h>

typedef enum
{
    FLASH_CMD_WRITE_META = 0,
} FlashCmdType_t;

/* ---------------- flash 分区表（本文件私有，外部无需知晓） ---------------- */
#define FLASH_SECTOR_SIZE 4096U

#define META_SECTOR_A 0U   /* meta 版本链主力 */
#define META_SECTOR_B 1U   /* meta GC 替补 */
#define LOG_SECTOR_BASE 2U /* 预留：日志区起始，当前未使用 */
#define LOG_SECTOR_COUNT 4U

/* ---------------- TLV 记录头（8 字节，布局即协议） ---------------- */
#define TLV_MAGIC 0xAA55U
#define TLV_PAYLOAD_MAX 96U /* 上限防御，meta 实际只用 5B */

typedef struct __attribute__((packed))
{
    uint16_t magic; /* TLV_MAGIC，身份识别 */
    uint8_t crc;    /* 覆盖 len(1)+seq(4) 共 5 字节 + payload；
                       len/seq 必须参与 —— len 坏则扫描迷路，seq 坏则认贼作父 */
    uint8_t len;    /* payload 长度，决定下一条记录的偏移 */
    uint32_t seq;   /* 逻辑时钟：单调递增，判新旧；GC 搬运不改写 */
} TlvHdr_t;

/* ---------------- 扫描结果 ---------------- */
typedef struct
{
    uint16_t end_off; /* 最后一条有效记录的末尾 = 下次追加位置 */
    uint32_t max_seq;
    uint8_t has_valid;
} meta_scan_t;

typedef struct
{
    uint8_t led_red_state;
    uint8_t last_ui_page;
    uint8_t ws2812_brightness;
    uint8_t ws2812_color;
    uint8_t ws2812_mode_power;
} MetaData_t;

/* ---- Meta 访问接口） ---- */
void Flash_MetaLock(void);
void Flash_MetaUnlock(void);
MetaData_t *Flash_MetaGet(void); /* 拿 RAM 正本指针，改完发 WRITE_META */

void Flash_Task_Entry(void *argument);

#endif
