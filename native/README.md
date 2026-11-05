# Native SkiFree for webOS — 1.0.0

C++20; original 1.04 reconstructed game core; SDL2 window/context/input; Spice Home raw GLES2 shaders. No browser or Rust runtime.

Desktop dependencies: CMake, a C++20 compiler, SDL2 headers/runtime, libpng and a working GLES2 driver. The SDL functions are loaded dynamically. The following Linux commands run from this `native` directory:

```sh
cmake -S . -B ../.ai/evidence/desktop -DSKIFREE_STRICT=ON
cmake --build ../.ai/evidence/desktop -j4
ctest --test-dir ../.ai/evidence/desktop --output-on-failure
../.ai/evidence/desktop/skifree-native --assets assets
```

On Windows, use a Visual Studio Developer PowerShell with CMake, Ninja and an existing vcpkg installation. `scripts/Build-Desktop.ps1 -VcpkgRoot <vcpkg-path> -Run` configures, builds, tests and launches the application. The SDL2 and PNG DLLs must be on the runtime search path. This helper does not install the toolchain or dependencies.

Native desktop UI checks require Python 3, Mesa, Xvfb and `xvfb-run` (including its `xauth` dependency). They use isolated score files and a software renderer; they do not control a connected TV:

```sh
python3 tests/classic_ui_qa.py --binary ../.ai/evidence/desktop/skifree-native --out ../.ai/evidence/classic-ui
python3 tests/public_stage_tests.py
```

Each UI case writes `frame.png`, `report.json`, its input script and a log below the output directory. Use a fresh output directory for each evidence run. Score-result and malformed-save cases are deliberate QA fixtures, not human-earned scores. The `--script` and `--fixed-frame` options are rejected by the webOS build.

Sanitized core checks:

```sh
cmake -S . -B ../.ai/evidence/sanitize -DSKIFREE_BUILD_UI=OFF -DSKIFREE_SANITIZE=ON
cmake --build ../.ai/evidence/sanitize -j4
UBSAN_OPTIONS=halt_on_error=1 ASAN_OPTIONS=detect_leaks=1 ctest --test-dir ../.ai/evidence/sanitize --output-on-failure
```

The reconstructed core uses fixed-width Win32 types and wrapping arithmetic. Strict warnings apply to the new platform/UI/test driver; the legacy reconstruction retains old C idioms. See `src/classic/PROVENANCE.md` for the exact source pins and adaptations.

For webOS, set `SKIFREE_WEBOS_ENV` to the installed SDK environment script, or `SKIFREE_WEBOS_TOOLCHAIN` to its verified ARM toolchain. Choose fresh `SKIFREE_WEBOS_BUILD_DIR` and `SKIFREE_WEBOS_STAGE_DIR` paths under the root `.ai/evidence/<task>/`, then run `scripts/build-webos.sh` with the actual SDL headers and target libpng paths. It refuses non-ARM output and existing stage directories. Follow `docs/WEBOS-RUNTIME.md` to stage and audit runtime libraries, then the root release instructions for IPK packaging and verification.

The application includes 89 original resources. F2 restarts; F3/Back pauses; F toggles fast mode; click/OK performs the original mouse jump; Insert/Space performs the smaller keyboard jump. The remote pause menu exposes fast mode. Read the root README for the full input mapping.

High scores use `scores-initials-v1.txt` in the SDL preference directory. When absent, the store imports a valid `scores-v104.txt` table with `---` initials, without writing to that old file or the prototype's `record.txt`. The new format records course, value and three uppercase initials per entry under a `skifree-initials-v1` header. Malformed named or legacy data and orphaned temporary files block writes and are preserved. Qualifying results start at `AAA`; confirmed or pending letters are persisted on save/exit. Older binaries do not read the new file.

On webOS the corrected package uses `state/user-<runtime-uid>/scores-initials-v1.txt`. The package gives only the `state/` parent sticky mode 01777; the non-root app creates and owns its child directory. Old installation-root named scores take precedence over numeric legacy scores during import, and remain untouched. Startup saves the validated table immediately so write/replace failure disables saving before play. Developer Mode launch, real file creation/replacement and unchanged legacy bytes are verified on the C3; a corrected-build earned-score run is pending. Non-empty round-trip/migration pass host tests. Non-empty TV upgrade, uninstall and UID-change survival remain unverified.

Remote colours use native SDL-webOS scancodes 486–489 plus a Red-keycode compatibility path and keyboard-state edge detection. F5–F8 mirror Red/Green/Yellow/Blue. The optional `src/recreation` implementation shares the same 89 textures and retains independent in-session state. No launch preference is persisted: Original remains the default.
