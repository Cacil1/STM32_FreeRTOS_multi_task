#ifndef __SOFT_SPI_H
#define __SOFT_SPI_H

#include "stm32f1xx_hal.h"
#include "main.h"

/* 根据精英板P4座子实际接线修改这三个宏 */
#define SOFT_SPI_SCLK(x)  HAL_GPIO_WritePin(OLED_SCLK_GPIO_Port, OLED_SCLK_Pin, (x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define SOFT_SPI_SDIN(x)  HAL_GPIO_WritePin(OLED_SDIN_GPIO_Port, OLED_SDIN_Pin, (x)?GPIO_PIN_SET:GPIO_PIN_RESET)

void soft_spi_init(void);
void soft_spi_write_byte(uint8_t data);

#endif
