#!/usr/bin/env python3
"""Build the PS Vita bubble and LiveArea images into vita/sce_sys/.

Uses the main menu's title art (assets/hud/menu/Title.png, Subtitle.png from
tools/render_menu.py) and the Speedway font on the menu's dark background.
The Vita requires 8-bit palette PNGs: icon0 128x128, LiveArea background
840x500, startup gate 280x158.

    python3 tools/make_livearea.py
"""

import os

from PIL import Image, ImageDraw, ImageFont

OUT = "vita/sce_sys"
MENU = "assets/hud/menu"
FONT = "assets/Speedway BV.ttf"

BG_TOP = (14, 16, 22)        # dark, like the main menu panel
BG_BOTTOM = (4, 5, 8)
ACCENT = (255, 128, 32)      # orange, like the menu cursor
TEXT = (255, 255, 255)


def gradient(w, h):
    img = Image.new("RGB", (w, h))
    px = img.load()
    for y in range(h):
        t = y / max(1, h - 1)
        c = tuple(int(a + (b - a) * t) for a, b in zip(BG_TOP, BG_BOTTOM))
        for x in range(w):
            px[x, y] = c
    return img


def paste_scaled(dst, src, width, x, y):
    h = round(src.height * width / src.width)
    s = src.resize((width, h), Image.LANCZOS)
    dst.paste(s, (x, y), s)
    return h


def save_8bit(img, path):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    img.convert("RGB").quantize(colors=256, method=Image.MEDIANCUT).save(path, optimize=True)
    print("wrote", path)


def icon():
    img = gradient(128, 128)
    d = ImageDraw.Draw(img)
    big = ImageFont.truetype(FONT, 74)
    small = ImageFont.truetype(FONT, 22)
    d.text((64, 56), "RR4", font=big, fill=TEXT, anchor="mm")
    d.rectangle((14, 92, 114, 96), fill=ACCENT)
    d.text((64, 110), "REDLINE", font=small, fill=ACCENT, anchor="mm")
    return img


def background(title, subtitle):
    img = gradient(840, 500)
    d = ImageDraw.Draw(img)
    h = paste_scaled(img, title, 560, 60, 170)
    paste_scaled(img, subtitle, round(subtitle.width * 560 / title.width), 60, 170 + h + 8)
    d.rectangle((40, 170, 46, 170 + h), fill=ACCENT)
    return img


def startup(title):
    img = gradient(280, 158)
    d = ImageDraw.Draw(img)
    paste_scaled(img, title, 240, 20, 60)
    d.rectangle((20, 104, 260, 106), fill=ACCENT)
    return img


TEMPLATE = """<?xml version="1.0" encoding="utf-8"?>
<livearea style="a1" format-ver="01.00" content-rev="1">
  <livearea-background>
    <image>bg.png</image>
  </livearea-background>
  <gate>
    <startup-image>startup.png</startup-image>
  </gate>
</livearea>
"""


def main():
    title = Image.open(os.path.join(MENU, "Title.png")).convert("RGBA")
    subtitle = Image.open(os.path.join(MENU, "Subtitle.png")).convert("RGBA")
    save_8bit(icon(), os.path.join(OUT, "icon0.png"))
    contents = os.path.join(OUT, "livearea", "contents")
    save_8bit(background(title, subtitle), os.path.join(contents, "bg.png"))
    save_8bit(startup(title), os.path.join(contents, "startup.png"))
    with open(os.path.join(contents, "template.xml"), "w") as f:
        f.write(TEMPLATE)
    print("wrote", os.path.join(contents, "template.xml"))


if __name__ == "__main__":
    main()
