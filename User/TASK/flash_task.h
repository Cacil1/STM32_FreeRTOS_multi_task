#ifndef __FLASH_TASK_H
#define __FLASH_TASK_H

#include <stdint.h>

typedef enum
{
    FLASH_CMD_WRITE_META = 0,
} FlashCmdType_t;

/* ---------------- flash 分区表 ---------------- */
#define FLASH_SECTOR_SIZE 4096U

#define META_SECTOR_A 0U   /* meta存储A扇区 */
#define META_SECTOR_B 1U   /* meta存储B扇区 */

/* ---------------- TLV 记录头 ---------------- */
#define TLV_MAGIC 0xAA55U
#define TLV_PAYLOAD_MAX 96U /* Meta数据最大值 */

typedef struct __attribute__((packed))
{
    uint16_t magic; /* TLV_MAGIC，身份识别 */
    uint8_t crc;    /* 覆盖 len(1)+seq(4) 共 5 字节 + payload */
    uint8_t len;    /* payload 长度，决定下一条记录的偏移 */
    uint32_t seq;   /* 逻辑时钟：单调递增，判新旧；GC 搬运不改写 */
} TlvHdr_t;

/* ---------------- 扫描结果 ---------------- */
typedef struct
{
    uint16_t end_off; /* 最后一条有效记录的末尾 = 下次追加位置 */
    uint32_t max_seq;
    uint8_t has_valid;
    uint8_t tail_clean;
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
MetaData_t *Flash_MetaGet(void);
void init_meta(void);

void Flash_Task_Entry(void *argument);

#endif
