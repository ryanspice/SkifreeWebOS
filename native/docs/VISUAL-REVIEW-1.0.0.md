# Local visual and documentation review — 1.0.0

**Historical snapshot:** this review preceded the HUD/initials revision. The subsequent lighter HUD removes the boost-contrast issue recorded below and changes the footer and score-entry screens. See [current revision evidence](VERIFICATION.md#latest-local-revision-quieter-hud-and-arcade-initials).

November 4, 2026. The current native source was built and run locally under Linux/WSL with GCC 13.3, strict warnings, SDL and Mesa software GLES. Rendering reported llvmpipe (LLVM 20.1.2), OpenGL ES 3.2 and Mesa 25.2.8. This is a desktop review, not a new TV installation or physical-input test.

## Captures and checks

The fresh Release build passed all four CTest suites. The existing native UI runner passed all **55 cases**, each with an unedited framebuffer PNG, input script and JSON report. Sixteen additional runs produced **71 captures in total**. All captures were visually inspected, using full-size views for primary screens and contact sheets for repeated input/lifecycle variants.

The set covers all seven application screens: playing, paused, controls, high scores, choose a run, game-files error and exit confirmation. It includes:

- Original opening and moving lift after 20 simulated seconds, with the credit clear, no added cables, crossed-out 1.04 and WebOS 1.0.0 annotation.
- Slalom clock running/paused/resumed, Tree Slalom running, Freestyle play and a fall, and Original's eaten/restart state.
- Recreation, boost cooldown, Yeti Chase, chase pause and completed-run prompt.
- Help for each run, normal and fast-mode pause menus, run selection, exit confirmation and cancel.
- Score tables for all three Original courses, a full top ten, the current result in the last row, a nonqualifying result, save/reload and the failed-save warning.
- Missing, partial and corrupt assets; pointer scaling and letterboxing; keyboard/pointer handoff; focus/hide pause; run preservation.
- 1280×720 and 1920×1080 views, minimum 640×360 controls and 1280×960 / 1280×1024 letterboxed views.

Score-result injections and malformed files are deliberate QA fixtures. They do not represent human-earned results. Course-start captures use original input commands; they are not complete human course playthroughs. The long Original free-ski capture shows the eaten state after a 300-second simulation; it does not independently prove the exact trigger frame or resolve coordinate-wrap equivalence.

## Findings

**Minor visual follow-up — Recreation boost contrast.** In the `recreation_boost` capture, a tree renders behind the `BOOST IN 12 S` label. The label has no opaque backing, so scenery reduces contrast. The relevant rendering is in `src/main.cpp`, immediately after the Recreation HUD. A small opaque label background would address the overlap without changing gameplay. This review records the issue; it does not change the renderer or the installed candidate.

No panel clipping was found in the inspected menus, full score tables, warning or error screens. Small-window text is naturally smaller; this does not establish readability at every physical viewing distance. The current screenshot coverage does not exercise every possible terrain arrangement or animation frame.

## Documentation corrections

- Explain that the colour strip fades while remote and keyboard shortcuts remain active.
- Add Recreation/Chase controls and separate them from Original's commands.
- Explain that high-score tables appear at course completion, with separate rankings for each Original course. Reopening goes to the mountain; 1.0.0 has no score-browser menu.
- Distinguish a storage warning from a missing-assets error and describe focus-return behavior.
- Move specialist original character commands into an expandable README section.
- Clarify desktop build prerequisites, the existing Windows helper, isolated QA output and fixture scope.
- Remove conversational release-approval wording from the release notes and align support-page branding.

The screenshot packet and per-image hashes are retained with the maintainer's local review evidence. They are separate from the three selected public documentation images. [Verification](VERIFICATION.md) records the candidate's earlier package/TV checks and the still-pending owner save test.
