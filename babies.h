#ifndef BABIES_H
#define BABIES_H

#include <stdint.h>
#include <math.h>
#include "st7735.h"

// Helper function to calculate distance between two points
static inline float baby_distance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

// Helper function to check if point is in circle
static inline bool baby_in_circle(int x, int y, int cx, int cy, int radius) {
    return baby_distance(x, y, cx, cy) <= radius;
}

// RGB565 color helper
static inline uint16_t baby_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Draw happy baby
void draw_baby_happy(void) {
    const int cx = 64;  // Center x
    const int cy = 80;  // Center y
    
    const uint16_t bg_color = BLACK;
    const uint16_t skin_color = baby_rgb565(255, 220, 185);  // Peachy skin
    const uint16_t hair_color = baby_rgb565(120, 80, 50);    // Brown hair
    const uint16_t eye_color = baby_rgb565(60, 40, 20);      // Dark eyes
    const uint16_t eye_white = WHITE;
    const uint16_t mouth_color = baby_rgb565(200, 100, 100); // Pink mouth
    const uint16_t cheek_color = baby_rgb565(255, 180, 180); // Rosy cheeks
    
    st7735_set_addr_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    
    gpio_put(ST7735_DC_PIN, 1);
    gpio_put(ST7735_CS_PIN, 0);
    
    for (int y = 0; y < ST7735_HEIGHT; y++) {
        for (int x = 0; x < ST7735_WIDTH; x++) {
            uint16_t color = bg_color;
            
            // Baby face (large round head)
            if (baby_in_circle(x, y, cx, cy, 50)) {
                color = skin_color;
                
                // Hair on top (cute tuft)
                if (y < cy - 30 && baby_in_circle(x, y, cx, cy - 35, 20)) {
                    color = hair_color;
                }
                
                // Left eye
                if (baby_in_circle(x, y, cx - 15, cy - 10, 8)) {
                    color = eye_white;
                    // Pupil
                    if (baby_in_circle(x, y, cx - 15, cy - 10, 4)) {
                        color = eye_color;
                    }
                    // Highlight
                    if (baby_in_circle(x, y, cx - 13, cy - 12, 2)) {
                        color = eye_white;
                    }
                }
                
                // Right eye
                if (baby_in_circle(x, y, cx + 15, cy - 10, 8)) {
                    color = eye_white;
                    // Pupil
                    if (baby_in_circle(x, y, cx + 15, cy - 10, 4)) {
                        color = eye_color;
                    }
                    // Highlight
                    if (baby_in_circle(x, y, cx + 17, cy - 12, 2)) {
                        color = eye_white;
                    }
                }
                
                // Happy smile (arc)
                int mx = x - cx;
                int my = y - cy;
                float angle = atan2f(my, mx);
                float dist = baby_distance(x, y, cx, cy);
                
                if (angle > 0.5 && angle < 2.64 && dist > 18 && dist < 23 && y > cy + 10) {
                    color = mouth_color;
                }
                
                // Rosy cheeks
                if (baby_in_circle(x, y, cx - 25, cy + 5, 8)) {
                    // Blend with skin
                    color = cheek_color;
                }
                if (baby_in_circle(x, y, cx + 25, cy + 5, 8)) {
                    color = cheek_color;
                }
            }
            
            uint8_t buf[2] = {color >> 8, color & 0xFF};
            spi_write_blocking(spi0, buf, 2);
        }
    }
    
    gpio_put(ST7735_CS_PIN, 1);
}

// Draw cold baby
void draw_baby_cold(void) {
    const int cx = 64;
    const int cy = 80;
    
    const uint16_t bg_color = BLACK;
    const uint16_t skin_color = baby_rgb565(200, 210, 230);  // Pale bluish skin
    const uint16_t blanket_color = baby_rgb565(100, 180, 255); // Blue blanket
    const uint16_t hair_color = baby_rgb565(120, 80, 50);
    const uint16_t eye_color = baby_rgb565(60, 40, 20);
    const uint16_t eye_white = WHITE;
    const uint16_t mouth_color = baby_rgb565(150, 160, 200);
    const uint16_t tear_color = baby_rgb565(150, 200, 255);
    
    st7735_set_addr_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    
    gpio_put(ST7735_DC_PIN, 1);
    gpio_put(ST7735_CS_PIN, 0);
    
    for (int y = 0; y < ST7735_HEIGHT; y++) {
        for (int x = 0; x < ST7735_WIDTH; x++) {
            uint16_t color = bg_color;
            
            // Blanket wrapped around baby
            if (baby_in_circle(x, y, cx, cy + 20, 45)) {
                color = blanket_color;
            }
            
            // Baby face (cold/pale)
            if (baby_in_circle(x, y, cx, cy - 5, 40)) {
                color = skin_color;
                
                // Hair on top
                if (y < cy - 30 && baby_in_circle(x, y, cx, cy - 35, 18)) {
                    color = hair_color;
                }
                
                // Left eye (wide open, worried)
                if (baby_in_circle(x, y, cx - 15, cy - 15, 9)) {
                    color = eye_white;
                    if (baby_in_circle(x, y, cx - 15, cy - 15, 5)) {
                        color = eye_color;
                    }
                    if (baby_in_circle(x, y, cx - 13, cy - 17, 2)) {
                        color = eye_white;
                    }
                }
                
                // Right eye
                if (baby_in_circle(x, y, cx + 15, cy - 15, 9)) {
                    color = eye_white;
                    if (baby_in_circle(x, y, cx + 15, cy - 15, 5)) {
                        color = eye_color;
                    }
                    if (baby_in_circle(x, y, cx + 17, cy - 17, 2)) {
                        color = eye_white;
                    }
                }
                
                // Small worried mouth (oval)
                if (baby_in_circle(x, y, cx, cy + 8, 8) && y > cy + 8) {
                    color = mouth_color;
                }
            }
            
            // Shivering lines (small marks around head)
            if (x == cx - 50 && y > cy - 10 && y < cy - 5) color = baby_rgb565(180, 200, 255);
            if (x == cx - 52 && y > cy - 8 && y < cy - 3) color = baby_rgb565(180, 200, 255);
            if (x == cx + 50 && y > cy + 5 && y < cy + 10) color = baby_rgb565(180, 200, 255);
            if (x == cx + 52 && y > cy + 7 && y < cy + 12) color = baby_rgb565(180, 200, 255);
            
            // Tear drop
            if (baby_in_circle(x, y, cx + 25, cy - 5, 5)) {
                color = tear_color;
            }
            if (x > cx + 23 && x < cx + 27 && y > cy - 12 && y < cy - 5) {
                color = tear_color;
            }
            
            uint8_t buf[2] = {color >> 8, color & 0xFF};
            spi_write_blocking(spi0, buf, 2);
        }
    }
    
    gpio_put(ST7735_CS_PIN, 1);
}

// Draw hot baby
void draw_baby_hot(void) {
    const int cx = 64;
    const int cy = 80;
    
    const uint16_t bg_color = BLACK;
    const uint16_t skin_color = baby_rgb565(255, 150, 130);  // Reddish hot skin
    const uint16_t hair_color = baby_rgb565(120, 80, 50);
    const uint16_t eye_color = baby_rgb565(60, 40, 20);
    const uint16_t eye_white = WHITE;
    const uint16_t mouth_color = baby_rgb565(180, 80, 80);
    const uint16_t sweat_color = baby_rgb565(150, 200, 255);
    
    st7735_set_addr_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    
    gpio_put(ST7735_DC_PIN, 1);
    gpio_put(ST7735_CS_PIN, 0);
    
    for (int y = 0; y < ST7735_HEIGHT; y++) {
        for (int x = 0; x < ST7735_WIDTH; x++) {
            uint16_t color = bg_color;
            
            // Baby face (red/hot)
            if (baby_in_circle(x, y, cx, cy, 50)) {
                color = skin_color;
                
                // Hair on top (messy from sweating)
                if (y < cy - 30 && baby_in_circle(x, y, cx - 5, cy - 35, 15)) {
                    color = hair_color;
                }
                if (y < cy - 32 && baby_in_circle(x, y, cx + 8, cy - 37, 12)) {
                    color = hair_color;
                }
                
                // Squinting eyes (hot/uncomfortable)
                // Left eye
                if (x > cx - 23 && x < cx - 10 && abs(y - (cy - 10)) < 2) {
                    color = eye_color;
                }
                
                // Right eye  
                if (x > cx + 10 && x < cx + 23 && abs(y - (cy - 10)) < 2) {
                    color = eye_color;
                }
                
                // Open crying mouth
                if (baby_in_circle(x, y, cx, cy + 15, 12) && y > cy + 15) {
                    color = mouth_color;
                }
            }
            
            // Sweat drops
            // Left drop
            if (baby_in_circle(x, y, cx - 35, cy - 10, 6)) {
                color = sweat_color;
            }
            if (x > cx - 37 && x < cx - 33 && y > cy - 18 && y < cy - 10) {
                color = sweat_color;
            }
            
            // Right drop
            if (baby_in_circle(x, y, cx + 35, cy, 6)) {
                color = sweat_color;
            }
            if (x > cx + 33 && x < cx + 37 && y > cy - 8 && y < cy) {
                color = sweat_color;
            }
            
            // Forehead drop
            if (baby_in_circle(x, y, cx + 10, cy - 30, 5)) {
                color = sweat_color;
            }
            if (x > cx + 8 && x < cx + 12 && y > cy - 36 && y < cy - 30) {
                color = sweat_color;
            }
            
            uint8_t buf[2] = {color >> 8, color & 0xFF};
            spi_write_blocking(spi0, buf, 2);
        }
    }
    
    gpio_put(ST7735_CS_PIN, 1);
}

#endif // BABIES_H
