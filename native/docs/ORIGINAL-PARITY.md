# Red Original: behaviour parity contract

Red Original targets the observable behaviour of the supplied SkiFree 1.04 executable. Implement reproducible movement, controls, collisions, NPCs, course rules, scoring, timing, restart and yeti behaviour in C++ wherever practical. Use the executable as the authority when it differs from a reconstruction. Lack of the author's original C source is not itself an implementation blocker.

The original game and artwork remain credited to Chris Pirih. Original launches by default; Recreation and its gentler chase are separate choices.

## Explicit presentation and platform exceptions

- The opening scene can be unique. Lift graphics shift left while its original credit is visible. No cable lines are added in 0.8.0; the original version label gets a drawn strike and an annotation, updated to WebOS 1.0.0 at the owner's request for the local 1.0 candidate. These drawing changes do not move world objects or collisions.
- TV menus, status font, fixed logical viewport, local score file and colour-button switching replace Windows-specific interfaces.
- Remote OK invokes the original mouse action. On webOS it shares Return's SDL input path, so ordinary keyboard Return there follows that convenience action too; desktop and keypad Enter keep the original restart-only role.
- Back/focus loss pauses rather than minimizing the Windows program. Switching games freezes the inactive run.

## Evidence and outstanding coverage

- Original resource pixels and seven relevant executable table ranges: exact checked matches.
- Preserved movement, collision, terrain, courses and NPC/yeti routines: source identity against the pinned reconstruction; not yet an exhaustive EXE runtime comparison.
- Delayed timer delivery, original character commands, millisecond reset seeding and score-result behaviour: 0.7.0 corrections supported by direct EXE inspection and focused tests.
- Physical colours: owner accepted 0.5.0. Gentler Recreation chase: owner accepted 0.6.0. Neither substitutes for a full Original playthrough or acceptance of a later build.
- Timed Slalom/Tree Slalom start, pause/resume, finish and penalty paths have focused tests; a complete physical-TV course playthrough remains open.
- Remaining runtime coverage: rare collision/NPC/animation branches, manual single-step timing across a real Windows pause, resize-dependent density and precise event scheduling. Record a mismatch when observed and fix it against the executable evidence; do not dismiss it because the reconstruction is incomplete.

The executable itself is not included in the release. [Comparison evidence](ORIGINAL-COMPARISON.md) and [verification](VERIFICATION.md) distinguish implemented behaviours from verified scope.


## Local 1.0 milestone
On 2026-11-04 the owner accepted majority preservation of the reconstructed C with intended Original behavior as the local 1.0 release bar and reported that tested inputs seem good. Continue to correct demonstrated mismatches. Do not imply exhaustive EXE runtime equivalence, and do not treat publication-specific gates as preventing the authorized local build/install.
