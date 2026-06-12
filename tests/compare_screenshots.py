#!/usr/bin/env python3
# Copyright (c) 2026 LVGL
# SPDX-License-Identifier: Apache-2.0
"""Compare two 24-bit uncompressed BMP screenshots.

Usage: compare_screenshots.py REFERENCE CANDIDATE [DIFF_OUTPUT]

Exits non-zero if the images differ in size or if more than MAX_DIFF_RATIO
of the pixels differ by more than CHANNEL_TOLERANCE on any channel. Small
tolerances absorb anti-aliasing differences between libc/compiler versions.

If DIFF_OUTPUT is given, a visualization is written: matching pixels are
dimmed, differing pixels are marked red. Only Python stdlib is used.
"""

import struct
import sys

CHANNEL_TOLERANCE = 8     # per-channel difference treated as equal (0-255)
MAX_DIFF_RATIO = 0.01     # allowed fraction of differing pixels


def read_bmp(path):
    with open(path, "rb") as f:
        data = f.read()
    if data[:2] != b"BM":
        sys.exit(f"{path}: not a BMP file")
    pixel_offset = struct.unpack_from("<I", data, 10)[0]
    width, height = struct.unpack_from("<ii", data, 18)
    planes, bpp = struct.unpack_from("<HH", data, 26)
    compression = struct.unpack_from("<I", data, 30)[0]
    if bpp != 24 or compression != 0 or height <= 0:
        sys.exit(f"{path}: expected uncompressed bottom-up 24-bit BMP")
    row_size = ((width * 3) + 3) & ~3
    rows = []
    for y in range(height - 1, -1, -1):
        start = pixel_offset + y * row_size
        rows.append(data[start:start + width * 3])
    return width, height, rows


def write_bmp(path, width, height, rows):
    row_size = ((width * 3) + 3) & ~3
    padding = b"\x00" * (row_size - width * 3)
    file_size = 54 + row_size * height
    header = struct.pack(
        "<2sIHHIIiiHHIIiiII",
        b"BM", file_size, 0, 0, 54, 40,
        width, height, 1, 24, 0, row_size * height, 0, 0, 0, 0,
    )
    with open(path, "wb") as f:
        f.write(header)
        for row in reversed(rows):
            f.write(row + padding)


def main():
    if len(sys.argv) not in (3, 4):
        sys.exit(__doc__)
    ref_path, cand_path = sys.argv[1], sys.argv[2]
    diff_path = sys.argv[3] if len(sys.argv) == 4 else None

    ref_w, ref_h, ref_rows = read_bmp(ref_path)
    cand_w, cand_h, cand_rows = read_bmp(cand_path)
    if (ref_w, ref_h) != (cand_w, cand_h):
        sys.exit(f"size mismatch: {ref_w}x{ref_h} vs {cand_w}x{cand_h}")

    diff_count = 0
    diff_rows = []
    for ref_row, cand_row in zip(ref_rows, cand_rows):
        diff_row = bytearray()
        for x in range(0, ref_w * 3, 3):
            differs = any(
                abs(ref_row[x + c] - cand_row[x + c]) > CHANNEL_TOLERANCE
                for c in range(3)
            )
            if differs:
                diff_count += 1
                diff_row += b"\x00\x00\xff"  # red (BGR)
            else:
                gray = sum(cand_row[x:x + 3]) // 6  # dimmed
                diff_row += bytes((gray, gray, gray))
        diff_rows.append(bytes(diff_row))

    total = ref_w * ref_h
    ratio = diff_count / total
    print(f"{diff_count}/{total} pixels differ ({ratio:.4%}), "
          f"allowed {MAX_DIFF_RATIO:.0%}")

    if diff_path:
        write_bmp(diff_path, ref_w, ref_h, diff_rows)
        print(f"diff visualization written to {diff_path}")

    if ratio > MAX_DIFF_RATIO:
        sys.exit("screenshot does not match the reference")
    print("screenshot matches the reference")


if __name__ == "__main__":
    main()
