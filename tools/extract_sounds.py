#!/usr/bin/env python3
"""Extract the original Redline Rumble 4 Sfx cast members as WAV files.

The Director movie stores every sound as an uncompressed 16-bit mono 44.1 kHz
big-endian PCM `snd ` chunk. `KEY_-3.json` maps cast-member IDs to chunk section
IDs, and `CAS_-6473.json` is the Sfx cast slot table, so each member can be
written with its original name.

Usage: python3 tools/extract_sounds.py [dump_dir] [out_dir]
"""

import json
import os
import re
import struct
import sys

DUMP = sys.argv[1] if len(sys.argv) > 1 else "work/dump/redlinerumble4"
OUT = sys.argv[2] if len(sys.argv) > 2 else "assets/sounds"
CHUNKS = os.path.join(os.path.dirname(os.path.dirname(DUMP)), "dump2", "redlinerumble4", "chunks")


def wav_from_snd(blob):
    """Director snd chunk -> canonical little-endian mono 16-bit WAV."""
    # 80-byte header: sample rate is a big-endian uint16 at 0x16, mono 16-bit
    # PCM data starts at 0x50.
    rate = struct.unpack(">H", blob[0x16:0x18])[0]
    pcm = blob[0x50:]
    pcm = pcm[: len(pcm) & ~1]
    swapped = bytearray(len(pcm))
    swapped[0::2] = pcm[1::2]
    swapped[1::2] = pcm[0::2]

    data_len = len(swapped)
    header = b"RIFF" + struct.pack("<I", 36 + data_len) + b"WAVE"
    header += b"fmt " + struct.pack("<IHHIIHH", 16, 1, 1, rate, rate * 2, 2, 16)
    header += b"data" + struct.pack("<I", data_len)
    return header + swapped


def main():
    key_raw = open(os.path.join(DUMP, "chunks", "KEY_-3.json"), encoding="latin-1").read()
    entries = re.findall(
        r'\{\s*"sectionID":\s*(\d+),\s*"castID":\s*(\d+),\s*"fourCC":\s*"(.*?)"\s*\}',
        key_raw,
    )
    sound_sections = {
        int(cast): int(section)
        for section, cast, fourcc in entries
        if fourcc.strip() == "snd"
    }

    slots = json.load(open(os.path.join(DUMP, "chunks", "CAS_-6473.json")))["memberIDs"]
    names = {}
    chunk_dir = os.path.join(DUMP, "chunks")
    for fn in os.listdir(chunk_dir):
        m = re.fullmatch(r"CASt-(-?\d+)\.json", fn)
        if not m:
            continue
        member = json.load(open(os.path.join(chunk_dir, fn)))
        names[int(m.group(1))] = member["info"].get("name", "")

    os.makedirs(OUT, exist_ok=True)
    written = 0
    index = []
    for slot, cast_id in enumerate(slots, 1):
        if not cast_id or cast_id not in sound_sections:
            continue
        name = names.get(cast_id) or f"Sfx_{slot:02d}"
        section = sound_sections[cast_id]
        src = os.path.join(CHUNKS, f"snd -{section}.bin")
        if not os.path.exists(src):
            print(f"missing chunk for {name}: {src}")
            continue
        blob = open(src, "rb").read()
        wav = wav_from_snd(blob)
        dst = os.path.join(OUT, name + ".wav")
        open(dst, "wb").write(wav)
        index.append(f"{slot:02d}\t{cast_id}\t{section}\t{name}\t{len(wav)}")
        written += 1

    open(os.path.join(OUT, "index.txt"), "w").write(
        "\n".join(["slot\tcastID\tsectionID\tname\tbytes"] + index) + "\n"
    )
    print(f"extracted {written} original Sfx members to {OUT}")


if __name__ == "__main__":
    main()
