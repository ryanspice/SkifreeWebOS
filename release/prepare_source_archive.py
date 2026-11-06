#!/usr/bin/env python3
"""Create a deterministic allowlisted source archive; never upload anything."""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path, PurePosixPath
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[1]
ROOT_FILES = {"README.md", "CONTRIBUTING.md", "AGENTS.md", "LICENSE", "NOTICE.md", ".gitignore", ".gitattributes"}
LOCAL_FILES = {"native/docs/PUBLISHING-PLAN-2026-11-04.md",
               "release/HOMEBREW-PR-DRAFT.md"}
LOCAL_PREFIXES = (".ai/", "private/", "native/docs/wayfinder/", "docs/marketing/")
PUBLIC_MEDIA = {"docs/media/skifree-original-c3.png", "docs/media/skifree-recreation-desktop.png",
                "docs/media/champions-development-preview.png"}
# New release outputs belong in ignored project-local .ai/ directories.
RELEASE_FILES = {"README.md", "AGENTS.md", "TAGGING.md", "Prepare-Homebrew.ps1", "homebrew-package.yml.in",
                 "finalize_ipk.py", "verify_ipk.py", "prepare_source_archive.py",
                 "source_archive_tests.py", "ipk_permissions_tests.py", "RELEASE-NOTES-1.0.0.md",
                 "RELEASE-NOTES-0.3.0.md", "RELEASE-NOTES-0.4.0.md", "RELEASE-NOTES-0.5.0.md",
                 "RELEASE-NOTES-0.6.0.md", "RELEASE-NOTES-0.7.0.md", "RELEASE-NOTES-0.8.0.md"}
NATIVE_FILES = {"CMakeLists.txt", "appinfo.json", "LICENSE", "NOTICE.md", "README.md", "AGENTS.md",
                "icon.png", "remote-scancodes.example.txt"}
TEXT_SUFFIXES = {".md", ".txt", ".py", ".ps1", ".sh", ".json", ".yml", ".yaml",
                 ".in", ".cpp", ".hpp", ".h", ".c", ".svg"}
PUBLIC_DOCS = {"docs/INSTALL.md", "docs/README.md", "docs/REPOSITORY-REVIEW.md",
               "docs/ROADMAP-1.1.0.md",
               "docs/TV-BUILD.md", "docs/COMPATIBILITY.md", "docs/support.md",
               "docs/media/README.md", "docs/media/manifest.json",
               "native/docs/ASSETS.md", "native/docs/CPP-SUBSET.md",
               "native/docs/ORIGINAL-COMPARISON.md", "native/docs/ORIGINAL-PARITY.md",
               "native/docs/VERIFICATION.md", "native/docs/VISUAL-REVIEW-1.0.0.md",
               "native/docs/WEBOS-RUNTIME.md", "native/docs/DEBUG-BUILD.md"}


def allowed(name):
    path = PurePosixPath(name)
    if not name or "\\" in name or ":" in name or path.is_absolute() or ".." in path.parts:
        return False
    if name in LOCAL_FILES or name.startswith(LOCAL_PREFIXES):
        return False
    if name in PUBLIC_MEDIA:
        return True
    if name in ROOT_FILES:
        return True
    if len(path.parts) == 2 and path.parts[0] == "native" and path.parts[1] in NATIVE_FILES:
        return True
    if name in {"native/assets/README.md", "native/assets/manifest.json", "native/state/README.md"}:
        return True
    if len(path.parts) == 3 and path.parts[:2] == ("native", "assets") and path.suffix == ".png":
        manifest = json.loads((ROOT / "native/assets/manifest.json").read_text(encoding="utf-8-sig"))
        return path.name in {item["name"] for item in manifest["files"]}
    if path.suffix not in TEXT_SUFFIXES:
        return False
    if path.parts[0] == "release":
        return len(path.parts) == 2 and path.name in RELEASE_FILES
    if path.parts[0] == "docs" or name.startswith("native/docs/"):
        return name in PUBLIC_DOCS
    if path.parts[0] == ".github":
        return True
    return (len(path.parts) >= 3 and path.parts[0] == "native" and
            path.parts[1] in {"src", "include", "compat", "scripts", "tools", "tests", "docs"})


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, required=True, help="New ZIP under ignored .ai/outbox")
    args = parser.parse_args()
    target = args.out.resolve()
    output_root = (ROOT / ".ai" / "outbox").resolve()
    if not output_root.is_relative_to(ROOT) or not target.is_relative_to(output_root) or target.suffix.lower() != ".zip":
        parser.error("--out must be a .zip inside this project's .ai/outbox")
    receipts = [target, target.with_suffix(".zip.sha256"), target.with_suffix(".files.json")]
    if any(p.exists() for p in receipts):
        parser.error("Choose a fresh output name; an archive or receipt already exists")
    git_root = subprocess.check_output(["git", "-C", str(ROOT), "rev-parse", "--show-toplevel"], text=True).strip()
    if Path(git_root).resolve() != ROOT:
        raise RuntimeError("Unexpected Git root")
    names = subprocess.check_output(["git", "-C", str(ROOT), "ls-files", "--cached", "--others",
                                     "--exclude-standard", "-z"]).decode("utf-8").split("\0")
    names = sorted(set(n for n in names if n))
    unexpected = [n for n in names if not allowed(n)]
    if unexpected:
        raise RuntimeError("Refusing unexpected public paths: " + ", ".join(unexpected))
    required = {"README.md", "LICENSE", "native/CMakeLists.txt", "native/appinfo.json",
                "native/src/main.cpp", "native/icon.png", "native/NOTICE.md", "native/tools/generate_icon.py"}
    if missing := required - set(names):
        raise RuntimeError("Required source files missing from Git-visible tree: " + ", ".join(sorted(missing)))
    appinfo = json.loads((ROOT / "native" / "appinfo.json").read_text(encoding="utf-8-sig"))
    if appinfo.get("id") != "com.ryanspice.skifree" or appinfo.get("version") != "1.0.0":
        raise RuntimeError("Expected native SkiFreeWebOS 1.0.0 source")
    asset_manifest = json.loads((ROOT / "native/assets/manifest.json").read_text(encoding="utf-8-sig"))
    asset_hashes = {"native/assets/" + item["name"]: item["sha256"] for item in asset_manifest["files"]}
    if len(asset_hashes) != 89 or not set(asset_hashes).issubset(names):
        raise RuntimeError("The complete 89-sprite inventory is required")
    media_manifest = json.loads((ROOT / "docs/media/manifest.json").read_text(encoding="utf-8"))
    media_hashes = {item["path"]: item["sha256"] for item in media_manifest["files"]}
    if len(media_manifest["files"]) != len(PUBLIC_MEDIA) or set(media_hashes) != PUBLIC_MEDIA or not PUBLIC_MEDIA.issubset(names):
        raise RuntimeError("The explicit documentation media inventory is required")
    blobs = []
    for name in names:
        source = ROOT / name
        components = [source, *list(source.parents)[:len(PurePosixPath(name).parts) - 1]]
        redirected = any(p.is_symlink() or getattr(p, "is_junction", lambda: False)() for p in components)
        if redirected or not source.is_file() or not source.resolve().is_relative_to(ROOT):
            raise RuntimeError("Refusing symlink, missing or non-file path: " + name)
        if source.stat().st_size > 2 * 1024 * 1024:
            raise RuntimeError("Unexpectedly large source file: " + name)
        blob = source.read_bytes()
        if name in asset_hashes:
            if hashlib.sha256(blob).hexdigest() != asset_hashes[name]:
                raise RuntimeError("Sprite differs from its attributed manifest: " + name)
        elif name in media_hashes:
            if hashlib.sha256(blob).hexdigest() != media_hashes[name] or not blob.startswith(b"\x89PNG\r\n\x1a\n"):
                raise RuntimeError("Documentation image differs from its manifest: " + name)
        elif name != "native/icon.png":
            blob.decode("utf-8-sig")  # All other allowlisted content must actually be text.
            if b"\x00" in blob:
                raise RuntimeError("Binary content in text path: " + name)
        blobs.append((name, blob))
    icon_spec = importlib.util.spec_from_file_location("original_icon", ROOT / "native/tools/generate_icon.py")
    icon_generator = importlib.util.module_from_spec(icon_spec)
    icon_spec.loader.exec_module(icon_generator)
    if dict(blobs)["native/icon.png"] != icon_generator.icon_bytes():
        raise RuntimeError("Icon differs from the project's original procedural icon")
    target.parent.mkdir(parents=True, exist_ok=True)
    prefix = "SkifreeWebOS-1.0.0/"
    with zipfile.ZipFile(target, "x", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for name, blob in blobs:
            info = zipfile.ZipInfo(prefix + name, date_time=(1980, 1, 1, 0, 0, 0))
            info.create_system = 3
            info.external_attr = (0o100755 if name.endswith(".sh") else 0o100644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            archive.writestr(info, blob, compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)
    digest = hashlib.sha256(target.read_bytes()).hexdigest()
    receipts[1].write_text(f"{digest}  {target.name}\n", encoding="utf-8")
    report = {"archive": target.name, "sha256": digest, "bytes": target.stat().st_size,
              "version": "1.0.0", "published": False, "bundled_sprite_count": 89, "files": [
                  {"path": name, "bytes": len(blob), "sha256": hashlib.sha256(blob).hexdigest()}
                  for name, blob in blobs]}
    receipts[2].write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"archive": str(target), "sha256": digest, "files": len(blobs), "published": False}))


if __name__ == "__main__":
    main()
