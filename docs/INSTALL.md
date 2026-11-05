# Install and play

SkiFree for webOS 1.0.0 is currently a local release candidate. There is no public download or Homebrew Channel listing yet. These instructions are for the audited IPK when supplied by the maintainer, or a package you build from this source.

## Compatibility

The candidate has been installed and rendered on an **LG OLED48C3PUA (C3), firmware 43.21.84**. The TV reports SDK 11.2.0. This is one tested configuration, not a minimum webOS release or a promise of support on all LG TVs.

The ARM32 build requires GLIBC 2.38 and GLIBCXX 3.4.32. Older TV software may not provide those libraries. Rooted versus unrooted compatibility has not been independently established; no root requirement is asserted. See [runtime details](../native/docs/WEBOS-RUNTIME.md) and [verification](../native/docs/VERIFICATION.md).

## Install through Developer Mode

1. Follow [LG's Developer Mode setup](https://webostv.developer.lge.com/develop/getting-started/developer-mode-app) to connect your TV and computer. The [webOS Homebrew setup guide](https://www.webosbrew.org/devmode/) is another walkthrough. LG requires an account for this installation method; SkiFree itself requires no account.
2. Obtain the candidate IPK and compare its SHA-256 with the maintainer's checksum. The audited 1.0.0 candidate is `com.ryanspice.skifree_1.0.0_arm.ipk`, 1,953,364 bytes:

   ```text
   2fc0282c4a924cd6743389bbc8f802af46ae8499b3aafe47d392a0db96dd1e9b
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

## Scores and troubleshooting

Original maintains a local high-score table. If storage is unreadable, malformed or cannot be replaced safely, the menu warns that scores cannot be saved. Preserve the existing file if investigating a problem; reinstalling or deleting data is not a recovery step.

Non-empty save/reload is covered by desktop tests. A human check that an earned score survives closing and reopening on the C3 is pending. Survival across uninstall is not promised.

For a report, include the app version, TV model, exact firmware, input device, selected run and steps to reproduce. A short description of what you expected and what happened helps. Keep credentials, network addresses and unrelated logs out of the report.

If installation succeeds but launch fails on a different TV, include the software version; do not replace the TV's system libraries to work around a dependency mismatch.
