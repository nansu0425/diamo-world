#Requires -Version 5.1
<#
.SYNOPSIS
    Single entry point that bootstraps the VS Developer environment, then configures,
    builds, and (optionally) tests diamo-world.

.DESCRIPTION
    The project's build only works inside "Developer PowerShell for VS 2026" (cl/cmake/
    ninja/clang-tidy/clang-format on PATH, VCINSTALLDIR set). Running cmake from a plain
    shell fails with "cmake: not recognized". This script removes that fragility: it finds
    the VS install via vswhere (machine independent — no hardcoded absolute paths), injects
    the Developer environment into the current session, then runs the cmake/ctest presets.

    Run it from any shell, from the project root:
        ./Build.ps1                 # configure (if needed) + build Debug
        ./Build.ps1 release         # build Release
        ./Build.ps1 debug -Test     # build + run tests
        ./Build.ps1 -Reconfigure    # force a fresh configure
#>
[CmdletBinding()]
param(
    [ValidateSet('debug', 'release')]
    [string]$Config = 'debug',

    [switch]$Test,

    [switch]$Reconfigure
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$RepoRoot = $PSScriptRoot
$BuildDir = Join-Path $RepoRoot 'out/build/ninja-msvc'

# Run a native command and fail fast on a non-zero exit code (propagating it).
function Invoke-Native {
    param([Parameter(Mandatory)][string]$What, [Parameter(Mandatory)][scriptblock]$Action)
    Write-Host ">> $What" -ForegroundColor Cyan
    & $Action
    if ($LASTEXITCODE -ne 0) {
        Write-Host "!! $What failed (exit $LASTEXITCODE)" -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

# --- 1. Bootstrap the VS Developer environment (skip if already inside one) ----------
if ($env:VCINSTALLDIR) {
    Write-Host ">> VS Developer environment already active; skipping bootstrap" -ForegroundColor DarkGray
}
else {
    $vsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    if (-not (Test-Path $vsWhere)) {
        throw "vswhere.exe not found at '$vsWhere'. Is Visual Studio installed?"
    }

    # Resolve the install path + instanceId of the latest VS with the C++ toolset.
    # (No -property: vswhere accepts only one, so take the full json record and pick fields.)
    $vs = @(& $vsWhere -latest -prerelease `
            -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
            -format json | ConvertFrom-Json)
    if ($vs.Count -eq 0) {
        throw "No Visual Studio install with the C++ toolset (VC.Tools.x86.x64) was found."
    }
    $install = $vs[0].installationPath
    $instanceId = $vs[0].instanceId

    # Drive Enter-VsDevShell directly (this is what Launch-VsDevShell.ps1 wraps) — it avoids
    # the launcher's noisy internal re-discovery of vswhere and keeps the bootstrap deterministic.
    $devShellModule = Join-Path $install 'Common7/Tools/Microsoft.VisualStudio.DevShell.dll'
    if (-not (Test-Path $devShellModule)) {
        throw "DevShell module not found at '$devShellModule'."
    }

    Write-Host ">> Bootstrapping VS Developer environment from '$install'" -ForegroundColor Cyan
    # Enter-VsDevShell shells out to vcvars, which calls a bare `vswhere.exe`; put its folder on
    # PATH so that internal call resolves cleanly instead of erroring.
    $env:PATH = (Split-Path $vsWhere) + [IO.Path]::PathSeparator + $env:PATH
    Import-Module $devShellModule
    # -SkipAutomaticLocation keeps the current working directory (don't cd away from the repo).
    Enter-VsDevShell -VsInstanceId $instanceId -SkipAutomaticLocation `
        -DevCmdArguments '-arch=x64 -host_arch=x64' | Out-Null
}

# --- 2. Configure (only when needed) -------------------------------------------------
$cacheFile = Join-Path $BuildDir 'CMakeCache.txt'
if ($Reconfigure -or -not (Test-Path $cacheFile)) {
    Invoke-Native 'cmake --preset ninja-msvc' { cmake --preset ninja-msvc }
}
else {
    Write-Host ">> Already configured ($cacheFile); skipping (use -Reconfigure to force)" -ForegroundColor DarkGray
}

# --- 3. Build ------------------------------------------------------------------------
Invoke-Native "cmake --build --preset $Config" { cmake --build --preset $Config }

# --- 4. Test (optional) --------------------------------------------------------------
if ($Test) {
    Invoke-Native "ctest --preset $Config" { ctest --preset $Config }
}

Write-Host ">> Done." -ForegroundColor Green
