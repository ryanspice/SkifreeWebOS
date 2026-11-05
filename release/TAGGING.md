# Prepare the 1.0.0 tag

Proposed tag: **`v1.0.0`**. The current repository is private. Preparing this
checklist does not create a Git tag, publish a GitHub Release or submit Homebrew.

## Release brief

SkiFree for webOS brings the reconstructed Original 1.04 game to LG TVs with
Magic Remote, mouse and keyboard controls. Separate Recreation and Yeti Chase
runs offer simplified skiing and a gentler pursuit. The candidate includes a
compact white Recreation HUD, hidden in-run footer and AAA-style initials for
qualifying Original scores, with legacy score preservation.

Use [1.0.0 release notes](RELEASE-NOTES-1.0.0.md) for the eventual release body,
[TV build brief](../docs/TV-BUILD.md) for packaging and
[verification](../native/docs/VERIFICATION.md) for evidence boundaries.

## Tag gates

- [x] Version 1.0.0 aligned in app metadata, CMake, intro and release tooling.
- [x] Host core/storage tests, scripted UI evidence and ARM package audit exist.
- [x] Requested Terra peer review completed; its two release-doc/source-boundary
  findings are addressed in the preparation changes and regression checks.
- [x] Rooted/unrooted and version research synthesized into a compatibility plan.
- [ ] Install and accept the exact latest IPK on the C3.
- [ ] Confirm physical initials controls and a human-earned score after relaunch;
  retain the distinction from host fixtures and earlier empty-table evidence.
- [ ] Final owner/human review of code, original-game quality, credits and claims.
- [ ] Decide private tag versus public release. Complete rights and hosting fields
  before public redistribution/Homebrew submission.
- [ ] Commit final source, regenerate a fresh source ZIP and checksum set, and
  record exact commit, binary/IPK/source identities.

## Commands after acceptance

Review the exact commit and ensure the tag is unused before running:

```sh
git status --short
git show --stat HEAD
git tag --list v1.0.0
git tag -a v1.0.0 -m "SkiFree for webOS 1.0.0"
git push origin v1.0.0
```

Attach the audited ARM IPK, SHA256SUMS and source ZIP/manifest to the eventual
release. Verify hosted bytes after upload. A tag alone does not establish TV
acceptance, all-version compatibility or a Homebrew listing. Keep social drafts
unsent until the release destination and community-specific copy are finalized.
