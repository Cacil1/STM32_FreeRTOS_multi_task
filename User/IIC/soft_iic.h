#ifndef __SOFT_IIC_H__
#define __SOFT_IIC_H__

#include "stm32f1xx_hal.h" /* HAL �����뺯�� */
#include "main.h"

#define SOFT_IIC_SCL_GPIO_PORT GPIOB
#define SOFT_IIC_SCL_GPIO_PIN GPIO_PIN_10
#define SOFT_IIC_SCL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

#define SOFT_IIC_SDA_GPIO_PORT GPIOB
#define SOFT_IIC_SDA_GPIO_PIN GPIO_PIN_11
#define SOFT_IIC_SDA_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()


/******************************************************************************************/

/* IO���� */
#define SOFT_IIC_SCL(x)  HAL_GPIO_WritePin(SOFT_IIC_SCL_GPIO_PORT, SOFT_IIC_SCL_GPIO_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define SOFT_IIC_SDA(x)  HAL_GPIO_WritePin(SOFT_IIC_SDA_GPIO_PORT, SOFT_IIC_SDA_GPIO_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)

#define SOFT_IIC_READ_SDA HAL_GPIO_ReadPin(SOFT_IIC_SDA_GPIO_PORT, SOFT_IIC_SDA_GPIO_PIN) /* ��ȡSDA */

#define SOFT_IIC_Init      soft_iic_init
#define SOFT_IIC_Start     soft_iic_start
#define SOFT_IIC_Stop      soft_iic_stop
#define SOFT_IIC_Wait_Ack  soft_iic_wait_ack
#define SOFT_IIC_Send_Byte soft_iic_send_byte
#define SOFT_IIC_Read_Byte soft_iic_read_byte

/* IIC���в������� */
void soft_iic_init(void);                      /* ��ʼ��IIC��IO�� */
void soft_iic_start(void);                     /* ����IIC��ʼ�ź� */
void soft_iic_stop(void);                      /* ����IICֹͣ�ź� */
void soft_iic_ack(void);                       /* IIC����ACK�ź� */
void soft_iic_nack(void);                      /* IIC������ACK�ź� */
uint8_t soft_iic_wait_ack(void);               /* IIC�ȴ�ACK�ź� */
void soft_iic_send_byte(uint8_t txd);          /* IIC����һ���ֽ� */
uint8_t soft_iic_read_byte(unsigned char ack); /* IIC��ȡһ���ֽ� */

/* �豸�������� */
uint8_t mpu_read_who_am_i(void);
#endif
