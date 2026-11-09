# TV build brief

Build target: **LG C3 / ARM32**, application `com.ryanspice.skifree`, candidate
version **1.0.0**. This package includes Original, Recreation, Yeti Chase and
three-letter score entry. Build/package evidence does not replace its C3 run.

Release package: `com.ryanspice.skifree_1.0.0_arm.ipk`,
**3,033,540 bytes**, SHA-256
`b2aae0695f7818dfc939ee04a96373045e9e0a22a6e88f12f18bfed5b36a7450`.
It reuses the C3-verified executable (`ed646651…`) from the November 5 build
with the current notices. The exact package was installed and
launched on the C3 on November 9, 2026. The earlier package's non-root score-file
evidence remains the storage evidence.

1.0.0 is a single-TV release: the owner's C3 is the only tested configuration.
Other models and older webOS versions are theoretical targets; no backward
compatibility is claimed until a later release has its own device evidence.

## Build inputs

- C++20, CMake, Ninja and the verified ARM webOS SDK environment/toolchain.
- SDL-webOS 2.32.10 headers/runtime and libpng 1.6.43 built for the same target.
- The manifest's 89 sprites, required app files and attribution/license files.
- The webOS CLI packager plus Python 3 for finalization and exact package audit.

See [native setup](../native/README.md) and
[runtime staging](../native/docs/WEBOS-RUNTIME.md). Choose fresh build/stage/output
paths under `.ai/evidence/<task>/`, and release deliverables under `.ai/outbox/`.

## Build and package sequence

1. Set `SKIFREE_WEBOS_ENV` to the SDK environment script (or provide the verified
   `SKIFREE_WEBOS_TOOLCHAIN`), plus fresh `SKIFREE_WEBOS_BUILD_DIR` and
   `SKIFREE_WEBOS_STAGE_DIR` paths.
2. Run `native/scripts/build-webos.sh`, supplying the target SDL headers and PNG
   library/include paths. It builds Release and rejects non-ARM output.
3. Stage SDL/PNG using `native/scripts/stage-webos-runtime.py`; audit the runtime
   against the exact SDK sysroot and run `native/scripts/audit_public_stage.py`.
4. Package the audited stage with `ares-package`. Normalize Unix file modes with
   `release/finalize_ipk.py`, then run `release/verify_ipk.py` against the exact
   stage audit. The finalizer also gives the packaged `state/` parent sticky
   mode 01777, enabling a per-user writable child without changing executable or
   asset permissions. Keep the SHA-256, byte size and audit reports together.
5. Follow [installation](INSTALL.md). Launch through the application manager.

The package must retain executable permissions and contain exactly the audited
stage plus package metadata. Do not copy the SDK's libc, loader or graphics
libraries into it. [Release preparation](../release/README.md) provides the
finalizer/verifier commands; each Python helper supports `--help`.

## C3 acceptance brief

Record package checksum and installation route. Verify Original opens by default,
all four colours work, pointer/directions/OK/Back behave correctly, and focus loss
pauses until resumed. Finish a course, enter initials, close/reopen, then use
**Back → High Scores** to check its saved table. Check that the opening HUD is
hidden and live statistics plus the correct course’s saved BEST appear while skiing. Verify legacy import with isolated fixtures
or a protected copy; preserve the owner's actual scores.

The current package requires GLIBC 2.38 and GLIBCXX 3.4.32. Rooted/unrooted routes
and older webOS versions need their own evidence; see the
[compatibility plan](COMPATIBILITY.md). Package identity and local build results
are recorded in [verification](../native/docs/VERIFICATION.md).
