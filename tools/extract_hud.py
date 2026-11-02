#!/usr/bin/env python3
"""Extract the original Redline Rumble 4 HUD bitmaps from the Director dump.

The gameplay HUD is drawn by Director behaviours (BehaviorScript 18 -
Score&Timer.ls, BehaviorScript 20 - Overlays.ls, BehaviorScript 10 -
InfoPanels.ls and doSetupRadar in PlayerCar.ls) as camera overlays using
bitmap cast members of the Textures cast. Those members are stored as JPEG
(`ediM`) or Director bitmaps (`BITD`), each with an 8-bit alpha channel
(`ALFA`).

This tool decodes the members and writes RGBA PNGs plus an index to
assets/hud/. The exported files are the original art at their original pixel
sizes; src/hud.* positions them at the overlay coordinates from the Lingo.

Usage: python3 tools/extract_hud.py [dump_dir] [out_dir]
"""

import json
import os
import struct
import sys

from PIL import Image

DUMP = sys.argv[1] if len(sys.argv) > 1 else "work/dump/redlinerumble4"
OUT = sys.argv[2] if len(sys.argv) > 2 else "assets/hud"
CHUNKS = os.path.join(os.path.dirname(os.path.dirname(DUMP)), "dump2",
                      "redlinerumble4", "chunks")

# cast slot -> short name used by src/hud.cpp
SLOTS = {
    1: "MuteOff",
    2: "MuteOn",
    6: "BurnSmoke",       # doSetupBurnSmoke particle texture
    52: "FadeToBlack",    # FadeToBlack.ls overlay
    12: "DamageBar",
    13: "DamageBarRed",
    14: "Skull",
    15: "RedSkull",
    16: "BoostBar",
    17: "BoostBarYellow",
    18: "BoostLogo",
    21: "MessagePanel",
    47: "TimerBackground",
    54: "RadarRing",
    55: "RadarFix",
    56: "RadarRace",
    57: "RadarStage",
    58: "RadarPointPod",
    59: "RadarRacePod",
    # Front end: logo, menu buttons, pause backdrop
    10: "MenuLogo",
    71: "ButtonHowToPlay",
    72: "ButtonStart",
    73: "ButtonCredits",
    74: "ButtonBack",
    77: "PauseBackground",
    # doColorTank: splash (161..168) and paint can (171..178) textures
    161: "ColorSplash1",
    162: "ColorSplash2",
    163: "ColorSplash3",
    164: "ColorSplash4",
    165: "ColorSplash5",
    166: "ColorSplash6",
    167: "ColorSplash7",
    168: "ColorSplash8",
    171: "ColorCan1",
    172: "ColorCan2",
    173: "ColorCan3",
    174: "ColorCan4",
    175: "ColorCan5",
    176: "ColorCan6",
    177: "ColorCan7",
    178: "ColorCan8",
}
# Car paint jobs: doSetupShells swaps each shell texture for
# member(150 + g_CarsColorIDList[car], "Textures").
for i in range(1, 9):
    SLOTS[150 + i] = "Paint%d" % i
for i in range(10):
    SLOTS[25 + i] = "Score%d" % i
    SLOTS[37 + i] = "Timer%d" % i


def read_key():
    """KEY chunk: u16 entrySize, u16 entrySize2, u32 count, then entries."""
    raw = open(os.path.join(CHUNKS, "KEY_-3.bin"), "rb").read()
    count = struct.unpack_from("<I", raw, 4)[0]
    table = {}
    for i in range(count):
        section, cast, fcc = struct.unpack_from("<II4s", raw, 12 + i * 12)
        tag = fcc.decode("latin1")[::-1]
        table.setdefault(cast, {})[tag] = section
    return table


def read_cast():
    path = os.path.join(DUMP, "chunks", "CAS_-6439.json")
    return json.load(open(path))["memberIDs"]


def member_specific(member_id):
    path = os.path.join(CHUNKS, "CASt-%d.bin" % member_id)
    raw = open(path, "rb").read()
    typ, info_len, spec_len = struct.unpack_from(">III", raw, 0)
    return raw[12 + info_len:12 + info_len + spec_len]


def member_name(member_id):
    path = os.path.join(DUMP, "chunks", "CASt-%d.json" % member_id)
    try:
        return json.load(open(path))["info"]["name"]
    except (FileNotFoundError, KeyError):
        return ""


def bitmap_info(spec):
    """Bitmap member specific data (Director 5+ layout)."""
    pitch = struct.unpack_from(">H", spec, 0)[0]
    color = bool(pitch & 0x8000)
    pitch &= 0x3FFF if color else 0x0FFF
    top, left, bottom, right = struct.unpack_from(">HHHH", spec, 2)
    width = right - left
    height = bottom - top
    bpp = 32 if color and len(spec) > 23 and spec[23] else 8
    return width, height, pitch, bpp


def rle(data):
    out = bytearray()
    i = 0
    while i < len(data):
        d = data[i]
        i += 1
        if d & 0x80:
            length = ((d ^ 0xFF) & 0xFF) + 2
            value = data[i]
            i += 1
            out += bytes([value]) * length
        else:
            length = d + 1
            out += data[i:i + length]
            i += length
    return bytes(out)


def alpha_channel(raw, width, height):
    if len(raw) == width * height:
        return raw
    unpacked = rle(raw)
    if len(unpacked) < width * height:
        unpacked += b"\xff" * (width * height - len(unpacked))
    return unpacked[:width * height]


def decode_bitd(raw, width, height, bpp):
    if bpp != 32:
        raise SystemExit("unsupported HUD BITD depth %d" % bpp)
    need = width * height * 4
    if len(raw) == need:
        # Uncompressed bitmaps (the 2 x 2 paint jobs) are interleaved XRGB.
        rgb = bytearray()
        for k in range(width * height):
            rgb += raw[k * 4 + 1:k * 4 + 4]
        return Image.frombytes("RGB", (width, height), bytes(rgb))
    pixels = rle(raw)
    if len(pixels) < need:
        pixels += b"\x00" * (need - len(pixels))
    rgb = bytearray(width * height * 3)
    for y in range(height):
        row = y * width * 4
        for x in range(width):
            # Director 32-bit RLE stores four planes per row: A, R, G, B.
            r = pixels[row + width + x]
            g = pixels[row + width * 2 + x]
            b = pixels[row + width * 3 + x]
            o = (y * width + x) * 3
            rgb[o:o + 3] = bytes([r, g, b])
    return Image.frombytes("RGB", (width, height), bytes(rgb))


def main():
    key = read_key()
    cast = read_cast()
    os.makedirs(OUT, exist_ok=True)
    index = []
    for slot, short in sorted(SLOTS.items()):
        member_id = cast[slot - 1]
        chunks = key.get(member_id, {})
        spec = member_specific(member_id)
        width, height, pitch, bpp = bitmap_info(spec)
        alpha_id = chunks.get("ALFA")
        alpha = b""
        if alpha_id:
            alpha = alpha_channel(
                open(os.path.join(CHUNKS, "ALFA-%d.bin" % alpha_id), "rb").read(),
                width, height)

        if "ediM" in chunks:
            raw = open(os.path.join(CHUNKS, "ediM-%d.bin" % chunks["ediM"]), "rb").read()
            img = Image.open(__import__("io").BytesIO(raw)).convert("RGB")
            width, height = img.size
            if alpha and len(alpha) != width * height:
                alpha = alpha_channel(
                    open(os.path.join(CHUNKS, "ALFA-%d.bin" % alpha_id), "rb").read(),
                    width, height)
        elif "BITD" in chunks:
            raw = open(os.path.join(CHUNKS, "BITD-%d.bin" % chunks["BITD"]), "rb").read()
            img = decode_bitd(raw, width, height, bpp)
        else:
            print("skip slot %d %s: no image chunk" % (slot, short))
            continue

        img = img.convert("RGBA")
        if alpha:
            img.putalpha(Image.frombytes("L", (width, height), alpha))
        path = os.path.join(OUT, short + ".png")
        img.save(path)
        index.append("%d\t%s\t%s\t%d\t%d" % (slot, member_id, short, width, height))
        print("%-16s member %-5d %3dx%-3d %s" % (short, member_id, width, height,
                                                 member_name(member_id)))

    with open(os.path.join(OUT, "index.txt"), "w") as f:
        f.write("slot\tmember\tshort\twidth\theight\n")
        f.write("\n".join(index) + "\n")
    print("extracted %d original HUD bitmaps to %s" % (len(index), OUT))


if __name__ == "__main__":
    main()
