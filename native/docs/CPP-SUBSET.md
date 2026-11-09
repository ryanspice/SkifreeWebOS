# Reused C++ platform subset

The window/context and dynamic SDL loader, viewport geometry, GLES2 shader/texture renderer and ARM build gates were adapted from Ryan's Spice Home C++ project under MIT. The game simulation, SkiFree menus and resource handling are separate. SDL owns window/input/context; rendering uses raw GLES2.

No Rust, SDL_Renderer, browser runtime, launcher services, credentials, Home bindings or other applications are included. The source project was preserved. See `scripts/platform-provenance.json` for the original copied-file identities; those are provenance, not assertions that current files remain byte-identical.

The app icon is the checked-in `native/icon.png`, created by Ryan Spice for this project using the original SkiFree icon as a visual reference; its rights status is recorded in the [notices](../NOTICE.md). Bundled gameplay artwork is attributed separately to Chris Pirih and is outside the MIT code license. The private earlier reference handoff is not part of this source release.
