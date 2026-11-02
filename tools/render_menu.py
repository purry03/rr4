#!/usr/bin/env python3
"""Render the main menu title sprites and the runtime font atlas into
assets/hud/menu/ with the original Speedway BV typeface
(assets/Speedway BV.ttf).

Font.png holds the glyphs of FONT_CHARS in one row of cells that all share
the line height; Font.txt lists "height <px>" then "<code> <x> <width>" per
glyph (x and width in atlas pixels, width = advance). src/spritefont.cpp
reads both.

Sprites are drawn at 2x the 800 x 480 stage scale; src/menu.cpp draws them
at scale 0.5 so they stay sharp on larger windows.

Usage: python3 tools/render_menu.py [out_dir]
"""

import os
import sys

from PIL import Image, ImageDraw, ImageFilter, ImageFont

OUT = sys.argv[1] if len(sys.argv) > 1 else "assets/hud/menu"
FONT = "assets/Speedway BV.ttf"
SCALE = 2

WHITE = (255, 255, 255, 255)
ORANGE = (255, 122, 26, 255)

# name -> (text, stage pixel size, colour, soft shadow)
SPRITES = {
    "Title": ("REDLINE RUMBLE 4", 46, WHITE, True),
    "Subtitle": ("IGNITRO CITY", 20, ORANGE, True),
}


def render(text, size, colour, shadow):
    font = ImageFont.truetype(FONT, size * SCALE)
    left, top, right, bottom = font.getbbox(text)
    pad = 6 * SCALE
    w, h = right - left + 2 * pad, bottom - top + 2 * pad
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    if shadow:
        sh = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        ImageDraw.Draw(sh).text((pad - left + SCALE * 2, pad - top + SCALE * 2), text,
                                font=font, fill=(0, 0, 0, 170))
        img = Image.alpha_composite(img, sh.filter(ImageFilter.GaussianBlur(SCALE * 2)))
    ImageDraw.Draw(img).text((pad - left, pad - top), text, font=font, fill=colour)
    return img


FONT_CHARS = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/%.:-?!',()+<>"
FONT_PX = 64          # atlas glyph size in pixels
FONT_PAD = 3          # empty texels around each glyph (no bilinear bleed)


def render_atlas():
    font = ImageFont.truetype(FONT, FONT_PX)
    ascent, descent = font.getmetrics()
    height = ascent + descent + 2 * FONT_PAD
    cells = []
    x = 0
    for ch in FONT_CHARS:
        adv = int(round(font.getlength(ch))) + 2 * FONT_PAD
        cells.append((ch, x, adv))
        x += adv
    img = Image.new("RGBA", (x, height), (255, 255, 255, 0))
    draw = ImageDraw.Draw(img)
    for ch, cx, _ in cells:
        draw.text((cx + FONT_PAD, FONT_PAD), ch, font=font, fill=WHITE)
    img.save(os.path.join(OUT, "Font.png"))
    with open(os.path.join(OUT, "Font.txt"), "w") as f:
        f.write("height %d\n" % height)
        for ch, cx, adv in cells:
            f.write("%d %d %d\n" % (ord(ch), cx, adv))
    print(os.path.join(OUT, "Font.png"), "%dx%d" % (x, height))


def main():
    os.makedirs(OUT, exist_ok=True)
    for name, (text, size, colour, shadow) in SPRITES.items():
        path = os.path.join(OUT, name + ".png")
        render(text, size, colour, shadow).save(path)
        print(path)
    render_atlas()


if __name__ == "__main__":
    main()
