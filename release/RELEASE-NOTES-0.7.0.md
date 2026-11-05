# SkiFree for webOS 0.7.0

Original now follows the EXE more closely in delayed timer delivery, keyboard commands, restart seeding and score-result feedback. Red Original remains the default; Recreation's accepted gentler pursuit is unchanged.

- A delayed timer callback produces one movement update with measured elapsed time, following the EXE, rather than a burst of catch-up movement.
- Original `t`, `r`, `x/X`, `y/Y` and case/repeat-sensitive `f` commands work. Desktop Enter restarts only after the skier is eaten; remote OK remains the TV jump action.
- New runs reseed from monotonic milliseconds outside deterministic QA.
- Score results preserve tie order, identify the current row, and show non-top-ten results with a try-again message. Repeating a course finish is no longer suppressed.
- The opening chairlift has cable lines and sits left of Chris Pirih's credit. This is a drawing-only intro adjustment; gameplay world/collision coordinates stay intact.

See the [Original parity contract](../native/docs/ORIGINAL-PARITY.md), [comparison report](../native/docs/ORIGINAL-COMPARISON.md) and [verification](../native/docs/VERIFICATION.md). This candidate does not claim exhaustive EXE runtime equivalence. No public release or social post has been sent.
