# Release preparation

Version 1.0.0 is a free bundled candidate. This folder prepares artifacts and drafts; it does not publish or send anything.

## Build, package and verify

Build using `native/README.md`. Stage the executable, the explicit 89-sprite inventory, icon, licenses and required SDL/libpng runtimes. Run `native/scripts/audit_public_stage.py --stage <stage> --report <outside-stage-report.json>`.

Package that directory with `ares-package`. On Windows, write this first package to an ignored intermediate directory. Then run:

```powershell
python release/finalize_ipk.py --input $intermediateIpk --output $finalIpk
python release/verify_ipk.py --ipk $finalIpk --stage-audit $stageAudit --report $finalAudit
python release/source_archive_tests.py
python release/ipk_permissions_tests.py
python release/prepare_source_archive.py --out .ai/outbox/release-1.0.0/SkifreeWebOS-1.0.0-source.zip
```

The finalizer normalizes Unix modes, including execute permissions on the ELF. The verifier compares every packaged app file against the audited stage. The source builder permits source files, explicitly listed documentation, the original procedural icon, the 89 hash-verified sprites and three hash-verified documentation images. It excludes original EXE/ZIP files, raw resource dumps, known private/planning paths, unsent launch drafts, unapproved documentation text and generated release metadata. This path/asset gate is not a content-level secret scanner; review source contents before publication. Documentation images are not added to the IPK. Source ZIPs include the Git-visible tree, including allowed uncommitted files, and omit ignored build dependencies; pinned dependency preparation remains documented.

The builder refuses existing output files. Preserve previous archives and use a fresh revision name after changes. The ZIP, adjacent SHA-256 file and per-file manifest form one source-release set. A source-only documentation refresh does not change the audited IPK or its checksum.

The corrected C3 storage build is in `.ai/outbox/storage-fix-1.0.0/`. It retains
version 1.0.0 while the release is unpublished; use the installation guide's exact
checksum. Its Developer Mode launch and actual non-root file creation/replacement
are verified. A corrected-build earned-score playthrough and broader TV migration
checks remain pending. The finalizer must preserve the sticky 01777 `state/`
parent; direct packaging without this step does not satisfy the storage contract.
See the [TV build brief](../docs/TV-BUILD.md), [compatibility synthesis](../docs/COMPATIBILITY.md)
and [tag preparation](TAGGING.md).

New work products use `.ai/inbox/`, `.ai/outbox/`, `.ai/evidence/` and `.ai/archive/`.
Reviews/plans are reconciled through `.ai/SYNTHESIS-QUEUE.json`. Earlier external
recovery/build products have been ingested with per-file hashes and originals
retained; immutable receipts may still contain their historical paths.

## Homebrew metadata

`Prepare-Homebrew.ps1` wraps the official Homebrew manifest generator and checks app identity, version, byte size and SHA-256. Supply the final IPK, real public source/icon/IPK/manifest/support URLs, observed root requirement and minimum tested numeric webOS release. It produces the manifest, package YAML, checksum and receipt in a fresh output directory.

The script defaults to `-Pool non-free` because the original artwork is separate from the code's MIT license. Confirm the appropriate pool and redistribution terms against the current repository rules before submission; free pricing does not describe a source/artwork license. No license grant is fabricated by this tooling. Public URL and compatibility fields remain explicit inputs rather than plausible placeholders.

The website is https://ryanspice.com/ and the source handoff repository is
https://github.com/ryanspice/SkifreeWebOS. The repository is private; it is not a
public Homebrew source or download URL. No GitHub Release or public IPK has been
published. The TV jail lacks `/etc/starfish-release`; the observed SDK version
alone is not used to fill minimum-release metadata.

## Submission checklist

- [x] Strict native builds, game/input checks and exact package audit.
- [x] Earlier candidate: install and native GPU title-screen launch on OLED48C3PUA, firmware 43.21.84.
- [x] Earlier candidates: owner input observations and empty-score update/relaunch evidence recorded.
- [x] Corrected storage IPK: Developer Mode install and native render on the C3, exact checksum recorded.
- [x] Corrected storage IPK: actual score-file creation/replacement as non-root UID 6252; legacy bytes preserved.
- [ ] Corrected storage IPK: earned score/initials after relaunch, broader controls/focus and non-empty migration.
- [ ] Complete publication support claims only for specifically evidenced device/input/lifecycle/save cases.
- [ ] Meaningful human code review, design explanation and honest AI disclosure.
- [ ] Record original artwork and reconstruction redistribution terms; confirm repository pool.
- [ ] Choose and publish the authorized public repository/release URLs.
- [ ] Generate concrete metadata and verify its remote downloads and checksums.
- [ ] Submit the completed package YAML and PR body to webosbrew/apps-repo.

Current [Homebrew rules](https://www.webosbrew.org/develop/guides/publishing/rules/) require meaningful human development/review, accurate AI disclosure and real-TV evidence. [Submission instructions](https://www.webosbrew.org/develop/guides/publishing/how-to/) describe self-hosting the IPK/manifest and opening the package PR. Maintainer acceptance remains separate from local readiness.

## Support and promotion

The description links to the website and a public copy of `docs/support.md`, which introduces Champions of Snowboarding 2026. `.github/FUNDING.yml` uses the verified website as a custom support link. No donation account or paid campaign was created.

Unsent Reddit, LinkedIn and X drafts remain in the maintainer's local working copy and are excluded from the public source archive. Complete the release destination and check each community's current rules before posting; obtain moderator approval where required. No post, moderator request, schedule or ad has been sent.
