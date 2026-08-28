# Baby Warmer Display Demo (ST7735 + RP2040)

Standalone demo of the new graphical UI for the baby warmer.
Runs on a custom RP2040 PCB with a 1.8" 128×160 ST7735 LCD.

## Wiring

| Display | RP2040 GPIO | Notes |
|--------:|:-----------:|:------|
| SCK     | GPIO 22     | spi0 SCK |
| SDA     | GPIO 23     | spi0 TX (MOSI) |
| CS      | GPIO 25     | software-controlled |
| A0 (DC) | GPIO 24     | software-controlled |
| RESET   | GPIO 21     | software-controlled |
| LED     | GPIO 20     | backlight, software-controlled |
| VCC     | 3.3 V       | |
| GND     | GND         | |

## Build

```bash
mkdir build
cd build
cmake -DPICO_SDK_PATH=$PICO_SDK_PATH ..
make -j4
```

Flash `warmer_display_demo.uf2` to the board.

Toolchain confirmed: `gcc-arm-none-eabi-10.3-2021.10` + pico-sdk.

## What the demo does

It boots into a few seconds of `DISPLAY_MODE_TEXT`, typing out a sample
status screen one `clear`/`seek`/`write` command at a time (paced so
it's watchable), holds it, then switches to `DISPLAY_MODE_GRAPHICAL`
and cycles through 10 warmer states, 4 seconds each, then loops -
sweeping the full heater-power range, both mode icons, all 3 faces,
and every fault/warning/alarm combination. Underneath, the demo
simulates a single push-button pulse on `apgar_start` at boot (birth)
and then leaves the clock running untouched, counting up regardless of
which screen or step is showing - so the real 1/5/10-minute checkpoints
can be watched without the 10-step loop (40 s), or the text-mode intro,
restarting it along the way:

1. Comfortable, heater idle (0 %)
2. Comfortable, gentle maintain heat (~35 %)
3. Cold, heater ramping (~70 %)
4. Cold, heater at full power (100 %)
5. Warming up, approaching target (~55 %)
6. Too hot, heater off — warning + alarm
7. Sensor fallen off, safe mode — sensor icon blinks
8. Heater failure — element forced red, no rays, alarm
9. Manual mode, comfortable
10. Manual mode, cold, heating hard

UART output (default `pico_default_uart_pins`, 115200 baud) prints the
current step and a periodic "loop rate" measurement so you can verify
the main loop is not being stalled by the display updates.

## Architecture

```
  main.c                       — demo state cycler
  └── display.c                — state machine, blinking, dirty tracking
        ├── icons.c            — 4 status icons: FAIL, ALARM, SENSOR, MODE
        │     └── icon_bitmaps.h    (baked RGB565, input/art/render_icons.py)
        ├── baby.c              — baby face, blits one of 3 OpenMoji emoji
        │     └── face_bitmaps.h    (baked RGB565, input/art/render_faces.py)
        ├── heat_indicator.c    — resistor-style heater element + heat rays,
        │                         color-coded green..red by heater percent
        ├── apgar_timer.c       — MM:SS elapsed-time readout below the face
        │     └── digit_bitmaps.h   (baked RGB565, input/art/render_timer.py)
        ├── text_console.c      — full-screen 21x20 fixed-font ASCII console
        │     └── font_bitmaps.h    (baked RGB565, input/art/render_font.py)
        ├── gfx.c               — geometric primitives (line, circle, triangle)
        └── st7735.c            — low-level SPI + ST7735 commands
```

Layout: a 4-icon column on the left, a dim vertical divider, then the
heater element + rays centered above the baby's face, and an APGAR
timer (`MM:SS`, counting up) centered below it in the remaining strip
down to the bottom edge. See the top comment in `display.h`/`display.c`
for exact coordinates. That's `DISPLAY_MODE_GRAPHICAL`; the other mode,
`DISPLAY_MODE_TEXT`, replaces all of it with a plain character grid -
see "Text mode" below.

## Text mode

`warmer_display_state_t.screen_mode` picks between the graphical UI
above (`DISPLAY_MODE_GRAPHICAL`, value `0`, the default) and a
full-screen ASCII text console (`DISPLAY_MODE_TEXT`, value `1`). `0` is
deliberately the graphical default: any caller that doesn't yet know
about `screen_mode` - including every existing state struct in this
demo and in picoos once re-imported - leaves it zero-initialized and
keeps getting today's graphical UI unchanged. This mirrors how
`apgar_start` was added: purely additive, no existing behavior moves.

The text console is a 21-column x 20-row grid of a baked 6x8 monospace
font (`font_bitmaps.h`, full printable ASCII 32-126, same cairo +
supersampling pipeline as the APGAR digits - see the size/legibility
tradeoff notes in `art/render_font.py`). It's driven by a small
cursor-addressed command set, not a framebuffer the caller fills in:

```c
state.text.cmd = TEXT_CMD_CLEAR;   // or SEEK or WRITE
state.text.seq++;                  // bump to fire `cmd` - REQUIRED every command
state.text.row = 3;                // SEEK's target row (ignored otherwise)
strncpy(state.text.line, "Heater: 42 %", TEXT_COLS);
```

- `TEXT_CMD_CLEAR` - wipes the screen, cursor back to row 0.
- `TEXT_CMD_SEEK` - moves the cursor to `.row` (0..19), no drawing.
- `TEXT_CMD_WRITE` - blits `.line` at the cursor's row (blank-padded
  to 21 columns, so a shorter line fully erases a longer previous one),
  then advances the cursor by one row - so printing several lines in a
  row only needs one `SEEK` up front, like a simple terminal.

Only one command executes per `display_update()` call, and it fires on
a `seq` bump - the same edge-detection `apgar_start` already uses - so
`seq` must increase (not just `cmd` change) every time you want a
command to run. `display.c` owns the cursor row entirely; the caller
never tracks position itself. Switching `screen_mode` at either edge
clears the screen and forces a full repaint of whichever mode is being
entered, and the APGAR clock keeps running underneath regardless of
which mode is on screen. See the top comment in `display.h` for the
full struct.

The APGAR timer shows elapsed time since warming started so staff can
see at a glance when the next scheduled APGAR check is due, without a
separate wall clock. The display owns the clock itself: the state
struct only carries a momentary trigger, `apgar_start` (a bool) - a
rising edge (e.g. a push button, wired upstream) (re)starts the clock
at 0. The caller never computes elapsed time itself; `display.c` tracks
`now_ms` internally and derives `MM:SS` (capped at `99:59`) from it.
The demo here simulates the button pulse in `main.c`. The digit font
('0'-'9' + ':') is baked the same way as the icons/face artwork:
rendered with cairo (DejaVu Sans Mono Bold, supersampled) into
fixed-size glyph cells, then blitted — no on-device text layout or font
rendering needed.

Real APGAR scores are taken at specific checkpoints (1 and 5 minutes
after birth always, 10 minutes too if the score is still low), not
read continuously - so the readout flashes for a 15-second window
around each checkpoint, in sync with the shared blink timer, as a
"score now" cue (`apgar_timer.c`'s `apgar_timer_in_checkpoint_window()`).
The flash is a solid block - black digits on a filled yellow cell (same
yellow as the warning icon), not just colored text - a plain color swap
on such small digits read as too subtle on the bench. Outside those
windows it's a plain white-on-black elapsed-time readout.
`APGAR_FAST_DEMO` (a `CMakeLists.txt`
option, **ON by default in this bench-prototype build**) shrinks the
checkpoints from minutes to seconds (10 s/25 s/40 s) so the flash can
be previewed in under a minute instead of waiting for the real
intervals - turn it OFF (`-DAPGAR_FAST_DEMO=OFF`) for anything
resembling a real deployment.

The baby figure and status icons used to be drawn procedurally (circles/
lines/triangles at runtime); both are now pre-rendered offline
(`input/art/render_*.py`, cairo + supersampling, or rsvg-convert for the
OpenMoji SVGs) and baked into flash as RGB565 bitmaps, then blitted with
`st7735_draw_bitmap()` — much cleaner edges than on-device primitives
can produce, still no RAM cost (bitmaps are `const`, read straight from
flash). The heater element/rays stay procedural since they change
continuously with heater percent — a bitmap can't represent that.

## Integration plan

Once the demo looks good on the PCB:

1. Drop `st7735.c/h`, `gfx.c/h`, `icons.c/h`, `icon_bitmaps.h`,
   `baby.c/h`, `face_bitmaps.h`, `heat_indicator.c/h`,
   `apgar_timer.c/h`, `digit_bitmaps.h`, `text_console.c/h`,
   `font_bitmaps.h`, `display.c/h`
   into the existing warmer project.
2. Add the same lines to its `CMakeLists.txt`.
3. In the warmer's main loop:
   - Call `display_init()` once at startup.
   - Build a `warmer_display_state_t` from existing state variables.
   - Call `display_update(&state, now_ms)` once per loop pass.

`display_update()` is idempotent and uses dirty-tracking, so calling
it every loop costs <1 ms in the typical case. The PID, button
scanning, and 7-segment refresh remain undisturbed.

## Performance notes

- The full screen has 128×160×2 = 40 960 bytes of pixel data. At
  32 MHz SPI, that's ~10 ms for a full-screen refresh. The demo
  redraws individual icons (~600 bytes each, ~0.2 ms) and the baby
  (~10 KB, ~3 ms). Both are non-blocking from a human's perspective.
- All `display_update()` paths complete in well under 5 ms, so
  there is no risk of disturbing the PID's millisecond-level timing
  loop.
- If you find any specific call too slow (e.g. baby redraw on every
  state change), it can be split into per-region updates later.

## Artwork attribution

The 3 baby-face emoji (`face_bitmaps.h`) are from
[OpenMoji](https://openmoji.org/) — 😊 U+1F60A, 🥶 U+1F976, 🥵 U+1F975 —
fetched from `openmoji_src/`, License: CC BY-SA 4.0. The 4 status icons
(`icon_bitmaps.h`) are original artwork rendered for this project. The
APGAR timer digits (`digit_bitmaps.h`) and the text-console font
(`font_bitmaps.h`) are rendered from the system font DejaVu Sans Mono
Bold (Bitstream Vera-derived license, permissive, redistributable) —
only the resulting baked bitmaps are embedded, no font file is shipped.

## Color or orientation wrong?

In `st7735.c`, the `MADCTL` value is currently `0xC0` (matches the
working code from the previous experiment). If colors are inverted,
try `0xC8`. If image is upside-down, try `0x00` or `0x08`.
