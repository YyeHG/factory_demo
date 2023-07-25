#ifndef _SPI_INIT_H_
#define _SPI_INIT_H_

#include "driver/spi_master.h"

#define Flash_CLK_SPEED 6 * 1000 * 1000 // 6M的时钟
#define SPI_PIN_CLK        GPIO_NUM_12  // Intended for SPI2 CS
#define SPI_PIN_MOSI        GPIO_NUM_11  // Intended for SPI2 D (MOSI)
#define SPI_PIN_MISO        GPIO_NUM_13  // Intended for SPI2 Q (MISO)
#define SPI_PIN_CS        GPIO_NUM_10  // Intended for SPI2 CS
#define LCD_W 160
#define LCD_H 80


typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;



DRAM_ATTR static const lcd_init_cmd_t st_init_cmds[]={
    /* Memory Data Access Control, MX=MV=1, MY=ML=MH=0, RGB=0 */
    {0x36, {(1<<5)|(1<<6)}, 1},
    /* Interface Pixel Format, 16bits/pixel for RGB/MCU interface */
    {0x3A, {0x55}, 1},
    /* Porch Setting */
    {0xB2, {0x0c, 0x0c, 0x00, 0x33, 0x33}, 5},
    /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
    {0xB7, {0x45}, 1},
    /* VCOM Setting, VCOM=1.175V */
    {0xBB, {0x2B}, 1},
    /* LCM Control, XOR: BGR, MX, MH */
    {0xC0, {0x2C}, 1},
    /* VDV and VRH Command Enable, enable=1 */
    {0xC2, {0x01, 0xff}, 2},
    /* VRH Set, Vap=4.4+... */
    {0xC3, {0x11}, 1},
    /* VDV Set, VDV=0 */
    {0xC4, {0x20}, 1},
    /* Frame Rate Control, 60Hz, inversion=0 */
    {0xC6, {0x0f}, 1},
    /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
    {0xD0, {0xA4, 0xA1}, 2},
    /* Positive Voltage Gamma Control */
    {0xE0, {0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43, 0x47, 0x09, 0x15, 0x12, 0x16, 0x19}, 14},
    /* Negative Voltage Gamma Control */
    {0xE1, {0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44, 0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19}, 14},
    /* Sleep Out */
    {0x11, {0}, 0x80},
    /* Display On */
    {0x29, {0}, 0x80},
    {0, {0}, 0xff}
};

extern spi_device_handle_t handle;
esp_err_t spi_writedata(uint8_t *data, uint8_t len);
esp_err_t spi_writedcmd(uint8_t cmd, bool keep_cs_active);
esp_err_t spi_read(uint8_t *data);
esp_err_t spi_init();
void st7735_show();
void spi_ShowChar(int x1,int y1, char ch);
void LCD_Clear(uint16_t Color);
#endif
