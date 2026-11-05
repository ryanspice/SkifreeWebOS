#!/usr/bin/env python3
"""Read an IPK without extracting it and compare every app file to its audited stage."""
import argparse
import hashlib
import io
import json
from pathlib import Path
import tarfile


def ar_members(raw):
    if not raw.startswith(b"!<arch>\n"):
        raise ValueError("Not an ar/IPK archive")
    offset, members = 8, {}
    while offset < len(raw):
        header = raw[offset:offset + 60]
        if len(header) != 60 or header[58:] != b"`\n":
            raise ValueError("Invalid ar header")
        name = header[:16].decode("ascii").strip().rstrip("/")
        size = int(header[48:58])
        start, end = offset + 60, offset + 60 + size
        if size < 0 or end > len(raw) or name in members:
            raise ValueError("Invalid or duplicate ar member")
        members[name] = raw[start:end]
        offset = end + size % 2
    if set(members) != {"debian-binary", "control.tar.gz", "data.tar.gz"}:
        raise ValueError("Unexpected IPK members")
    return members


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ipk", type=Path, required=True)
    parser.add_argument("--stage-audit", type=Path, required=True)
    parser.add_argument("--report", type=Path, required=True)
    args = parser.parse_args()
    audit = json.loads(args.stage_audit.read_text(encoding="utf-8"))
    if not audit.get("public_stage") or audit.get("original_artwork_included") is not True or audit.get("bundled_sprite_count") != 89:
        raise ValueError("A successful public-stage audit is required")
    expected = {entry["path"]: entry["sha256"] for entry in audit["files"]}
    raw = args.ipk.read_bytes()
    members = ar_members(raw)
    prefix = "usr/palm/applications/com.ryanspice.skifree/"
    package_info = "usr/palm/packages/com.ryanspice.skifree/packageinfo.json"
    seen, records = set(), []
    state_directory_seen = False
    with tarfile.open(fileobj=io.BytesIO(members["data.tar.gz"]), mode="r:gz") as archive:
        for member in archive:
            if member.isdir():
                name = member.name.removeprefix("./").rstrip("/")
                if name == prefix.rstrip("/") + "/state":
                    if state_directory_seen or member.mode != 0o1777 or member.uid != 0 or member.gid != 0:
                        raise ValueError("Runtime state directory lacks audited sticky permissions")
                    state_directory_seen = True
                elif member.mode != 0o755 or member.uid != 0 or member.gid != 0:
                    raise ValueError("Unsafe packaged directory permissions/ownership: " + name)
                continue
            if not member.isfile() or member.size > 16 * 1024 * 1024:
                raise ValueError("Unexpected file type/size in package: " + member.name)
            name = member.name.removeprefix("./")
            expected_mode = 0o755 if name == prefix + "skifree-native" else 0o644
            if member.mode != expected_mode or member.uid != 0 or member.gid != 0:
                raise ValueError("Unsafe packaged file permissions/ownership: " + name)
            if name in seen:
                raise ValueError("Duplicate packaged path")
            seen.add(name)
            blob = archive.extractfile(member).read()
            digest = hashlib.sha256(blob).hexdigest()
            if name == prefix + "skifree-native" and member.mode & 0o111 != 0o111:
                raise ValueError("Native executable lacks executable permissions in IPK")
            if name == package_info:
                info = json.loads(blob)
                if info.get("id") != "com.ryanspice.skifree" or info.get("version") != audit["version"]:
                    raise ValueError("Package metadata identity/version mismatch")
            elif not name.startswith(prefix) or expected.get(name[len(prefix):]) != digest:
                raise ValueError("Unexpected or changed packaged content: " + name)
            records.append({"path": name, "bytes": len(blob), "sha256": digest})
    if not state_directory_seen:
        raise ValueError("Runtime state directory missing")
    if seen != {prefix + name for name in expected} | {package_info}:
        raise ValueError("Packaged files do not match the complete stage")
    report = {"ipk": args.ipk.name, "bytes": len(raw), "sha256": hashlib.sha256(raw).hexdigest(),
              "version": audit["version"], "original_artwork_included": True, "bundled_sprite_count": 89,
              "original_executable_included": False, "files": records}
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"PASS: {len(records)} packaged files match the audited bundled stage; SHA256 {report['sha256']}")


if __name__ == "__main__":
    main()
