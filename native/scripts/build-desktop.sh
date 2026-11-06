#!/usr/bin/env bash
# Adapted from Spice Home C++ (MIT). Uses installed development dependencies.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${SKIFREE_BUILD_DIR:-$ROOT/../.ai/evidence/desktop-linux}"
cmake -S "$ROOT" -B "$BUILD" -G Ninja -DCMAKE_BUILD_TYPE=Release -DSKIFREE_STRICT=ON "$@"
cmake --build "$BUILD" --parallel "${SKIFREE_JOBS:-4}"
ctest --test-dir "$BUILD" --output-on-failure
printf 'Native executable: %s\n' "$BUILD/skifree-native"
