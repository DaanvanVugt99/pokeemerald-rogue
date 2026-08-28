@echo off
rem Resolve mGBA without embedding a developer-specific path.

set "MGBA_EXE="

if defined DIVERGENCE_MGBA if exist "%DIVERGENCE_MGBA%" set "MGBA_EXE=%DIVERGENCE_MGBA%"
if not defined MGBA_EXE for %%I in (mgba.exe) do if not "%%~$PATH:I"=="" set "MGBA_EXE=%%~$PATH:I"
if not defined MGBA_EXE if exist "%LOCALAPPDATA%\mGBA\mGBA.exe" set "MGBA_EXE=%LOCALAPPDATA%\mGBA\mGBA.exe"
if not defined MGBA_EXE if exist "%ProgramFiles%\mGBA\mGBA.exe" set "MGBA_EXE=%ProgramFiles%\mGBA\mGBA.exe"
if not defined MGBA_EXE if exist "%ProgramFiles(x86)%\mGBA\mGBA.exe" set "MGBA_EXE=%ProgramFiles(x86)%\mGBA\mGBA.exe"

if not defined MGBA_EXE (
    echo Error: Could not find mGBA. Add mgba.exe to PATH or set 1>&2
    echo DIVERGENCE_MGBA to its full path. 1>&2
    exit /b 1
)

exit /b 0
