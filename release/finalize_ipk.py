#!/usr/bin/env python3
"""Normalize a locally packaged IPK's Unix permissions without changing file bytes.

The Windows ares packager can omit executable bits for extensionless ELF files.
Always verify the result with verify_ipk.py against the audited public stage.
"""
import argparse
import gzip
import io
from pathlib import Path
import tarfile

from verify_ipk import ar_members


def normalize_data(raw):
    output = io.BytesIO()
    main = "usr/palm/applications/com.ryanspice.skifree/skifree-native"
    state = "usr/palm/applications/com.ryanspice.skifree/state"
    found = False
    state_found = False
    with tarfile.open(fileobj=io.BytesIO(raw), mode="r:gz") as source:
        with tarfile.open(fileobj=output, mode="w", format=tarfile.USTAR_FORMAT) as target:
            for item in source:
                if not (item.isdir() or item.isfile()):
                    raise ValueError("Unexpected data archive entry")
                if item.size > 16 * 1024 * 1024:
                    raise ValueError("Unexpected data archive size")
                name = item.name.removeprefix("./")
                if name.startswith("/") or ".." in Path(name).parts:
                    raise ValueError("Unsafe archive path")
                item.uid = item.gid = item.mtime = 0
                item.uname = item.gname = "root"
                item.mode = 0o755 if item.isdir() or name == main else 0o644
                if name == state:
                    if not item.isdir():
                        raise ValueError("Runtime state path must be a directory")
                    item.mode = 0o1777
                    state_found = True
                if name == main:
                    if not item.isfile():
                        raise ValueError("Native executable is not a regular file")
                    found = True
                target.addfile(item, source.extractfile(item) if item.isfile() else None)
    if not found:
        raise ValueError("Native executable missing")
    if not state_found:
        raise ValueError("Runtime state directory missing")
    return gzip.compress(output.getvalue(), mtime=0)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        parser.error("Choose a new output path; no package will be overwritten")
    members = ar_members(args.input.read_bytes())
    members["data.tar.gz"] = normalize_data(members["data.tar.gz"])
    result = bytearray(b"!<arch>\n")
    for name in ("debian-binary", "control.tar.gz", "data.tar.gz"):
        data = members[name]
        header = f"{name + '/':<16}{0:<12}{0:<6}{0:<6}{'100644':<8}{len(data):<10}`\n".encode("ascii")
        if len(header) != 60:
            raise ValueError("Invalid ar output header")
        result.extend(header)
        result.extend(data)
        if len(data) % 2:
            result.extend(b"\n")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("xb") as stream:
        stream.write(result)
    print(f"Normalized native executable permissions: {args.output}")


if __name__ == "__main__":
    main()
