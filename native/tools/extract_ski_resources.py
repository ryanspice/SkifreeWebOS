#!/usr/bin/env python3
"""Statically extract SkiFree PE bitmap resources. Never loads or executes the EXE.

Requires Python 3 and Pillow for lossless BMP-to-PNG conversion. PE parsing and
the preservation copies use only the standard library. All resource ranges are
validated before reading; only uncompressed BITMAPINFOHEADER DIBs are accepted.
"""
from __future__ import annotations

import argparse
import hashlib
import io
import json
from pathlib import Path
import struct
import zipfile

from PIL import Image, ImageDraw


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def checked(data: bytes, start: int, size: int) -> bytes:
    if start < 0 or size < 0 or start + size > len(data):
        raise ValueError(f"Out-of-file byte range {start}+{size}")
    return data[start:start + size]


def u16(data: bytes, offset: int) -> int:
    return struct.unpack("<H", checked(data, offset, 2))[0]


def u32(data: bytes, offset: int) -> int:
    return struct.unpack("<I", checked(data, offset, 4))[0]


def bitmap_resources(exe: bytes) -> dict[int, bytes]:
    if checked(exe, 0, 2) != b"MZ":
        raise ValueError("Not a DOS/PE executable")
    pe = u32(exe, 0x3C)
    if checked(exe, pe, 4) != b"PE\0\0" or u16(exe, pe + 4) != 0x14C:
        raise ValueError("Expected a 32-bit x86 PE")
    count, optional_size = u16(exe, pe + 6), u16(exe, pe + 20)
    optional = pe + 24
    if u16(exe, optional) != 0x10B or optional_size < 120:
        raise ValueError("Expected PE32 optional header")
    if count < 1 or count > 96:
        raise ValueError("Unexpected section count")
    sections = []
    for n in range(count):
        at = optional + optional_size + n * 40
        checked(exe, at, 40)
        sections.append((u32(exe, at + 12), u32(exe, at + 16), u32(exe, at + 20)))

    def rva_to_file(rva: int, size: int) -> int:
        for base, raw_size, raw_start in sections:
            if base <= rva and rva - base + size <= raw_size:
                start = raw_start + rva - base
                checked(exe, start, size)
                return start
        raise ValueError(f"Unbacked RVA {rva:#x}+{size}")

    resource_rva, resource_size = struct.unpack("<II", checked(exe, optional + 112, 8))
    resource_start = rva_to_file(resource_rva, resource_size)
    resource = checked(exe, resource_start, resource_size)

    def entries(offset: int):
        checked(resource, offset, 16)
        n = u16(resource, offset + 12) + u16(resource, offset + 14)
        if n > 1024:
            raise ValueError("Oversized resource directory")
        for index in range(n):
            at = offset + 16 + index * 8
            yield u32(resource, at), u32(resource, at + 4)

    roots = [target for name, target in entries(0) if name == 2]
    if len(roots) != 1 or not roots[0] & 0x80000000:
        raise ValueError("No unique bitmap resource directory")
    result = {}
    for resource_id, directory in entries(roots[0] & 0x7FFFFFFF):
        if resource_id & 0x80000000 or not directory & 0x80000000:
            raise ValueError("Unexpected named bitmap or directory shape")
        languages = list(entries(directory & 0x7FFFFFFF))
        if len(languages) != 1 or languages[0][1] & 0x80000000:
            raise ValueError("Expected exactly one bitmap language leaf")
        leaf = languages[0][1]
        checked(resource, leaf, 16)
        rva, size = u32(resource, leaf), u32(resource, leaf + 4)
        if resource_id in result:
            raise ValueError("Duplicate bitmap resource ID")
        result[resource_id] = checked(exe, rva_to_file(rva, size), size)
    if set(result) != set(range(1, 90)):
        raise ValueError("Expected exactly SkiFree bitmap resource IDs 1 through 89")
    return result


def dib_to_bmp(dib: bytes) -> bytes:
    if u32(dib, 0) != 40:
        raise ValueError("Unsupported DIB header")
    width, height = struct.unpack("<ii", checked(dib, 4, 8))
    planes, bits = u16(dib, 12), u16(dib, 14)
    if planes != 1 or bits not in (1, 4, 8, 24, 32) or u32(dib, 16) != 0:
        raise ValueError("Unsupported bitmap encoding")
    if width < 1 or abs(height) < 1 or width > 4096 or abs(height) > 4096:
        raise ValueError("Invalid bitmap dimensions")
    colours = u32(dib, 32) or ((1 << bits) if bits <= 8 else 0)
    if colours > 256:
        raise ValueError("Invalid palette count")
    pixels = 40 + colours * 4
    stride = ((width * bits + 31) // 32) * 4
    checked(dib, pixels, stride * abs(height))
    return struct.pack("<2sIHHI", b"BM", len(dib) + 14, 0, 0, pixels + 14) + dib


# Resource IDs mapped against the extracted contact sheet and the original actor
# frame table. The raw resources remain available, independent of these labels.
ALIASES = {
    "skier_down": 1, "skier_left_soft": 2, "skier_left": 3, "skier_stop_left": 4,
    "skier_right_soft": 5, "skier_right": 6, "skier_stop_right": 7,
    "skier_ouch": 12, "skier_crash": 13, "skier_jump": 14,
    "skier_trick_1": 18, "skier_trick_2": 19, "skier_trick_3": 20,
    "flag_red": 23, "flag_blue": 24, "moguls": 27,
    "dog_right": 33, "dog_left": 34,
    "snowboarder_left": 37, "snowboarder_right": 38,
    "rock": 45, "stump": 46, "bump": 48, "tree_small": 49,
    "tree_dead": 50, "tree_large": 51, "ramp": 52,
    "title_original": 53, "sign_start": 57, "sign_finish": 59,
    "yeti_idle": 68, "yeti_left": 70, "yeti_left_2": 71,
    "yeti_right": 72, "yeti_right_2": 73,
    "yeti_eat": 76, "yeti_eat_2": 77, "yeti_eat_3": 78,
    "yeti_eat_4": 79, "yeti_eat_5": 80,
    "tree_fire": 83,
}


def preserve(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists() and path.read_bytes() != data:
        raise ValueError(f"Refusing to overwrite a different reference: {path}")
    path.write_bytes(data)


def extract(archive: Path, output: Path, reference: Path) -> dict:
    archive_bytes = archive.read_bytes()
    with zipfile.ZipFile(io.BytesIO(archive_bytes)) as zipped:
        entries = zipped.infolist()
        if len(entries) != 1 or entries[0].filename != "ski32.exe":
            raise ValueError("Expected a ZIP containing only ski32.exe")
        if entries[0].file_size > 4 * 1024 * 1024:
            raise ValueError("Unexpectedly large executable")
        exe = zipped.read(entries[0])
    resources = bitmap_resources(exe)
    preserve(reference / "ski32.zip", archive_bytes)
    preserve(reference / "ski32.exe", exe)
    output.mkdir(parents=True, exist_ok=True)
    original_dir = output / "original-bitmaps"
    original_dir.mkdir(exist_ok=True)
    manifest = {
        "schema": 1,
        "provenance": "SkiFree 1.04 resources from user-supplied ski32.exe; Chris Pirih",
        "archive_sha256": sha(archive_bytes), "executable_sha256": sha(exe),
        "executable_md5": hashlib.md5(exe).hexdigest(),
        "executable_size": len(exe), "resource_count": len(resources),
        "alpha_rule": "Exact RGB (255,255,255) becomes alpha 0; all other RGB values preserved at alpha 255.",
        "resources": [], "aliases": ALIASES,
    }
    sheet = Image.new("RGB", (900, 1100), "#e0e5eb")
    draw = ImageDraw.Draw(sheet)
    images = {}
    for index, (resource_id, dib) in enumerate(sorted(resources.items())):
        bmp = dib_to_bmp(dib)
        rgb = Image.open(io.BytesIO(bmp)).convert("RGB")
        # Byte iteration works across Pillow versions, without palette or
        # premultiplication surprises and without modifying non-white colours.
        source_pixels = list(zip(*[iter(rgb.tobytes())] * 3))
        rgba = Image.new("RGBA", rgb.size)
        rgba.putdata([(r, g, b, 0 if (r, g, b) == (255, 255, 255) else 255) for r, g, b in source_pixels])
        basename = f"ski32_{resource_id:03d}"
        (original_dir / f"{basename}.bmp").write_bytes(bmp)
        png_path = original_dir / f"{basename}.png"
        rgba.save(png_path)
        decoded = Image.open(png_path).convert("RGBA")
        if decoded.tobytes() != rgba.tobytes():
            raise ValueError(f"PNG round-trip mismatch: {resource_id}")
        if decoded.convert("RGB").tobytes() != rgb.tobytes():
            raise ValueError(f"RGB preservation mismatch: {resource_id}")
        images[resource_id] = rgba
        manifest["resources"].append({"id": resource_id, "width": rgb.width, "height": rgb.height,
                                      "dib_sha256": sha(dib), "bmp_sha256": sha(bmp),
                                      "png_sha256": sha(png_path.read_bytes())})
        x, y = (index % 9) * 100, (index // 9) * 110
        draw.text((x + 4, y + 3), f"{resource_id}  {rgb.width}x{rgb.height}", fill="#152638")
        sheet.paste(rgba, (x + 8, y + 24), rgba)
    sheet.save(original_dir / "contact-sheet.png")
    for name, resource_id in ALIASES.items():
        images[resource_id].save(output / f"{name}.png")
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    (reference / "SHA256SUMS.txt").write_text(
        f"{sha(archive_bytes)}  ski32.zip\n{sha(exe)}  ski32.exe\n", encoding="ascii")
    return manifest


def self_test(archive: Path) -> None:
    """Exercise real resource inventory and malformed header/range rejection."""
    with zipfile.ZipFile(archive) as zipped:
        exe = zipped.read("ski32.exe")
    bitmaps = bitmap_resources(exe)
    checks = 0
    for resource_id, dib in bitmaps.items():
        bmp = dib_to_bmp(dib)
        image = Image.open(io.BytesIO(bmp))
        image.load()
        if image.width < 1 or image.height < 1:
            raise ValueError(f"Invalid decoded resource {resource_id}")
        checks += 1
    damaged_executables = [b"", b"MZ", exe[:128], exe[:-4096]]
    invalid_rva = bytearray(exe)
    struct.pack_into("<I", invalid_rva, u32(exe, 0x3C) + 24 + 112, 0xFFFFFFFF)
    damaged_executables.append(bytes(invalid_rva))
    for damaged in damaged_executables:
        try:
            bitmap_resources(damaged)
        except ValueError:
            checks += 1
        else:
            raise ValueError("Malformed executable unexpectedly accepted")
    for dib in (b"", bitmaps[1][:40], bitmaps[1][:-1]):
        try:
            dib_to_bmp(dib)
        except ValueError:
            checks += 1
        else:
            raise ValueError("Truncated bitmap unexpectedly accepted")
    print(f"PASS: {checks} extraction checks (89 bitmap decodes + 8 malformed-input rejections)")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--zip", type=Path, required=True)
    parser.add_argument("--out", type=Path)
    parser.add_argument("--reference", type=Path)
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if args.self_test:
        self_test(args.zip)
        return
    if args.out is None or args.reference is None:
        parser.error("Extraction requires --out and --reference")
    manifest = extract(args.zip, args.out, args.reference)
    print(json.dumps({key: manifest[key] for key in ("resource_count", "executable_size", "executable_sha256", "executable_md5")}, indent=2))


if __name__ == "__main__":
    main()
