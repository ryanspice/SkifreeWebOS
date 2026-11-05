#!/usr/bin/env bash
# Optional local dependency build. No SDK/global installation and no TV operations.
# libpng v1.6.43: https://github.com/glennrp/libpng/tree/v1.6.43
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SOURCE="$ROOT/../vendor/libpng"
PIN=ed217e3e601d8e462f7fd1e04bed43ac42212429
: "${SKIFREE_WEBOS_ENV:?Set SKIFREE_WEBOS_ENV to the installed SDK environment-setup file}"
[[ -f "$SKIFREE_WEBOS_ENV" ]] || { echo 'SDK environment file missing' >&2; exit 2; }
if [[ ! -e "$SOURCE" ]]; then
  git clone --depth 1 --branch v1.6.43 https://github.com/glennrp/libpng.git "$SOURCE"
fi
if [[ -d "$SOURCE/.git" ]]; then
  [[ "$(git -C "$SOURCE" rev-parse HEAD)" == "$PIN" ]] || { echo 'libpng source revision mismatch' >&2; exit 2; }
  [[ -z "$(git -C "$SOURCE" status --porcelain --untracked-files=no)" ]] || { echo 'libpng source has modifications' >&2; exit 2; }
else
  [[ -f "$ROOT/../vendor/libpng.sha256" ]] || { echo 'Source archive needs vendor/libpng.sha256' >&2; exit 2; }
  (cd "$SOURCE" && sha256sum --check --quiet ../libpng.sha256)
fi
# shellcheck source=webos-sdk-env.sh
source "$ROOT/scripts/webos-sdk-env.sh"
TOOLCHAIN="$SKIFREE_WEBOS_TOOLCHAIN"
BUILD="${SKIFREE_LIBPNG_BUILD_DIR:-$ROOT/build/deps/libpng-webos-clang}"
PREFIX="${SKIFREE_LIBPNG_PREFIX:-$ROOT/build/deps/libpng-webos}"
cmake -S "$SOURCE" -B "$BUILD" -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PREFIX" -DCMAKE_INSTALL_LIBDIR=lib \
  -DPNG_SHARED=ON -DPNG_STATIC=ON -DPNG_TESTS=OFF -DPNG_TOOLS=OFF \
  -DPNG_ARM_NEON=off -DCMAKE_POSITION_INDEPENDENT_CODE=ON
cmake --build "$BUILD" --parallel "${SKIFREE_JOBS:-4}"
cmake --install "$BUILD"
readelf -h "$PREFIX/lib/libpng16.a" > "$BUILD/elf-headers.txt"
grep -Eq 'Machine:.*ARM$' "$BUILD/elf-headers.txt" || { echo 'libpng archive is not ARM' >&2; exit 2; }
if grep 'Machine:' "$BUILD/elf-headers.txt" | grep -Evq 'ARM$'; then
  echo 'libpng archive contains a non-ARM object' >&2; exit 2
fi
printf 'Pinned ARM static libpng installed locally: %s\n' "$PREFIX"
printf 'Shared libpng16.so.16 is also built for SDL-webOS mandatory startup loading.\n'
printf 'CMake arguments: -DPNG_LIBRARY=%s/lib/libpng16.a -DPNG_PNG_INCLUDE_DIR=%s/include\n' "$PREFIX" "$PREFIX"
