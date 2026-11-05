# Repository review — 2026-11-04

## Scope and outcome

Reviewed the pending changes against `1f4c566`, concentrating on the application
screen/input state machine, named-score ranking/storage, migration tests, updated
player documentation and source archive boundaries. Also inspected the tracked
layout, release helper, notices and existing verification records.

No new blocking defect was demonstrated in this scoped review. This is an
AI-assisted source review and local verification pass, not independent human
review or proof of exhaustive equivalence to the Windows executable.

## Changes included in this handoff

- Recreation and Yeti Chase use a compact white HUD; in-run footer controls are
  hidden while hardware shortcuts remain active.
- Qualifying Original results prompt for three initials, starting at AAA, using
  keyboard, pointer or directional controls. Existing scores import as `---`.
- Named results use a separate file. Legacy scores remain unchanged; malformed
  files and interrupted writes preserve recovery data and disable saving.
- Root/native/release agent guides are tracked, portable and scoped. Contributor
  instructions and a documentation map make build and evidence paths discoverable.
- The source allowlist includes those guides. Private evidence, local planning,
  build products, unsent posts and generated release metadata stay excluded.

The gameplay and asset directories retain their existing layout. Moving the
reconstructed core or renaming sprite resources would add risk without improving
navigation; the documentation map provides the organization instead.

## Verification

For this repository pass, the five source-boundary tests and seven stage-boundary
checks passed. A fresh MSVC Release build passed all four CTest suites. The initial
parallel Debug build encountered PDB/library write errors, and its serial retry
stalled in the linker and was stopped. The contributor example now uses serial
Release. All four suites also passed again in the existing Debug build.

The unchanged gameplay revision already has four passing Linux sanitizer suites,
four Windows suites, 67 native SDL/GLES UI cases and an audited ARM package. Those
results were checked against the existing local receipts, not represented as a
new TV session. See [verification](../native/docs/VERIFICATION.md) for artifact
identities, test scope and historical TV evidence.

Source archive creation checks every included path and asset/media hash. The
final handoff also checks relative documentation links, Git whitespace and common
credential patterns in the Git-visible tree/history. Pattern checks are a hygiene
measure, not a comprehensive security audit.

## Remaining acceptance work

1. Test this HUD/initials binary on the C3, including physical initials input and
   a human-earned score after closing/reopening. Earlier TV checks cover a
   different binary and an empty legacy table.
2. Complete public-release metadata, rights evidence, meaningful human review
   and the Homebrew compatibility fields before submitting the listing.
3. Broader EXE runtime comparison remains incomplete. The pinned reconstruction
   and reviewed disassembly support specific paths, not every possible state.
4. Score storage assumes a single writer. It does not guarantee POSIX power-loss
   durability or protect against another process changing a loaded file.

GitHub source hosting is a private handoff. No downloadable release, Homebrew
submission or social post is part of this repository push.
