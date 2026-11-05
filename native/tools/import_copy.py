#!/usr/bin/env python3
"""Import a user's local SkiFree 1.04 copy without running or distributing it.

Requires Pillow. Produces private PNGs and provenance, never an EXE/archive copy.
No network access. The source must be the supported original ski32.exe build.
"""
from __future__ import annotations

import argparse
import hashlib
import io
import json
from pathlib import Path
import shutil
import tempfile
import zipfile

from PIL import Image
from extract_ski_resources import ALIASES, bitmap_resources, dib_to_bmp

SUPPORTED_SHA256 = "3572d3757638bc1ae388c3d007ba59ba1f370911901e2a35d2f1297e8cf0ff35"
MAX_SOURCE_BYTES = 4 * 1024 * 1024


def read_copy(source: Path) -> bytes:
    if not source.is_file() or source.stat().st_size > MAX_SOURCE_BYTES:
        raise ValueError("Choose a local ski32.exe or small ZIP containing it.")
    raw = source.read_bytes()
    if raw[:2] == b"MZ":
        executable = raw
    else:
        with zipfile.ZipFile(io.BytesIO(raw)) as archive:
            entries = archive.infolist()
            if len(entries) != 1 or entries[0].filename.lower() != "ski32.exe":
                raise ValueError("The ZIP must contain only ski32.exe at its root.")
            entry = entries[0]
            if entry.file_size > MAX_SOURCE_BYTES or entry.flag_bits & 1:
                raise ValueError("Unsupported ZIP member size or encryption.")
            executable = archive.read(entry)
    if hashlib.sha256(executable).hexdigest() != SUPPORTED_SHA256:
        raise ValueError("Unsupported copy. This importer supports the original SkiFree 1.04 ski32.exe; the input was not executed.")
    return executable


def import_copy(source: Path, output: Path) -> dict:
    output = output.resolve()
    project = Path(__file__).resolve().parents[2]
    if output.is_relative_to(project) and not output.is_relative_to(project / "private"):
        raise ValueError("Imports inside this repository must go under private/, never the public source or package stage.")
    if output.exists():
        raise ValueError("Output already exists. Choose a new private directory; no imported files were overwritten.")
    executable = read_copy(source)
    resources = bitmap_resources(executable)
    if any(resource not in resources for resource in ALIASES.values()):
        raise ValueError("The copy is missing required bitmap resources.")
    output.parent.mkdir(parents=True, exist_ok=True)
    temporary = Path(tempfile.mkdtemp(prefix=".skifree-import-", dir=output.parent))
    try:
        files = []
        for name, resource in ALIASES.items():
            with Image.open(io.BytesIO(dib_to_bmp(resources[resource]))) as bitmap:
                rgb = bitmap.convert("RGB")
                pixels = list(zip(*[iter(rgb.tobytes())] * 3))
                rgba = Image.new("RGBA", rgb.size)
                rgba.putdata([(r, g, b, 0 if (r, g, b) == (255, 255, 255) else 255) for r, g, b in pixels])
                path = temporary / f"{name}.png"
                rgba.save(path)
                with Image.open(path) as decoded:
                    if decoded.convert("RGBA").tobytes() != rgba.tobytes():
                        raise ValueError("PNG round-trip verification failed.")
                files.append({"name": path.name, "resource": resource, "bytes": path.stat().st_size,
                              "sha256": hashlib.sha256(path.read_bytes()).hexdigest()})
        manifest = {"schema": 1, "private_user_import": True,
                    "notice": "Original game and artwork: Chris Pirih. Not licensed by this project's MIT code license.",
                    "executable_sha256": SUPPORTED_SHA256, "executable_executed": False,
                    "network_used": False, "files": files}
        (temporary / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
        temporary.rename(output)
    finally:
        if temporary.exists():
            shutil.rmtree(temporary)
    return manifest


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    try:
        result = import_copy(args.source, args.out)
    except (OSError, ValueError, zipfile.BadZipFile) as error:
        parser.exit(1, f"Import stopped: {error}\n")
    print(f"Imported {len(result['files'])} private sprites to {args.out.resolve()}")
    print("No executable was run, downloaded, copied to output, or uploaded.")
    print("Development extraction only. Release packaging uses the separate explicit sprite inventory.")


if __name__ == "__main__":
    main()
