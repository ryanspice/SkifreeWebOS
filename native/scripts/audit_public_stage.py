#!/usr/bin/env python3
"""Fail-closed audit of the free package and its exact attributed sprite inventory."""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path

REQUIRED = {"skifree-native", "appinfo.json", "icon.png", "LICENSE", "NOTICE.md", "CLASSIC-PROVENANCE.md", "assets/README.md", "assets/manifest.json"}
OPTIONAL = {"lib/libSDL2-2.0.so.0", "lib/libpng16.so.16", "licenses/SDL2-LICENSE.txt",
            "licenses/libpng-LICENSE.txt", "RUNTIME-FILES.json", "RUNTIME-AUDIT.json"}


def audit(stage: Path) -> dict:
    stage = stage.resolve(strict=True)
    source_assets = Path(__file__).resolve().parents[1] / "assets"
    manifest = json.loads((source_assets / "manifest.json").read_text(encoding="utf-8-sig"))
    expected_assets = {"assets/" + item["name"]: item["sha256"] for item in manifest["files"]}
    if len(expected_assets) != 89 or any(not name.endswith(".png") or Path(name).parent.as_posix() != "assets" for name in expected_assets):
        raise ValueError("Unexpected sprite inventory")
    required = REQUIRED | expected_assets.keys()
    files = {}
    for path in stage.rglob("*"):
        if path.is_symlink():
            raise ValueError(f"Symlinks are forbidden in public stage: {path.relative_to(stage)}")
        if path.is_file():
            name = path.relative_to(stage).as_posix()
            if name not in required | OPTIONAL:
                raise ValueError(f"Unexpected public file (possible private material): {name}")
            files[name] = path
    if missing := required - files.keys():
        raise ValueError(f"Required stage files missing: {sorted(missing)}")
    for name, digest in expected_assets.items():
        if hashlib.sha256(files[name].read_bytes()).hexdigest() != digest:
            raise ValueError("Bundled sprite hash mismatch: " + name)
    if files["assets/manifest.json"].read_bytes() != (source_assets / "manifest.json").read_bytes():
        raise ValueError("Staged sprite manifest differs from the audited source")
    binary = files["skifree-native"].read_bytes()
    if binary[:4] != b"\x7fELF":
        raise ValueError("Public executable must be the native ELF, never a Windows executable.")
    icon_source = Path(__file__).resolve().parents[1] / "tools/generate_icon.py"
    spec = importlib.util.spec_from_file_location("original_icon", icon_source)
    generator = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(generator)
    if files["icon.png"].read_bytes() != generator.icon_bytes():
        raise ValueError("Icon is not the original procedural project icon.")
    app = json.loads(files["appinfo.json"].read_text(encoding="utf-8-sig"))
    if app["id"] != "com.ryanspice.skifree" or app["main"] != "skifree-native":
        raise ValueError("Unexpected app identity or executable.")
    return {"schema": 2, "public_stage": True, "original_artwork_included": True, "bundled_sprite_count": len(expected_assets),
            "original_executable_included": False, "version": app["version"],
            "files": [{"path": name, "bytes": path.stat().st_size,
                       "sha256": hashlib.sha256(path.read_bytes()).hexdigest()} for name, path in sorted(files.items())]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--stage", type=Path, required=True)
    parser.add_argument("--report", type=Path)
    args = parser.parse_args()
    report = audit(args.stage)
    if args.report:
        if args.report.resolve().is_relative_to(args.stage.resolve()):
            parser.error("Audit report must be outside the audited stage.")
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"PASS: free bundled stage contains {len(report['files'])} allowlisted files, 89 hash-verified sprites, and no original executable.")


if __name__ == "__main__":
    main()
