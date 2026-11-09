# SkiFree 1.04 reconstruction provenance

The game logic and tables in this directory are adapted from:

- Eric Fry, [skifree_decomp](https://github.com/yuv422/skifree_decomp), commit `7b473c51f79671c7fc54c7b14eb4e118687b2048`.
- Jeff, [skifree_sdl](https://github.com/jeff-1amstudios/skifree_sdl), commit `a072d6372363cfcd8e3fb97d2dc7e17d94719de4`, the immediate C source used for this adaptation.
- Original game by **Chris Pirih**, [official SkiFree site](https://ski.ihoc.net/).

These projects reconstruct the Windows SkiFree 1.04 executable. They are not Chris Pirih's published original C source. The supplied executable matches the reconstruction's reference MD5, `c5515451d8f90a822457a4a8e4bf1791`; its SHA-256 is `3572d3757638bc1ae388c3d007ba59ba1f370911901e2a35d2f1297e8cf0ff35`. It is not shipped or executed by this app.

The reconstruction retains unresolved byte-accuracy TODOs. This adaptation must not be described as proven byte-for-byte or pixel-for-pixel equivalent to the Windows executable. No standalone license grant was found in either pinned reconstruction tree. The platform's MIT license does **not** relicense this code, the reconstructed tables, original game or artwork. Public source/package publication requires resolving the applicable upstream redistribution terms; local testing and release drafting are separate from publication.

The adaptation retains the original actor tables, nominal 40 ms timer cadence, course entrances and gates, collision/scoring routines, original NPCs, and four mountain-boundary yetis. Downhill pursuit activates after the original coordinate passes 32,000 (2,000 metres at 16 pixels/metre). F is a persistent fast-mode toggle. Mouse click and Insert have their original different jump strengths.

Platform changes:

- C++ compilation with fixed-width Win32 integer types and explicit wrapping arithmetic; a portable implementation of the original Microsoft CRT random sequence.
- SDL window/context/input and the Spice Home raw GLES renderer replace GDI and the reference SDL_Renderer. No SDL_ttf, SDL_image or Rust runtime is used.
- Original bitmap resources 1–89 are separate nearest-filtered textures. Bitmap dimensions and original depth ordering are preserved; dirty-region bookkeeping remains in the core.
- A 640×360 logical game area is scaled to the TV; the original window was resizable. The status font and pause/help/score dialogs are platform adaptations, not original Windows UI pixels.
- The platform pauses on focus loss, maps remote OK to the original mouse click action, and exposes fast mode in the pause menu for remotes without F.
- Course results use an app-local top-ten table instead of `entpack.ini`. TV update/uninstall persistence has not been established.
- The original audio backend remains inactive. Native tests do not establish physical input behaviour or human acceptance.

`classic_fixtures.hpp` is included only by the test build. It supplies no teleport/debug interface in the installed application. At the owner's explicit request, 0.5.0 includes the earlier 0.3.0 simulation separately under `src/recreation`; it is selectable with coloured buttons, labelled as a recreation, and does not change this original-style core or the default selection.


0.7.0 uses the supplied executable as the behavioural reference, not just the SDL reconstruction. The missing author source does not limit which reproducible behaviours should be ported. The timer now delivers one game update per due callback with measured active elapsed time, matching the EXE's delayed-callback behaviour rather than fixed-step catch-up. Production restarts reseed the Microsoft CRT-compatible generator from monotonic milliseconds. Original character commands are routed through their reconstructed handler. Score insertion, current-result marking and nonqualifying-result feedback follow the EXE; a result serial prevents dropping a repeated finish of the same course.

The owner explicitly permits a unique intro. A presentation-only 64-pixel left offset applies to lift sprites while the opening graphic remains visible, preserving Chris Pirih's credit; original world and collision positions are unchanged. Once the opening graphic scrolls away, rendering uses the original positions. 0.8.0 removes the cable lines at the owner's request and draws a strike through the existing 1.04 label with a WebOS 1.04 note below/right. The original bitmap itself is untouched. Recreation's easier pursuit does not alter Original's yeti.

Desktop Enter follows the EXE's restart-only action. webOS Return continues to serve remote OK/jump because that SDL input path does not identify a separate keyboard origin; keypad Enter uses the original action. The wrapper pauses on Back/focus loss instead of minimizing a Windows window. These TV input adaptations are explicit, not evidence of unrestricted 1:1 equivalence.


For the local 1.0.0 candidate, the owner changed the overlay label to WebOS 1.0.0. His release bar is the preserved reconstructed C behavior and intended Original gameplay, with aggregate positive input feedback; exhaustive Windows runtime equivalence remains unclaimed. Score-file validation and failure feedback live in the platform wrapper rather than changing Original's course rules.
