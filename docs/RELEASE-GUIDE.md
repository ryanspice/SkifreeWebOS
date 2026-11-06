# Release walkthrough — GitHub and webOS Homebrew

Open [the visual walkthrough](RELEASE-WALKTHROUGH.html) locally for a step-by-step
view. GitHub shows HTML source rather than running this page; download/open it
from your checkout. The page never runs commands or publishes anything. Its
checkboxes are your personal checklist, not acceptance evidence.

This guide covers the **offline 1.0.0 candidate**. Online scores are planned for
[1.1.0](ROADMAP-1.1.0.md). Commands below run from the repository root in
PowerShell 7. Public-release commands are future steps, not a record that they
have been executed. Recheck the official links before a later release.

## 1. Consolidate and verify main

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
complete meaningful human code review, and retain the named-score acceptance.
The Homebrew submission needs honest AI disclosure and the tested TV/release;
automated model reviews do not replace human review. See the current
[Homebrew rules](https://www.webosbrew.org/develop/guides/publishing/rules/).

```powershell
python release/source_archive_tests.py
python release/ipk_permissions_tests.py
python native/tests/public_stage_tests.py
git diff --check
gh run list --repo ryanspice/SkifreeWebOS --workflow checks.yml --limit 3
```

Keep [Original provenance](../native/src/classic/PROVENANCE.md), artwork terms and
[notices](../native/NOTICE.md) separate from the platform code's MIT license.
Record the permission/terms supporting this exact bundled distribution before
public release. A `non-free` listing does not resolve missing rights. Ask
maintainers about eligibility if the documented terms remain unclear; do not
change the bundled/import product behavior merely to complete a form.

## 3. Freeze one exact bundle

The prepared private handoff is `.ai/outbox/release-tutorial-1.0.0/`. It contains
the IPK, explicit source ZIP and per-file manifest, release notes, release
identity manifest and checksums. Check that `RELEASE-MANIFEST.json.sourceCommit`
is the intended release commit. The IPK retains its audited gameplay build when
only documentation changed. A source ZIP refresh does not imply a new TV test.

```powershell
$repo = 'ryanspice/SkifreeWebOS'
$tag = 'v1.0.0'
$bundle = '.ai/outbox/release-tutorial-1.0.0'
$ipk = Join-Path $bundle 'com.ryanspice.skifree_1.0.0_arm.ipk'
$expected = '494726b3024c7982d47596fc520515c8f529ae8322b53ae53e6d86676f3271fd'
if ((Get-FileHash $ipk -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expected) {
    throw 'IPK differs from the audited C3 candidate. Rebuild/review its evidence.'
}
Get-Content (Join-Path $bundle 'RELEASE-MANIFEST.json')
Get-Content (Join-Path $bundle 'SHA256SUMS.txt')
```

If native source changes, follow [the TV build brief](TV-BUILD.md), create a fresh
stage/IPK and repeat its audits and affected TV acceptance. If source changes
after this handoff, regenerate the explicit source ZIP in a fresh output folder
and update its commit/checksum receipts. Never reuse a reviewed output path or
overwrite the existing `v1.0.0` tag after publication.

## 4. Create the tag and prepare public hosting

Only after acceptance and redistribution checks: select the exact reviewed
commit, confirm the tag is unused, then create and push the annotated tag.

```powershell
$releaseCommit = (git rev-parse HEAD).Trim()
git tag --list v1.0.0
git ls-remote --tags origin refs/tags/v1.0.0
# Continue only if both tag checks are empty and this is the approved commit.
git tag -a v1.0.0 $releaseCommit -m 'SkiFree for webOS 1.0.0'
git push origin v1.0.0
```

In GitHub **Settings → General → Danger Zone → Change repository visibility**,
make the intended source repository public only when ready. Review the tracked
tree and history first. A private release or draft cannot serve Homebrew users.
If you choose separate public hosting instead, verify every supplied URL without
sign-in and meet the applicable source obligations.

Open **Releases → the existing 1.0.0 draft → Edit**. Review target/tag, title,
notes and attachments against the frozen bundle. Leave the release as a draft
while preparing its Homebrew manifest. No public URL should be claimed live yet.

## 5. Generate Homebrew metadata

The [official submission guide](https://www.webosbrew.org/develop/guides/publishing/how-to/)
describes hosting the IPK and manifest together. The current official toolbox
source provides `webosbrew-gen-manifest`; older installations may expose
`webosbrew-toolbox-gen-manifest`. Our helper recognizes either. The generator's
[current CLI source](https://github.com/webosbrew/dev-toolbox-cli/blob/main/packages/gen-manifest/src/main.rs)
matches the helper's package/icon/link/root/output arguments.

Install the generator from the [official toolbox repository](https://github.com/webosbrew/dev-toolbox-cli)
if absent. With an existing Rust/Cargo toolchain, the source-install command is
`cargo install --git https://github.com/webosbrew/dev-toolbox-cli --locked gen-manifest`.
Review its selected revision/dependencies before installing; this guide does not
install tools. Check `webosbrew-gen-manifest --help` after installation.

The minimum release must come from actual compatibility evidence and the TV's
`/etc/starfish-release`. Do not substitute firmware `43.21.84`, SDK `11.2.0` or
a marketing name. Check the oldest release you claim. `rootRequired=false`
describes this app's runtime, independently of how Homebrew Channel is installed;
Developer Mode CLI installation is the verified non-root route for this build.

```powershell
$minimumRelease = Read-Host 'Measured minimum compatible starfish release (numeric)'
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

## 6. Publish and verify public bytes

Attach `com.ryanspice.skifree.manifest.json` from the metadata folder to the
existing GitHub draft alongside the frozen release files. For a stable 1.0.0,
clear **Set as a pre-release**, choose **Set as the latest release**, then
**Publish release**. The `releases/latest/download/` manifest route needs a
public stable latest release; a draft/prerelease is not sufficient.

From a fresh unauthenticated download, check the manifest, IPK hash and byte
count, original icon, source and support links. Keep downloads under `.ai/`.

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
checkout inside `.ai/inbox/`, add the generated YAML as
`packages/com.ryanspice.skifree.yml`; do not add its IPK or local receipts.
Create a feature branch in that fork and open a PR to upstream `main`. Here
“upstream” means webosbrew/apps-repo, not this game's origin/main.

Use the [current PR template](https://github.com/webosbrew/apps-repo/blob/main/.github/pull_request_template.md).
Describe the app, exact TV model and measured release, tested controls/saves,
known compatibility limits, original-game attribution, distribution evidence
and support URL. Disclose AI use accurately. Only claim experienced human review
after it has actually happened; do not select a declaration to make CI pass.

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
links, screenshots and a modest optional support/Champions link. Check each
community's current posting rules. Sending posts remains a separate action.

For 1.1.0: keep the application ID, increment versions together, rebuild/test,
create a new tag and release, regenerate its manifest, and keep the stable
manifest URL. Homebrew can discover a release through that URL; change the
package YAML again when listing fields or requirements change.
