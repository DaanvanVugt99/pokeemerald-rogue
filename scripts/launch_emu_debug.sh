#!/usr/bin/env bash
set -u

script_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"
cd "$repo_root" || exit 1

if ! command -v mgba >/dev/null 2>&1; then
    echo "Error: mgba not found in PATH."
    exit 127
fi

echo "== Launching mGBA (gdb server) =="
echo "Launching: mgba -g pokeemerald.gba"
mgba -g pokeemerald.gba
