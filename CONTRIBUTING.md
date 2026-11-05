# Contributing

Start with the [overview](README.md) and [documentation map](docs/README.md).
Original follows a pinned SkiFree 1.04 reconstruction; Recreation and Yeti Chase
are separate experiences. Read [provenance](native/src/classic/PROVENANCE.md) and
[license boundaries](native/NOTICE.md) before changing or redistributing code/art.

## Build and test

Run from the repository root. Core tests need CMake 3.20 or newer and a C++20
compiler; they do not require SDL, a GPU or a TV:

```sh
cmake -S native -B native/build/core -DSKIFREE_BUILD_UI=OFF -DSKIFREE_STRICT=ON -DCMAKE_BUILD_TYPE=Release
cmake --build native/build/core --config Release --parallel 1
ctest --test-dir native/build/core -C Release --output-on-failure
python release/source_archive_tests.py
python native/tests/public_stage_tests.py
git diff --check
```

Use `python3` where that is the Python 3 command. Four CTest suites exercise
Original, Recreation, ranking and score storage. Python checks validate source
and package boundaries. For SDL/GLES desktop builds, sanitizer runs and scripted
UI tests, follow [native setup](native/README.md). ARM cross-compilation, runtime
staging and IPK auditing are in [release preparation](release/README.md).

The example uses a serial Release build. Debug and parallel builds are supported:
the earlier PDB/library write failures were investigated and B: was found to have
zero free bytes. A fresh parallel MSVC Debug build on C: passed all four suites
with unchanged compiler settings. Choose a build directory on a volume with
enough free space; see the [diagnostic record](native/docs/DEBUG-BUILD.md).

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
details or original executable/resource archives.

## Release boundaries

Desktop checks do not prove TV input, save survival, exhaustive EXE equivalence
or compatibility with all LG TVs. The latest HUD/initials revision has host and
ARM evidence; physical-TV acceptance remains pending. Current details live in
[verification](native/docs/VERIFICATION.md).

Use fresh output names and exact hashes. GitHub source hosting does not publish
an IPK or submit to Homebrew. See the [release checklist](release/README.md#submission-checklist).
