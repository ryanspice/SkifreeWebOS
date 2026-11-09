# Prepare the 1.0.0 tag

Proposed tag: **`v1.0.0`**. The source repository is public. An annotated tag
`v1.0.0` (object `42cac15`, peeled to commit `c51f237`) and a draft release
already exist; the draft is not published. The tag predates the final
documentation/tooling changes, so it must be reconciled with the exact release
commit before publication. Do not create a second `v1.0.0` tag.

The [release walkthrough](../docs/RELEASE-GUIDE.md) explains these steps in order,
including public hosting, generated Homebrew metadata and the listing PR.

## Release brief

SkiFree for webOS brings the reconstructed Original 1.04 game to LG TVs with
Magic Remote, mouse and keyboard controls. Separate Recreation and Yeti Chase
runs offer simplified skiing and a gentler pursuit. The release includes a
compact white Recreation HUD, hidden in-run footer and AAA-style initials for
qualifying Original scores, with legacy score preservation.

Use [1.0.0 release notes](RELEASE-NOTES-1.0.0.md) for the eventual release body,
[TV build brief](../docs/TV-BUILD.md) for packaging and
[verification](../native/docs/VERIFICATION.md) for evidence boundaries.

## Current package

- IPK: `com.ryanspice.skifree_1.0.0_arm.ipk`
- Bytes: **3,034,062**
- SHA-256: `b0801db115564c79c84c784e234d274ca2e8b8e074b40ce6fab5c51b3eae548f`
- Executable: `ed646651…` (byte-identical to the earlier C3-verified package)
- C3: installed and launched through Developer Mode on 2026-11-09.

The maintainer's private release handoff holds the IPK, source set, manifests and
receipts. They are not part of the public source archive until uploaded to the
release.

## Tag gates

- [x] Version 1.0.0 aligned in app metadata, CMake, intro and release tooling.
- [x] Host core/storage tests, scripted UI evidence and ARM package audit exist.
- [x] Independent AI review completed; its release-doc/source-boundary and
  verifier-permission findings are addressed with regression checks.
- [x] Rooted/unrooted and version research synthesized into a compatibility plan.
- [x] Release package installed and launched on the C3.
- [x] Storage-fix package: non-root file creation/replacement and preserved old
  score bytes verified; owner-confirmed non-empty records survived an update.
- [x] Owner recorded the free fan-release basis and the icon authorship. No
  upstream artwork/reconstruction license grant is claimed.
- [x] Draft release asset replaced with the audited package and current notices.
- [x] Owner reported that tested inputs and Original behavior are good.
- [ ] Independent human code/design review, or an explicit owner sign-off on
  credits and claims.
- [ ] Decide the Homebrew pool/eligibility and measured minimum webOS release.
- [ ] Reconcile the existing `v1.0.0` tag with the final release commit; publish
  the draft; regenerate the source set if any tracked file changes.
- [ ] Submit the completed package YAML and PR body to `webosbrew/apps-repo`.
- [ ] Send social posts only after the release and listing exist.

## Commands before publication

The tag already exists. Only run a tag update after the exact release commit is
frozen and after confirming that force-updating an unpublished tag is intended:

```sh
git status --short
git show --stat HEAD
git rev-parse HEAD
git tag -f -a v1.0.0 <release-commit> -m "SkiFree for webOS 1.0.0"
git push --force origin refs/tags/v1.0.0
```

Attach the audited ARM IPK, source ZIP/manifest, checksums and release notes to
the draft. Verify hosted bytes after upload. A tag alone does not establish TV
acceptance, all-version compatibility or a Homebrew listing. Keep social drafts
unsent until the release destination and community-specific copy are finalized.
