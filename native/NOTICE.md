# SkiFree for webOS notices

Original SkiFree game and bitmap artwork: **Chris Pirih**, https://ski.ihoc.net/. This free, independent adaptation bundles the 89 resources identified and hashed in `assets/manifest.json`. Original rights and attribution remain separate. The original executable and ZIP are not included or run. There is no affiliation or endorsement by Chris Pirih, Microsoft or LG.

The Original game core is adapted from Eric Fry's `yuv422/skifree_decomp` and Jeff's `jeff-1amstudios/skifree_sdl`, pinned and described in `CLASSIC-PROVENANCE.md` in the package or `src/classic/PROVENANCE.md` in source. These are reconstructions of the matching 1.04 executable, not a published original source release. The MIT license for new platform work does not relicense the reconstructed core or original assets. The pinned reconstruction trees contain no standalone license grant; their publication terms remain a release gate.

The window/input loader, viewport mapping, GLES2 shader/texture renderer and build gates were adapted from Spice Home C++, copyright 2026 Spice Home contributors, under MIT. SDL supplies the window, graphics context and input; drawing uses raw GLES2. No Rust, browser engine, SDL_Renderer, launcher integrations, credentials or Home bindings are included.

The project icon (`native/icon.png`, 1254×1254 pixels) was onboarded from a supplied PNG. Its authorship and licence terms are not yet recorded here and must be settled before public release. SDL2/SDL-webOS retains its zlib license; libpng retains its PNG Reference Library license; zlib retains its zlib license. Preserve the packaged runtime notices.

The status font and pause/help/high-score dialogs are TV adaptations. The reconstruction has outstanding byte-accuracy TODOs; this project does not claim perfect executable equivalence. Original audio playback remains inactive.
