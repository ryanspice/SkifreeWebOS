#!/usr/bin/env python3
"""Return the project's checked-in webOS icon bytes.

The icon artwork was onboarded 2026-11-09 from the supplied
skifree-webos-icon.png (1254x1254). This module preserves the
icon_bytes() interface used by the stage audit and source-archive
builder, returning native/icon.png as the source of truth.
"""
from pathlib import Path


def icon_bytes() -> bytes:
    return (Path(__file__).resolve().parents[1] / "icon.png").read_bytes()


if __name__ == "__main__":
    print(Path(__file__).resolve().parents[1] / "icon.png")
