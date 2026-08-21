#ifndef EMOJI_DISPLAY_H
#define EMOJI_DISPLAY_H

#include "st7735.h"

// Choose how the emojis get onto the screen:
//   0 (default) = procedural drawing, from emojis.h
//                 - drawn with circles/primitives at runtime
//                 - tiny flash footprint, but hand-drawn look
//   1           = bitmap, from emoji_bitmaps_real.h (OpenMoji artwork)
//                 - full-screen 128x160 RGB565 images stored in flash
//                 - ~123KB flash, much nicer looking
//
// Override from the command line / CMakeLists.txt with:
//   target_compile_definitions(st7735 PUBLIC EMOJI_USE_BITMAPS=1)
// or by editing the default below.
#ifndef EMOJI_USE_BITMAPS
#define EMOJI_USE_BITMAPS 0
#endif

#if EMOJI_USE_BITMAPS

#include "emoji_bitmaps_real.h"

static inline void display_emoji_happy(void) { st7735_draw_bitmap_fullscreen(emoji_happy); }
static inline void display_emoji_cold(void)  { st7735_draw_bitmap_fullscreen(emoji_cold); }
static inline void display_emoji_hot(void)   { st7735_draw_bitmap_fullscreen(emoji_hot); }

#else

#include "emojis.h"

static inline void display_emoji_happy(void) { draw_emoji_happy(); }
static inline void display_emoji_cold(void)  { draw_emoji_cold(); }
static inline void display_emoji_hot(void)   { draw_emoji_hot(); }

#endif // EMOJI_USE_BITMAPS

#endif // EMOJI_DISPLAY_H
