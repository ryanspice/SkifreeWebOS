#!/usr/bin/env python3
"""Check public-source boundaries without building or touching a TV."""
import hashlib
import importlib.util
import json
from pathlib import Path
import unittest

SPEC = importlib.util.spec_from_file_location("source_archive", Path(__file__).with_name("prepare_source_archive.py"))
archive = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(archive)


class PublicSourceBoundaries(unittest.TestCase):
    def test_expected_source_and_docs(self):
        for name in ("README.md", "CONTRIBUTING.md", "AGENTS.md", "native/AGENTS.md", "release/AGENTS.md",
                     "docs/README.md", "docs/REPOSITORY-REVIEW.md", "docs/ROADMAP-1.1.0.md",
                     "docs/RELEASE-GUIDE.md", "docs/RELEASE-WALKTHROUGH.html",
                     ".github/workflows/checks.yml", "native/src/main.cpp", "native/include/skifree/scores.hpp",
                     "docs/TV-BUILD.md", "docs/COMPATIBILITY.md", "native/docs/DEBUG-BUILD.md", "release/TAGGING.md",
                     "docs/INSTALL.md", "docs/support.md", "release/RELEASE-NOTES-0.3.0.md",
                     "release/source_archive_tests.py", "release/ipk_permissions_tests.py", "native/state/README.md"):
            with self.subTest(name=name):
                self.assertTrue(archive.allowed(name))

    def test_local_material_is_never_public(self):
        for name in ("native/docs/PUBLISHING-PLAN-2026-11-04.md",
                     "native/docs/wayfinder/1.0.0/MAP.md", "docs/marketing/SOCIAL-DRAFTS.md",
                     "release/HOMEBREW-PR-DRAFT.md", "private/receipt.json",
                     ".ai/SYNTHESIS-QUEUE.json", ".ai/evidence/device.json",
                     ".ai/archive/original.md", ".ai/outbox/release/manifest.json"):
            with self.subTest(name=name):
                self.assertFalse(archive.allowed(name))

    def test_unapproved_documentation_text_is_rejected(self):
        for name in ("docs/credentials.json", "native/docs/private-receipt.txt",
                     "docs/install-receipt.md", "native/docs/local-settings.yml", "docs/private-page.html",
                     "native/src/private-page.html"):
            with self.subTest(name=name):
                self.assertFalse(archive.allowed(name))

    def test_generated_release_outputs_rejected(self):
        for name in ("release/PREPARATION-RECEIPT.json", "release/manifest.json",
                     "release/com.ryanspice.skifree.yml", "release/SHA256SUMS.txt",
                     "release/output/manifest.json"):
            with self.subTest(name=name):
                self.assertFalse(archive.allowed(name))

    def test_unknown_binary_or_unsafe_path_rejected(self):
        for name in ("", "../README.md", "/README.md", "C:/README.md", "docs\\README.md",
                     "docs/media/extra.png", "native/assets/extra.png", "ski32.exe",
                     "docs/original.zip", "docs/credentials.key"):
            with self.subTest(name=name):
                self.assertFalse(archive.allowed(name))

    def test_documentation_media_inventory(self):
        manifest = json.loads((archive.ROOT / "docs/media/manifest.json").read_text(encoding="utf-8"))
        self.assertEqual(len(manifest["files"]), 3)
        self.assertEqual({item["path"] for item in manifest["files"]}, archive.PUBLIC_MEDIA)
        for item in manifest["files"]:
            with self.subTest(name=item["path"]):
                self.assertTrue(archive.allowed(item["path"]))
                blob = (archive.ROOT / item["path"]).read_bytes()
                self.assertEqual(hashlib.sha256(blob).hexdigest(), item["sha256"])
                self.assertTrue(blob.startswith(b"\x89PNG\r\n\x1a\n"))


if __name__ == "__main__":
    unittest.main()
