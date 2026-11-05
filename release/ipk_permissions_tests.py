#!/usr/bin/env python3
"""Check the non-root state-directory contract independently of the TV."""
import io
import contextlib
import hashlib
import json
from pathlib import Path
import sys
import tarfile
import tempfile
import unittest
from unittest.mock import patch

from finalize_ipk import normalize_data
import verify_ipk

APP = "usr/palm/applications/com.ryanspice.skifree"
PACKAGE = "usr/palm/packages/com.ryanspice.skifree/packageinfo.json"


def fixture(state=True, state_is_file=False, unsafe=False):
    output = io.BytesIO()
    with tarfile.open(fileobj=output, mode="w:gz") as archive:
        entries = [(APP, None), (APP + "/assets", None), (APP + "/skifree-native", b"ELF"),
                   (APP + "/assets/sprite.png", b"sprite"),
                   (PACKAGE, b'{"id":"com.ryanspice.skifree","version":"1.0.0"}')]
        if state:
            entries.append((APP + "/state", b"wrong" if state_is_file else None))
        if unsafe:
            entries.append(("../outside", b"bad"))
        for name, data in entries:
            item = tarfile.TarInfo(name)
            item.mode = 0o777
            item.uid = item.gid = 1234
            if data is None:
                item.type = tarfile.DIRTYPE
            else:
                item.size = len(data)
            archive.addfile(item, None if data is None else io.BytesIO(data))
    return output.getvalue()


class Permissions(unittest.TestCase):
    def verify_fixture(self, path=None, mode=None, uid=None):
        raw = normalize_data(fixture())
        changed = io.BytesIO()
        with tarfile.open(fileobj=io.BytesIO(raw), mode="r:gz") as source:
            with tarfile.open(fileobj=changed, mode="w:gz") as target:
                for item in source:
                    if item.name == path:
                        if mode is not None:
                            item.mode = mode
                        if uid is not None:
                            item.uid = uid
                    target.addfile(item, source.extractfile(item) if item.isfile() else None)
        ipk = bytearray(b"!<arch>\n")
        for name, blob in (("debian-binary", b"2.0\n"), ("control.tar.gz", b"fixture"),
                           ("data.tar.gz", changed.getvalue())):
            ipk.extend(f"{name + '/':<16}{0:<12}{0:<6}{0:<6}{'100644':<8}{len(blob):<10}`\n".encode("ascii"))
            ipk.extend(blob)
            if len(blob) % 2:
                ipk.extend(b"\n")
        scratch = Path(__file__).resolve().parents[1] / ".ai/evidence/ipk-permission-tests"
        scratch.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=scratch) as directory:
            folder = Path(directory)
            (folder / "fixture.ipk").write_bytes(ipk)
            (folder / "stage.json").write_text(json.dumps({"public_stage": True,
                "original_artwork_included": True, "bundled_sprite_count": 89, "version": "1.0.0",
                "files": [{"path": name, "sha256": hashlib.sha256(blob).hexdigest()}
                          for name, blob in (("skifree-native", b"ELF"), ("assets/sprite.png", b"sprite"))]}))
            with patch.object(sys, "argv", ["verify_ipk.py", "--ipk", str(folder / "fixture.ipk"),
                    "--stage-audit", str(folder / "stage.json"), "--report", str(folder / "report.json")]):
                with contextlib.redirect_stdout(io.StringIO()):
                    verify_ipk.main()
            return json.loads((folder / "report.json").read_text())

    def test_only_state_parent_is_sticky_writable(self):
        with tarfile.open(fileobj=io.BytesIO(normalize_data(fixture())), mode="r:gz") as archive:
            for item in archive:
                with self.subTest(path=item.name):
                    self.assertEqual((item.uid, item.gid), (0, 0))
                    self.assertEqual(item.mode, 0o1777 if item.name == APP + "/state" else
                                     0o755 if item.isdir() or item.name.endswith("/skifree-native") else 0o644)
            self.assertEqual(archive.extractfile(APP + "/assets/sprite.png").read(), b"sprite")

    def test_missing_state_directory_rejected(self):
        with self.assertRaisesRegex(ValueError, "state directory missing"):
            normalize_data(fixture(state=False))

    def test_state_file_rejected(self):
        with self.assertRaisesRegex(ValueError, "must be a directory"):
            normalize_data(fixture(state_is_file=True))

    def test_unsafe_path_rejected(self):
        with self.assertRaisesRegex(ValueError, "Unsafe"):
            normalize_data(fixture(unsafe=True))

    def test_verifier_accepts_normalized_fixture(self):
        self.assertEqual(len(self.verify_fixture()["files"]), 3)

    def test_verifier_rejects_permission_only_tampering(self):
        # File bytes and stage hashes stay identical: only metadata changes.
        cases = [(APP + "/skifree-native", 0o777, None),
                 (APP + "/skifree-native", None, 42),
                 (APP + "/assets/sprite.png", 0o666, None),
                 (APP + "/assets", 0o775, None),
                 (APP + "/assets", None, 42),
                 (APP + "/state", 0o755, None)]
        for path, mode, uid in cases:
            with self.subTest(path=path, mode=mode, uid=uid):
                with self.assertRaisesRegex(ValueError, "permissions"):
                    self.verify_fixture(path, mode, uid)


if __name__ == "__main__":
    unittest.main()
