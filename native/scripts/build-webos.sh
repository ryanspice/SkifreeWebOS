#!/usr/bin/env bash
# Adapted from Spice Home C++ (MIT): cross-build and stage only, no install/upload.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [[ -n "${SKIFREE_WEBOS_ENV:-}" ]]; then
  # shellcheck source=webos-sdk-env.sh
  source "$ROOT/scripts/webos-sdk-env.sh"
fi
: "${SKIFREE_WEBOS_TOOLCHAIN:?Set SKIFREE_WEBOS_TOOLCHAIN to the verified SDK CMake toolchain file}"
[[ -f "$SKIFREE_WEBOS_TOOLCHAIN" ]] || { echo 'Toolchain file missing' >&2; exit 2; }
command -v readelf >/dev/null || { echo 'readelf is required for the ARM gate' >&2; exit 2; }
BUILD="${SKIFREE_WEBOS_BUILD_DIR:-$ROOT/build/webos}"
STAGE="${SKIFREE_WEBOS_STAGE_DIR:-$ROOT/dist/webos-stage}"
if [[ "${SKIFREE_WEBOS_BUILD_ONLY:-0}" != 1 ]]; then
  [[ ! -e "$STAGE" ]] || { echo "Stage already exists; choose a new SKIFREE_WEBOS_STAGE_DIR: $STAGE" >&2; exit 2; }
fi
cmake -S "$ROOT" -B "$BUILD" -G Ninja "$@" \
  -DCMAKE_TOOLCHAIN_FILE="$SKIFREE_WEBOS_TOOLCHAIN" \
  -DCMAKE_BUILD_TYPE=Release -DSKIFREE_WEBOS=ON -DSKIFREE_STRICT=ON -DBUILD_TESTING=OFF
cmake --build "$BUILD" --parallel "${SKIFREE_JOBS:-4}"
BIN="$BUILD/skifree-native"
readelf -h "$BIN" > "$BUILD/elf-header.txt"
grep -Eq 'Class:.*ELF32' "$BUILD/elf-header.txt" || { echo 'Refusing non-ELF32 artifact' >&2; exit 2; }
grep -Eq 'Machine:.*ARM$' "$BUILD/elf-header.txt" || { echo 'Refusing non-ARM artifact' >&2; exit 2; }
readelf -d "$BIN" > "$BUILD/elf-dependencies.txt"
if [[ "${SKIFREE_WEBOS_BUILD_ONLY:-0}" == 1 ]]; then
  printf 'ARM executable built, not staged: %s\n' "$BIN"
  exit 0
fi
cmake --install "$BUILD" --prefix "$STAGE"
python3 "$ROOT/scripts/audit_public_stage.py" --stage "$STAGE" --report "$BUILD/public-stage-audit.json"
printf 'ARM executable staged at: %s\n' "$STAGE"
printf 'Audit target SDL-webOS (loaded dynamically) and every library before packaging. No TV was contacted.\n'
