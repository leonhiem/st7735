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
- **emojis.h** - Three full-screen emoji generators, drawn procedurally (happy, cold, hot)
- **emoji_bitmaps_real.h** - Same three emojis as full-screen RGB565 bitmaps (OpenMoji artwork)
- **emoji_display.h** - Picks between the two above via `EMOJI_USE_BITMAPS` (see below)
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

`main.c` calls `display_emoji_happy()` / `display_emoji_cold()` / `display_emoji_hot()` from
`emoji_display.h`, which forwards to one of two implementations depending on
`EMOJI_USE_BITMAPS` (see "Procedural drawing vs. bitmaps" below):

```c
display_emoji_happy();  // Yellow happy face
display_emoji_cold();   // Blue freezing face
display_emoji_hot();    // Red sweating face
```

You can still call the underlying implementations directly if you want both at once:

```c
draw_emoji_happy();                       // procedural, from emojis.h
st7735_draw_bitmap_fullscreen(emoji_hot); // bitmap, from emoji_bitmaps_real.h
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

## Procedural Drawing vs. Bitmaps

There are two ways to render the three emojis, both wired up through `emoji_display.h`:

| Mode | Source | Look | Flash | RAM |
|---|---|---|---|---|
| Procedural (default) | `emojis.h` | hand-drawn circles/primitives | ~few KB of code | negligible |
| Bitmap | `emoji_bitmaps_real.h` | OpenMoji artwork, much more polished | ~123KB (`const`, stored in flash) | negligible — read directly from flash, never copied to RAM |

Both live in this repo side by side; only one is compiled into the running image at a time,
selected by the `EMOJI_USE_BITMAPS` preprocessor define.

**To switch to the bitmap emojis**, configure with the CMake option:

```bash
cd build
cmake -DEMOJI_USE_BITMAPS=ON ..
make -j4
```

Run `cmake -DEMOJI_USE_BITMAPS=OFF ..` (or just delete `build/` and reconfigure) to go back to
procedural drawing — it's the default so an unconfigured build already uses it.

Without CMake, you can get the same effect by defining it before `emoji_display.h` is included,
e.g. adding `#define EMOJI_USE_BITMAPS 1` at the top of `main.c`, or passing
`-DEMOJI_USE_BITMAPS=1` on the compiler command line.

The bitmaps in `emoji_bitmaps_real.h` are from [OpenMoji](https://openmoji.org/)
(😊 U+1F642, 🥶 U+1F976, 🥵 U+1F975), resized to 128×128 and centered on the 128×160 panel.
License: CC BY-SA 4.0 — free to use with attribution. `preview_happy.png`, `preview_cold.png`
and `preview_hot.png` show how they look before flashing.

## Memory Usage

The procedural emoji approach saves significant flash compared to storing bitmaps:
- Bitmap approach: 3 × 128 × 160 × 2 bytes ≈ 122,880 bytes of flash
- Procedural approach: ~few KB of code

Neither approach uses meaningful RAM — bitmaps are `const` and read straight from flash.

Enjoy your LCD display!
