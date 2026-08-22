from PIL import Image, ImageDraw
import math

W, H = 128, 160
DIM_GREY = (32, 32, 32)          # divider + off-icons + heaterpower=0

# 6-stop palette across 0..100%, per direction:
# grey -> light grey -> dark red -> red -> dark orange -> orange
STOPS = [
    (0,   (32, 32, 32)),
    (20,  (110, 110, 110)),
    (40,  (120, 20, 20)),
    (60,  (215, 35, 35)),
    (80,  (180, 90, 10)),
    (100, (255, 150, 0)),
]

def heat_color(percent):
    p = max(0, min(100, percent))
    for (p0, c0), (p1, c1) in zip(STOPS, STOPS[1:]):
        if p0 <= p <= p1:
            t = (p - p0) / (p1 - p0) if p1 > p0 else 0
            return tuple(int(c0[i] + t * (c1[i] - c0[i])) for i in range(3))
    return STOPS[-1][1]

def draw_resistor(d, cx, y, w, h, color):
    body_w = int(w * 0.55)
    lead_w = (w - body_w) // 2
    cy = y + h // 2
    # wire leads (silver)
    d.line([(cx - w // 2, cy), (cx - body_w // 2, cy)], fill=(180, 180, 180), width=2)
    d.line([(cx + body_w // 2, cy), (cx + w // 2, cy)], fill=(180, 180, 180), width=2)
    # capsule body: rounded rect
    x0, x1 = cx - body_w // 2, cx + body_w // 2
    d.rounded_rectangle([x0, y, x1, y + h], radius=h // 2, fill=color, outline=(210, 210, 210))

def draw_screen(percent):
    img = Image.new("RGB", (W, H), (0, 0, 0))
    d = ImageDraw.Draw(img)

    icons_order = ["warning", "bell", "thermometer", "gear"]
    slot_h = 40
    for i, name in enumerate(icons_order):
        icon = Image.open(f"out/icon_{name}_off_actual.png").resize((28, 28), Image.NEAREST)
        # tint check: use the *_off bitmap as-is if already regenerated with dim grey; else just paste
        img.paste(icon, (2, i * slot_h + 6))

    d.line([(33, 0), (33, H)], fill=DIM_GREY, width=1)

    color = heat_color(percent)
    draw_resistor(d, 79, 14, 70, 10, color)

    ray_xs_ratio = [-0.36, -0.19, 0.0, 0.19, 0.36]
    ray_top, ray_bot = 29, 58
    segs = 12
    for ratio in ray_xs_ratio:
        rx = 79 + ratio * 70
        pts = []
        for s in range(segs + 1):
            t = s / segs
            y = ray_top + (ray_bot - ray_top) * t
            x = rx + 2.2 * math.sin(2 * math.pi * t)
            pts.append((x, y))
        for a, b in zip(pts, pts[1:]):
            d.line([a, b], fill=color, width=1)

    return img

for pct in (0, 20, 40, 60, 80, 100):
    scr = draw_screen(pct)
    face = Image.open("out/preview_1F60A.png").resize((80, 80), Image.LANCZOS)
    scr.paste(face, (79 - 40, 100 - 40), face)
    scr.resize((W * 3, H * 3), Image.NEAREST).save(f"out/heatmock2_{pct}.png")

imgs = [Image.open(f"out/heatmock2_{p}.png") for p in (0, 20, 40, 60, 80, 100)]
sheet = Image.new("RGB", (sum(i.width for i in imgs) + 70, imgs[0].height + 20), (30, 30, 30))
x = 10
for im in imgs:
    sheet.paste(im, (x, 10)); x += im.width + 10
sheet.save("out/heatmock2_contact_sheet.png")
print("done")
