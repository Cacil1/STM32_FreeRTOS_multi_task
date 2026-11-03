#ifndef __IIC_H__
#define __IIC_H__

#include "stm32f1xx_hal.h" /* HAL �����뺯�� */
#include "main.h"

#define IIC_SCL_GPIO_PORT GPIOB
#define IIC_SCL_GPIO_PIN GPIO_PIN_10
#define IIC_SCL_GPIO_CLK_ENABLE()     \
    do                                \
    {                                 \
        __HAL_RCC_GPIOD_CLK_ENABLE(); \
    } while (0)

#define IIC_SDA_GPIO_PORT GPIOB
#define IIC_SDA_GPIO_PIN GPIO_PIN_11
#define IIC_SDA_GPIO_CLK_ENABLE()     \
    do                                \
    {                                 \
        __HAL_RCC_GPIOG_CLK_ENABLE(); \
    } while (0)

/******************************************************************************************/

/* IO���� */
#define IIC_SCL(x)                                                                                                                                         \
    do                                                                                                                                                     \
    {                                                                                                                                                      \
        x ? HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN, GPIO_PIN_RESET); \
    } while (0) /* SCL */

#define IIC_SDA(x)                                                                                                                                         \
    do                                                                                                                                                     \
    {                                                                                                                                                      \
        x ? HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN, GPIO_PIN_RESET); \
    } while (0) /* SDA */

#define IIC_READ_SDA HAL_GPIO_ReadPin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN) /* ��ȡSDA */

#define IIC_Init iic_init
#define iic_start IIC_Start
#define iic_stop IIC_Stop
#define iic_wait_ack IIC_Wait_Ack
#define iic_send_byte IIC_Send_Byte
#define iic_read_byte IIC_Read_Byte

/* IIC���в������� */
void iic_init(void);                      /* ��ʼ��IIC��IO�� */
void iic_start(void);                     /* ����IIC��ʼ�ź� */
void iic_stop(void);                      /* ����IICֹͣ�ź� */
void iic_ack(void);                       /* IIC����ACK�ź� */
void iic_nack(void);                      /* IIC������ACK�ź� */
uint8_t iic_wait_ack(void);               /* IIC�ȴ�ACK�ź� */
void iic_send_byte(uint8_t txd);          /* IIC����һ���ֽ� */
uint8_t iic_read_byte(unsigned char ack); /* IIC��ȡһ���ֽ� */

/* �豸�������� */
uint8_t mpu_read_who_am_i(void);
void iic_bus_scan(void);
#endif
