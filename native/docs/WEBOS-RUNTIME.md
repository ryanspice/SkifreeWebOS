# webOS runtime

Target: ARM32 EABI5 with interpreter `/usr/lib/ld-linux.so.3`. The installed SDK uses Clang 18.1.6. The binary needs GLIBC_2.38 and GLIBCXX_3.4.32; do not claim compatibility with every webOS generation.

Bundle SDL-webOS 2.32.10 and the pinned libpng 1.6.43 startup codec. The game also uses static libpng. SDL dynamically loads its mandatory PNG codec; the wrapper preloads it from `$ORIGIN/lib`. Do not bundle or replace libc, libstdc++, Wayland, EGL/GLES or other TV platform libraries.

Run the dependency audit and public-stage hash audit before packaging. The final IPK verifier checks every app file against that stage and requires executable permissions. The Windows packager's missing ELF execute bit was observed and corrected by `release/finalize_ipk.py`.

The C3 target reports OLED48C3PUA, firmware 43.21.84, SDK 11.2.0, glibc 2.39 and GLIBCXX_3.4.32. Its Developer Mode jail does not expose `/etc/starfish-release`; minimum-release metadata must not be inferred from a marketing name. Runtime/install/input acceptance for the current artifact is recorded separately in VERIFICATION.md.

Launch via the webOS application manager. Direct execution over the Developer Mode SSH service was rejected as the wrong application identity and is not a valid graphics test.

The corrected November 5 package launched through Developer Mode as non-root
UID 6252 on the observed C3, and created its score file without an elevated
service. The TV's global root status and other install routes are not inferred.
Developer Mode and rooted Homebrew are separate routes; a rooted installation
does not imply the app runs as root.
See the [compatibility research and plan](../../docs/COMPATIBILITY.md).

The current runtime audit resolves dependency paths and checks ELF32 ARM while
recording required symbol versions. It does not compare every undefined symbol
and version with exports from an exact TV firmware, and dynamically loaded
libraries are not a complete proven closure. A passing SDK audit is packaging
evidence; it cannot establish older-firmware compatibility.
