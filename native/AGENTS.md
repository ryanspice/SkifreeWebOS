# Native implementation guide

The root [AGENTS.md](../AGENTS.md) applies. Read [README.md](README.md) for setup.

## Code map

- `src/classic/core.cpp`: reconstructed Original simulation; read the adjacent
  `PROVENANCE.md` and preserve fixed-width types, wrapping arithmetic and timing.
- `src/main.cpp`: screens, input dispatch, focus/pause and score entry.
- `src/recreation/`: separate Recreation and Yeti Chase rules and drawing.
- `src/platform/`, `include/spice/`: SDL lifecycle and GLES2 rendering.
- `include/skifree/scores.hpp`, `score_store.hpp`: ranking and persistent storage.
- `assets/manifest.json`: exact original sprite inventory and hashes.
- `tests/`: core, recreation, ranking, storage, scripted UI and package checks.

## Change rules

Keep Original as the default. Presentation and remote adaptations must not change
course gates, penalties, timer, collisions or yeti rules without supported
reference evidence. Put optional gameplay in Recreation. Keep test-only fixtures
unavailable to production webOS builds.

Named scores use `scores-initials-v1.txt`. Import `scores-v104.txt` only when the
new file is absent; preserve legacy data and `record.txt`. Retain validation,
orphan-temp handling, failure warnings and isolated tests when changing storage.

## Verification

Use [CONTRIBUTING.md](../CONTRIBUTING.md) from the root. Run all four CTest suites
for gameplay or score changes, with Linux ASan/UBSan for arithmetic or persistence.
For input, screens, rendering or lifecycle changes, build the desktop UI and run
`tests/classic_ui_qa.py` as documented in the native README, then inspect affected
screenshots. Scripted UI success is not physical remote acceptance.

For asset/staging changes run `python native/tests/public_stage_tests.py` from
the root, then audit the exact stage and package following release instructions.
Never infer ARM compatibility from a host build. TV installation requires the
intended device and task authorization; leave unrelated TV state alone.

New builds/captures belong in root `.ai/evidence/<task>/`; deliverable IPKs belong
in `.ai/outbox/<release>/`. Use explicit paths and process-local TEMP/TMP.
Preserve existing build products as historical evidence.
