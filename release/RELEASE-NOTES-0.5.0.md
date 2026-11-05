# SkiFreeWebOS 0.5.0 — original and recreation choices

Original-style SkiFree 1.04 remains the launch default. The owner asked to keep the earlier recreation available for comparison, so the native app now offers:

- **Red:** original-style 1.04.
- **Green:** earlier recreation with its simplified slope and boost/trick rules.
- **Yellow:** the recreation's extra Yeti Chase.
- **Blue:** help for the selected game.

Switching retains each run within the current app session. The choices are also clickable at the start/pause screen and available through **Back → Switch Game**. Keyboard F5–F8 provide equivalent shortcuts. Original F2/F3/F controls keep their roles. The extra rules are clearly labelled and never replace the launch default.

The direct [executable comparison](../native/docs/ORIGINAL-COMPARISON.md) checked all 89 bitmap resources and 944 bytes across seven relevant data ranges. The F toggle and yeti boundary were inspected in the supplied EXE's disassembly. The Windows UI capture was unreliable and Computer Use was stopped with physical Escape; a complete side-by-side runtime comparison is not claimed.

26 sanitized original-core checks and 31 native UI cases pass. Package and corrected-version TV receipts are tracked separately in the verification document. The owner confirmed all four physical colour buttons work on the C3 with 0.5.0 on November 4, 2026. Other physical input types remain separately unconfirmed; the app-local receipt records the last colour event for diagnosis.

No public release, Git push, Homebrew submission, social message or ad purchase is part of this candidate. Original artwork/reconstruction rights and attribution remain separate from the new platform's MIT license.
