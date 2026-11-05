[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$IpkPath,
    [Parameter(Mandatory)][string]$SourceUrl,
    [Parameter(Mandatory)][string]$IconUrl,
    [Parameter(Mandatory)][string]$IpkUrl,
    [Parameter(Mandatory)][string]$ManifestUrl,
    [Parameter(Mandatory)][string]$SupportPageUrl,
    [Parameter(Mandatory)][ValidateSet('true','false','optional')][string]$RootRequired,
    [Parameter(Mandatory)][string]$MinimumWebOSRelease,
    [Parameter(Mandatory)][string]$OutputDirectory,
    [ValidateSet('main','non-free')][string]$Pool = 'non-free',
    [string]$ManifestTool
)
$ErrorActionPreference = 'Stop'

function Assert-PublicHttpsUrl([string]$Value, [string]$Label) {
    $parsedUri = $null
    if (-not [Uri]::TryCreate($Value, [UriKind]::Absolute, [ref]$parsedUri) -or
        $parsedUri.Scheme -ne 'https' -or $parsedUri.IsLoopback -or
        $parsedUri.UserInfo -or $Value -match '[{}<>"\\\s]' -or
        $parsedUri.Host -match '(^|\.)(example\.(com|org|net)|localhost)$') {
        throw "$Label must be an explicit public HTTPS URL without credentials or placeholders."
    }
    $parsedAddress = $null
    if ([Net.IPAddress]::TryParse($parsedUri.Host, [ref]$parsedAddress) -or $parsedUri.Host -notmatch '\.') {
        throw "$Label must use a public DNS hostname, not a device address or local host."
    }
}

foreach ($entry in @{
    SourceUrl=$SourceUrl; IconUrl=$IconUrl; IpkUrl=$IpkUrl;
    ManifestUrl=$ManifestUrl; SupportPageUrl=$SupportPageUrl
}.GetEnumerator()) { Assert-PublicHttpsUrl $entry.Value $entry.Key }
if ($MinimumWebOSRelease -notmatch '^\d+(\.\d+){1,3}$') {
    throw 'MinimumWebOSRelease must be a measured numeric release from the device, not its marketing name.'
}
$packagePath = (Resolve-Path -LiteralPath $IpkPath).Path
if (-not (Test-Path -LiteralPath $packagePath -PathType Leaf) -or
    [IO.Path]::GetExtension($packagePath) -ne '.ipk') { throw 'IpkPath must name an existing IPK.' }
if (Test-Path -LiteralPath $OutputDirectory) { throw 'OutputDirectory already exists. Choose a fresh output directory.' }

if (-not $ManifestTool) {
    foreach ($candidate in @('webosbrew-gen-manifest', 'webosbrew-toolbox-gen-manifest')) {
        $foundTool = Get-Command $candidate -ErrorAction SilentlyContinue
        if ($foundTool) { $ManifestTool = $foundTool.Source; break }
    }
}
if (-not $ManifestTool) { throw 'Install/use the official Homebrew dev-toolbox manifest generator, or pass -ManifestTool with its executable path.' }
$resolvedTool = (Get-Command $ManifestTool -ErrorAction Stop).Source
$outputPath = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $outputPath | Out-Null
$manifestPath = Join-Path $outputPath 'com.ryanspice.skifree.manifest.json'
& $resolvedTool --pkgfile $packagePath --icon $IconUrl --link $SourceUrl --root $RootRequired --output $manifestPath
if ($LASTEXITCODE -ne 0) { throw "Manifest generator failed with exit code $LASTEXITCODE; keep its diagnostics and choose a fresh output directory for retry." }
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
if ($manifest.id -ne 'com.ryanspice.skifree' -or $manifest.version -ne '1.0.0' -or $manifest.type -ne 'native') {
    throw 'The IPK is not the expected SkiFreeWebOS native 1.0.0 package.'
}
if (-not $manifest.title -or $manifest.title.Length -gt 30) { throw 'Package title must be 1–30 characters.' }
$packageHash = (Get-FileHash -LiteralPath $packagePath -Algorithm SHA256).Hash.ToLowerInvariant()
$packageSize = (Get-Item -LiteralPath $packagePath).Length
if ($manifest.ipkHash.sha256 -ne $packageHash -or $manifest.ipkSize -ne $packageSize) {
    throw 'Generated manifest hash/size does not match the exact IPK.'
}
$manifest.ipkUrl = $IpkUrl
$manifest | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $manifestPath -Encoding utf8NoBOM

$template = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'homebrew-package.yml.in') -Raw
$yaml = $template -replace '^# TEMPLATE ONLY[^\r\n]*\r?\n', ''
$yaml = $yaml -replace '(?m)^title:.*$', ('title: ' + (ConvertTo-Json -InputObject $manifest.title -Compress))
$yaml = $yaml.Replace('{{PUBLIC_ORIGINAL_ICON_URL}}', $IconUrl).
    Replace('{{PUBLIC_MANIFEST_URL}}', $ManifestUrl).
    Replace('{{PUBLIC_SUPPORT_PAGE_URL}}', $SupportPageUrl).
    Replace('{{MINIMUM_TESTED_WEBOS_RELEASE}}', $MinimumWebOSRelease).
    Replace('{{PACKAGE_POOL}}', $Pool)
if ($yaml -match '\{\{') { throw 'Unresolved package template field.' }
$yaml | Set-Content -LiteralPath (Join-Path $outputPath 'com.ryanspice.skifree.yml') -Encoding utf8NoBOM
"$packageHash  $([IO.Path]::GetFileName($packagePath))" |
    Set-Content -LiteralPath (Join-Path $outputPath 'SHA256SUMS.txt') -Encoding utf8NoBOM
@{
    preparedAtUtc = [DateTime]::UtcNow.ToString('o')
    appId = $manifest.id
    version = $manifest.version
    ipkSha256 = $packageHash
    ipkBytes = $packageSize
    sourceUrl = $SourceUrl
    ipkUrl = $IpkUrl
    manifestUrl = $ManifestUrl
    rootRequired = $RootRequired
    packagePool = $Pool
    minimumWebOSRelease = $MinimumWebOSRelease
    published = $false
    remoteUrlsChecked = $false
    tvTestingEstablishedByThisScript = $false
    originalArtworkAuditEstablishedByThisScript = $false
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $outputPath 'PREPARATION-RECEIPT.json') -Encoding utf8NoBOM
Write-Output "Prepared metadata in $outputPath. No files uploaded; verify all remote URLs and complete the release checklist before publication."
