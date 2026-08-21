#ifndef EMOJIS_H
#define EMOJIS_H

#include <stdint.h>
#include <math.h>
#include "st7735.h"

// Helper function to calculate distance between two points
static inline float distance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

// Helper function to check if point is in circle
static inline bool in_circle(int x, int y, int cx, int cy, int radius) {
    return distance(x, y, cx, cy) <= radius;
}

// Helper function to check if point is in ring (between two circles)
static inline bool in_ring(int x, int y, int cx, int cy, int r_outer, int r_inner) {
    float dist = distance(x, y, cx, cy);
    return (dist <= r_outer) && (dist >= r_inner);
}

// RGB565 color helper
static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Draw happy yellow smiley emoji
void draw_emoji_happy(void) {
    const int cx = 64;  // Center x (128/2)
    const int cy = 80;  // Center y (160/2)
    const int radius = 55;
    
    const uint16_t bg_color = BLACK;
    const uint16_t face_color = rgb565(255, 220, 0);  // Yellow
    const uint16_t eye_color = rgb565(60, 40, 20);    // Dark brown
    const uint16_t mouth_color = rgb565(60, 40, 20);  // Dark brown
    
    st7735_set_addr_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    
    gpio_put(ST7735_DC_PIN, 1);
    gpio_put(ST7735_CS_PIN, 0);
    
    for (int y = 0; y < ST7735_HEIGHT; y++) {
        for (int x = 0; x < ST7735_WIDTH; x++) {
            uint16_t color = bg_color;
            
            // Face circle
            if (in_circle(x, y, cx, cy, radius)) {
                color = face_color;
                
                // Left eye
                if (in_circle(x, y, cx - 20, cy - 15, 6)) {
                    color = eye_color;
                }
                
                // Right eye
                if (in_circle(x, y, cx + 20, cy - 15, 6)) {
                    color = eye_color;
                }
                
                // Smile (arc)
                int mx = x - cx;
                int my = y - cy;
                float angle = atan2f(my, mx);
                float dist = distance(x, y, cx, cy);
                
                // Draw smile arc (bottom half circle)
                if (angle > 0.3 && angle < 2.84 && dist > 25 && dist < 30 && y > cy + 5) {
                    color = mouth_color;
                }
            }
            
            uint8_t buf[2] = {color >> 8, color & 0xFF};
            spi_write_blocking(spi0, buf, 2);
        }
    }
    
    gpio_put(ST7735_CS_PIN, 1);
}

// Draw cold freezing blue emoji
void draw_emoji_cold(void) {
    const int cx = 64;  // Center x (128/2)
    const int cy = 80;  // Center y (160/2)
    const int radius = 55;
    
    const uint16_t bg_color = BLACK;
    const uint16_t face_color = rgb565(100, 180, 255);  // Light blue
    const uint16_t eye_color = rgb565(40, 40, 100);     // Dark blue
    const uint16_t mouth_color = rgb565(40, 40, 100);
    const uint16_t teeth_color = WHITE;
    
    st7735_set_addr_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    
    gpio_put(ST7735_DC_PIN, 1);
    gpio_put(ST7735_CS_PIN, 0);
    
    for (int y = 0; y < ST7735_HEIGHT; y++) {
        for (int x = 0; x < ST7735_WIDTH; x++) {
            uint16_t color = bg_color;
            
            // Face circle
            if (in_circle(x, y, cx, cy, radius)) {
                color = face_color;
                
                // Round open eyes (circles)
                // Left eye
                if (in_circle(x, y, cx - 20, cy - 15, 8)) {
                    color = eye_color;
                }
                
                // Right eye
                if (in_circle(x, y, cx + 20, cy - 15, 8)) {
                    color = eye_color;
                }
                
                // Horizontal open mouth showing teeth
                int mouth_top = cy + 15;
                int mouth_bottom = cy + 28;
                int mouth_left = cx - 20;
                int mouth_right = cx + 20;
                
                // Mouth outline (dark)
                if (y >= mouth_top && y <= mouth_bottom && 
                    x >= mouth_left && x <= mouth_right) {
                    color = mouth_color;
                    
                    // Upper teeth (white rectangles)
                    if (y >= mouth_top + 2 && y <= mouth_top + 7) {
                        color = teeth_color;
                    }
                    
                    // Lower teeth (white rectangles)
                    if (y >= mouth_bottom - 7 && y <= mouth_bottom - 2) {
                        color = teeth_color;
                    }
                    
                    // Tooth gaps (dark lines between teeth)
                    if ((x == cx - 12 || x == cx - 4 || x == cx + 4 || x == cx + 12) &&
                        ((y >= mouth_top + 2 && y <= mouth_top + 7) ||
                         (y >= mouth_bottom - 7 && y <= mouth_bottom - 2))) {
                        color = mouth_color;
                    }
                }
                
                // Icicles hanging from bottom of mouth (larger triangular shapes)
                // Left icicle
                int ice1_x = cx - 12;
                int ice1_top = mouth_bottom;
                if (x >= ice1_x - 5 && x <= ice1_x + 5 && 
                    y >= ice1_top && y <= ice1_top + 12 &&
                    abs(x - ice1_x) * 2 <= (ice1_top + 12 - y)) {
                    color = rgb565(200, 230, 255);
                }
                
                // Middle icicle
                int ice2_x = cx;
                int ice2_top = mouth_bottom;
                if (x >= ice2_x - 6 && x <= ice2_x + 6 && 
                    y >= ice2_top && y <= ice2_top + 15 &&
                    abs(x - ice2_x) * 2 <= (ice2_top + 15 - y)) {
                    color = rgb565(200, 230, 255);
                }
                
                // Right icicle
                int ice3_x = cx + 12;
                int ice3_top = mouth_bottom;
                if (x >= ice3_x - 5 && x <= ice3_x + 5 && 
                    y >= ice3_top && y <= ice3_top + 12 &&
                    abs(x - ice3_x) * 2 <= (ice3_top + 12 - y)) {
                    color = rgb565(200, 230, 255);
                }
                
                // Long pointy icicles on sides of mouth (asymmetrical, further out)
                // Left side icicle (long and pointy) - positioned more to the left
                int ice_side_left_x = mouth_left - 8;
                int ice_side_left_top = mouth_top + 3;
                if (x >= ice_side_left_x - 7 && x <= ice_side_left_x + 2 && 
                    y >= ice_side_left_top && y <= ice_side_left_top + 28 &&
                    abs(x - ice_side_left_x) * 2 <= (ice_side_left_top + 28 - y)) {
                    color = rgb565(200, 230, 255);
                }
                
                // Right side icicle (long and pointy) - positioned more to the right, different length
                int ice_side_right_x = mouth_right + 10;
                int ice_side_right_top = mouth_top + 6;
                if (x >= ice_side_right_x - 2 && x <= ice_side_right_x + 7 && 
                    y >= ice_side_right_top && y <= ice_side_right_top + 24 &&
                    abs(x - ice_side_right_x) * 2 <= (ice_side_right_top + 24 - y)) {
                    color = rgb565(200, 230, 255);
                }
                
                // Forehead icicles near eyes (asymmetrical)
                // Left forehead icicle
                int ice_forehead_left_x = cx - 25;
                int ice_forehead_left_top = cy - 25;
                if (x >= ice_forehead_left_x - 6 && x <= ice_forehead_left_x + 2 && 
                    y >= ice_forehead_left_top && y <= ice_forehead_left_top + 18 &&
                    abs(x - ice_forehead_left_x) * 2 <= (ice_forehead_left_top + 18 - y)) {
                    color = rgb565(200, 230, 255);
                }
                
                // Right forehead icicle (different position and length)
                int ice_forehead_right_x = cx + 28;
                int ice_forehead_right_top = cy - 22;
                if (x >= ice_forehead_right_x - 2 && x <= ice_forehead_right_x + 6 && 
                    y >= ice_forehead_right_top && y <= ice_forehead_right_top + 22 &&
                    abs(x - ice_forehead_right_x) * 2 <= (ice_forehead_right_top + 22 - y)) {
                    color = rgb565(200, 230, 255);
                }
            }
            
            uint8_t buf[2] = {color >> 8, color & 0xFF};
            spi_write_blocking(spi0, buf, 2);
        }
    }
    
    gpio_put(ST7735_CS_PIN, 1);
}

// Draw hot red sweating emoji
void draw_emoji_hot(void) {
    const int cx = 64;  // Center x (128/2)
    const int cy = 80;  // Center y (160/2)
    const int radius = 55;
    
    const uint16_t bg_color = BLACK;
    const uint16_t face_color = rgb565(255, 100, 80);   // Red
    const uint16_t eye_color = rgb565(100, 40, 20);     // Dark red
    const uint16_t mouth_color = rgb565(100, 40, 20);
    const uint16_t sweat_color = rgb565(150, 200, 255); // Light blue
    const uint16_t tongue_color = rgb565(255, 120, 120); // Pink
    
    st7735_set_addr_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    
    gpio_put(ST7735_DC_PIN, 1);
    gpio_put(ST7735_CS_PIN, 0);
    
    for (int y = 0; y < ST7735_HEIGHT; y++) {
        for (int x = 0; x < ST7735_WIDTH; x++) {
            uint16_t color = bg_color;
            
            // Face circle
            if (in_circle(x, y, cx, cy, radius)) {
                color = face_color;
                
                // Wide open eyes (circles)
                // Left eye
                if (in_circle(x, y, cx - 20, cy - 15, 8)) {
                    color = eye_color;
                }
                
                // Right eye
                if (in_circle(x, y, cx + 20, cy - 15, 8)) {
                    color = eye_color;
                }
                
                // Horizontal open mouth
                int mouth_top = cy + 15;
                int mouth_bottom = cy + 30;
                int mouth_left = cx - 22;
                int mouth_right = cx + 22;
                
                if (y >= mouth_top && y <= mouth_bottom && 
                    x >= mouth_left && x <= mouth_right) {
                    color = mouth_color;
                }
                
                // Super long tongue sticking out (rounded organic shape)
                int tongue_cx = cx;
                int tongue_cy = cy + 35;
                int tongue_top = cy + 22;
                int tongue_bottom = cy + 52;
                int tongue_width = 18;
                
                // Draw tongue with rounded shape
                if (y >= tongue_top && y <= tongue_bottom) {
                    // Calculate width at this y position (tapers slightly towards tip)
                    float progress = (float)(y - tongue_top) / (tongue_bottom - tongue_top);
                    int current_width = tongue_width - (int)(progress * 4);  // Slight taper
                    
                    if (abs(x - tongue_cx) <= current_width) {
                        // Round the sides
                        if (abs(x - tongue_cx) > current_width - 3) {
                            int edge_dist = abs(x - tongue_cx) - (current_width - 3);
                            if (edge_dist <= 3) {
                                color = tongue_color;
                            }
                        } else {
                            color = tongue_color;
                        }
                    }
                    
                    // Round the bottom tip
                    if (y > tongue_bottom - 8) {
                        if (in_circle(x, y, tongue_cx, tongue_bottom - 5, 10)) {
                            color = tongue_color;
                        }
                    }
                }
            }
            
            // Smaller sweat drop on cheek
            int sweat_x = cx + 40;
            int sweat_y = cy + 5;
            
            // Smaller sweat drop shape (teardrop)
            if (in_circle(x, y, sweat_x, sweat_y, 7)) {
                color = sweat_color;
            }
            // Tail of sweat drop
            if (x > sweat_x - 3 && x < sweat_x + 3 && 
                y > sweat_y - 12 && y < sweat_y) {
                int taper = abs(x - sweat_x);
                int max_taper = (sweat_y - y) / 4;
                if (taper <= max_taper) {
                    color = sweat_color;
                }
            }
            
            uint8_t buf[2] = {color >> 8, color & 0xFF};
            spi_write_blocking(spi0, buf, 2);
        }
    }
    
    gpio_put(ST7735_CS_PIN, 1);
}

#endif // EMOJIS_H
