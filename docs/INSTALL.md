# Install and play

SkiFree for webOS 1.0.0 is currently a local release candidate. There is no public download or Homebrew Channel listing yet. These instructions are for the audited IPK when supplied by the maintainer, or a package you build from this source.

## Compatibility

The 1.0.0 test package was installed and rendered on one TV only: the owner's
**LG OLED48C3PUA (C3), firmware 43.21.84**, on November 5, 2026. The TV
reports SDK 11.2.0. Its non-root application process created the new score file
successfully. Other TVs, webOS releases and firmware versions are TBD, and so is
the minimum webOS release.

The ARM32 build requires GLIBC 2.38 and GLIBCXX 3.4.32. Older TV software may not
provide those libraries. The observed install used Developer Mode and a non-root
process; no elevated service was needed for saving. The TV's global root status
and other installation routes are not established. See
[runtime details](../native/docs/WEBOS-RUNTIME.md) and [verification](../native/docs/VERIFICATION.md).

## Install through Developer Mode

This is the route used for the C3 tests. LG's official Developer Mode
instructions describe app setup and testing; the native TV toolchain and SDL port
used here are community-supported. See the [compatibility brief](COMPATIBILITY.md)
for the distinction between installation method and app privileges.

1. Follow [LG's Developer Mode setup](https://webostv.developer.lge.com/develop/getting-started/developer-mode-app) to connect your TV and computer. The [webOS Homebrew setup guide](https://www.webosbrew.org/devmode/) is another walkthrough. LG requires an account for this installation method; SkiFree itself requires no account.
2. Obtain the IPK and its `SHA256SUMS.txt` from the same release, then compare the
   package's SHA-256 with the listed checksum. In PowerShell, use
   `Get-FileHash -Algorithm SHA256 <path-to-ipk>`. The package verified on the C3 is
   1,999,474 bytes with SHA-256:

   ```text
   494726b3024c7982d47596fc520515c8f529ae8322b53ae53e6d86676f3271fd
   ```

   A different package has not been verified on that TV.
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
- **Blue:** Help, which lists the controls.

The on-screen buttons and **Back → Choose a Run** offer the same choices. Keyboard F5–F8 also work. **Back / Escape** pauses or opens the menu; **F2** starts a new game. Switching runs preserves their progress until the app closes. [Full Original controls](../README.md#original-controls).

The on-screen colour strip and footer hints stay hidden during skiing. The physical colour buttons and F5–F8 remain active. Open the pause menu to show the clickable choices again. After returning from another app, use **Resume** or **Back** to continue; focus returning does not resume the run automatically.

## Scores and troubleshooting

Original maintains a local high-score table. If storage is unreadable, malformed or cannot be replaced safely, the menu warns that scores cannot be saved. Preserve the existing file if investigating a problem; reinstalling or deleting data is not a recovery step.

Tables appear after a course finish. Each Original course has its own table; lower times win in Slalom and Tree Slalom, higher style scores in Freestyle. A qualifying result prompts for three letters, starting at **AAA**. Type letters and press Enter, or use Left/Right, Up/Down and OK. The plus/minus buttons also work with a pointer. Back keeps the current letters. Open **Back → High Scores** whenever you want to see saved records, including
after reopening. Use **Left / Right** or the on-screen course buttons to change
tables; **Back** returns to the pause menu. Recreation and Yeti Chase scores are not added to these tables.

The TV package writes `scores-initials-v1.txt` beneath
`state/user-<runtime-uid>/` in its application folder. If absent, it imports a valid
installation-root named table first, or a valid numeric `scores-v104.txt` table
with **---** initials. The old files and `record.txt` remain untouched. Invalid
or interrupted data blocks migration. Older app builds read only the old location,
so newer results do not appear after downgrading.

Builds from November 4 saved to the protected installation folder and could
show the warning after a score. Install the current package instead; root or
TV-wide permission changes are not needed. Startup now verifies an actual write
before reporting scores as available. On the C3, score-file creation and replacement
after relaunch were checked. Owner-confirmed Slalom and Freestyle records survived
a fresh launch and a package update unchanged. Non-empty import/migration also passes host
tests. Uninstall, app-UID changes and other upgrade paths remain unverified.

Original’s top-right status box is hidden on the opening screen. After downhill
movement it shows the active course’s LIVE statistics and a separate saved BEST
record. A new run’s live time/style values are distinct from the saved table.

**Game files unavailable** is a different error: one or more bundled sprites could not load. A local desktop build needs the complete `native/assets` folder supplied through `--assets`. For an installed TV package, verify its checksum and obtain a complete package; preserve score data before reinstalling. **Check Again** retries loading the files. There is no artwork-import step.

For a report, include the app version, TV model, exact firmware, input device, selected run and steps to reproduce. A short description of what you expected and what happened helps. Keep credentials, network addresses and unrelated logs out of the report.

If installation succeeds but launch fails on a different TV, include the software version; do not replace the TV's system libraries to work around a dependency mismatch.
