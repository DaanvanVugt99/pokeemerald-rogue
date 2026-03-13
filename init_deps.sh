#!/bin/sh

set -e

UNAME_S="$(uname -s)"

echo "Gathering dependencies:"

echo "Poryscript.."
if [ -d "tools/poryscript" ]
then
  echo "Skipping (Already exists)"
else
  mkdir -p tools/poryscript
  if [ "$UNAME_S" = "Darwin" ]; then
    if command -v poryscript >/dev/null 2>&1; then
      mkdir -p tools/poryscript/poryscript-macos
      cp "$(command -v poryscript)" tools/poryscript/poryscript-macos/poryscript
      chmod +x tools/poryscript/poryscript-macos/poryscript
      echo "Installed macOS poryscript from PATH."
    else
      echo "poryscript is required on macOS but was not found on PATH."
      echo "Install it from https://github.com/huderlem/poryscript and rerun ./init_deps.sh."
      exit 1
    fi
  else
    wget -O tools/poryscript/poryscript-linux.zip https://github.com/huderlem/poryscript/releases/download/3.5.2/poryscript-linux.zip
    wget -O tools/poryscript/poryscript-windows.zip https://github.com/huderlem/poryscript/releases/download/3.5.2/poryscript-windows.zip
    unzip tools/poryscript/poryscript-linux.zip -d tools/poryscript
    unzip tools/poryscript/poryscript-windows.zip -d tools/poryscript
  fi
  echo "Done."
fi

echo "UPS Patcher.."
if [ -d "tools/ups" ]
then
  echo "Skipping (Already exists)"
else
  mkdir -p tools/ups
  wget -O tools/ups/windows-arm.zip https://github.com/rameshvarun/ups/releases/download/v0.2.1/windows-arm.zip
  wget -O tools/ups/linux-arm.zip https://github.com/rameshvarun/ups/releases/download/v0.2.1/linux-arm.zip
  unzip tools/ups/windows-arm.zip -d tools/ups/windows-arm
  unzip tools/ups/linux-arm.zip -d tools/ups/linux-arm
  echo "Done."
fi

echo "Gathering dependencies finished."
