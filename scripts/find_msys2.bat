@echo off
rem Resolve the MSYS2 launcher used by the Windows build wrappers.
rem Set DIVERGENCE_MSYS2_SHELL to override automatic discovery.

set "MSYS2_SHELL="

if defined DIVERGENCE_MSYS2_SHELL if exist "%DIVERGENCE_MSYS2_SHELL%" (
    set "MSYS2_SHELL=%DIVERGENCE_MSYS2_SHELL%"
    goto :found
)

if defined DEVKITPRO (
    if exist "%DEVKITPRO%\msys2\msys2_shell.cmd" set "MSYS2_SHELL=%DEVKITPRO%\msys2\msys2_shell.cmd"
    if not defined MSYS2_SHELL if exist "%DEVKITPRO%\msys2\msys2_shell.bat" set "MSYS2_SHELL=%DEVKITPRO%\msys2\msys2_shell.bat"
)

if not defined MSYS2_SHELL if exist "C:\devkitPro\msys2\msys2_shell.cmd" set "MSYS2_SHELL=C:\devkitPro\msys2\msys2_shell.cmd"
if not defined MSYS2_SHELL if exist "C:\devkitPro\msys2\msys2_shell.bat" set "MSYS2_SHELL=C:\devkitPro\msys2\msys2_shell.bat"

if not defined MSYS2_SHELL for %%I in (msys2_shell.cmd) do if not "%%~$PATH:I"=="" set "MSYS2_SHELL=%%~$PATH:I"
if not defined MSYS2_SHELL for %%I in (msys2_shell.bat) do if not "%%~$PATH:I"=="" set "MSYS2_SHELL=%%~$PATH:I"

if not defined MSYS2_SHELL (
    echo Error: Could not find the devkitPro MSYS2 launcher. 1>&2
    echo Install the devkitPro GBA Development tools, or set 1>&2
    echo DIVERGENCE_MSYS2_SHELL to the full path of msys2_shell.cmd. 1>&2
    exit /b 1
)

:found
exit /b 0
