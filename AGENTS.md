# Agent guide

This repository contains the native C++20 SkiFree adaptation for LG webOS TVs.
Start with [README.md](README.md), [CONTRIBUTING.md](CONTRIBUTING.md) and scoped
instructions in `native/AGENTS.md` or `release/AGENTS.md` before editing.

## Working practices

- Inspect the current branch, diff and applicable instructions. Preserve unrelated
  work. Keep recovery copies outside tracked source and verify hashes before
  restoring over later edits. No sibling checkout is required for normal work.
- The lead owns integration and final acceptance. If work is delegated, assign
  non-overlapping files and require scoped evidence and unresolved issues back.
- Use the smallest relevant checks in CONTRIBUTING.md plus scoped gates. Review
  the staged diff and run `git diff --check` before committing.
- Keep project work products under ignored `.ai/`: `inbox/` for intake/plans,
  `outbox/` for deliverables, `evidence/` for builds/checks/captures, and `archive/`
  for retained originals/recovery. Preserve task/source grouping. Do not use
  another project's folders or harness scratch for project storage.
- Set process-local TEMP/TMP and explicit output/download paths where supported.
  Ingest/hash-check unavoidable external output and record the exception. Leave
  credentials, sessions and native harness configuration tool-managed.
- Register plans/reviews/guides as `pending-synthesis` in
  `.ai/SYNTHESIS-QUEUE.json`. Reconcile accepted decisions into maintained
  docs/source/tests, record source/destination hashes, receipt and unresolved
  issues, then mark synthesized. Preserve originals; synthesis is not deletion
  authority. Existing `private/` and native build/dist history is retained.
- Preserve saves; never replace user scores with test fixtures. Use isolated
  preference paths in tests. Update user docs when controls or storage change.
- Keep historical evidence labelled. A new artifact does not inherit the old
  artifact's TV or human acceptance.

## Product and release boundaries

- This is a standalone source candidate. Do not modify sibling reference projects as part of routine work here.
- The current user direction is a free bundled release. Ship only the 89 original-game bitmap resources listed and hashed in `native/assets/manifest.json`, alongside the original project icon. Never add the original executable, ZIP, full resource archive, unrelated reference material or private TV receipts.
- A public IPK must pass the explicit bundled-file and hash audit. Original artwork keeps its own rights and attribution; the new code's MIT license does not cover it.
- Keep original-game credits separate from the license for the new implementation. Do not imply ownership of the original game or that importing a copy grants redistribution rights.
- Distinguish source checks, desktop tests, ARM packaging, real-TV execution, physical input testing, and human play acceptance.
- Drafting release files and posts does not authorize publishing, pushing, sending social posts, or purchasing advertising. Preserve explicit user authorization recorded in the conversation.
- Do not alter the TV's system libraries, launcher/Home binding, unrelated applications, or provider credentials.

- The original-style core is adapted from the pinned 1.04 reconstruction. Read `native/src/classic/PROVENANCE.md`; do not apply the platform MIT license to reconstructed code or original resources. Preserve its original gameplay. The owner explicitly requested the earlier recreation as a separate colour-button choice in 0.5.0; keep those extra rules labelled, with original-style as the launch default.
