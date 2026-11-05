# Compatibility brief and synthesis plan

Research date: 2026-11-04. Two requested GPT-6.1 Terra research lanes examined
rooted/unrooted routes and consumer webOS versions. Their findings are source
and documentation research; no new TV or firmware-symbol test was performed.

## Current package

The candidate targets ARM32 EABI5 with `/usr/lib/ld-linux.so.3`, GLIBC 2.38 and
GLIBCXX 3.4.32. The earlier package ran on an OLED48C3PUA, firmware 43.21.84,
reported SDK 11.2.0. The current HUD/initials package needs its own device pass.
Marketing webOS names, firmware releases and SDK version fields identify different
things; none alone establishes a native ABI minimum.

| Target | Evidence | Next acceptance gate |
| --- | --- | --- |
| Observed C3 configuration | Earlier package installed/rendered; current package built and audited | Exact current IPK: launch, controls, pause/resume, earned-score relaunch and migration |
| webOS TV 25/26 families | No family-wide native compatibility evidence | Exact firmware dependency inventory and representative TV runs |
| webOS TV 22–24 / 6.x | Current binary may require newer libraries than individual firmwares export | Required-symbol comparison, then device tests |
| webOS TV 4.x–5.x | ABI and SDL/Wayland compatibility unknown | Separate older-ABI build and SDL investigation |
| webOS TV 2.x–3.x | No support established; system SDL may predate the app's 2.0.10 minimum | Legacy build and SDL integration design, then device tests |
| webOS TV 1.x / other architectures | No support established; packaging currently permits ARM32 | Architecture, interpreter and ABI inventory before a separate build |

LG's [platform table](https://webostv.developer.lge.com/develop/specifications/web-api-and-web-engine)
documents web engines, not a native ELF ABI promise. Its
[device information reference](https://webostv.developer.lge.com/develop/references/webostvjs-webos)
distinguishes SDK and software version fields. Native TV development uses a
[community toolchain](https://www.webosbrew.org/pages/native-development).

## Rooted and unrooted installation

Developer Mode and rooted Homebrew are alternative installation routes. Source
inspection found no declared elevated permission or identified root-service
dependency in SkiFree. This does not prove the app's runtime UID, jail access or
successful operation through either route. A root-installed app need not run as
root. The earlier C3 observation did not establish root status.

The intended Developer Mode route follows
[LG setup](https://webostv.developer.lge.com/develop/getting-started/developer-mode-app),
but LG's general testing guide is not official endorsement of this native C++
package. [Brew's environment guide](https://www.webosbrew.org/develop/guides/env-setup/)
describes the unofficial native setup. Use the application manager to launch;
direct SSH execution does not provide the same graphics/application identity.
See [LG application-manager documentation](https://webostv.developer.lge.com/develop/references/application-manager).

For each route, record exact IPK hash, device/firmware, installation method,
graphics startup, physical input, focus/pause behavior and writable preference
path. Test an earned score across relaunch and an upgrade without overwriting
the owner's data. Developer Mode expiration/removal and uninstall are separate
save-survival cases. [Homebrew Channel](https://github.com/webosbrew/webos-homebrew-channel)
and the [Brew filesystem guide](https://www.webosbrew.org/develop/guides/filesystem/)
provide context for installation and filesystem differences.

## Source and build discrepancies

1. **Runtime audit scope.** `audit-webos-runtime.py` records required symbol
   versions but currently gates dependency-path resolution and ELF32 ARM only.
   It does not compare undefined symbols/versions with exact firmware exports.
   SDL dynamic-library names are candidates, not a proven required closure.
2. **Older libc/C++ ABI.** The current build's GLIBC/GLIBCXX requirements cannot
   be repaired by changing a minimum-version label. App, PNG and SDL need a
   compatible toolchain/sysroot strategy and their own audit.
3. **SDL/Wayland.** [Brew's native samples](https://github.com/webosbrew/samples)
   report newer bundled SDL video failures with older Wayland and demonstrate
   system SDL. SkiFree requires SDL 2.0.10 and mandatory loaded symbols, so swapping
   libraries is not an established fix. Compare
   [SDL-webOS](https://github.com/webosbrew/SDL-webOS) options in a separate variant.
4. **Native lifecycle/input.** Browser key and lifecycle tables do not prove SDL
   event delivery. Test pointer, arrows, OK, Back, colours, focus and compositor
   behavior on each representative firmware.

## Synthesis plan

| Order | Work | Completion evidence |
| --- | --- | --- |
| 1 | Accept the exact current C3 build and record install route | Native launch, physical inputs, focus/resume, earned-score relaunch and migration |
| 2 | Strengthen the dependency audit with exact provider symbol/version comparisons, explicit dlopen requirements and ARM ABI attributes | Missing-provider/version/architecture negative fixtures and labeled firmware inventory; assess [Brew's verifier](https://github.com/webosbrew/dev-toolbox-cli) as an independent check |
| 3 | Prototype an older-ABI build separately using a pinned [native toolchain](https://github.com/webosbrew/native-toolchain) or newer compiler against a verified older sysroot | Reproducible compiler/sysroot hashes; app/SDL/PNG rebuilt; no unsupported GLIBC requirements |
| 4 | Select a legacy SDL strategy after checking symbols, structures, compositor and input ABI | Video/context/shader launch, pointer/keys, focus/resume and performance on representative older hardware |
| 5 | Publish a narrow tested matrix and set Homebrew minimum/root metadata | Exact package identities tied to observed device results and field semantics |

Keep the working C3 package available while evaluating legacy variants. Do not
bundle or replace the TV's libc, loader, Wayland or graphics stack. A static C++
runtime may be evaluated only after checking its remaining libc/transitive needs.
Every compatibility variant needs separate package and physical acceptance.
