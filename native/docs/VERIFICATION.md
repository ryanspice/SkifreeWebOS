# Verification — SkiFree for webOS 1.0.0

Updated November 4, 2026. This record describes the local candidate and distinguishes automated checks, physical-device observations and human feedback. It is not a claim of exhaustive equivalence to the Windows executable.

## Original gameplay

The core is adapted from the pinned SkiFree 1.04 reconstruction described in [provenance](../src/classic/PROVENANCE.md). The current inventory identifies **67 token-identical shared functions and 22 adapted functions** among 89 shared with the pinned SDL source. Selected direct EXE disassembly supports Tree Slalom start, finish, gate-side comparisons, five-second penalties, interpolation and yeti thresholds. The Original core required no gameplay change during the 1.0 hardening review.

All 89 bitmap resources and 944 bytes across seven reachable game-data ranges were compared against the reference executable. The portable Microsoft CRT random generator matched 4,000 reference outputs across four seeds. [Comparison details](ORIGINAL-COMPARISON.md) and [parity contract](ORIGINAL-PARITY.md) describe the scope and permitted TV adaptations.

Collision, NPC, coordinate-wrap and all other paths were not exhaustively compared at runtime against Windows. Upstream byte-accuracy TODOs remain. The 67/22 inventory is source evidence, not a percentage of proven runtime equivalence. The TV font, dialogs, intro and documented input mappings are adaptations. Recreation and Yeti Chase have separate rules.

## Automated checks

| Check | Recorded result | Scope |
| --- | --- | --- |
| Linux AddressSanitizer / UndefinedBehaviorSanitizer | 118 assertions passed | 70 Original, 15 Recreation, 15 ranking, 18 score-store assertions |
| Original simulation | 72,000 mixed-input ticks covered | Course clocks, crossings, penalties, pause and delayed callbacks included |
| Native desktop SDL/GLES UI | 55 cases passed | Input switching, menus, rendering, save/reload between processes, truncated-file preservation |
| Public-stage boundaries | 7 cases passed | Allowlisted content, altered assets and unexpected files |
| Windows MSVC | Score-store and ranking suites passed | Host filesystem behavior |
| ARM32 build and runtime audit | Passed; 12 dependency records | Target build and library requirements |
| Staged package and IPK | 103 staged / 104 packaged files matched | Extra package metadata accounts for one file; 89 sprite hashes matched; no Windows EXE |

These results belong to the candidate build work. Publication-only documentation edits do not constitute a new run of these suites. The final source addition extended the startup receipt with storage state and score counts; its ARM compile and TV output were checked directly.

Independent static reviews of Original timing/course/yeti behavior, input/lifecycle handling and release evidence found no demonstrated blocking gameplay defect. Static review is not a further test run or physical-TV acceptance.

## Physical TV

Tested configuration: **LG OLED48C3PUA (C3), firmware 43.21.84, SDK 11.2.0**. The version information was queried again during publication preparation. The audited IPK installed and launched through the application manager, and its actual framebuffer was inspected at **1920×1080 on Mali-G52 / GLES 3.2**. Installed executable and asset-manifest hashes matched the audited stage.

Close/relaunch was exercised. One immediate sequence briefly left no running app entry; a subsequent launch after closure succeeded. No crash cause was established from that observation. No compatibility claim is made for other TV generations, and a minimum numeric webOS release or rooted/unrooted requirement has not been verified. See [runtime requirements](WEBOS-RUNTIME.md).

![Native opening screen on the C3](../../docs/media/skifree-original-c3.png)

## Human feedback

The owner confirmed all four physical colour buttons in 0.5.0, accepted the gentler Recreation pursuit in 0.6.0, and subsequently reported that the inputs he tested seemed good. This is positive aggregate feedback; it does not enumerate a complete device-by-device input matrix for 1.0.0.

The local 1.0 candidate follows the owner's release bar: preserve the reconstructed C behavior and intended Original gameplay, with **WebOS 1.0.0** on the intro. Full Windows runtime equivalence remains unclaimed.

## Score persistence

Host tests cover non-empty save/reload, malformed tables, ranking validation, interrupted temporary files and replacement failures. The UI warns when saving is disabled. Existing malformed files and recovery material are preserved. The store does not guarantee concurrent-writer safety or POSIX power-loss durability.

On the C3, an existing empty 13-byte table remained byte-identical through update and close/relaunch. Startup reported writable storage and zero entries. That demonstrates preservation of the empty table only. **A human-earned score surviving close/reopen is awaiting the owner's result. Non-empty update survival and uninstall survival remain unverified.**

## Artifact identity and disclosure

| Artifact | SHA-256 |
| --- | --- |
| 1.0.0 IPK, 1,953,364 bytes | `2fc0282c4a924cd6743389bbc8f802af46ae8499b3aafe47d392a0db96dd1e9b` |
| Installed native executable | `17cc6e5dcbaa71b732ce46a0d64888545b80813c8924d47ad16ea4ae908ef396` |
| Asset manifest | `e86293722f01e2a9fb1b3732b87e66ab9512ccb3291acc622feb3467f9ba197e` |
| Original core source | `bf9922eefc15b4af9a715dc38170c7a455cbb270b54637b68859c71e7a91463b` |

Development used substantial AI assistance for implementation, testing, documentation and review. Automated and model reviews do not substitute for meaningful human code review. Original game/artwork and reconstructed code retain separate attribution and rights; the new platform's MIT license does not relicense them. [Notices](../NOTICE.md).

Public hosting, redistribution terms, final Homebrew metadata, human publication review and maintainer acceptance remain separate from local readiness. No public release or social post has been sent.
