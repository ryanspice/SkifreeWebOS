#!/usr/bin/env python3
"""Create the project's original geometric mountain/ski icon, without game assets.

Standard library only. All pixels come from the geometry below, not an image.
"""
from pathlib import Path
import struct
import zlib


def icon_bytes() -> bytes:
    size = 160
    pixels = bytearray()
    for y in range(size):
        pixels.append(0)
        for x in range(size):
            color = (9, 37, 72, 255)
            # Broad stylized mountain and two diagonal ski tracks.
            if 26 <= y <= 124 and abs(x - 80) <= (y - 26) * 0.73:
                color = (38, 103, 151, 255)
            if 26 <= y <= 62 and abs(x - 80) <= (y - 26) * 0.73:
                color = (239, 249, 255, 255)
            if 67 <= y <= 139 and (abs(x - (117 - (y - 67) * .74)) <= 3 or abs(x - (132 - (y - 67) * .74)) <= 3):
                color = (117, 221, 236, 255)
            pixels.extend(color)
    def chunk(kind: bytes, data: bytes) -> bytes:
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data) & 0xffffffff)
    return (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", size, size, 8, 6, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(bytes(pixels), 9)) + chunk(b"IEND", b""))


if __name__ == "__main__":
    destination = Path(__file__).resolve().parents[1] / "icon.png"
    destination.write_bytes(icon_bytes())
    print(destination)
