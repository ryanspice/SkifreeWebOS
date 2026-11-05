# SkiFree for webOS

A free, independent adaptation of SkiFree for LG webOS TVs. Play with a Magic Remote, mouse or keyboard. Original game and artwork by **Chris Pirih**.

![SkiFree for webOS 1.0.0 opening screen on an LG C3](docs/media/skifree-original-c3.png)

*Native 1.0.0 framebuffer captured on an LG C3. Original is the default run.*

**[Install and play](docs/INSTALL.md)** · [Controls](#original-controls) · [Compatibility and checks](native/docs/VERIFICATION.md) · [Champions of Snowboarding 2026](docs/support.md)

**Release status:** 1.0.0 is a locally tested candidate. A public download and Homebrew Channel listing have not been published.

## Choose your run

- **Original** follows the reconstructed SkiFree 1.04 rules: Slalom, Tree Slalom and Freestyle on the original mountain, with its original characters, scoring and fast-mode toggle. The yeti appears during ordinary play, including beyond 2,000 metres downhill.
- **Recreation** offers free skiing with simplified steering, timed boosts and aerial tricks.
- **Yeti Chase** is Recreation's pursuit mode, with a head start and room to recover after a fall.

Original is the default. **Red** selects Original, **Green** selects Recreation, **Yellow** selects Yeti Chase, and **Blue** opens controls. Click the on-screen choices or use **Back → Choose a Run**. Keyboard F5–F8 provide the same shortcuts. Switching preserves each run until the app closes.

Version **1.0.0** keeps the original lift sprites and opening credit, with the original 1.04 crossed out above **WebOS 1.0.0**. Score files are validated before loading or replacing, and menus show a warning if high scores cannot be saved. All 89 original bitmap resources are included; no import, account or payment is required to play. The Windows executable is neither included nor run.

![The separate Recreation run](docs/media/skifree-recreation-desktop.png)

*Recreation, captured from the native desktop test build. Its simplified rules and gentler pursuit are separate from Original.*

## Original controls

| Control | Action |
| --- | --- |
| Mouse / Magic Remote pointer | Aim the skis using the original mouse direction logic |
| Mouse click / remote OK | Original mouse jump or airborne trick; restart after being eaten |
| Arrows / directional pad | Original turning, downhill and uphill stepping directions |
| Insert / numpad 0 / Space | Original smaller keyboard jump |
| f | Toggle original fast mode; repeats and letter case follow the EXE |
| Enter | Desktop: restart only after being eaten; on webOS Return also serves remote OK |
| t / r | Original single-step / redraw commands |
| x / X / y / Y | Original two-pixel right / left / downhill / uphill nudges |
| F2 | New game |
| F3 / Back / Escape | Pause or resume; pause menu includes fast mode, help and exit |

**TIME** is the original race clock: it starts at the Slalom or Tree Slalom start line, excludes pauses, includes five-second missed-gate penalties, and freezes at the finish. Free skiing and Freestyle do not start a new race clock.

Keyboard input takes control until the pointer moves again. Focus loss pauses the game; returning does not silently resume it. The pause menu makes F-key functions available on a remote.

The status font, pause/help dialogs and local high-score storage are adapted for the TV. This is a reconstruction-based adaptation, not a claim of proven byte-for-byte equivalence. Read [provenance and limits](native/src/classic/PROVENANCE.md) and [verification](native/docs/VERIFICATION.md).

## Build and release

[Native build instructions](native/README.md) · [Release preparation](release/README.md) · [1.0.0 notes](release/RELEASE-NOTES-1.0.0.md)

C++20 and GLES2 draw the game; SDL2 supplies the window and input. App ID: `com.ryanspice.skifree`. Homebrew publication and social posts remain drafts. The new platform code's MIT license does not cover the original resources or reconstructed core; see [notices](native/NOTICE.md).

The [original-game comparison](native/docs/ORIGINAL-COMPARISON.md) records the checked executable resources and reconstruction functions. The executable is the behavioural reference for Red Original. Missing author source is not a reason to omit a reproducible feature. The unique intro, TV presentation and explicitly documented input adaptations are separate from gameplay fidelity; see the [parity contract](native/docs/ORIGINAL-PARITY.md).

## More from Ryan

[Website](https://ryanspice.com/) · [GitHub](https://github.com/ryanspice) · [Support my work and discover Champions of Snowboarding 2026](docs/support.md)

Development used substantial AI assistance. Build, automated tests, TV execution, physical controls and human authenticity acceptance are recorded separately in [verification](native/docs/VERIFICATION.md).
