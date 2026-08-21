# ST7735 LCD Driver for RP2040

Simple driver for 1.8" ST7735 LCD (128x160 portrait) connected to Raspberry Pi Pico (RP2040).

## Hardware Connections

```
ST7735 LCD  ->  RP2040 GPIO
---------------------------------
SCK         ->  GPIO 22
SDA (MOSI)  ->  GPIO 23
CS          ->  GPIO 25
A0 (DC)     ->  GPIO 24
RESET       ->  GPIO 21
LED         ->  GPIO 20
VCC         ->  3.3V
GND         ->  GND
```

## Files Included

- **st7735.h/c** - Display driver with SPI communication
- **font.h** - 5x7 ASCII font for text rendering
- **emojis.h** - Three full-screen emoji generators (happy, cold, hot)
- **main.c** - Example program

## CMakeLists.txt Integration

Add these lines to your CMakeLists.txt:

```cmake
add_executable(your_project_name
    main.c
    st7735.c
)

target_link_libraries(your_project_name
    pico_stdlib
    hardware_spi
)

# Enable USB or UART for printf debugging (optional)
pico_enable_stdio_usb(your_project_name 1)
pico_enable_stdio_uart(your_project_name 0)

pico_add_extra_outputs(your_project_name)
```

Make sure you have the pico-sdk properly configured and the toolchain path set.

## API Usage

### Initialization
```c
st7735_init();  // Initialize display and SPI
```

### Drawing Functions
```c
// Fill screen with color
st7735_fill_screen(WHITE);

// Draw single pixel
st7735_draw_pixel(x, y, RED);

// Draw text
st7735_draw_string(10, 50, "Hello!", WHITE, BLACK);

// Draw character
st7735_draw_char(10, 50, 'A', GREEN, BLACK);

// Control backlight
st7735_backlight(true);  // On
st7735_backlight(false); // Off
```

### Emoji Functions
```c
draw_emoji_happy();  // Yellow happy face
draw_emoji_cold();   // Blue freezing face
draw_emoji_hot();    // Red sweating face
```

### Predefined Colors (RGB565)
- BLACK, WHITE
- RED, GREEN, BLUE
- CYAN, MAGENTA, YELLOW

### Custom Colors
```c
// Create custom RGB565 color
uint16_t custom = rgb565(r, g, b);  // r, g, b = 0-255
```

## Example Program Flow

The included main.c demonstrates:
1. Initialize display
2. Show "Hello World" text for 3 seconds
3. Cycle through three emojis (3 seconds each)

## Notes

- SPI speed is set to 32MHz for fast updates
- Display uses RGB565 color format (16-bit)
- Emojis are generated procedurally to save memory
- Font is 5x7 pixels (6x8 with spacing)
- The A0 pin is also called DC (Data/Command) on some displays

## Customization

To create your own graphics:
- Use `st7735_set_addr_window()` to define drawing area
- Use `st7735_draw_pixel()` for individual pixels
- Or create procedural drawing functions like the emojis

## Memory Usage

The procedural emoji approach saves significant RAM compared to storing bitmaps:
- Bitmap approach: 3 × 128 × 160 × 2 bytes = 122,880 bytes
- Procedural approach: ~few KB of code

Enjoy your LCD display!
