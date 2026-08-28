# Handoff to picoos — changes since the last integration

Last integration into picoos was commit `3d8b0a0` in this repo (picoos-side
commit `d5c3254`, 2026-08-23) — see that project's own memory for the
original `/dev/tft/*` + `bin/tftwire` design. Everything below landed in
`input/` afterward and has **not** been brought into picoos yet.

As before, `display_init()` / `display_update(&state, now_ms)` keep the same
signatures — every change here is either a new field on `warmer_display_state_t`
or purely internal to `input/`. Re-importing is still: drop the files listed
below into the picoos project, add them to its `CMakeLists.txt`, done.

## New/changed fields on `warmer_display_state_t`

All of these default-initialize to values that reproduce today's picoos
behavior — a struct that doesn't set them at all (zero-initialized) keeps
working exactly as it does now.

- **`apgar_start`** (`bool`) — momentary trigger, meant to be wired to a real
  push button. A rising edge (re)starts an on-screen `MM:SS` elapsed-time
  clock at 0; the display owns the clock entirely (tracks `now_ms`, derives
  the readout and a checkpoint-flash cue itself) — picoos just needs to
  forward the raw button signal, no computation. **Action item**: wire this
  to a push button, per the plan already in `tft-picoos-integration-spec`
  (`/dev/tft/apgar` or similar).
- **`screen_mode`** (`display_mode_t`: `DISPLAY_MODE_GRAPHICAL` = 0 default,
  `DISPLAY_MODE_TEXT` = 1) — picks between today's graphical warmer UI and a
  full-screen 21x16 ASCII text console. **Not yet wired to anything in
  picoos** — no action needed unless/until a use for the text console comes
  up (e.g. a debug/status screen); leaving it unset is entirely safe.
- **`text`** (struct: `cmd`, `seq`, `row`, `line[22]`) — only meaningful when
  `screen_mode == DISPLAY_MODE_TEXT`. A small cursor-addressed command set
  (`TEXT_CMD_CLEAR` / `SEEK` / `WRITE`), fired by bumping `seq` — same
  edge-detection pattern as `apgar_start`. See `display.h`'s top comment and
  `README.md`'s "Text mode" section for the full API and an example.

## New files to copy over

In addition to the files from the last integration (`st7735.c/h`, `gfx.c/h`,
`icons.c/h`, `icon_bitmaps.h`, `baby.c/h`, `face_bitmaps.h`,
`heat_indicator.c/h`, `display.c/h`), this round adds:

- `apgar_timer.c/h`, `digit_bitmaps.h` — the APGAR `MM:SS` readout
- `text_console.c/h`, `font_bitmaps.h` — the ASCII text console
- `cue_icons.c/h`, `cue_icon_bitmaps.h` — thermometer/clock label icons
- (`art/*.py`, `art/fonts/6x10.bdf` are dev-time bake scripts/source art, not
  needed at runtime — same as `openmoji_src/` wasn't needed last time either)

## Gotchas

- **`APGAR_FAST_DEMO`** is a `CMakeLists.txt` option, **ON by default** in
  this repo's bench build (shrinks the APGAR checkpoints from minutes to
  seconds for quick preview). **Must be OFF** for anything resembling a real
  build, or the checkpoint flash fires every 10/25/40 seconds instead of
  1/5/10 minutes.

## What changed and why (commit-by-commit)

1. `e72271a` — divider/off-icon grey bumped for visibility, heat rays
   thickened, first pass at the APGAR timer (later reworked, see below).
2. `5af8598` — APGAR readout flashes at the real checkpoints (1/5/10 min
   after birth) instead of running as a plain silent stopwatch.
3. `89686b3` — reworked the APGAR field from a caller-computed
   `apgar_seconds` to the `apgar_start` bool-trigger design described above.
4. `0572837` — checkpoint flash changed from yellow text to a solid
   black-on-yellow block (plain color swap on 5x16px digits read as too
   subtle on the bench).
5. `0883487`, `6d05748` — added the text console (`screen_mode`/`text`
   fields). Font went through two more attempts before landing on a real
   bitmap font (X11 "misc-fixed" 6x10, the classic xterm default,
   transcribed pixel-for-pixel) — anti-aliased/thresholded vector fonts read
   as ragged/blurry at 6px wide, see `art/render_font.py`'s docstring for
   the full story.
6. `1824631`, `2a69555` — added the thermometer/clock cue icons and a 2nd
   (horizontal) divider, resolving a concern raised once the APGAR timer
   first landed: 3 emoji faces (happy/cold/hot, about temperature) next to
   an APGAR clock (unrelated, a checkpoint-based assessment) read as a
   conceptual mismatch with no visual distinction between the two. No new
   state-struct fields — purely a graphical-mode layout change.

This repo's own `README.md` has the full rationale/design story behind each
of these if more detail is wanted than this summary gives.
