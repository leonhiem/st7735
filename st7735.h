#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// GPIO Pin definitions
#define ST7735_SCK_PIN   22
#define ST7735_MOSI_PIN  23
#define ST7735_CS_PIN    25
#define ST7735_DC_PIN    24  // A0 pin (Data/Command)
#define ST7735_RST_PIN   21
#define ST7735_LED_PIN   20

// Display dimensions (portrait orientation: 128 wide x 160 tall)
#define ST7735_WIDTH     128
#define ST7735_HEIGHT    160

// ST7735 Commands
#define ST7735_NOP       0x00
#define ST7735_SWRESET   0x01
#define ST7735_RDDID     0x04
#define ST7735_RDDST     0x09
#define ST7735_SLPIN     0x10
#define ST7735_SLPOUT    0x11
#define ST7735_PTLON     0x12
#define ST7735_NORON     0x13
#define ST7735_INVOFF    0x20
#define ST7735_INVON     0x21
#define ST7735_DISPOFF   0x28
#define ST7735_DISPON    0x29
#define ST7735_CASET     0x2A
#define ST7735_RASET     0x2B
#define ST7735_RAMWR     0x2C
#define ST7735_RAMRD     0x2E
#define ST7735_PTLAR     0x30
#define ST7735_COLMOD    0x3A
#define ST7735_MADCTL    0x36
#define ST7735_FRMCTR1   0xB1
#define ST7735_FRMCTR2   0xB2
#define ST7735_FRMCTR3   0xB3
#define ST7735_INVCTR    0xB4
#define ST7735_DISSET5   0xB6
#define ST7735_PWCTR1    0xC0
#define ST7735_PWCTR2    0xC1
#define ST7735_PWCTR3    0xC2
#define ST7735_PWCTR4    0xC3
#define ST7735_PWCTR5    0xC4
#define ST7735_VMCTR1    0xC5
#define ST7735_RDID1     0xDA
#define ST7735_RDID2     0xDB
#define ST7735_RDID3     0xDC
#define ST7735_RDID4     0xDD
#define ST7735_PWCTR6    0xFC
#define ST7735_GMCTRP1   0xE0
#define ST7735_GMCTRN1   0xE1

// Color definitions (RGB565)
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF

// Function prototypes
void st7735_init(void);
void st7735_reset(void);
void st7735_write_cmd(uint8_t cmd);
void st7735_write_data(uint8_t data);
void st7735_write_data_buffer(const uint8_t *buffer, size_t len);
void st7735_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void st7735_fill_screen(uint16_t color);
void st7735_draw_pixel(uint8_t x, uint8_t y, uint16_t color);
void st7735_draw_char(uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg);
void st7735_draw_string(uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bg);
void st7735_draw_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t *bitmap);
void st7735_draw_bitmap_fullscreen(const uint16_t *bitmap);
void st7735_backlight(bool on);

#endif // ST7735_H
