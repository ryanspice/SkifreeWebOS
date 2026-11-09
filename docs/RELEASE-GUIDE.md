# Release walkthrough — GitHub and webOS Homebrew

Open [the visual walkthrough](RELEASE-WALKTHROUGH.html) locally for a step-by-step
view. GitHub shows HTML source rather than running this page; download/open it
from your checkout. The page never runs commands or publishes anything. Its
checkboxes are your personal checklist, not acceptance evidence.

This guide covers the **offline 1.0.0 release**. Online scores are planned for
[1.1.0](ROADMAP-1.1.0.md). Commands below run from the repository root in
PowerShell 7. **1.0.0 is published**; this guide records the sequence used,
including the final documentation refresh that replaced its assets. Future
releases use a new version and tag. Recheck the official links before a later
release.

## 1. Consolidate and verify main

Current state: the source repository, `v1.0.0` tag and GitHub release are public.
The tag points at the final release commit and `main` contains the full history.
The 1.0.0 tag was moved once for the final documentation refresh before the
assets were replaced. Future releases start from `main` with a new version and
tag.

At the 2026-11-05 consolidation, only `main` existed locally and on origin. No
separate upstream remote or open PR needed merging. Fetch plus fast-forward
merge found no remote changes; the planning/cleanup work was committed to main.
Do not manufacture a merge commit or delete unrelated branches to tidy a list.

```powershell
git status --short
git fetch origin
git switch main
git merge --ff-only origin/main
git log -3 --oneline
git branch -avv
```

Expected: reviewed source on main, no uncommitted changes, and the same local and
remote commit after pushing. If branches later diverge, review their diff and
ownership before merging; do not reset or force-push to make them match.

## 2. Finish candidate acceptance

Read [the tag checklist](../release/TAGGING.md). Core/storage checks and the
current C3 package evidence exist. Confirm the latest HUD/browser visually,
record owner review and acceptance, and retain the named-score acceptance.
The Homebrew submission needs the tested TV/release and the current template.
See the current [Homebrew rules](https://www.webosbrew.org/develop/guides/publishing/rules/).

```powershell
python release/source_archive_tests.py
python release/ipk_permissions_tests.py
python native/tests/public_stage_tests.py
git diff --check
gh run list --repo ryanspice/SkifreeWebOS --workflow checks.yml --limit 3
```

Keep [Original provenance](../native/src/classic/PROVENANCE.md), artwork terms and
[notices](../native/NOTICE.md) separate from the platform code's MIT license. The
owner has recorded a free, attributed fan-release basis; no upstream license
grant is claimed. That is a distribution decision,
not a legal conclusion. A `non-free` listing does not create rights. Ask maintainers
about eligibility if the documented terms remain unclear; do not change the
bundled/import product behavior merely to complete a form.

## 3. Freeze one exact bundle

The maintainer's private handoff is outside the public source archive (for
example, a project-local `.ai/outbox/` folder). It contains the IPK, explicit
source ZIP and per-file manifest, release notes, release identity manifest and
checksums. Check that `RELEASE-MANIFEST.json.sourceCommit` is the intended release
commit. The release IPK reuses the TV-verified executable; a documentation-only
refresh does not change it. A source ZIP refresh does not imply a new TV test.

```powershell
$repo = 'ryanspice/SkifreeWebOS'
$tag = 'v1.0.0'
$bundle = $env:SKIFREE_RELEASE_BUNDLE
if (-not $bundle) { throw 'Set SKIFREE_RELEASE_BUNDLE to the private handoff folder.' }
$ipk = Join-Path $bundle 'com.ryanspice.skifree_1.0.0_arm.ipk'
$expected = 'b0e38b31545c153494795c0fcd7f98b42c37c2425d7dfe872c69984791df3588'
if ((Get-FileHash $ipk -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expected) {
    throw 'IPK differs from the audited C3 candidate. Rebuild/review its evidence.'
}
Get-Content (Join-Path $bundle 'RELEASE-MANIFEST.json')
Get-Content (Join-Path $bundle 'SHA256SUMS.txt')
```

If native source changes, follow [the TV build brief](TV-BUILD.md), create a fresh
stage/IPK and repeat its audits and affected TV acceptance. If source changes
after this handoff, regenerate the explicit source ZIP in a fresh output folder
and update its commit/checksum receipts. Never reuse a reviewed output path. The
final 1.0.0 documentation refresh was the one tag update for this release.

## 4. Tag and publish hosting

For 1.0.0, the annotated tag and GitHub release were refreshed from the final
documentation commit before the assets were replaced. The release page is the
source/download URL for Homebrew.

```powershell
git show --stat v1.0.0
git ls-remote --tags origin refs/tags/v1.0.0
gh release view v1.0.0 --json isDraft,isPrerelease,tagName,url,assets
```

The source repository is public. Review the published tag, target, notes and
assets against the frozen bundle. Future releases use a new version and tag; a
separate hosting route must verify every supplied URL without sign-in and meet
the applicable source obligations.

## 5. Generate Homebrew metadata

The [official submission guide](https://www.webosbrew.org/develop/guides/publishing/how-to/)
describes hosting the IPK and manifest together. The current official toolbox
source provides `webosbrew-gen-manifest`; older installations may expose
`webosbrew-toolbox-gen-manifest`. Our helper recognizes either. The generator's
[current CLI source](https://github.com/webosbrew/dev-toolbox-cli/blob/main/packages/gen-manifest/src/main.rs)
matches the helper's package/link/root/output arguments.

Install the generator from the [official toolbox repository](https://github.com/webosbrew/dev-toolbox-cli)
if absent. With an existing Rust/Cargo toolchain, the source-install command is
`cargo install --git https://github.com/webosbrew/dev-toolbox-cli --locked gen-manifest`.
Review its selected revision/dependencies before installing; this guide does not
install tools. Check `webosbrew-gen-manifest --help` after installation.

The 1.0.0 measured minimum is **11.2.0**. The C3 jail lacks
`/etc/starfish-release`; Homebrew firmware data maps the C3 `43.21.x` W23O build
to release `11.2.0`, and the tested TV reports SDK `11.2.0`. Use `>=11.2.0`.
Do not claim an older release. `rootRequired=false` describes this app's runtime,
independently of how Homebrew Channel is installed; Developer Mode CLI installation
is the verified non-root route for this build.

```powershell
$minimumRelease = '>=11.2.0'
$metadata = '.ai/outbox/homebrew-' + (Get-Date -Format 'yyyyMMdd-HHmmss')
$releaseBase = 'https://github.com/ryanspice/SkifreeWebOS/releases/download/v1.0.0'
$manifestUrl = 'https://github.com/ryanspice/SkifreeWebOS/releases/latest/download/com.ryanspice.skifree.manifest.json'
./release/Prepare-Homebrew.ps1 `
  -IpkPath $ipk `
  -SourceUrl 'https://github.com/ryanspice/SkifreeWebOS/tree/v1.0.0' `
  -IconUrl 'https://raw.githubusercontent.com/ryanspice/SkifreeWebOS/v1.0.0/native/icon.png' `
  -IpkUrl "$releaseBase/com.ryanspice.skifree_1.0.0_arm.ipk" `
  -ManifestUrl $manifestUrl `
  -SupportPageUrl 'https://github.com/ryanspice/SkifreeWebOS/blob/v1.0.0/docs/support.md' `
  -RootRequired false -MinimumWebOSRelease $minimumRelease `
  -Pool non-free -OutputDirectory $metadata
```

Review the selected pool against the actual license terms; `non-free` is the
current conservative template default, not a license determination. The helper
checks package identity/hash/size, but does not establish compatibility, rights
or live URLs. Inspect the generated JSON and YAML. It leaves private preparation
receipts locally; do not upload those as public artifacts.

## 6. Verify public bytes

1.0.0 is already published as a stable, latest release; the package manifest is
attached. Do not republish or replace its assets. From a fresh unauthenticated
download, check the manifest, IPK hash and byte count, source and support links.
Keep downloads under `.ai/`.

The `releases/latest/download/` manifest route needs a public stable latest
release; 1.0.0 satisfies that. A draft or prerelease would not.

```powershell
$verify = '.ai/evidence/public-download-' + (Get-Date -Format 'yyyyMMdd-HHmmss')
New-Item -ItemType Directory -Path $verify | Out-Null
$downloadedManifest = Join-Path $verify 'manifest.json'
Invoke-WebRequest -Uri $manifestUrl -OutFile $downloadedManifest
$m = Get-Content $downloadedManifest -Raw | ConvertFrom-Json
$downloadedIpk = Join-Path $verify 'download.ipk'
Invoke-WebRequest -Uri $m.ipkUrl -OutFile $downloadedIpk
if ((Get-FileHash $downloadedIpk -Algorithm SHA256).Hash.ToLowerInvariant() -ne $m.ipkHash.sha256) {
    throw 'Downloaded IPK hash differs from manifest'
}
if ((Get-Item $downloadedIpk).Length -ne $m.ipkSize) { throw 'IPK size differs' }
if ($m.ipkHash.sha256 -ne $expected) { throw 'Manifest points at a different candidate' }
```

These public requests use no GitHub authorization header. Check that responses
are actual expected files, not login/error pages. A manifest URL that works only
for the maintainer is not ready for a listing.

## 7. Submit to Homebrew

Fork [webosbrew/apps-repo](https://github.com/webosbrew/apps-repo). In a separate
private checkout outside this public tree, add the generated YAML as
`packages/com.ryanspice.skifree.yml`; do not add its IPK or local receipts.
Create a feature branch in that fork and open a PR to upstream `main`. Here
“upstream” means webosbrew/apps-repo, not this game's origin/main.

Use the [current PR template](https://github.com/webosbrew/apps-repo/blob/main/.github/pull_request_template.md).
Describe the app, exact TV model and measured release, tested controls/saves,
known compatibility limits, original-game attribution, distribution evidence
and support URL. Use the tested evidence and the current template; let the
maintainers review the listing.

Maintainers review the submission and may request changes. A successful GitHub
release does not itself create a Homebrew listing. Repository source/schema is
authoritative if the guide and repo differ; for example, screenshot support can
be newer than the prose documentation. Keep optional fields out until checked.

## 8. Confirm the listing, then announce

After the Homebrew PR is merged and the listing appears, verify its description,
download, launch and update behavior on a supported TV. Preserve scores during
update checks; do not uninstall the owner's real game as a test. Confirm the
listed hash/version before sharing the listing URL.

Finish the existing Reddit, LinkedIn and X drafts with real download/listing
links, screenshots and a modest optional support link. Check each
community's current posting rules. Sending posts remains a separate action.

For 1.1.0: keep the application ID, increment versions together, rebuild/test,
create a new tag and release, regenerate its manifest, and keep the stable
manifest URL. Homebrew can discover a release through that URL; change the
package YAML again when listing fields or requirements change.
