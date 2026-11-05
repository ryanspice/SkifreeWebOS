# Source this helper only inside a build shell. It never changes installed SDK files.
: "${SKIFREE_WEBOS_ENV:?Set SKIFREE_WEBOS_ENV to the installed SDK environment-setup file}"
[[ -f "$SKIFREE_WEBOS_ENV" ]] || { echo 'SDK environment file missing' >&2; return 2; }
# shellcheck source=/dev/null
source "$SKIFREE_WEBOS_ENV"
: "${OECORE_NATIVE_SYSROOT:?SDK did not set its native sysroot}"
: "${OECORE_TARGET_SYSROOT:?SDK did not set its target sysroot}"
[[ "${OECORE_TARGET_ARCH:-}" == arm ]] || { echo 'An ARM SDK environment is required' >&2; return 2; }
# This installed SDK supplies --dyld-prefix=/usr, which its GCC driver rejects.
# Select the SDK's own Clang driver. Move its link-only -rdynamic flag to LDFLAGS
# so strict per-source compilation does not warn about an unused linker flag.
: "${CLANGCC:?SDK must export its Clang C driver}"
: "${CLANGCXX:?SDK must export its Clang C++ driver}"
export CC="${CLANGCC/ -rdynamic/}"
export CXX="${CLANGCXX/ -rdynamic/}"
export LDFLAGS="${LDFLAGS:-} -rdynamic"
export SKIFREE_WEBOS_TOOLCHAIN="$OECORE_NATIVE_SYSROOT/usr/share/cmake/OEToolchainConfig.cmake"
[[ -f "$SKIFREE_WEBOS_TOOLCHAIN" ]] || { echo 'SDK CMake toolchain missing' >&2; return 2; }
