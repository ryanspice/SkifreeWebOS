# Review against the supplied SkiFree 1.04 executable

Reviewed November 4, 2026. Reference SHA-256: `3572d3757638bc1ae388c3d007ba59ba1f370911901e2a35d2f1297e8cf0ff35`; MD5: `c5515451d8f90a822457a4a8e4bf1791`. The file was not altered. This review compares the **Original** selection; the green/yellow recreations deliberately have different rules.

## Findings

The original-style selection is substantially closer to the executable than the 0.3.0 prototype. No mismatch was found in the bitmap pixels or reachable entries of the seven tables checked below. That is bounded static evidence, not certification of every reconstructed routine or a complete runtime equivalence test.

| Area | Evidence from the actual executable | Current Original selection |
| --- | --- | --- |
| Artwork | All 89 DIB resources decoded directly from the PE resource section | Exact dimensions and RGB values; exact white keyed transparent, all other alpha opaque; all 89 match |
| Actor animation | 128-byte frame-to-sprite table at `0x40a1ac` | Exact bytes |
| Actor initial frames | 44 bytes at `0x40a22c` | Exact bytes |
| Turning | 176-byte left/right frame table at `0x40a258` | Exact bytes |
| Skier movement | 352-byte velocity/frame table at `0x40a308` | Exact bytes |
| Landing | All nine reachable entries for airborne frames 13–21, 36 bytes at `0x40a468` | Exact bytes; unused preceding memory is represented by harmless zero padding in the reconstruction |
| Beginners | 80-byte movement table at `0x40a490` | Exact bytes |
| Snowboarders | 128-byte movement table at `0x40a4e0` | Exact bytes; this NPC is original, not a new character |
| Fast mode | Character dispatch at `0x406780` reaches the Boolean toggle at `0x4067a0`; movement routine `0x402be0` adds velocity again when enabled | Persistent F toggle; no duration/cooldown in Original-style |
| Yeti | `0x404350` checks mountain boundaries; downhill player comparison at `0x404508` is `y > 0x7d00` (32,000 pixels) | Ordinary play beyond 2,000 m at 16 pixels/metre, plus the original side/uphill boundaries |
| Courses | Start/finish/gate setup and course routines in the matching reconstruction | Slalom, Tree Slalom and Freestyle entered on the mountain; entrance fixtures pass |

The seven checked data ranges total **944 bytes**. Six tables match in full; the landing comparison covers every entry the original player-update routine can reach. The whole reconstructed codebase was not byte-compared. In particular, the upstream comments around collision, course and actor routines still contain incomplete reconstruction notes. No claim of perfect physics equivalence is justified by these checks alone.

## Deliberate platform differences and remaining gaps

- **Window and pixels:** the original resizable Win32 client area is replaced by a 640×360 logical playfield, scaled to 1920×1080 on the tested TV. Original game sprites use nearest filtering. The status font, pause/help dialogs, coloured controls and score dialog are new TV UI.
- **Timing:** both use a nominal 40 ms game tick. As of 0.7.0, overdue events coalesce into one game update per callback and carry measured elapsed milliseconds, following the EXE timer routine. The old catch-up loop and 250 ms time clamp are removed. Paused/inactive time remains excluded by the TV lifecycle adapter; full scheduler equivalence is not claimed.
- **Controls:** original arrow/numpad direction logic, Insert jump, mouse jump/tricks and F are preserved. Desktop Enter has its original restart-only behaviour. webOS Return/remote OK and Space are convenience mappings to existing actions; Back/Escape pauses rather than minimizing a Windows window. F5–F8 mirror the coloured shortcuts for desktop testing.
- **Scores:** results use an app-local top-ten file instead of Windows `entpack.ini`. The TV preference path is inside the installed app directory; survival across update/uninstall is unproven. The original Windows dialog's exact presentation is not reproduced.
- **Session switching:** red returns to Original-style, green opens the earlier recreation, yellow its extra chase, blue help. Each run is retained within the current app session; this does not imply persisted runs after closing the app. Extras are labelled and are never the launch default.
- **Runtime comparison limit:** the Windows executable was launched, but the capture did not reliably target its game window. Computer Use was stopped by the user's physical Escape key, and no further desktop input was sent. Therefore side-by-side Windows gameplay, precise timing and full collision/score equivalence remain **unverified**. Do not present the attempted launch as a completed runtime review.

## Validation

26 original-core checks pass under AddressSanitizer/UndefinedBehaviorSanitizer, including 72,000 mixed-input simulation ticks. 31 native software-GLES UI checks pass, including colour scancodes, the native Red-keycode fallback, clickable colour choices, both directions of run preservation, menu selection and all prior input/lifecycle/asset cases. Separately, the owner confirmed **"All four colours work"** on the C3 with 0.5.0 on November 4, 2026. This passes physical colour-button acceptance for that tested TV/remote; it does not prove every other input or full gameplay equivalence.

The installed SDL-webOS headers define colour scancodes **486–489**. The adapter also checks keyboard-state rising edges because some compositor paths do not emit ordinary key events, and supports the observed native Red keycode `2097169`. It does not use the browser keycodes 403–406. Sources: [SDL-webOS](https://github.com/webosbrew/SDL-webOS), [native webOS client implementation notes](https://github.com/punktfunk/client-webos/blob/main/docs/NOTES.md). The runtime writes only the last colour selection to an app-local diagnostic receipt; it is not a general input logger.

Original reconstruction sources and their exact revisions are in [PROVENANCE.md](../src/classic/PROVENANCE.md). Private review receipts retain the table comparisons, per-resource results and disassembly excerpts without putting the Windows executable in a release.

## 0.6.0 preservation audit

The player-facing name is now **Original**. Its gameplay core, tables and constants are unchanged from 0.5.0. SHA-256 of `src/classic/core.cpp`: `43f4e2885518e7c19399531715c249762810860ca6597bfff2088f0985da32cc`.

A separate source audit against the pinned SDL reconstruction verified **67 shared functions are token-identical**, excluding comments and whitespace. These include player movement, collision handling, all three course-scoring routines, terrain population, beginner/dog/snowboarder behaviour and `updateYeti`. All game-data initializers before the platform globals and all constants match. The audit inventories **22 platform-adapted functions**; no unintended gameplay-rule change was found in that scope. Win32 integer widths and the intended wrapping arithmetic remain explicit.

The portable random-number generator was independently compared with Windows `msvcrt.dll`: 4,000 outputs across four seeds matched. This corrects the SDL port's use of platform-specific libc `rand`; matching sequences still require matching seeds and call order. Launch/restart seed selection and fixed 40 ms timing remain platform adaptations, as described above.

All 89 bitmap comparisons and seven executable table-range checks pass again. This establishes preservation of the checked reconstruction and resources. It cannot establish a literal 1:1 port of unavailable original C source, nor settle unresolved upstream reconstruction or Windows runtime differences. Recreation's easier pursuit does not alter Original's yeti routine.

## 0.7.0 executable-led corrections

The EXE is the target for observable behaviour. A missing original C source release is not a reason to leave a reproducible feature out. The unique intro, cable rendering and TV adaptations are intentional exceptions; see [the parity contract](ORIGINAL-PARITY.md).

Direct executable evidence closed these gaps:

- Timer callback `0x401000` reads `GetTickCount` and updates once. The port now coalesces overdue callbacks, carries actual active elapsed milliseconds, and computes speed from that interval instead of executing several fixed catch-up moves. Deterministic 40 ms inputs still preserve normal movement fixtures.
- Character dispatch at `0x406780` exposes `f`, `r`, `t`, `x`, `X`, `y`, `Y`. Those commands are now routed from the keyboard, including case and repeat behaviour. Desktop Enter retains the original no-op while the skier is alive.
- Reset at `0x404970` seeds `srand` from the current millisecond clock. Production restarts now use monotonic milliseconds; deterministic test runs retain explicit seeds. Different machines/boot times naturally produce different courses.
- Score routine `0x402ec0` inserts equal results after existing ties, marks the current qualifying row, and displays an unsuccessful current result below the top ten with a try-again message. The port now does the same while retaining its existing score file. A serial identifies every completed result, including another finish of the same course.

The 67 unchanged gameplay functions and original data tables remain preserved. These changes correct the surrounding adapters; they do not replace Original's movement, collision, course, NPC or yeti rules with Recreation's rules. Direct bitmap/table checks remain applicable. Whole-game Windows runtime equivalence is still a verification task, not a prerequisite for implementing these observed behaviours.

## 0.8.0 intro revision and timer verification

The owner clarified that 0.7.0 had no visual defect, then requested removal of the added lift cables, a playful crossed-out 1.04 / WebOS 1.04 annotation, centered bottom controls and timer verification. Those are presentation changes; the original 89 resource images and 0.7.0 gameplay core are unchanged.

TIME is the original timed-course display. Slalom and Tree Slalom start timing at the 40 m entrance; free skiing and Freestyle do not start that clock. Focused fixtures now cover both timed entrances, progression, pause/resume, delayed callbacks, actual finish crossings/interpolation, frozen results and five-second missed-gate penalties. The original brief signed value on the start-crossing callback is retained. Complete Windows/TV playthrough parity remains distinct from these fixtures.
