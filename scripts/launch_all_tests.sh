#!/bin/zsh

# Change to the root of your project
cd "$(dirname "$0")/../"

echo "== Launching mGBA =="

# Path to mGBA GUI binary on macOS (adjust if different)
MGBA_PATH="/Applications/mGBA.app/Contents/MacOS/mGBA"
ROM_PATH="$PWD/pokeemerald-test.elf"

echo "Launching: \"$MGBA_PATH\" \"$ROM_PATH\""
open -a "$MGBA_PATH" "$ROM_PATH"
