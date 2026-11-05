# Windows Debug write investigation — 2026-11-04

The earlier parallel MSVC Debug build failed with C2471 while writing compiler
PDBs and LNK1108 while writing a library. Its serial retry later stalled in the
linker. An existing Debug build and a fresh Release build passed their tests.

The follow-up established that **B: had zero free bytes**. A new configure failed
with `No space left on device` while writing CMake's compiler-ID source, and a
source recovery-copy write failed for the same reason. This demonstrates current
storage exhaustion and supports it as the explanation for the earlier write
errors; the exact free-space state of that earlier build was not recorded.

Generated output from the failed Debug build was copied to C: and hash-checked.
Its top-level Debug output was moved to C: to reclaim a small amount of space.
No user source, scores, published artifacts or unrelated data were removed.
New build outputs use C: while B: remains nearly full.

## Verification

A fresh Visual Studio 2022 / MSVC 19.44.35226.0 configuration on C: built Debug
with four parallel jobs in 5.2 seconds. All four CTest suites passed. This used
the repository's existing `/Zi` configuration; no `/Z7`, `/FS`, toolchain or
global environment change was needed.

For a local reproduction, choose a fresh directory on a volume with free space:

```powershell
cmake -S native -B C:/build/skifree-debug -DSKIFREE_BUILD_UI=OFF -DSKIFREE_STRICT=ON
cmake --build C:/build/skifree-debug --config Debug --parallel 4
ctest --test-dir C:/build/skifree-debug -C Debug --output-on-failure
```

Use an existing writable build root appropriate for your machine. If C2471
returns with sufficient free space, inspect file permissions/sharing and retain
the precise build log before changing flags. Microsoft's
[C2471 guidance](https://learn.microsoft.com/en-us/cpp/error-messages/compiler-errors-1/compiler-error-c2471?view=msvc-170)
identifies free space, read-only files and sharing conflicts as checks.
Compiler debug-format changes such as CMake's
[CMP0141 setting](https://cmake.org/cmake/help/latest/policy/CMP0141.html)
remain an option to investigate, not a demonstrated fix needed here.
