#!/usr/bin/env python3
"""Copy edited textures from assets/textures/ back into assets/rr4_world.bin.

The pack (magic `RR4W`, version 1) was built once from the original
Shockwave 3D stream; `assets/textures/tex-N.{jpg,png}` are its texture entries
as files. Replace or upscale one of those files, then run

    python3 tools/pack_textures.py [pack] [texture_dir]

Every entry whose file differs from the packed bytes is replaced, with its
stored width/height updated. Materials refer to textures by index, so the
mapping stays intact. Requires Pillow (for the image size).

Texture entry layout (little endian, see W3DAssets::load in src/assets.cpp):
    u32 width, u32 height, u32 length, u8 encoding (0 = JPEG, 1 = PNG), bytes
"""

import glob
import os
import struct
import sys

from PIL import Image

PACK = sys.argv[1] if len(sys.argv) > 1 else "assets/rr4_world.bin"
TEX_DIR = sys.argv[2] if len(sys.argv) > 2 else "assets/textures"

MAGIC = b"RR4W"
VERSION = 1
HEADER = struct.Struct("<4s4I")   # magic, version, textures, materials, meshes
ENTRY = struct.Struct("<3IB")     # width, height, length, encoding
ENCODING = {"JPEG": 0, "PNG": 1}


def main():
    data = open(PACK, "rb").read()
    magic, version, tex_count, _, _ = HEADER.unpack_from(data, 0)
    if magic != MAGIC or version != VERSION:
        sys.exit("%s: not an RR4W v%d pack" % (PACK, VERSION))

    out = [data[:HEADER.size]]
    o = HEADER.size
    changed = 0
    for i in range(tex_count):
        w, h, length, enc = ENTRY.unpack_from(data, o)
        o += ENTRY.size
        packed = data[o:o + length]
        o += length

        files = glob.glob(os.path.join(TEX_DIR, "tex-%d.*" % i))
        new = open(files[0], "rb").read() if files else packed
        if new != packed:
            with Image.open(files[0]) as img:
                if img.format not in ENCODING:
                    sys.exit("%s: unsupported format %s" % (files[0], img.format))
                w, h = img.size
                enc = ENCODING[img.format]
            print("tex-%d: %s %dx%d (%d bytes)" % (i, files[0], w, h, len(new)))
            changed += 1
        out.append(ENTRY.pack(w, h, len(new), enc))
        out.append(new)
    out.append(data[o:])  # materials and meshes, unchanged

    if not changed:
        print("no texture changes")
        return
    open(PACK, "wb").write(b"".join(out))
    print("wrote %s: %d texture(s) replaced" % (PACK, changed))


if __name__ == "__main__":
    main()
