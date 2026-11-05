#!/usr/bin/env python3
"""Check the exact bundled sprite inventory and reject altered or extra payloads."""
import importlib.util
from pathlib import Path
import tempfile
import shutil


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main():
    native = Path(__file__).resolve().parents[1]
    audit = load("stage_audit", native / "scripts/audit_public_stage.py")
    icon = load("icon_generator", native / "tools/generate_icon.py")
    with tempfile.TemporaryDirectory(prefix="skifree-public-stage-test-") as directory:
        stage = Path(directory)
        for name in audit.REQUIRED:
            (stage / name).parent.mkdir(parents=True, exist_ok=True)
            (stage / name).write_bytes(b"test")
        shutil.copytree(native / "assets", stage / "assets", dirs_exist_ok=True)
        (stage / "skifree-native").write_bytes(b"\x7fELFfixture")
        (stage / "appinfo.json").write_text('{"id":"com.ryanspice.skifree","main":"skifree-native","version":"test"}')
        (stage / "icon.png").write_bytes(icon.icon_bytes())
        audit.audit(stage)
        count = 1
        for name in ("assets/extra.png", "ski32.exe", "private/reference.zip", "unexpected.txt"):
            extra = stage / name
            extra.parent.mkdir(parents=True, exist_ok=True)
            extra.write_bytes(b"unexpected private bytes")
            try:
                audit.audit(stage)
            except ValueError:
                count += 1
            else:
                raise AssertionError(f"Accepted {name}")
            extra.unlink()
        sprite = stage / "assets/ski32_001.png"
        original = sprite.read_bytes()
        sprite.write_bytes(b"altered sprite")
        try:
            audit.audit(stage)
        except ValueError:
            count += 1
        else:
            raise AssertionError("Altered sprite accepted")
        sprite.write_bytes(original)
        (stage / "icon.png").write_bytes(b"foreign artwork")
        try:
            audit.audit(stage)
        except ValueError:
            count += 1
        else:
            raise AssertionError("Foreign icon accepted")
    print(f"PASS: {count} public-stage boundary checks")


if __name__ == "__main__":
    main()
