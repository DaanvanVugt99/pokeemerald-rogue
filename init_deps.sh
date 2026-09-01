#!/bin/sh
set -eu

poryscript_version="3.5.2"
host_os="$(uname -s)"

download_file() {
  source_url="$1"
  destination_path="$2"

  if command -v curl >/dev/null 2>&1; then
    curl -fsSL "$source_url" -o "$destination_path"
  elif command -v wget >/dev/null 2>&1; then
    wget -q -O "$destination_path" "$source_url"
  else
    echo "Error: curl or wget is required to download dependencies." >&2
    exit 1
  fi
}

extract_zip() {
  archive_path="$1"
  destination_dir="$2"

  mkdir -p "$destination_dir"
  if command -v unzip >/dev/null 2>&1; then
    unzip -q -o "$archive_path" -d "$destination_dir"
  elif command -v python3 >/dev/null 2>&1; then
    python3 -m zipfile -e "$archive_path" "$destination_dir"
  else
    echo "Error: unzip or Python 3 is required to extract dependencies." >&2
    exit 1
  fi
}

echo "Gathering dependencies:"
echo "Poryscript.."

case "$host_os" in
  Darwin)
    poryscript_archive="poryscript-mac.zip"
    poryscript_archive_dir="poryscript-mac"
    poryscript_install_dir="tools/poryscript/poryscript-macos"
    ;;
  Linux)
    poryscript_archive="poryscript-linux.zip"
    poryscript_archive_dir="poryscript-linux"
    poryscript_install_dir="tools/poryscript/poryscript-linux"
    ;;
  *)
    echo "Error: init_deps.sh does not support host '$host_os'." >&2
    exit 1
    ;;
esac

poryscript_binary="$poryscript_install_dir/poryscript"
if [ -x "$poryscript_binary" ] && [ -f "tools/poryscript/command_config.json" ]; then
  echo "Skipping (Already installed)"
else
  dependency_temp_dir="$(mktemp -d "${TMPDIR:-/tmp}/pokeemerald-rogue-deps.XXXXXX")"
  trap 'rm -rf "$dependency_temp_dir"' EXIT HUP INT TERM

  poryscript_url="https://github.com/huderlem/poryscript/releases/download/$poryscript_version/$poryscript_archive"
  download_file "$poryscript_url" "$dependency_temp_dir/$poryscript_archive"
  extract_zip "$dependency_temp_dir/$poryscript_archive" "$dependency_temp_dir"

  mkdir -p "$poryscript_install_dir"
  cp "$dependency_temp_dir/$poryscript_archive_dir/poryscript" "$poryscript_binary"
  chmod +x "$poryscript_binary"
  cp "$dependency_temp_dir/$poryscript_archive_dir/command_config.json" tools/poryscript/command_config.json
  cp "$dependency_temp_dir/$poryscript_archive_dir/font_config.json" tools/poryscript/font_config.json

  rm -rf "$dependency_temp_dir"
  trap - EXIT HUP INT TERM
  echo "Done."
fi

echo "UPS Patcher.."
if [ "$host_os" != "Linux" ]; then
  echo "Skipping (Not required on $host_os)"
elif [ -f "tools/ups/windows-arm/ups.exe" ] && [ -x "tools/ups/linux-arm/ups" ]; then
  echo "Skipping (Already exists)"
else
  mkdir -p tools/ups/windows-arm tools/ups/linux-arm
  download_file "https://github.com/rameshvarun/ups/releases/download/v0.2.1/windows-arm.zip" "tools/ups/windows-arm.zip"
  download_file "https://github.com/rameshvarun/ups/releases/download/v0.2.1/linux-arm.zip" "tools/ups/linux-arm.zip"
  extract_zip "tools/ups/windows-arm.zip" tools/ups/windows-arm
  extract_zip "tools/ups/linux-arm.zip" tools/ups/linux-arm
  chmod +x tools/ups/linux-arm/ups
  echo "Done."
fi

echo "Gathering dependencies finished."
