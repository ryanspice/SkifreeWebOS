# Prepare the 1.0.0 tag

Published tag: **`v1.0.0`**. The source repository and the GitHub release are
public, and the annotated tag points at the final 1.0.0 release commit.

For 1.0.0, the tag was refreshed once before the final assets were replaced
because it predated the final documentation cleanup. That refresh is recorded
here; future releases use a new version and tag.

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
- Bytes: **3,033,558**
- SHA-256: `b0e38b31545c153494795c0fcd7f98b42c37c2425d7dfe872c69984791df3588`
- Executable: `ed646651…` (byte-identical to the earlier C3-verified package)
- Minimum webOS: **11.2.0** (measured on the C3; no claim below)
- C3: installed and launched through Developer Mode on 2026-11-09.

The maintainer's private release handoff holds the IPK, source set, manifests and
receipts. They are not part of the public source archive until uploaded to the
release.

## Tag gates

- [x] Version 1.0.0 aligned in app metadata, CMake, intro and release tooling.
- [x] Host core/storage tests, scripted UI evidence and ARM package audit exist.
- [x] Release-doc/source-boundary and verifier-permission findings are
  addressed with regression checks.
- [x] Rooted/unrooted and version research synthesized into a compatibility plan.
- [x] Release package installed and launched on the C3.
- [x] Storage-fix package: non-root file creation/replacement and preserved old
  score bytes verified; owner-confirmed non-empty records survived an update.
- [x] Owner recorded the free fan-release basis. No upstream
  artwork/reconstruction license grant is claimed.
- [x] Release asset replaced with the audited package and current notices; release published.
- [x] Owner reported that tested inputs and Original behavior are good.
- [x] Owner reviewed and accepted the build, credits and claims.
- [ ] Confirm the Homebrew pool/eligibility.
- [x] Reconcile the existing `v1.0.0` tag with the final release commit; publish
  the release; regenerate the source set when tracked files change.
- [ ] Submit the completed package YAML and PR body to `webosbrew/apps-repo`.
- [ ] Send social posts only after the release and listing exist.

## Published state

The ARM IPK, source ZIP/manifest, checksums, release notes and package audit are
attached to the published GitHub release. The 1.0.0 tag was moved once to this
final documentation commit before the assets were replaced. A tag or successful
download does not establish all-version compatibility, a Homebrew listing or
maintainer acceptance. Keep social drafts unsent until the listing and
community-specific copy are finalized.
