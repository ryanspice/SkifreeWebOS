# Release tooling guide

The root [AGENTS.md](../AGENTS.md) applies. Follow [README.md](README.md).

- Keep packaging deterministic and allowlisted. Validate exact sprite hashes,
  staged files, ELF architecture/runtime dependencies and executable permissions.
  Compare every IPK application file against the audited stage.
- The source archive is separate from the IPK. Explicitly include portable agent
  guides and contributor docs. Exclude private plans, unsent social drafts,
  credentials, raw receipts and generated release metadata.
- When changing an allowlist, update `source_archive_tests.py` or native
  public-stage tests to prove both allowed files and rejected extras.
- Use fresh ignored output directories and revision names. Never overwrite an
  archive or receipt used as evidence. Record exact hashes and byte sizes.
- Source-only docs changes do not require a new binary. Gameplay changes do:
  do not reuse old binary, TV or package evidence for new source.
- Keep historical release notes historical. Update current version checks,
  app metadata, title and packaging together when a version changes.
- Public URLs, root requirements, minimum webOS versions, redistribution terms
  and human review must be evidenced. Do not fill unknowns with guesses.
- Git hosting, downloadable releases, Homebrew submission and social posts are
  distinct actions. Carry out only those authorized by the user.

Run `python release/source_archive_tests.py`, affected packaging checks and
`git diff --check`. Build a fresh source archive after source-boundary changes.
