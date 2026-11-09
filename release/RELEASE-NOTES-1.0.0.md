# SkiFree for webOS 1.0.0

A free, native C++/GLES2 adaptation of SkiFree for LG webOS TVs. Play from the sofa with a Magic Remote, mouse or keyboard.

Original game and artwork by **Chris Pirih**. This is an independent fan release with no affiliation or endorsement by Chris Pirih, Microsoft or LG. The original Windows executable and ZIP are not included or run.

## What's included

- **Original** is the default. It follows the reconstructed SkiFree 1.04 behavior: Slalom, Tree Slalom and Freestyle, the original mountain, characters, scoring, fast mode and yeti encounters.
- **Recreation** adds simplified steering, timed boosts and aerial tricks. **Yeti Chase** is its gentler pursuit mode.
- Original scores use a classic three-letter initials screen. **Back → High Scores** browses the three saved course tables without finishing another run.
- The last 1.0.0 pass adds a compact white Recreation HUD, hides the in-run footer and shows the active Original course with live statistics and a separate saved BEST row.
- All 89 original bitmap resources are bundled. No import, account or payment is required.

## Controls

- **Pointer / mouse:** aim the skis; **OK / click** jumps or performs an airborne trick.
- **Directions:** turn, brake uphill or point downhill.
- **Red / Green / Yellow:** Original, Recreation, Yeti Chase. **Blue:** help.
- **Back:** pause menu. **F2:** new game. **F5–F8:** the four run shortcuts.
- Keyboard: **Insert / numpad 0 / Space** for the smaller original jump; **f** toggles original fast mode.

The full mapping is in `README.md` in the attached source ZIP.

## Install

1. Download the IPK and `SHA256SUMS.txt` from this release.
2. Compare the package SHA-256 with the listed value:

   ```text
   a1244a9c2d40effac00445265a6c45654769e14b6b08a2d753628cd41cda4cd6
   ```

   The release IPK is **3,033,578 bytes**.
3. Install and launch through the webOS CLI:

   ```powershell
   ares-install --device my-tv .\com.ryanspice.skifree_1.0.0_arm.ipk
   ares-launch --device my-tv com.ryanspice.skifree
   ```

The timestamp-corrected package was installed and launched on the owner's LG OLED48C3PUA (C3), firmware 43.21.84, on November 9, 2026. Its executable is byte-identical to the earlier C3-verified 1.0.0 package, and its files are unchanged; only the archive timestamps are real 2026 dates so TV installers accept it.

## Compatibility and limits

- Tested on one TV: **LG OLED48C3PUA (C3), firmware 43.21.84, webOS 11.2.0**, Developer Mode, non-root app process.
- **Single-TV release. Measured minimum: 11.2.0.** No support claim below 11.2.0; other models and firmware are theoretical targets only.
- ARM32; the binary requires GLIBC 2.38 and GLIBCXX 3.4.32. Rooted routes and older firmware were not tested.
- **Known issue:** in some nested menus/dialogs, the first **Back** press opens the exit confirmation instead of canceling the current dialog. Cancel and press **Back** again. It does not affect gameplay or score saving; a later release will fix it.
- The reconstruction has documented byte-accuracy TODOs; this is not a claim of perfect executable or pixel equivalence. Original audio playback remains inactive.
- Score storage is local. Non-empty records survived a fresh session and an update on the C3; uninstall, app-UID changes and other migration routes remain unverified.

## Verification

The 1.0.0 source passed four Windows suites and four Linux sanitizer suites with **118 assertions**; the native UI suite passed **83 cases**. The release ARM stage contains 104 allowlisted files; the IPK verifier matched all 105 packaged files, including the 89 hash-checked sprites. The release package was installed and launched on the C3. See `native/docs/VERIFICATION.md` in the attached source ZIP for exact artifact identities and scope.

## Credits and rights

- Original SkiFree game and artwork: **Chris Pirih**, https://ski.ihoc.net/.
- Original game core adapted from the pinned reconstructions by **Eric Fry** (`yuv422/skifree_decomp`) and **Jeff** (`jeff-1amstudios/skifree_sdl`).
- TV platform, renderer and input adaptation: new code under MIT.

This is a free fan release; no royalties or payments are requested. That is a distribution decision, not a license grant from Chris Pirih or the reconstruction authors. The original artwork and reconstructed core keep their separate rights; the MIT license for the new platform code does not cover them. Details: `native/NOTICE.md` in the attached source ZIP.

Owner tested the controls, Original gameplay and score saving on the C3. Automated checks and TV evidence are recorded in `native/docs/VERIFICATION.md` in the attached source ZIP.

## Links

[Website](https://ryanspice.com/) · [GitHub](https://github.com/ryanspice)
