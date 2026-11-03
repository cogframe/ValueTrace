param(
    [Parameter(Mandatory=$true)][string[]]$Source,
    [Parameter(Mandatory=$true)][string]$Output,
    [switch]$Link,
    [switch]$NoDebug,
    [string]$Define = '',
    [string[]]$Extra = @()
)
$ErrorActionPreference = 'Stop'
$vtRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$vtBin = Join-Path $vtRoot 'third_party/llvm-23.1.4/bin'
$vtSourceItems = @($Source | ForEach-Object { $_ -split ',' } | ForEach-Object { $_.Trim() } | Where-Object { $_ })
$vtSources = @($vtSourceItems | ForEach-Object { (Resolve-Path -LiteralPath $_).Path }) -join '|'
$vtOutput = [System.IO.Path]::GetFullPath($Output)
$vtDebug = if ($NoDebug) { 'OFF' } else { 'ON' }
$vtLink = if ($Link) { 'ON' } else { 'OFF' }
& cmake "-DVT_CLANG=$(Join-Path $vtBin 'clang++.exe')" "-DVT_OPT=$(Join-Path $vtBin 'opt.exe')" `
    "-DVT_LINK_TOOL=$(Join-Path $vtBin 'llvm-link.exe')" "-DVT_SOURCES=$vtSources" `
    "-DVT_OUTPUT=$vtOutput" "-DVT_LINK=$vtLink" "-DVT_DEBUG=$vtDebug" `
    "-DVT_DEFINE=$Define" "-DVT_EXTRA=$($Extra -join '|')" `
    -P (Join-Path $PSScriptRoot 'prepare-ir.cmake')
exit $LASTEXITCODE
