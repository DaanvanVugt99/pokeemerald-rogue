@echo off
setlocal
cd /d "%~dp0.." || exit /b 1

call "%~dp0find_msys2.bat" || exit /b 1
call "%MSYS2_SHELL%" -mingw64 -here -defterm -no-start "%~dp0launch_build_release.sh" %*
exit /b %ERRORLEVEL%
