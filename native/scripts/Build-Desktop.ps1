[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$VcpkgRoot,
    [string]$BuildDir,
    [switch]$Run
)
# Adapted from Spice Home C++ (MIT). Does not install Visual Studio or vcpkg.
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$ProjectRoot = Split-Path $PSScriptRoot -Parent
if (-not $BuildDir) { $BuildDir = Join-Path (Split-Path $ProjectRoot -Parent) '.ai\evidence\desktop-windows' }
$Toolchain = Join-Path $VcpkgRoot 'scripts\buildsystems\vcpkg.cmake'
if (-not (Test-Path -LiteralPath $Toolchain -PathType Leaf)) { throw "vcpkg toolchain missing: $Toolchain" }
foreach ($Tool in @('cmake','ninja','cl')) {
    if (-not (Get-Command $Tool -ErrorAction SilentlyContinue)) {
        throw "$Tool not found. Use a Visual Studio Developer PowerShell with C++ build tools and Ninja."
    }
}
function Invoke-Checked {
    param([string]$Command, [string[]]$Arguments)
    & $Command @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$Command exited with code $LASTEXITCODE" }
}
Invoke-Checked -Command 'cmake' -Arguments @('-S',$ProjectRoot,'-B',$BuildDir,'-G','Ninja',
    '-DCMAKE_BUILD_TYPE=Release','-DSKIFREE_STRICT=ON',
    "-DCMAKE_TOOLCHAIN_FILE=$Toolchain",'-DVCPKG_TARGET_TRIPLET=x64-windows')
Invoke-Checked -Command 'cmake' -Arguments @('--build',$BuildDir,'--parallel','4')
Invoke-Checked -Command 'ctest' -Arguments @('--test-dir',$BuildDir,'--output-on-failure')
$Exe = Join-Path $BuildDir 'skifree-native.exe'
if (-not (Test-Path -LiteralPath $Exe -PathType Leaf)) { throw 'Native executable was not created.' }
Write-Host "Built: $Exe"
Write-Host 'Ensure the matching SDL2 and PNG runtime DLLs are on your runtime search path.'
if ($Run) { Invoke-Checked -Command $Exe -Arguments @('--assets',(Join-Path $ProjectRoot 'assets')) }
