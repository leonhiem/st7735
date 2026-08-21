#include "st7735.h"
#include "font.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

#define SPI_PORT spi0

// Helper function to write command
void st7735_write_cmd(uint8_t cmd) {
    gpio_put(ST7735_DC_PIN, 0); // Command mode
    gpio_put(ST7735_CS_PIN, 0); // Select chip
    spi_write_blocking(SPI_PORT, &cmd, 1);
    gpio_put(ST7735_CS_PIN, 1); // Deselect chip
}

// Helper function to write data
void st7735_write_data(uint8_t data) {
    gpio_put(ST7735_DC_PIN, 1); // Data mode
    gpio_put(ST7735_CS_PIN, 0); // Select chip
    spi_write_blocking(SPI_PORT, &data, 1);
    gpio_put(ST7735_CS_PIN, 1); // Deselect chip
}

// Write data buffer
void st7735_write_data_buffer(const uint8_t *buffer, size_t len) {
    gpio_put(ST7735_DC_PIN, 1); // Data mode
    gpio_put(ST7735_CS_PIN, 0); // Select chip
    spi_write_blocking(SPI_PORT, buffer, len);
    gpio_put(ST7735_CS_PIN, 1); // Deselect chip
}

// Hardware reset
void st7735_reset(void) {
    gpio_put(ST7735_RST_PIN, 1);
    sleep_ms(5);
    gpio_put(ST7735_RST_PIN, 0);
    sleep_ms(20);
    gpio_put(ST7735_RST_PIN, 1);
    sleep_ms(150);
}

// Set address window for drawing
void st7735_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // Column address set
    st7735_write_cmd(ST7735_CASET);
    st7735_write_data(0x00);
    st7735_write_data(x0);
    st7735_write_data(0x00);
    st7735_write_data(x1);

    // Row address set
    st7735_write_cmd(ST7735_RASET);
    st7735_write_data(0x00);
    st7735_write_data(y0);
    st7735_write_data(0x00);
    st7735_write_data(y1);

    // Write to RAM
    st7735_write_cmd(ST7735_RAMWR);
}

// Initialize the display
void st7735_init(void) {
    // Initialize SPI
    spi_init(SPI_PORT, 32000000); // 32MHz SPI
    gpio_set_function(ST7735_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(ST7735_MOSI_PIN, GPIO_FUNC_SPI);
    
    // Initialize control pins
    gpio_init(ST7735_CS_PIN);
    gpio_set_dir(ST7735_CS_PIN, GPIO_OUT);
    gpio_put(ST7735_CS_PIN, 1);
    
    gpio_init(ST7735_DC_PIN);
    gpio_set_dir(ST7735_DC_PIN, GPIO_OUT);
    
    gpio_init(ST7735_RST_PIN);
    gpio_set_dir(ST7735_RST_PIN, GPIO_OUT);
    
    gpio_init(ST7735_LED_PIN);
    gpio_set_dir(ST7735_LED_PIN, GPIO_OUT);
    gpio_put(ST7735_LED_PIN, 1); // Backlight on
    
    // Hardware reset
    st7735_reset();
    
    // Software reset
    st7735_write_cmd(ST7735_SWRESET);
    sleep_ms(150);
    
    // Out of sleep mode
    st7735_write_cmd(ST7735_SLPOUT);
    sleep_ms(120);
    
    // Frame rate control - normal mode
    st7735_write_cmd(ST7735_FRMCTR1);
    st7735_write_data(0x01);
    st7735_write_data(0x2C);
    st7735_write_data(0x2D);
    
    // Frame rate control - idle mode
    st7735_write_cmd(ST7735_FRMCTR2);
    st7735_write_data(0x01);
    st7735_write_data(0x2C);
    st7735_write_data(0x2D);
    
    // Frame rate control - partial mode
    st7735_write_cmd(ST7735_FRMCTR3);
    st7735_write_data(0x01);
    st7735_write_data(0x2C);
    st7735_write_data(0x2D);
    st7735_write_data(0x01);
    st7735_write_data(0x2C);
    st7735_write_data(0x2D);
    
    // Display inversion control
    st7735_write_cmd(ST7735_INVCTR);
    st7735_write_data(0x07);
    
    // Power control
    st7735_write_cmd(ST7735_PWCTR1);
    st7735_write_data(0xA2);
    st7735_write_data(0x02);
    st7735_write_data(0x84);
    
    st7735_write_cmd(ST7735_PWCTR2);
    st7735_write_data(0xC5);
    
    st7735_write_cmd(ST7735_PWCTR3);
    st7735_write_data(0x0A);
    st7735_write_data(0x00);
    
    st7735_write_cmd(ST7735_PWCTR4);
    st7735_write_data(0x8A);
    st7735_write_data(0x2A);
    
    st7735_write_cmd(ST7735_PWCTR5);
    st7735_write_data(0x8A);
    st7735_write_data(0xEE);
    
    // VCOM control
    st7735_write_cmd(ST7735_VMCTR1);
    st7735_write_data(0x0E);
    
    // Display settings
    st7735_write_cmd(ST7735_INVOFF);
    
    // Memory data access control (portrait orientation)
    st7735_write_cmd(ST7735_MADCTL);
    st7735_write_data(0xC0); // MY=1, MX=1 for portrait mode
    
    // Color mode - 16-bit color (RGB565)
    st7735_write_cmd(ST7735_COLMOD);
    st7735_write_data(0x05);
    
    // Gamma correction
    st7735_write_cmd(ST7735_GMCTRP1);
    st7735_write_data(0x02);
    st7735_write_data(0x1c);
    st7735_write_data(0x07);
    st7735_write_data(0x12);
    st7735_write_data(0x37);
    st7735_write_data(0x32);
    st7735_write_data(0x29);
    st7735_write_data(0x2d);
    st7735_write_data(0x29);
    st7735_write_data(0x25);
    st7735_write_data(0x2b);
    st7735_write_data(0x39);
    st7735_write_data(0x00);
    st7735_write_data(0x01);
    st7735_write_data(0x03);
    st7735_write_data(0x10);
    
    st7735_write_cmd(ST7735_GMCTRN1);
    st7735_write_data(0x03);
    st7735_write_data(0x1d);
    st7735_write_data(0x07);
    st7735_write_data(0x06);
    st7735_write_data(0x2e);
    st7735_write_data(0x2c);
    st7735_write_data(0x29);
    st7735_write_data(0x2d);
    st7735_write_data(0x2e);
    st7735_write_data(0x2e);
    st7735_write_data(0x37);
    st7735_write_data(0x3f);
    st7735_write_data(0x00);
    st7735_write_data(0x00);
    st7735_write_data(0x02);
    st7735_write_data(0x10);
    
    // Normal display mode
    st7735_write_cmd(ST7735_NORON);
    sleep_ms(10);
    
    // Display on
    st7735_write_cmd(ST7735_DISPON);
    sleep_ms(100);
}

// Fill entire screen with a color
void st7735_fill_screen(uint16_t color) {
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    
    st7735_set_addr_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    
    gpio_put(ST7735_DC_PIN, 1); // Data mode
    gpio_put(ST7735_CS_PIN, 0); // Select chip
    
    for (uint16_t i = 0; i < ST7735_WIDTH * ST7735_HEIGHT; i++) {
        uint8_t buf[2] = {hi, lo};
        spi_write_blocking(SPI_PORT, buf, 2);
    }
    
    gpio_put(ST7735_CS_PIN, 1); // Deselect chip
}

// Draw a single pixel
void st7735_draw_pixel(uint8_t x, uint8_t y, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    
    st7735_set_addr_window(x, y, x, y);
    
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint8_t buf[2] = {hi, lo};
    
    st7735_write_data_buffer(buf, 2);
}

// Draw a character
void st7735_draw_char(uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg) {
    if (c < 32 || c > 126) c = '?';
    
    const uint8_t *glyph = font5x7[c - 32];
    
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t line = glyph[i];
        for (uint8_t j = 0; j < 7; j++) {
            if (line & (1 << j)) {
                st7735_draw_pixel(x + i, y + j, color);
            } else {
                st7735_draw_pixel(x + i, y + j, bg);
            }
        }
    }
}

// Draw a string
void st7735_draw_string(uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bg) {
    while (*str) {
        st7735_draw_char(x, y, *str++, color, bg);
        x += 6; // 5 pixels + 1 pixel spacing
        if (x > ST7735_WIDTH - 5) {
            x = 0;
            y += 8;
        }
    }
}

// Draw bitmap (general purpose)
void st7735_draw_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t *bitmap) {
    st7735_set_addr_window(x, y, x + w - 1, y + h - 1);
    
    gpio_put(ST7735_DC_PIN, 1); // Data mode
    gpio_put(ST7735_CS_PIN, 0); // Select chip
    
    for (uint16_t i = 0; i < w * h; i++) {
        uint16_t color = bitmap[i];
        uint8_t buf[2] = {color >> 8, color & 0xFF};
        spi_write_blocking(SPI_PORT, buf, 2);
    }
    
    gpio_put(ST7735_CS_PIN, 1); // Deselect chip
}

// Draw full-screen bitmap
void st7735_draw_bitmap_fullscreen(const uint16_t *bitmap) {
    st7735_draw_bitmap(0, 0, ST7735_WIDTH, ST7735_HEIGHT, bitmap);
}

// Control backlight
void st7735_backlight(bool on) {
    gpio_put(ST7735_LED_PIN, on ? 1 : 0);
}
