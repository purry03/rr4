#!/usr/bin/env python3
"""Render the original InfoPanels messages onto the message panel bitmap.

doPanel (BehaviorScript 10 - InfoPanels.ls) takes member(messageID,
"infoPanels").image, uses its alpha as a mask, fills the mask with
messageColor and copies it onto a copy of member(21, "Textures") (the panel
bitmap) at offset (5, 20). Every call in the free-drive build uses white.

The infoPanels members are Director text members (XMED). This tool reads their
original strings and the layout values decoded from their style tables:

  * text box width 0xD2 = 210 px (text member header)
  * line spacing per paragraph style (fixed line space)
  * character size per character style

The text members use the embedded font "Speedway BV" (a Director PFR1 font,
XMED-6612 in the dump). FreeType cannot load PFR1, so the TrueType conversion
in assets/Speedway BV.ttf is used; a bold oblique system font stands in only
if that file is missing.

Usage: python3 tools/render_panels.py [--font font.ttf] [dump2_chunks] [out_dir]
"""

import argparse
import os
import re

from PIL import Image, ImageDraw, ImageFont

FALLBACK_FONTS = [
    "assets/Speedway BV.ttf",
    "/usr/share/fonts/truetype/freefont/FreeSansBoldOblique.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-BoldOblique.ttf",
]

TEXT_WIDTH = 210        # text member box width
PANEL_OFFSET = (5, 20)  # imagePanel.copyPixels(..., rect + rect(5, 20, 5, 20))

# messageID -> (XMED section of the infoPanels member, char size,
#               line spacing of the first paragraph, of the following ones)
MESSAGES = {
    1: (7154, 12, 16, 16),    # MEANIE CROPPER              (doFerrisWheel, car panels)
    2: (7156, 12, 16, 16),    # BITSASUSHI EVIL 6
    3: (7158, 12, 16, 16),    # AMAZED ARCS 8
    4: (7160, 12, 16, 16),    # NEESON HIGH WIRE
    5: (7162, 12, 16, 16),    # POSHER 9 ELF
    6: (7164, 12, 16, 16),    # FURORE 4 THIRSTY
    8: (7168, 12, 16, 16),    # FERRIS' WHEELS              (parking in the wheel)
    9: (7170, 12, 16, 16),    # RIX RAPID REPAIR SHOP       (doRixRapidRepairs)
    16: (7174, 16, 36, 24),   # IgNitro Pickup              (doIgNitro, first bottle)
    18: (7176, 72, 72, 72),   # Ready                       (race start line)
    19: (7178, 72, 72, 72),   # Set
    20: (7180, 72, 72, 72),   # Go!
    28: (7196, 16, 36, 24),   # RACE CANCELLED.
    30: (7200, 16, 36, 24),   # RACE FAILED / Car Wrecked!
    31: (7202, 16, 36, 18),   # Car Wrecked!                (doupdate, free drive)
    32: (7204, 12, 16, 16),   # RIX RAPID REPAIR SHOP       (doDamageMaxOutReset)
    33: (7206, 16, 36, 24),   # RACE FAILED / You left the race area.
    34: (7208, 16, 36, 24),   # RACE WINNER! You won your opponents car
    35: (7210, 16, 36, 24),   # Submit Time? Hit Spacebar
    37: (7214, 16, 36, 24),   # RACE LOST! Your opponent now OWNS your car
    38: (7216, 16, 36, 24),   # RACE LOST! Returning you back to Ferris' Wheels
    39: (7218, 16, 36, 24),   # RACE LOST! ... refused your pink slip
    44: (7224, 16, 36, 24),   # RACE WINNER! ... already own this car
}


def message_text(chunks, section):
    raw = open(os.path.join(chunks, "XMED-%d.bin" % section), "rb").read()
    # Section 0002 holds the string as "<hex length>,<text>".
    m = re.search(rb"\x01?([0-9A-F]+),", raw[raw.find(b"00020000"):])
    start = raw.find(b"00020000") + m.end()
    text = raw[start:start + int(m.group(1), 16)].decode("latin1")
    return [p for p in text.split("\r") if p]


def wrap(draw, font, text, width):
    lines, line = [], ""
    for word in text.split(" "):
        test = word if not line else line + " " + word
        if draw.textlength(test, font=font) <= width or not line:
            line = test
        else:
            lines.append(line)
            line = word
    lines.append(line)
    return lines


def render(panel, font_path, chunks, section, size, first_space, space):
    font = ImageFont.truetype(font_path, size)
    mask = Image.new("L", (TEXT_WIDTH, panel.height - PANEL_OFFSET[1]), 0)
    draw = ImageDraw.Draw(mask)
    ascent, _ = font.getmetrics()
    y = 0
    for i, para in enumerate(message_text(chunks, section)):
        line_space = first_space if i == 0 else space
        for line in wrap(draw, font, para, TEXT_WIDTH):
            # Fixed line space: the line box is line_space tall, text sits on
            # its baseline at the bottom of the box like Director draws it.
            draw.text((0, y + line_space - ascent - 2), line, font=font, fill=255)
            y += line_space
    out = panel.copy()
    white = Image.new("RGBA", mask.size, (255, 255, 255, 255))
    out.paste(white, PANEL_OFFSET, mask)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--font", help="font file (default: assets/Speedway BV.ttf)")
    ap.add_argument("chunks", nargs="?", default="work/dump2/redlinerumble4/chunks")
    ap.add_argument("out", nargs="?", default="assets/hud")
    args = ap.parse_args()
    font_path = args.font or next(p for p in FALLBACK_FONTS if os.path.exists(p))
    panel = Image.open(os.path.join(args.out, "MessagePanel.png")).convert("RGBA")
    for message_id, (section, size, first, space) in MESSAGES.items():
        img = render(panel, font_path, args.chunks, section, size, first, space)
        path = os.path.join(args.out, "Panel%d.png" % message_id)
        img.save(path)
        print("%s  (%s)" % (path, os.path.basename(font_path)))


if __name__ == "__main__":
    main()
