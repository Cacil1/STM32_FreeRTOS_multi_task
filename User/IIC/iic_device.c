#include "soft_iic.h"
#include "usart.h"

/**
 * @brief   ��ȡ MPU6050 �� WHO_AM_I �Ĵ�������֤�����������ݣ���ֻ��Ӧ��
 * @return  ���� ID������ӦΪ 0x68������ʧ�ܷ��� 0xFF
 */
uint8_t mpu_read_who_am_i(void)
{
    uint8_t id = 0xFF;

    soft_iic_start();
    soft_iic_send_byte(0x68 << 1);       /* 0xD0: MPU6050 д��ַ */
    if (soft_iic_wait_ack()) goto fail;

    soft_iic_send_byte(0x75);            /* WHO_AM_I �Ĵ�����ַ */
    if (soft_iic_wait_ack()) goto fail;

    soft_iic_start();                    /* �ظ���ʼ���л�Ϊ�� */
    soft_iic_send_byte((0x68 << 1) | 1); /* 0xD1: MPU6050 ����ַ */
    if (soft_iic_wait_ack()) goto fail;

    id = soft_iic_read_byte(0);          /* ��1�ֽڣ�NAK ���� */
    soft_iic_stop();
    LOG("MPU6050 WHO_AM_I = 0x%02X %s", id, (id == 0x68) ? "(OK)" : "(Unexpected!)");
    return id;

fail:
    soft_iic_stop();
    LOG("MPU6050 read failed (no ACK at 0xD0).");
    return 0xFF;
}
