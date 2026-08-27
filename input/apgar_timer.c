/*
 * apgar_timer.c - Blits the 5 fixed-size glyph cells (MM:SS) that make up
 * the APGAR timer. See apgar_timer.h and input/art/render_timer.py.
 */
#include "apgar_timer.h"
#include "st7735.h"
#include "digit_bitmaps.h"

static const uint16_t *DIGITS[10] = {
    digit_0, digit_1, digit_2, digit_3, digit_4,
    digit_5, digit_6, digit_7, digit_8, digit_9,
};

void apgar_timer_draw(int16_t cx, int16_t y, uint32_t total_seconds, uint16_t bg) {
    (void)bg; /* glyph bitmaps are full-bleed (black bg baked in) */

    if (total_seconds > 99 * 60 + 59) total_seconds = 99 * 60 + 59; /* cap at 99:59 */
    uint32_t mm = total_seconds / 60;
    uint32_t ss = total_seconds % 60;

    int16_t total_w = DIGIT_W * 4 + COLON_W;
    int16_t x = cx - total_w / 2;

    st7735_draw_bitmap(x, y, DIGIT_W, DIGIT_H, DIGITS[mm / 10]); x += DIGIT_W;
    st7735_draw_bitmap(x, y, DIGIT_W, DIGIT_H, DIGITS[mm % 10]); x += DIGIT_W;
    st7735_draw_bitmap(x, y, COLON_W, DIGIT_H, digit_colon);     x += COLON_W;
    st7735_draw_bitmap(x, y, DIGIT_W, DIGIT_H, DIGITS[ss / 10]); x += DIGIT_W;
    st7735_draw_bitmap(x, y, DIGIT_W, DIGIT_H, DIGITS[ss % 10]);
}
