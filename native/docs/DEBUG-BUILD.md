# Windows Debug build: disk-space failure

## Symptom

A parallel MSVC Debug build failed with C2471 while writing compiler PDB files, and
with LNK1108 while writing a library. A serial retry later stalled in the linker.

## Cause

The build volume had no free space. A later configure step failed with
`No space left on device` while writing CMake's compiler-ID source. That confirms
storage exhaustion as the likely cause of the earlier write errors. The free space
at the time of the first failure was not recorded.

## Resolution

Build on a volume with free space. A fresh parallel Debug build on C: with four jobs
passed all four CTest suites, using the repository's existing `/Zi` setting. No
toolchain or global environment change was needed.

Test environment: Visual Studio 2022, MSVC 19.44.35226.0.

## Reproduce

Use a writable build directory on a volume with free space:

```powershell
cmake -S native -B .ai/evidence/debug-build -DSKIFREE_BUILD_UI=OFF -DSKIFREE_STRICT=ON
cmake --build .ai/evidence/debug-build --config Debug --parallel 4
ctest --test-dir .ai/evidence/debug-build -C Debug --output-on-failure
```

## If C2471 returns with free space available

Check file permissions and sharing conflicts, and keep the full build log before
changing any flags. Microsoft's
[C2471 guidance](https://learn.microsoft.com/en-us/cpp/error-messages/compiler-errors-1/compiler-error-c2471?view=msvc-170)
lists the same checks. The [CMP0141 policy](https://cmake.org/cmake/help/latest/policy/CMP0141.html)
(debug-format setting) is an option to investigate, not a demonstrated fix.
