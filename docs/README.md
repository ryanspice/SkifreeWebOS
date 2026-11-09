# Documentation map

## Playing and support

- [Installation, compatibility and score troubleshooting](INSTALL.md)
- [TV build brief](TV-BUILD.md)
- [Rooted/unrooted and webOS version compatibility plan](COMPATIBILITY.md)
- [Controls and run selection](../README.md#original-controls)
- [Support Ryan and Champions of Snowboarding 2026](support.md)
- [Screenshot context and attribution](media/README.md)

## Development and evidence

- [1.1.0 plan: optional online scores, settings and acceptance](ROADMAP-1.1.0.md)
- [Contributor workflow](../CONTRIBUTING.md)
- [Security policy](../SECURITY.md)
- [Code of conduct](../CODE_OF_CONDUCT.md)
- [Native build and tests](../native/README.md)
- [Verification and remaining acceptance checks](../native/docs/VERIFICATION.md)
- [Repository review](REPOSITORY-REVIEW.md)
- [Windows Debug write investigation](../native/docs/DEBUG-BUILD.md)
- [Local visual review](../native/docs/VISUAL-REVIEW-1.0.0.md)
- [Original comparison](../native/docs/ORIGINAL-COMPARISON.md)
- [Original parity contract](../native/docs/ORIGINAL-PARITY.md)
- [Core provenance](../native/src/classic/PROVENANCE.md)
- [webOS runtime compatibility](../native/docs/WEBOS-RUNTIME.md)

## Release and rights

- [Release walkthrough: GitHub and Homebrew](RELEASE-GUIDE.md)
- [Visual release tutorial (download/open locally)](RELEASE-WALKTHROUGH.html)
- [Release preparation and Homebrew checklist](../release/README.md)
- [1.0.0 notes](../release/RELEASE-NOTES-1.0.0.md)
- [Tag preparation](../release/TAGGING.md)
- [License scope](../LICENSE) and [attribution notices](../native/NOTICE.md)

## Repository layout

| Location | Purpose |
| --- | --- |
| `native/src/`, `native/include/` | Game, application and platform code |
| `native/assets/` | Manifest and 89 original sprites |
| `native/tests/` | Core, storage, UI and staging checks |
| `native/scripts/`, `native/tools/` | Build, runtime audit and asset utilities |
| `native/docs/` | Technical provenance and verification |
| `docs/` | Player docs, support and approved screenshots |
| `release/` | Packaging, source boundaries and release notes |
| `.github/` | Host/source CI, issue template and support link |

Builds, packages, local dependencies, raw evidence and unsent marketing drafts
are ignored. They are not needed to navigate tracked source.
