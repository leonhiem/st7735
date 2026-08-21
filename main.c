#include <stdio.h>
#include "pico/stdlib.h"
#include "st7735.h"
#include "emoji_display.h"

int main() {
    // Initialize stdio for debugging (optional)
    stdio_init_all();
    
    // Initialize the display
    st7735_init();
    
    // Clear screen to black
    st7735_fill_screen(BLACK);
    sleep_ms(500);
    
    // Display "Hello World" text
    st7735_fill_screen(BLACK);
    st7735_draw_string(30, 50, "Hello World!", WHITE, BLACK);
    st7735_draw_string(20, 70, "ST7735 Display", CYAN, BLACK);
    sleep_ms(3000);
    
    // Main loop - cycle through emojis using bitmaps from Flash
    while (true) {
        // Show happy emoji
        display_emoji_happy();
        sleep_ms(3000);
        
        // Show cold emoji
        display_emoji_cold();
        sleep_ms(3000);
        
        // Show hot emoji
        display_emoji_hot();
        sleep_ms(3000);
    }
    
    return 0;
}
