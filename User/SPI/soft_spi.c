#include "soft_spi.h"

void soft_spi_init(void)
{
    /* GPIO配置已由CubeMX生成，这里只设空闲电平 */
    SOFTSPI_SCLK(0);
    SOFTSPI_SDIN(0);
}

void soft_spi_write_byte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        SOFT_SPI_SCLK(0);
        SOFT_SPI_SDIN(data & 0x80);
        SOFT_SPI_SCLK(1);        /* 上升沿，SSD1306采样 */
        data <<= 1;
    }
    SOFT_SPI_SCLK(0);            /* 收尾恢复空闲 */
}
