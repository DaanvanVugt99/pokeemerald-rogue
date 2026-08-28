[CmdletBinding()]
param(
    [switch]$InstallPoryscript
)

$ErrorActionPreference = 'Stop'
$PoryscriptVersion = '3.5.2'
$PoryscriptSha256 = '613AD446948B332E479EFD4DA2960BFAAE4F68DA610E7060F52D3BBA1B01288B'
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path

function Find-Msys2Shell {
    $candidates = [System.Collections.Generic.List[string]]::new()
    if ($env:DIVERGENCE_MSYS2_SHELL) { $candidates.Add($env:DIVERGENCE_MSYS2_SHELL) }
    if ($env:DEVKITPRO -and [System.IO.Path]::IsPathRooted($env:DEVKITPRO)) {
        $candidates.Add((Join-Path $env:DEVKITPRO 'msys2\msys2_shell.cmd'))
        $candidates.Add((Join-Path $env:DEVKITPRO 'msys2\msys2_shell.bat'))
    }
    $candidates.Add('C:\devkitPro\msys2\msys2_shell.cmd')
    $candidates.Add('C:\devkitPro\msys2\msys2_shell.bat')

    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }

    foreach ($name in @('msys2_shell.cmd', 'msys2_shell.bat')) {
        $command = Get-Command $name -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($command) { return $command.Source }
    }
    return $null
}

function Find-CommandPath([string]$Name) {
    $command = Get-Command $Name -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($command) { return $command.Source }
    return $null
}

function Find-WorkingPython {
    $candidates = [System.Collections.Generic.List[string]]::new()

    foreach ($name in @('python.exe', 'python3.exe')) {
        $path = Find-CommandPath $name
        if ($path) { $candidates.Add($path) }
    }

    $launcher = Find-CommandPath 'py.exe'
    if ($launcher) {
        $resolved = & $launcher -3 -c 'import sys; print(sys.executable)' 2>$null
        if ($LASTEXITCODE -eq 0 -and $resolved) {
            $candidates.Add(($resolved | Select-Object -First 1))
        }
    }

    foreach ($pattern in @(
        (Join-Path $env:LOCALAPPDATA 'Programs\Python\Python*\python.exe'),
        (Join-Path $env:ProgramFiles 'Python*\python.exe')
    )) {
        Get-Item -Path $pattern -ErrorAction SilentlyContinue |
            Sort-Object FullName -Descending |
            ForEach-Object { $candidates.Add($_.FullName) }
    }

    foreach ($path in ($candidates | Select-Object -Unique)) {
        $previousPreference = $ErrorActionPreference
        $ErrorActionPreference = 'Continue'
        $null = & $path --version 2>&1
        $exitCode = $LASTEXITCODE
        $ErrorActionPreference = $previousPreference
        if ($exitCode -eq 0) { return $path }
    }
    return $null
}

function Find-MgbaUi {
    $candidates = [System.Collections.Generic.List[string]]::new()
    if ($env:DIVERGENCE_MGBA) { $candidates.Add($env:DIVERGENCE_MGBA) }

    $commandPath = Find-CommandPath 'mgba.exe'
    if ($commandPath) { $candidates.Add($commandPath) }

    $candidates.Add((Join-Path $env:LOCALAPPDATA 'mGBA\mGBA.exe'))
    $candidates.Add((Join-Path $env:ProgramFiles 'mGBA\mGBA.exe'))
    if (${env:ProgramFiles(x86)}) {
        $candidates.Add((Join-Path ${env:ProgramFiles(x86)} 'mGBA\mGBA.exe'))
    }

    foreach ($candidate in ($candidates | Select-Object -Unique)) {
        if ($candidate -and (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }
    return $null
}

function Install-Poryscript {
    $toolsDir = Join-Path $RepoRoot 'tools\poryscript'
    $windowsDir = Join-Path $toolsDir 'poryscript-windows'
    $downloadDir = Join-Path $RepoRoot 'build\windows-setup'
    $archivePath = Join-Path $downloadDir "poryscript-$PoryscriptVersion-windows.zip"
    $expandedDir = Join-Path $downloadDir "poryscript-$PoryscriptVersion"
    $downloadUrl = "https://github.com/huderlem/poryscript/releases/download/$PoryscriptVersion/poryscript-windows.zip"

    New-Item -ItemType Directory -Force -Path $downloadDir, $expandedDir, $windowsDir | Out-Null
    try {
        Write-Host "Downloading Poryscript $PoryscriptVersion for Windows..."
        Invoke-WebRequest -Uri $downloadUrl -OutFile $archivePath
        $actualHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $archivePath).Hash
        if ($actualHash -ne $PoryscriptSha256) {
            throw "Poryscript archive checksum mismatch. Expected $PoryscriptSha256 but received $actualHash."
        }
        Expand-Archive -LiteralPath $archivePath -DestinationPath $expandedDir -Force

        $releaseDir = Join-Path $expandedDir 'poryscript-windows'
        Copy-Item -LiteralPath (Join-Path $releaseDir 'poryscript.exe') -Destination $windowsDir -Force
        Copy-Item -LiteralPath (Join-Path $releaseDir 'command_config.json') -Destination $toolsDir -Force
        Copy-Item -LiteralPath (Join-Path $releaseDir 'font_config.json') -Destination $toolsDir -Force
    } finally {
        if (Test-Path -LiteralPath $archivePath) { Remove-Item -LiteralPath $archivePath -Force }
        if (Test-Path -LiteralPath $expandedDir) { Remove-Item -LiteralPath $expandedDir -Recurse -Force }
    }
}

if ($InstallPoryscript) {
    Install-Poryscript
}

$msys2Shell = Find-Msys2Shell
$devkitArm = $null
if ($msys2Shell) {
    $devkitRoot = Split-Path (Split-Path $msys2Shell -Parent) -Parent
    $candidate = Join-Path $devkitRoot 'devkitARM\bin\arm-none-eabi-gcc.exe'
    if (Test-Path -LiteralPath $candidate -PathType Leaf) { $devkitArm = $candidate }
}
if (-not $devkitArm) { $devkitArm = Find-CommandPath 'arm-none-eabi-gcc.exe' }

$makePath = $null
if ($msys2Shell) {
    $candidate = Join-Path (Split-Path $msys2Shell -Parent) 'usr\bin\make.exe'
    if (Test-Path -LiteralPath $candidate -PathType Leaf) { $makePath = $candidate }
}
if (-not $makePath) { $makePath = Find-CommandPath 'make.exe' }

$poryscriptPath = Join-Path $RepoRoot 'tools\poryscript\poryscript-windows\poryscript.exe'
$poryscriptReady = $false
if (Test-Path -LiteralPath $poryscriptPath -PathType Leaf) {
    $installedVersion = & $poryscriptPath -v 2>$null
    $poryscriptReady = ($LASTEXITCODE -eq 0 -and $installedVersion.Trim() -eq $PoryscriptVersion)
}
$romTestPath = Join-Path $RepoRoot 'tools\mgba\mgba-rom-test.exe'
$pythonPath = Find-WorkingPython
$mgbaPath = Find-MgbaUi

$checks = @(
    [pscustomobject]@{ Component = 'devkitPro MSYS2'; Required = $true; Found = [bool]$msys2Shell; Path = $msys2Shell }
    [pscustomobject]@{ Component = 'GNU Make'; Required = $true; Found = [bool]$makePath; Path = $makePath }
    [pscustomobject]@{ Component = 'devkitARM GCC'; Required = $true; Found = [bool]$devkitArm; Path = $devkitArm }
    [pscustomobject]@{ Component = "Poryscript $PoryscriptVersion"; Required = $true; Found = $poryscriptReady; Path = $poryscriptPath }
    [pscustomobject]@{ Component = 'mgba-rom-test'; Required = $true; Found = (Test-Path -LiteralPath $romTestPath); Path = $romTestPath }
    [pscustomobject]@{ Component = 'mGBA UI'; Required = $false; Found = [bool]$mgbaPath; Path = $mgbaPath }
    [pscustomobject]@{ Component = 'Python'; Required = $false; Found = [bool]$pythonPath; Path = $pythonPath }
)

$checks | Format-Table Component, Required, Found, Path -AutoSize

$missingRequired = @($checks | Where-Object { $_.Required -and -not $_.Found })
if ($missingRequired.Count -gt 0) {
    Write-Host 'Windows setup is incomplete. See docs/windows_setup.md for the missing prerequisites.' -ForegroundColor Red
    exit 1
}

Write-Host 'Windows build prerequisites are ready.'
exit 0
