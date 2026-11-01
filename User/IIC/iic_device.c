#include "iic.h"
#include "usart.h"

/**
 * @brief   ��ȡ MPU6050 �� WHO_AM_I �Ĵ�������֤�����������ݣ���ֻ��Ӧ��
 * @return  ���� ID������ӦΪ 0x68������ʧ�ܷ��� 0xFF
 */
uint8_t mpu_read_who_am_i(void)
{
    uint8_t id = 0xFF;

    iic_start();
    iic_send_byte(0x68 << 1);       /* 0xD0: MPU6050 д��ַ */
    if (iic_wait_ack()) goto fail;

    iic_send_byte(0x75);            /* WHO_AM_I �Ĵ�����ַ */
    if (iic_wait_ack()) goto fail;

    iic_start();                    /* �ظ���ʼ���л�Ϊ�� */
    iic_send_byte((0x68 << 1) | 1); /* 0xD1: MPU6050 ����ַ */
    if (iic_wait_ack()) goto fail;

    id = iic_read_byte(0);          /* ��1�ֽڣ�NAK ���� */
    iic_stop();
    LOG("MPU6050 WHO_AM_I = 0x%02X %s", id, (id == 0x68) ? "(OK)" : "(Unexpected!)");
    return id;

fail:
    iic_stop();
    LOG("MPU6050 read failed (no ACK at 0xD0).");
    return 0xFF;
}
