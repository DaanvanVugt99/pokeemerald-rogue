@echo off
setlocal
cd /d "%~dp0.." || exit /b 1

echo == Launching mGBA ==

call "%~dp0find_mgba.bat" || exit /b 1
set "ROM_PATH=%cd%\pokeemerald.gba"

if not exist "%ROM_PATH%" (
    echo Error: ROM not found: "%ROM_PATH%" 1>&2
    exit /b 1
)

echo Launching: "%MGBA_EXE%" -g "%ROM_PATH%"
start "" "%MGBA_EXE%" -g "%ROM_PATH%"
