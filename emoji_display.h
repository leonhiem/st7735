#ifndef EMOJI_DISPLAY_H
#define EMOJI_DISPLAY_H

#include "st7735.h"
#include "emoji_bitmaps.h"

// Display emoji from Flash memory (no RAM copy needed!)
// The bitmaps are stored as const in Flash, so we read directly from Flash
void display_emoji_happy(void) {
    st7735_draw_bitmap_fullscreen(emoji_happy);
}

void display_emoji_cold(void) {
    st7735_draw_bitmap_fullscreen(emoji_cold);
}

void display_emoji_hot(void) {
    st7735_draw_bitmap_fullscreen(emoji_hot);
}

#endif // EMOJI_DISPLAY_H
