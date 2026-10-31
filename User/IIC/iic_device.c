#include "iic.h"
#include "usart.h"

/**
 * @brief   读取 MPU6050 的 WHO_AM_I 寄存器，验证能真正读数据（不只是应答）
 * @return  返回 ID（正常应为 0x68），读失败返回 0xFF
 */
uint8_t mpu_read_who_am_i(void)
{
    uint8_t id = 0xFF;

    iic_start();
    iic_send_byte(0x68 << 1);       /* 0xD0: MPU6050 写地址 */
    if (iic_wait_ack()) goto fail;

    iic_send_byte(0x75);            /* WHO_AM_I 寄存器地址 */
    if (iic_wait_ack()) goto fail;

    iic_start();                    /* 重复起始，切换为读 */
    iic_send_byte((0x68 << 1) | 1); /* 0xD1: MPU6050 读地址 */
    if (iic_wait_ack()) goto fail;

    id = iic_read_byte(0);          /* 读1字节，NAK 结束 */
    iic_stop();
    LOG("MPU6050 WHO_AM_I = 0x%02X %s\r\n", id, (id == 0x68) ? "(OK)" : "(Unexpected!)");
    return id;

fail:
    iic_stop();
    LOG("MPU6050 read failed (no ACK at 0xD0).\r\n");
    return 0xFF;
}
