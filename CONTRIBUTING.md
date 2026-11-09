# Contributing

Start with the [overview](README.md) and [documentation map](docs/README.md).
Original follows a pinned SkiFree 1.04 reconstruction; Recreation and Yeti Chase
are separate experiences. Read [provenance](native/src/classic/PROVENANCE.md) and
[license boundaries](native/NOTICE.md) before changing or redistributing code/art.

## Build and test

Run from the repository root. Core tests need CMake 3.20 or newer and a C++20
compiler; they do not require SDL, a GPU or a TV:

```sh
cmake -S native -B .ai/evidence/core-build -DSKIFREE_BUILD_UI=OFF -DSKIFREE_STRICT=ON -DCMAKE_BUILD_TYPE=Release
cmake --build .ai/evidence/core-build --config Release --parallel 1
ctest --test-dir .ai/evidence/core-build -C Release --output-on-failure
python release/source_archive_tests.py
python release/ipk_permissions_tests.py
python native/tests/public_stage_tests.py
git diff --check
```

Use `python3` where that is the Python 3 command. Four CTest suites exercise
Original, Recreation, ranking and score storage. Python checks validate source
and package boundaries. For SDL/GLES desktop builds, sanitizer runs and scripted
UI tests, follow [native setup](native/README.md). ARM cross-compilation, runtime
staging and IPK auditing are in [release preparation](release/README.md).

The [host/source CI workflow](.github/workflows/checks.yml) runs the four core
suites, source/stage boundaries, IPK permission checks and source ZIP creation
on Linux. It does not build the SDL UI or ARM package, contact the TV, upload
artifacts or publish a release. A local equivalent pass does not establish that
the hosted workflow has run successfully.

The example uses a serial Release build. Debug and parallel builds are supported. If a
Windows build fails with disk-full or PDB/library write errors, see the
[Debug build notes](native/docs/DEBUG-BUILD.md). New work products stay under the
project's `.ai/`; external tool output requires ingestion and hash evidence.

## Making a change

1. Inspect the existing diff and preserve unrelated work. Avoid reformatting the
   reconstructed core alongside behavior fixes.
2. Explain the trigger and resulting behavior. For Original changes, cite
   reference evidence and identify deliberate platform adaptations.
3. Run relevant checks. Screens/input need native UI captures; storage needs
   migration, failure and existing-data preservation coverage.
4. Update controls, installation notes and verification claims as needed. Record
   exact artifact identity for packages and label checks that were not run.
5. Review the staged diff before committing. Keep generated products and private
   receipts out of Git. Never include credentials or the Windows EXE.

Use the bug-report template for reproducible issues. Include app version, run,
device/firmware, input, steps and sanitized errors. Do not attach private network
details or original executable/resource archives. Report vulnerabilities through
the private route in [SECURITY.md](SECURITY.md), not a public issue.

## Release boundaries

Desktop checks do not prove TV input, save survival, exhaustive EXE equivalence
or compatibility with all LG TVs. Device evidence on the C3 covers launch and
non-empty score persistence for the packages recorded in verification. Final
HUD/browser visual feedback and broader device acceptance remain pending. Current details live in
[verification](native/docs/VERIFICATION.md).

The [1.1.0 online-score plan](docs/ROADMAP-1.1.0.md) is separate from the current
offline 1.0.0 candidate. Keep planned features labelled until implemented and
tested; do not bump runtime versions merely to add planning documents.

Use fresh output names and exact hashes. GitHub source hosting does not publish
an IPK or submit to Homebrew. See the [release checklist](release/README.md#submission-checklist).
