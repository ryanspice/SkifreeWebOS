# Native SkiFree for webOS — 1.0.0

C++20; original 1.04 reconstructed game core; SDL2 window/context/input; Spice Home raw GLES2 shaders. No browser or Rust runtime.

Desktop dependencies: CMake, a C++20 compiler, SDL2 headers/runtime and libpng. The SDL functions are loaded dynamically. From this directory:

```sh
cmake -S . -B build/desktop -DSKIFREE_STRICT=ON
cmake --build build/desktop -j4
ctest --test-dir build/desktop --output-on-failure
build/desktop/skifree-native --assets assets
```

Native desktop UI checks (Linux/Mesa/Xvfb):

```sh
python3 tests/classic_ui_qa.py --binary build/desktop/skifree-native --out evidence/classic-ui
python3 tests/public_stage_tests.py
```

Sanitized core checks:

```sh
cmake -S . -B build/sanitize -DSKIFREE_BUILD_UI=OFF -DSKIFREE_SANITIZE=ON
cmake --build build/sanitize -j4
UBSAN_OPTIONS=halt_on_error=1 ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build/sanitize --output-on-failure
```

The reconstructed core uses fixed-width Win32 types and wrapping arithmetic. Strict warnings apply to the new platform/UI/test driver; the legacy reconstruction retains old C idioms. See `src/classic/PROVENANCE.md` for the exact source pins and adaptations.

For webOS, set `SKIFREE_WEBOS_ENV` to the installed SDK environment script, or `SKIFREE_WEBOS_TOOLCHAIN` to its verified ARM toolchain. Choose a fresh `SKIFREE_WEBOS_STAGE_DIR`, then run `scripts/build-webos.sh` with the actual SDL headers and target libpng paths. It refuses non-ARM output and existing stage directories. Follow `docs/WEBOS-RUNTIME.md` to stage and audit runtime libraries, then the root release instructions for IPK packaging and verification.

The application includes 89 original resources. F2 restarts; F3/Back pauses; F toggles fast mode; click/OK performs the original mouse jump; Insert/Space performs the smaller keyboard jump. The remote pause menu exposes fast mode. Read the root README for the full input mapping.

High scores use `scores-v104.txt` beside the SDL preference path, independently of the prototype's `record.txt`. The app does not overwrite the old record. On the tested TV the preference path resolves inside the installed app directory. The existing empty table survived the 0.8-to-1.0 update and relaunch unchanged. Non-empty round-trip passes on Windows/Linux and in native UI tests; a non-empty TV upgrade and uninstall survival remain unverified. Malformed data and orphaned temporary files are preserved, and the menu warns when saving is disabled.

Remote colours use native SDL-webOS scancodes 486–489 plus a Red-keycode compatibility path and keyboard-state edge detection. F5–F8 mirror Red/Green/Yellow/Blue. The optional `src/recreation` implementation shares the same 89 textures and retains independent in-session state. No launch preference is persisted: Original remains the default.
