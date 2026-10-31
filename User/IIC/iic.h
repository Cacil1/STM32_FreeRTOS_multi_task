#ifndef __IIC_H__
#define __IIC_H__

#include "stm32f1xx_hal.h"   /* HAL 类型与函数 */
#include "main.h"

#define IIC_SCL_GPIO_PORT               GPIOD
#define IIC_SCL_GPIO_PIN                GPIO_PIN_3
#define IIC_SCL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)

#define IIC_SDA_GPIO_PORT               GPIOG
#define IIC_SDA_GPIO_PIN                GPIO_PIN_13
#define IIC_SDA_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

/******************************************************************************************/

/* IO操作 */
#define IIC_SCL(x)        do{ x ? \
                              HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SCL */

#define IIC_SDA(x)        do{ x ? \
                              HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SDA */

#define IIC_READ_SDA     HAL_GPIO_ReadPin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN) /* 读取SDA */

#define IIC_Init iic_init
#define iic_start IIC_Start
#define iic_stop IIC_Stop
#define iic_wait_ack IIC_Wait_Ack
#define iic_send_byte IIC_Send_Byte
#define iic_read_byte IIC_Read_Byte

/* IIC所有操作函数 */
void iic_init(void);            /* 初始化IIC的IO口 */
void iic_start(void);           /* 发送IIC开始信号 */
void iic_stop(void);            /* 发送IIC停止信号 */
void iic_ack(void);             /* IIC发送ACK信号 */
void iic_nack(void);            /* IIC不发送ACK信号 */
uint8_t iic_wait_ack(void);     /* IIC等待ACK信号 */
void iic_send_byte(uint8_t txd);/* IIC发送一个字节 */
uint8_t iic_read_byte(unsigned char ack);/* IIC读取一个字节 */


/* 设备操作函数 */
uint8_t mpu_read_who_am_i(void);
void iic_bus_scan(void);
#endif
