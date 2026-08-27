#!/usr/bin/env python3
"""
render_icons.py - Render the 6 status-row icon shapes as bold, anti-aliased
artwork (cairo, supersampled) instead of on-device pixel-stamped primitives.

Feedback driving this redesign:
  - MODE:    auto vs. manual must be unambiguous at a glance -> gear (auto)
             vs. an open hand (manual), both bold single-color silhouettes.
  - HEATER:  must look clearly "lit" when heating -> a flame glyph, vivid
             gradient when on, flat grey when off.
  - SENSOR:  should stay quiet normally, light up only on a problem ->
             plain grey thermometer normally, bright red thermometer with
             an alert badge when there's a fault.
  - WARNING: standard hazard triangle + "!", just executed more boldly.
  - ALARM:   must actually read as a bell (previous attempt was a blob) ->
             proper bell silhouette + ringing sound-wave arcs.

Each icon gets exactly 2 baked bitmaps: "on" (bright) and "off" (grey).
The existing "failed/problem" state stays a runtime overlay: the on-bitmap
plus a red diagonal slash drawn in code (icons.c's draw_slash()) - same
mechanism already used for heater/mode failures, no 3rd bitmap needed.

Output:
  out/icon_<name>_<on|off>_actual.png  -- exact pixel output, upscaled for viewing
  out/icons_contact_sheet.png          -- all icons/states side by side
  ../icon_bitmaps.h                    -- C header with all 12 arrays
"""
import math
import os
import cairo
from PIL import Image

W = H = 28
SS = 10
SW = SH = W * SS

GREY = (0.215, 0.215, 0.215)  # dim grey, a bit more visible than near-black - matches the divider line
BG = (0, 0, 0)


def new_ctx():
    surf = cairo.ImageSurface(cairo.FORMAT_ARGB32, SW, SH)
    ctx = cairo.Context(surf)
    ctx.scale(SS, SS)
    ctx.set_operator(cairo.OPERATOR_SOURCE)
    ctx.set_source_rgba(0, 0, 0, 0)
    ctx.paint()
    ctx.set_operator(cairo.OPERATOR_OVER)
    ctx.set_line_cap(cairo.LINE_CAP_ROUND)
    ctx.set_line_join(cairo.LINE_JOIN_ROUND)
    return surf, ctx


def finish(surf):
    hires = Image.frombuffer("RGBA", (SW, SH), bytes(surf.get_data()), "raw", "BGRA", 0, 1)
    small = hires.resize((W, H), Image.LANCZOS)
    bg = Image.new("RGB", (W, H), BG)
    bg.paste(small, (0, 0), small)
    return bg


# ---------------------------------------------------------------- gear/auto
def draw_gear(ctx, cx, cy, color, teeth_color=None):
    teeth_color = teeth_color or color
    n_teeth = 8
    r_outer, r_inner_tooth, r_body = 12, 9.5, 8.5
    ctx.set_source_rgb(*teeth_color)
    for i in range(n_teeth):
        a = 2 * math.pi * i / n_teeth
        w = 0.34  # half-angle width of each tooth
        ctx.new_sub_path()
        ctx.move_to(cx + r_inner_tooth * math.cos(a - w), cy + r_inner_tooth * math.sin(a - w))
        ctx.line_to(cx + r_outer * math.cos(a - w * 0.5), cy + r_outer * math.sin(a - w * 0.5))
        ctx.line_to(cx + r_outer * math.cos(a + w * 0.5), cy + r_outer * math.sin(a + w * 0.5))
        ctx.line_to(cx + r_inner_tooth * math.cos(a + w), cy + r_inner_tooth * math.sin(a + w))
        ctx.close_path()
        ctx.fill()
    ctx.set_source_rgb(*color)
    ctx.arc(cx, cy, r_body, 0, 2 * math.pi)
    ctx.fill()
    # center hole (cut to transparent)
    ctx.set_operator(cairo.OPERATOR_CLEAR)
    ctx.arc(cx, cy, 3.4, 0, 2 * math.pi)
    ctx.fill()
    ctx.set_operator(cairo.OPERATOR_OVER)


def icon_gear(bright):
    surf, ctx = new_ctx()
    color = (0.18, 0.85, 0.85) if bright else GREY
    draw_gear(ctx, 14, 14, color)
    return finish(surf)


# ---------------------------------------------------------------- hand/manual
def icon_hand(bright):
    surf, ctx = new_ctx()
    color = (1.0, 0.82, 0.10) if bright else GREY
    ctx.set_source_rgb(*color)
    cx, cy = 14, 15

    # palm
    ctx.new_sub_path()
    ctx.arc(cx, cy + 4, 6.5, 0, 2 * math.pi)
    ctx.fill()
    ctx.rectangle(cx - 6.5, cy - 3, 13, 8)
    ctx.fill()

    # four fingers - equal bold rounded nubs
    finger_w = 3.0
    finger_positions = [-6, -2, 2, 6]
    finger_lens = [7, 9, 8.5, 6.5]
    for fx, flen in zip(finger_positions, finger_lens):
        top = cy - 3 - flen
        rrect_pts = (cx + fx - finger_w / 2, top, finger_w, flen + finger_w / 2)
        ctx.new_sub_path()
        ctx.arc(cx + fx, top, finger_w / 2, math.pi, 2 * math.pi)
        ctx.rectangle(*rrect_pts)
        ctx.fill()

    # thumb, angled off to the side
    ctx.save()
    ctx.translate(cx - 9, cy + 2)
    ctx.rotate(math.radians(-35))
    ctx.rectangle(-1.6, -6, 3.2, 7)
    ctx.arc(0, -6, 1.6, math.pi, 2 * math.pi)
    ctx.fill()
    ctx.restore()

    return finish(surf)


# ---------------------------------------------------------------- flame/heater
def icon_flame(bright):
    surf, ctx = new_ctx()
    cx, cy = 14, 16

    def flame_path(ctx, scale):
        ctx.new_sub_path()
        ctx.move_to(cx, cy - 12 * scale)
        ctx.curve_to(cx + 7 * scale, cy - 6 * scale, cx + 6 * scale, cy + 2 * scale, cx + 2 * scale, cy + 6 * scale)
        ctx.curve_to(cx + 4 * scale, cy + 2 * scale, cx + 1 * scale, cy, cx, cy + 3 * scale)
        ctx.curve_to(cx - 1 * scale, cy, cx - 4 * scale, cy + 2 * scale, cx - 2 * scale, cy + 6 * scale)
        ctx.curve_to(cx - 6 * scale, cy + 2 * scale, cx - 7 * scale, cy - 6 * scale, cx, cy - 12 * scale)
        ctx.close_path()

    if bright:
        grad = cairo.LinearGradient(cx, cy - 12, cx, cy + 6)
        grad.add_color_stop_rgb(0, 1.0, 0.75, 0.15)
        grad.add_color_stop_rgb(1, 0.95, 0.25, 0.05)
        ctx.set_source(grad)
    else:
        ctx.set_source_rgb(*GREY)
    flame_path(ctx, 1.0)
    ctx.fill()

    if bright:
        ctx.set_source_rgb(1.0, 0.95, 0.55)
        flame_path(ctx, 0.5)
        ctx.fill()

    # base bar
    ctx.set_source_rgb(*(0.85, 0.85, 0.85) if not bright else (0.95, 0.55, 0.15))
    ctx.set_source_rgb(*(GREY if not bright else (0.95, 0.55, 0.15)))
    ctx.rectangle(cx - 8, cy + 8, 16, 2.2)
    ctx.fill()

    return finish(surf)


# ---------------------------------------------------------------- thermometer/sensor
def icon_thermometer(bright):
    surf, ctx = new_ctx()
    cx, cy = 14, 14
    outline = (0.85, 0.15, 0.15) if bright else GREY
    mercury = (0.95, 0.20, 0.20) if bright else GREY

    stem_w = 4.4
    top_y, bulb_y = cy - 10, cy + 7
    # stem outline (capsule)
    ctx.set_source_rgb(*outline)
    ctx.new_sub_path()
    ctx.arc(cx, top_y, stem_w / 2, math.pi, 2 * math.pi)
    ctx.rectangle(cx - stem_w / 2, top_y, stem_w, bulb_y - top_y)
    ctx.fill()
    ctx.arc(cx, bulb_y, 5.6, 0, 2 * math.pi)
    ctx.fill()

    # inner mercury (thinner, black gap between outline and fill)
    inner_w = stem_w - 2.2
    ctx.set_source_rgb(0, 0, 0)
    ctx.new_sub_path()
    ctx.arc(cx, top_y + 1.2, inner_w / 2, math.pi, 2 * math.pi)
    ctx.rectangle(cx - inner_w / 2, top_y + 1.2, inner_w, bulb_y - top_y - 1.2)
    ctx.fill()
    ctx.arc(cx, bulb_y, 4.0, 0, 2 * math.pi)
    ctx.fill()

    ctx.set_source_rgb(*mercury)
    fill_top = top_y + 4 if bright else bulb_y - 3
    ctx.new_sub_path()
    ctx.rectangle(cx - inner_w / 2 + 0.6, fill_top, inner_w - 1.2, bulb_y - fill_top)
    ctx.fill()
    ctx.arc(cx, bulb_y, 3.3, 0, 2 * math.pi)
    ctx.fill()

    # alert badge, only when bright (problem state): small yellow triangle
    # (same shape language as the warning icon) with a "?" - the sensor
    # doesn't know why it's failed, unlike a hard fault (that's "!").
    if bright:
        bx, by = cx + 7.5, cy - 7
        r = 6.4
        ctx.set_source_rgb(0.95, 0.75, 0.10)
        ctx.move_to(bx, by - r)
        ctx.line_to(bx - r * 0.95, by + r * 0.8)
        ctx.line_to(bx + r * 0.95, by + r * 0.8)
        ctx.close_path()
        ctx.fill()

        # bold "?" mark: thick round-capped hook + dot (filled weight,
        # like the "!" mark, not a thin stroke - thin strokes vanish at
        # this size once downsampled)
        qx, qy = bx, by + 1.6
        ctx.set_source_rgb(0.15, 0.10, 0)
        ctx.set_line_width(2.1)
        ctx.new_sub_path()
        ctx.arc(qx, qy - 1.6, 1.7, math.radians(-75), math.radians(200))
        ctx.stroke()
        ctx.move_to(qx, qy + 0.1)
        ctx.line_to(qx, qy + 0.7)
        ctx.set_line_width(1.9)
        ctx.stroke()
        ctx.arc(qx, qy + 2.4, 0.85, 0, 2 * math.pi)
        ctx.fill()

    return finish(surf)


# ---------------------------------------------------------------- warning triangle
def icon_warning_tri(bright):
    surf, ctx = new_ctx()
    cx, cy = 14, 15
    color = (1.0, 0.80, 0.0) if bright else GREY

    ctx.set_source_rgb(*color)
    ctx.move_to(cx, cy - 11)
    ctx.line_to(cx - 11.5, cy + 8.5)
    ctx.line_to(cx + 11.5, cy + 8.5)
    ctx.close_path()
    ctx.fill()

    if bright:
        ctx.set_source_rgb(0.15, 0.10, 0)
        ctx.set_line_width(1.2)
        ctx.move_to(cx, cy - 11)
        ctx.line_to(cx - 11.5, cy + 8.5)
        ctx.line_to(cx + 11.5, cy + 8.5)
        ctx.close_path()
        ctx.stroke()

    # exclamation mark
    ctx.set_source_rgb(0.15, 0.10, 0)
    ctx.rectangle(cx - 1.3, cy - 5, 2.6, 8)
    ctx.fill()
    ctx.arc(cx, cy + 6, 1.5, 0, 2 * math.pi)
    ctx.fill()

    return finish(surf)


# ---------------------------------------------------------------- alarm bell
def icon_bell(bright):
    surf, ctx = new_ctx()
    cx, cy = 14, 14
    color = (0.95, 0.15, 0.15) if bright else GREY

    ctx.set_source_rgb(*color)
    # bell body: dome + flared skirt as one path
    ctx.new_sub_path()
    ctx.move_to(cx - 7, cy + 4)
    ctx.curve_to(cx - 7.5, cy - 2, cx - 6, cy - 9, cx, cy - 9)
    ctx.curve_to(cx + 6, cy - 9, cx + 7.5, cy - 2, cx + 7, cy + 4)
    ctx.curve_to(cx + 9, cy + 4.5, cx + 9, cy + 6.5, cx + 7, cy + 6.5)
    ctx.line_to(cx - 7, cy + 6.5)
    ctx.curve_to(cx - 9, cy + 6.5, cx - 9, cy + 4.5, cx - 7, cy + 4)
    ctx.close_path()
    ctx.fill()

    # hanger loop
    ctx.set_line_width(1.6)
    ctx.new_sub_path()
    ctx.arc(cx, cy - 10.5, 1.4, 0, 2 * math.pi)
    ctx.stroke()

    # clapper
    ctx.arc(cx, cy + 9.3, 1.8, 0, 2 * math.pi)
    ctx.fill()

    # ringing sound-wave arcs, only when bright - kept close enough to the
    # bell to stay clear of the canvas edge at this small a size
    if bright:
        ctx.set_line_width(1.7)
        for side in (-1, 1):
            ox = cx + side * 8.5
            for r in (2.2, 4.2):
                ctx.new_sub_path()
                if side > 0:
                    ctx.arc(ox, cy - 2, r, math.radians(-40), math.radians(40))
                else:
                    ctx.arc_negative(ox, cy - 2, r, math.radians(180 + 40), math.radians(180 - 40))
                ctx.stroke()

    return finish(surf)


ICONS = {
    "gear": icon_gear,
    "hand": icon_hand,
    # "flame" (icon_flame) dropped from the display - heater is now the
    # horizontal bar + rays above the face instead of a column icon.
    # icon_flame() is kept above in case it's wanted again later.
    "thermometer": icon_thermometer,
    "warning": icon_warning_tri,
    "bell": icon_bell,
}


def to_rgb565_words(img):
    img = img.convert("RGB")
    words = []
    for y in range(img.height):
        for x in range(img.width):
            r, g, b = img.getpixel((x, y))
            words.append(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
    return words


def write_c_header(path, guard, arrays):
    with open(path, "w") as f:
        f.write(f"#ifndef {guard}\n#define {guard}\n\n#include <stdint.h>\n\n")
        f.write("/* Auto-generated by input/art/render_icons.py - do not edit by hand. */\n\n")
        for name, words in arrays.items():
            f.write(f"const uint16_t {name}[{len(words)}] = {{\n")
            for i in range(0, len(words), 14):
                row = words[i:i + 14]
                f.write("    " + ",".join(f"0x{w:04X}" for w in row) + ",\n")
            f.write("};\n\n")
        f.write(f"#endif /* {guard} */\n")


def main():
    outdir = os.path.join(os.path.dirname(__file__), "out")
    os.makedirs(outdir, exist_ok=True)

    header_arrays = {}
    sheet_cells = []
    for name, fn in ICONS.items():
        for bright, tag in ((True, "on"), (False, "off")):
            img = fn(bright)
            img.resize((W * 8, H * 8), Image.NEAREST).save(f"{outdir}/icon_{name}_{tag}_actual.png")
            words = to_rgb565_words(img)
            header_arrays[f"icon_{name}_{tag}"] = words
            sheet_cells.append((f"{name}_{tag}", img))
        print(f"{name}: rendered (on+off, {W}x{H}, {W*H*2} bytes each)")

    header_path = os.path.join(os.path.dirname(__file__), "..", "icon_bitmaps.h")
    write_c_header(header_path, "ICON_BITMAPS_H", header_arrays)
    print(f"\nC header written: {os.path.abspath(header_path)}")

    # contact sheet: 6 icons x 2 states, actual pixels, nearest-upscaled
    scale = 6
    pad = 8
    cols = 4
    rows = (len(sheet_cells) + cols - 1) // cols
    cell_w = W * scale + pad
    cell_h = H * scale + pad + 14
    sheet = Image.new("RGB", (cols * cell_w + pad, rows * cell_h + pad), (30, 30, 30))
    for i, (label, img) in enumerate(sheet_cells):
        r, c = divmod(i, cols)
        big = img.resize((W * scale, H * scale), Image.NEAREST)
        sheet.paste(big, (pad + c * cell_w, pad + r * cell_h))
    sheet.save(f"{outdir}/icons_contact_sheet.png")
    print(f"contact sheet: {outdir}/icons_contact_sheet.png")


if __name__ == "__main__":
    main()
