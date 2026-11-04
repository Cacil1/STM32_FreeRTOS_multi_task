#ifndef __OLED_H
#define __OLED_H

#include "stdlib.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "soft_iic.h"          /* 引入软件IIC接口 */

#define OLED_I2C_ADDR   0x78   /* SSD1306 I2C地址（SA0接地时为0x78） */

#define OLED_CMD   0    /* 控制字节0x00：命令流 */
#define OLED_DATA  1    /* 控制字节0x40：数据流 */

#define OLED_LINE_CHARS_12PT 21

/* 以下所有函数声明保持原样，不用动 */
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_WR_Byte(uint8_t dat, uint8_t mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);
void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t r);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size1, uint8_t mode);
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size1, uint8_t mode);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size1, uint8_t mode);
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1, uint8_t mode);
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, uint8_t BMP[], uint8_t mode);
void OLED_Init(void);
void OLED_ShowFloatNum(uint8_t x, uint8_t y, float num, uint8_t z_len, uint8_t f_len, uint8_t size1, uint8_t mode);
void OLED_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t mode);
void OLED_ShowStringWrapped(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size1, uint8_t mode, uint8_t max_lines);

#endif
