/*
 * apgar_timer.h - APGAR elapsed-time readout (MM:SS), pre-baked digit font.
 *
 * The APGAR score is normally read at fixed minutes after birth; showing
 * a running MM:SS clock next to the baby's face lets staff see at a
 * glance when the next check is due, without a wall clock.
 */
#ifndef APGAR_TIMER_H
#define APGAR_TIMER_H

#include <stdint.h>

/* Draw "MM:SS" centered on cx, top edge at y. Caps display at 99:59.
 * `bg` is unused (glyph bitmaps are full-bleed) but kept for call-site
 * symmetry with the other render_*() helpers. */
void apgar_timer_draw(int16_t cx, int16_t y, uint32_t total_seconds, uint16_t bg);

#endif /* APGAR_TIMER_H */
