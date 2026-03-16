#!/bin/sh

echo "Gathering dependencies:"

echo "Poryscript.."
if [ -d "tools/poryscript" ]
then
  echo "Skipping (Already exists)"
else
  mkdir -p tools/poryscript
  wget -O tools/poryscript/poryscript-linux.zip https://github.com/huderlem/poryscript/releases/download/3.5.2/poryscript-linux.zip
  unzip -o tools/poryscript/poryscript-linux.zip -d tools/poryscript

  cp tools/poryscript/poryscript-linux/command_config.json tools/poryscript/command_config.json
  cp tools/poryscript/poryscript-linux/font_config.json tools/poryscript/font_config.json

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
