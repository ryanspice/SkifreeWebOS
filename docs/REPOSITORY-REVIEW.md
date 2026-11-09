# Repository review

## Current maintenance pass — 2026-11-05

Baseline: `65a813b`, with a clean worktree, 185 tracked files and approximately
1.95 MB of tracked file contents. The largest tracked item is a documentation
image (approximately 1.40 MB). Generated builds, private
TV receipts, local dependencies and unsent launch material are already ignored.
No deletion or history rewrite is needed to make the source tree navigable.

This pass adds the [1.1.0 online-score plan](ROADMAP-1.1.0.md), links it from the
overview/documentation map, updates contributor instructions and marks the old
review below as historical. The plan does not introduce networking or change
the 1.0.0 app version. Original simulation, artwork and real saves stay unchanged.

The Linux desktop helper now defaults to `.ai/evidence/desktop-linux/`, matching
the project-local output policy. The contributor command list includes the
existing IPK ownership/permission checks. A small Linux CI workflow exercises
core tests, source/stage boundaries, IPK permission checks and source ZIP creation
with read-only repository permission and pinned actions. It has no deployment,
TV access, artifact upload or publication step.

Keep these layers distinct:

- Root README and `docs/` describe installation, controls, roadmap and support.
- `native/docs/` owns technical provenance, runtime limits and verification.
- `release/` owns packaging, release notes and tag gates.
- `.ai/` owns task intake, checks, preserved preimages and synthesis receipts.

Verification: a fresh strict Linux Release build passed all four CTest suites.
The six source-boundary tests, six IPK permission tests (including metadata
rejection vectors) and seven stage-boundary checks passed on Linux and Windows.
The desktop helper passed shell syntax checks; mocked tool invocations verified
its default and explicit build paths. Workflow YAML and pinned-action structure
were checked locally. A fresh source archive and relative documentation links
are checked as part of this handoff. These are maintenance checks, not a new SDL
UI, ARM or TV run. Hosted CI execution is a separate gate. See the maintained
[verification record](../native/docs/VERIFICATION.md) and
[tag checklist](../release/TAGGING.md) for current product acceptance.

Follow-up cleanup should happen when the affected code changes: extract settings
and online-score coordination from `main.cpp` during 1.1.0, and review dead
diagnostic state then. Preserve reconstructed function/resource names and current
historical evidence. Do not mass-format the Original core, move sprite resources,
delete ignored build directories or recompress approved images as incidental
cleanup. Revisit version-specific release checks when implementing 1.1.0.

### Main consolidation and release tutorial

An authenticated fetch found only main on origin, matching the local branch,
with no separate upstream remote or open PR. Fast-forward merge had no remote
changes to integrate. The cleanup/roadmap landed as `b284697`; its hosted
[host/source workflow passed](https://github.com/ryanspice/SkifreeWebOS/actions/runs/42162092585).
The [release guide](RELEASE-GUIDE.md) and [visual walkthrough](RELEASE-WALKTHROUGH.html)
cover the GitHub/Homebrew sequence. The source repository and 1.0.0 GitHub release
are public; Homebrew submission and social posting remain unpublished. No branch
deletion or force-push was needed for this consolidation.

## Historical review — 2026-11-04

The sections below describe the earlier handoff. Their acceptance gaps and test
counts are historical, not the current package's status.

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

1. The release package was installed and launched on the C3; physical initials
   input and a new human-earned score after closing/reopening remain useful
   playtests. Earlier TV checks cover an empty legacy table and storage behavior.
2. Complete public-release metadata, meaningful human review and the Homebrew
   compatibility fields before submitting the listing. The owner's free
   fan-release decision is recorded in the notices.
3. Broader EXE runtime comparison remains incomplete. The pinned reconstruction
   and reviewed disassembly support specific paths, not every possible state.
4. Score storage assumes a single writer. It does not guarantee POSIX power-loss
   durability or protect against another process changing a loaded file.

The source repository and 1.0.0 GitHub release are public. No Homebrew submission
or social post is part of this handoff.

## Independent AI review follow-up

The requested independent AI source review found two release issues: earlier TV
observations appeared as completed current-package gates, and broad documentation
text rules could admit unapproved credential/receipt files to a source archive.
The checklist now labels earlier evidence and leaves current TV gates open.
Documentation paths are explicitly allowlisted, with negative cases for unapproved
JSON, text, Markdown and YAML files. The expanded six-test source suite passes; no
exposed sensitive file was identified by the review.

No demonstrated initials/migration defect was found in that bounded static pass.
The [Debug follow-up](../native/docs/DEBUG-BUILD.md) identifies disk exhaustion and
a passing fresh parallel Debug build. Two research lanes informed the
[compatibility plan](COMPATIBILITY.md); neither operated a TV. This AI peer review
does not fulfill Homebrew's meaningful human review requirement.
