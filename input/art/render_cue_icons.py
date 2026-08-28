#!/usr/bin/env python3
"""
render_cue_icons.py - Two tiny static label icons, addressing the
"emoji vs APGAR" conceptual-mismatch concern: a thermometer next to the
baby's face (this row is about temperature) and a clock next to the
APGAR timer (this row is a clock reading), so the two rows read as
clearly different kinds of information at a glance.

Not a reuse of icons.c's existing thermometer's *colors* (that one's
"on" state is red with a fault-alert badge, meant for an actual sensor
problem; its "off" state is COLOR_DIM_GREY, tuned to be quiet/inactive
and nearly invisible at this size) - both would send the wrong signal
here, this is a plain, permanent, neutral label, not a status
indicator. But the thermometer's *shape* (capsule + bulb) is reused
directly from icon_thermometer() in render_icons.py: a first attempt
at a from-scratch simplified thermometer read as "a small bag" at
16px - the capsule/bulb silhouette apparently needs the real icon's
proportions to stay recognizable that small, unlike the clock (simple
circle + hands held up fine simplified). So the thermometer here is
bigger (20px, using icons.c's actual path, just recolored and
rendered without the fault-alert badge) while the clock stays 16px.

Both sizes were picked from the real available space: the face
bitmap's visible circle only fills the center of its 80x80 box (see
baby.c), leaving ~24px of empty margin to the right before the screen
edge - room for a 20px icon with a couple px to spare either side. The
APGAR timer leaves ~25px free to its right for the same reason.

Bold filled shapes, not thin strokes, so unlike the text console font
anti-aliasing holds up fine at this size (see render_font.py for why
that's not true of 1px-wide glyph strokes).

Output:
  out/cue_icons_contact_sheet.png -- both icons, upscaled for viewing
  ../cue_icon_bitmaps.h           -- C header with both arrays
"""
import math
import os
import cairo
from PIL import Image

THERM_SIZE = 20
CLOCK_SIZE = 16
SS = 10
COLOR = (1.0, 1.0, 1.0)  # plain white - neutral label, not a status/alarm color


def new_ctx(size):
    surf = cairo.ImageSurface(cairo.FORMAT_ARGB32, size * SS, size * SS)
    ctx = cairo.Context(surf)
    ctx.scale(SS, SS)
    ctx.set_operator(cairo.OPERATOR_SOURCE)
    ctx.set_source_rgba(0, 0, 0, 0)
    ctx.paint()
    ctx.set_operator(cairo.OPERATOR_OVER)
    ctx.set_line_cap(cairo.LINE_CAP_ROUND)
    ctx.set_line_join(cairo.LINE_JOIN_ROUND)
    return surf, ctx


def finish(surf, size):
    hires = Image.frombuffer("RGBA", (size * SS, size * SS), bytes(surf.get_data()), "raw", "BGRA", 0, 1)
    small = hires.resize((size, size), Image.LANCZOS)
    bg = Image.new("RGB", (size, size), (0, 0, 0))
    bg.paste(small, (0, 0), small)
    return bg


def make_thermometer():
    """Same path as icon_thermometer() in render_icons.py (28x28 design
    size, cx=cy=14, "off"/quiet fill level), just recolored plain white
    with no fault-alert badge - see the module docstring for why the
    shape itself (not a simplified redraw) is reused here."""
    surf, ctx = new_ctx(28)
    cx, cy = 14, 14
    stem_w = 4.4
    top_y, bulb_y = cy - 10, cy + 7

    ctx.set_source_rgb(*COLOR)
    ctx.new_sub_path()
    ctx.arc(cx, top_y, stem_w / 2, math.pi, 2 * math.pi)
    ctx.rectangle(cx - stem_w / 2, top_y, stem_w, bulb_y - top_y)
    ctx.fill()
    ctx.arc(cx, bulb_y, 5.6, 0, 2 * math.pi)
    ctx.fill()

    inner_w = stem_w - 2.2
    ctx.set_source_rgb(0, 0, 0)
    ctx.new_sub_path()
    ctx.arc(cx, top_y + 1.2, inner_w / 2, math.pi, 2 * math.pi)
    ctx.rectangle(cx - inner_w / 2, top_y + 1.2, inner_w, bulb_y - top_y - 1.2)
    ctx.fill()
    ctx.arc(cx, bulb_y, 4.0, 0, 2 * math.pi)
    ctx.fill()

    ctx.set_source_rgb(*COLOR)
    fill_top = bulb_y - 3  # "off"/quiet fill level - this is a label, not a live reading
    ctx.new_sub_path()
    ctx.rectangle(cx - inner_w / 2 + 0.6, fill_top, inner_w - 1.2, bulb_y - fill_top)
    ctx.fill()
    ctx.arc(cx, bulb_y, 3.3, 0, 2 * math.pi)
    ctx.fill()
    # no badge - that's icon_thermometer()'s fault-alert overlay, not wanted here

    return finish(surf, 28).resize((THERM_SIZE, THERM_SIZE), Image.LANCZOS)


def make_clock():
    surf, ctx = new_ctx(CLOCK_SIZE)
    cx = cy = CLOCK_SIZE / 2
    r = CLOCK_SIZE / 2 - CLOCK_SIZE * 0.08

    ctx.set_source_rgb(*COLOR)
    ctx.set_line_width(CLOCK_SIZE * 0.11)
    ctx.arc(cx, cy, r, 0, 2 * math.pi)
    ctx.stroke()
    ctx.set_line_width(CLOCK_SIZE * 0.10)
    ctx.move_to(cx, cy)
    ctx.line_to(cx, cy - r * 0.55)
    ctx.stroke()
    ctx.move_to(cx, cy)
    ctx.line_to(cx + r * 0.42, cy + r * 0.12)
    ctx.stroke()
    return finish(surf, CLOCK_SIZE)


ICONS = {
    "cue_icon_thermometer": (make_thermometer, THERM_SIZE),
    "cue_icon_clock": (make_clock, CLOCK_SIZE),
}


def to_rgb565_words(img):
    img = img.convert("RGB")
    words = []
    for y in range(img.height):
        for x in range(img.width):
            r, g, b = img.getpixel((x, y))
            words.append(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
    return words


def main():
    outdir = os.path.join(os.path.dirname(__file__), "out")
    os.makedirs(outdir, exist_ok=True)

    header_arrays = {}
    sheet_cells = []
    for name, (fn, size) in ICONS.items():
        img = fn()
        img.resize((size * 10, size * 10), Image.NEAREST).save(f"{outdir}/{name}_actual.png")
        header_arrays[name] = to_rgb565_words(img)
        sheet_cells.append((name, img, size))
        print(f"{name}: rendered ({size}x{size}, {size*size*2} bytes)")

    header_path = os.path.join(os.path.dirname(__file__), "..", "cue_icon_bitmaps.h")
    with open(header_path, "w") as f:
        f.write("#ifndef CUE_ICON_BITMAPS_H\n#define CUE_ICON_BITMAPS_H\n\n#include <stdint.h>\n\n")
        f.write("/* Auto-generated by input/art/render_cue_icons.py - do not edit by hand. */\n\n")
        f.write(f"#define THERM_ICON_SIZE {THERM_SIZE}\n")
        f.write(f"#define CLOCK_ICON_SIZE {CLOCK_SIZE}\n\n")
        for name, words in header_arrays.items():
            f.write(f"const uint16_t {name}[{len(words)}] = {{\n")
            for i in range(0, len(words), 14):
                row = words[i:i + 14]
                f.write("    " + ",".join(f"0x{w:04X}" for w in row) + ",\n")
            f.write("};\n\n")
        f.write("#endif /* CUE_ICON_BITMAPS_H */\n")
    print(f"\nC header written: {os.path.abspath(header_path)}")

    scale = 10
    pad = 8
    x = pad
    max_h = max(size * scale for _, _, size in sheet_cells) + pad * 2
    sheet = Image.new("RGB", (sum(size * scale + pad for _, _, size in sheet_cells) + pad, max_h), (30, 30, 30))
    for name, img, size in sheet_cells:
        big = img.resize((size * scale, size * scale), Image.NEAREST)
        sheet.paste(big, (x, pad))
        x += size * scale + pad
    sheet.save(f"{outdir}/cue_icons_contact_sheet.png")
    print(f"contact sheet: {outdir}/cue_icons_contact_sheet.png")


if __name__ == "__main__":
    main()
