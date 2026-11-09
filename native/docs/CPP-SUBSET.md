# Reused C++ platform subset

The window/context and dynamic SDL loader, viewport geometry, GLES2 shader/texture renderer and ARM build gates were adapted from an earlier MIT-licensed C++ renderer by the same author. The game simulation, SkiFree menus and resource handling are separate. SDL owns window/input/context; rendering uses raw GLES2.

No Rust, SDL_Renderer, browser runtime, launcher services, credentials, Home bindings or other applications are included. The source project was preserved. See `scripts/platform-provenance.json` for the original copied-file identities; those are provenance, not assertions that current files remain byte-identical.

Bundled gameplay artwork is attributed separately to Chris Pirih and is outside the MIT code license. The private earlier reference handoff is not part of this source release.
