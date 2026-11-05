# Install and play

SkiFree for webOS 1.0.0 is currently a local release candidate. There is no public download or Homebrew Channel listing yet. These instructions are for the audited IPK when supplied by the maintainer, or a package you build from this source.

## Compatibility

The corrected 1.0.0 package was installed and rendered on an **LG OLED48C3PUA
(C3), firmware 43.21.84**, on November 5, 2026. The TV reports SDK 11.2.0. Its
non-root application process created the new score file successfully. This is
one tested configuration, not a minimum webOS release or a promise of all-TV support.

The ARM32 build requires GLIBC 2.38 and GLIBCXX 3.4.32. Older TV software may not
provide those libraries. The observed install used Developer Mode and a non-root
process; no elevated service was needed for saving. The TV's global root status
and other installation routes are not established. See
[runtime details](../native/docs/WEBOS-RUNTIME.md) and [verification](../native/docs/VERIFICATION.md).

## Install through Developer Mode

This is the tested setup route for the current native candidate on the C3.
LG's official Developer Mode
instructions describe app setup/testing; the native TV toolchain and SDL port
used here are community-supported. See the [compatibility brief](COMPATIBILITY.md)
for the distinction between installation method and app privileges.

1. Follow [LG's Developer Mode setup](https://webostv.developer.lge.com/develop/getting-started/developer-mode-app) to connect your TV and computer. The [webOS Homebrew setup guide](https://www.webosbrew.org/devmode/) is another walkthrough. LG requires an account for this installation method; SkiFree itself requires no account.
2. Obtain the candidate IPK and compare its SHA-256 with the maintainer's checksum.
   The corrected storage build is `com.ryanspice.skifree_1.0.0_arm.ipk` (under
   `.ai/outbox/storage-fix-1.0.0/` in the maintainer's local build), 1,989,972 bytes:

   ```text
   e7b3d6495e48e8cfd1307e6f47542947cb0ec4ebbaf3f09d1254a58eb5e165de
   ```

   In PowerShell, use `Get-FileHash -Algorithm SHA256 <path-to-ipk>`.
3. With the webOS CLI installed and your TV configured, replace `my-tv` with its device name:

   ```powershell
   ares-install --device my-tv .\com.ryanspice.skifree_1.0.0_arm.ipk
   ares-launch --device my-tv com.ryanspice.skifree
   ```

Launch the installed app through the TV launcher or `ares-launch`. Do not run the native executable directly over SSH; webOS supplies its graphics identity through the application manager.

Developer Mode has an expiring session. LG documents that disabling it removes installed development apps. Keep its session active if you want to retain the installation. Score survival after uninstall or Developer Mode removal has not been established.

## Your first run

Original opens automatically. Move the Magic Remote pointer or mouse to aim, or use the directional keys. Press OK or click to jump. The original yeti appears in ordinary mountain play; no extra mode is needed.

- **Red:** Original, with the Slalom, Tree Slalom and Freestyle courses.
- **Green:** Recreation, with simplified controls, boosts and tricks.
- **Yellow:** Recreation's gentler Yeti Chase.
- **Blue:** Controls and help.

The on-screen buttons and **Back → Choose a Run** offer the same choices. Keyboard F5–F8 also work. **Back / Escape** pauses or opens the menu; **F2** starts a new game. Switching runs preserves their progress until the app closes. [Full Original controls](../README.md#original-controls).

The on-screen colour strip and footer hints stay hidden during skiing. The physical colour buttons and F5–F8 remain active. Open the pause menu to show the clickable choices again. After returning from another app, use **Resume** or **Back** to continue; focus returning does not resume the run automatically.

## Scores and troubleshooting

Original maintains a local high-score table. If storage is unreadable, malformed or cannot be replaced safely, the menu warns that scores cannot be saved. Preserve the existing file if investigating a problem; reinstalling or deleting data is not a recovery step.

Tables appear after a course finish. Each Original course has its own table; lower times win in Slalom and Tree Slalom, higher style scores in Freestyle. A qualifying result prompts for three letters, starting at **AAA**. Type letters and press Enter, or use Left/Right, Up/Down and OK. The plus/minus buttons also work with a pointer. Back keeps the current letters. Reopening returns to the mountain, not a score browser. Finish the same course again to see its saved entries. Recreation and Yeti Chase scores are not added to these tables.

The corrected TV package writes `scores-initials-v1.txt` beneath
`state/user-<runtime-uid>/` in its application folder. If absent, it imports a valid
installation-root named table first, or a valid numeric `scores-v104.txt` table
with **---** initials. The old files and `record.txt` remain untouched. Invalid
or interrupted data blocks migration. Older app builds read only the old location,
so newer results do not appear after downgrading.

The November 4 package pointed saves at the protected installation root and could
show the warning after a score. Use the corrected package above; root or TV-wide
permission changes are unnecessary. Startup now verifies an actual write before
reporting scores as available. On the C3, creation and relaunch replacement are
checked; a new earned-score playthrough remains pending. Non-empty round-trip
and migration pass host tests. Uninstall, app-UID changes and non-empty upgrade
survival are not promised.

**Game files unavailable** is a different error: one or more bundled sprites could not load. A local desktop build needs the complete `native/assets` folder supplied through `--assets`. For an installed TV package, verify its checksum and obtain a complete package; preserve score data before reinstalling. **Check Again** retries loading the files. There is no artwork-import step.

For a report, include the app version, TV model, exact firmware, input device, selected run and steps to reproduce. A short description of what you expected and what happened helps. Keep credentials, network addresses and unrelated logs out of the report.

If installation succeeds but launch fails on a different TV, include the software version; do not replace the TV's system libraries to work around a dependency mismatch.
