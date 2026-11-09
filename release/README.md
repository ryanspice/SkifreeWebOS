# Release preparation

Version 1.0.0 is the free bundled release. The GitHub release is published; Homebrew submission and social posts remain drafts.

Follow the [release walkthrough](../docs/RELEASE-GUIDE.md) or open its
[visual tutorial](../docs/RELEASE-WALKTHROUGH.html) locally for the sequence from
main consolidation through GitHub hosting and a Homebrew submission.

## Build, package and verify

Build using `native/README.md`. Stage the executable, the explicit 89-sprite inventory, required app files, licenses and SDL/libpng runtimes. Run `native/scripts/audit_public_stage.py --stage <stage> --report <outside-stage-report.json>`.

Package that directory with `ares-package`. On Windows, write this first package to an ignored intermediate directory. Then run:

```powershell
python release/finalize_ipk.py --input $intermediateIpk --output $finalIpk
python release/verify_ipk.py --ipk $finalIpk --stage-audit $stageAudit --report $finalAudit
python release/source_archive_tests.py
python release/ipk_permissions_tests.py
python release/prepare_source_archive.py --out .ai/outbox/release-1.0.0/SkifreeWebOS-1.0.0-source.zip
```

The finalizer normalizes Unix modes, including execute permissions on the ELF, and sets real fixed archive timestamps (not 1970) so TV installers accept the package. The verifier compares every packaged app file against the audited stage. The source builder permits source files, explicitly listed documentation and policies, required app files, the 89 hash-verified sprites and two hash-verified documentation images. It excludes original EXE/ZIP files, raw resource dumps, known private/planning paths, unsent launch drafts, unapproved documentation text and generated release metadata. This path/asset gate is not a content-level secret scanner; review source contents before publication. Documentation images are not added to the IPK. Source ZIPs include the Git-visible tree, including allowed uncommitted files, and omit ignored build dependencies; pinned dependency preparation remains documented.

The builder refuses existing output files. Preserve previous archives and use a fresh revision name after changes. The ZIP, adjacent SHA-256 file and per-file manifest form one source-release set. A source-only documentation refresh does not change the audited IPK or its checksum.

The release package (SHA-256 `a1244a9c…`, 3,033,578 bytes) is attached to the
public 1.0.0 GitHub release. It is a Homebrew-compatible repack with real 2026
archive timestamps and unchanged file contents; it reuses the
C3-verified executable from the earlier `494726b3…` package and was installed and
launched on the C3 on 2026-11-09. The earlier package's actual non-root file
creation/replacement and owner-confirmed non-empty records are the storage
evidence. Broader TV migration checks remain pending. The finalizer must preserve
the sticky 01777 `state/` parent and set real archive timestamps; direct packaging
without these steps does not satisfy the install/storage contract. See the
[TV build brief](../docs/TV-BUILD.md),
[compatibility synthesis](../docs/COMPATIBILITY.md) and [tag preparation](TAGGING.md).

New work products use `.ai/inbox/`, `.ai/outbox/`, `.ai/evidence/` and `.ai/archive/`.
Reviews/plans are reconciled through `.ai/SYNTHESIS-QUEUE.json`. Earlier external
recovery/build products have been ingested with per-file hashes and originals
retained; immutable receipts may still contain their historical paths.

## Homebrew metadata

`Prepare-Homebrew.ps1` wraps the official Homebrew manifest generator and checks app identity, version, byte size and SHA-256. Supply the final IPK, public source/IPK/manifest/support URLs, observed root requirement and measured minimum webOS release. It writes the manifest, package YAML, checksum and receipt to a fresh output directory.

The script defaults to `-Pool non-free` because the original artwork is separate from the code's MIT license. Confirm the appropriate pool and redistribution terms against the current repository rules before submission; free pricing does not describe a source/artwork license. No license grant is fabricated by this tooling. Public URL and compatibility fields remain explicit inputs rather than plausible placeholders.

The website is https://ryanspice.com/ and the source repository is
https://github.com/ryanspice/SkifreeWebOS. The repository and the 1.0.0 GitHub
Release are public. No Homebrew listing has been published. The TV jail lacks
`/etc/starfish-release`; Homebrew firmware data maps the C3 `43.21.x` W23O build
to release `11.2.0`, and the tested TV reports SDK `11.2.0`. The measured minimum
is **11.2.0**; older releases were not tested.

## Submission checklist

- [x] Strict native builds, game/input checks and exact package audit.
- [x] Earlier candidate: install and native GPU title-screen launch on OLED48C3PUA, firmware 43.21.84.
- [x] Earlier candidates: owner input observations and empty-score update/relaunch evidence recorded.
- [x] Corrected storage IPK: Developer Mode install and native render on the C3, exact checksum recorded.
- [x] Corrected storage IPK: actual score-file creation/replacement as non-root UID 6252; legacy bytes preserved.
- [x] Owner-confirmed non-empty Slalom/Freestyle records reloaded and survived the UI package update unchanged.
- [x] Release package installed and launched on the C3; exact bytes and checksum recorded.
- [x] Owner recorded the free fan-release basis; no upstream license grant is claimed.
- [x] Release asset replaced with the audited package, current notices and refreshed source set.
- [x] Owner reported that tested inputs, Original behavior and score saving are good.
- [x] Published the GitHub release and verified the public bytes.
- [ ] Updated HUD/browser visual acceptance, broader controls/focus and migration routes.
- [x] Owner reviewed and tested the build.
- [x] Measured minimum webOS release: **11.2.0** on the C3; no claim below.
- [ ] Confirm the Homebrew pool/eligibility.
- [ ] Generate concrete metadata and verify its remote downloads and checksums.
- [ ] Submit the completed package YAML and PR body to webosbrew/apps-repo.

Current [Homebrew rules](https://www.webosbrew.org/develop/guides/publishing/rules/) require meaningful human development/review and real-TV evidence. [Submission instructions](https://www.webosbrew.org/develop/guides/publishing/how-to/) describe self-hosting the IPK/manifest and opening the package PR. Maintainer acceptance remains separate from local readiness.

## Support and promotion

The description links to the website. `.github/FUNDING.yml` uses the verified website as a custom support link. No donation account or paid campaign was created.

Unsent Reddit, LinkedIn and X drafts remain in the maintainer's local working copy and are excluded from the public source archive. Check each community's current rules before posting; obtain moderator approval where required. No post, moderator request, schedule or ad has been sent.
