#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIRECTORY="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIRECTORY}/.." && pwd)"
GENERATOR_DIRECTORY="${PROJECT_ROOT}/tools/Pokabbie/PokemonDataGenerator/PokemonDataGenerator"
PROJECT_FILE="${GENERATOR_DIRECTORY}/PokemonDataGenerator.csproj"
EXECUTABLE_DIRECTORY="${GENERATOR_DIRECTORY}/bin/Release"

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 refresh [--accept-removals] | generate | verify" >&2
    exit 2
fi

case "$1" in
    refresh|generate|verify)
        ;;
    *)
        echo "Unknown command '$1'. Expected refresh, generate, or verify." >&2
        exit 2
        ;;
esac

if command -v msbuild >/dev/null 2>&1; then
    msbuild "${PROJECT_FILE}" /p:Configuration=Release /verbosity:minimal
elif command -v xbuild >/dev/null 2>&1; then
    xbuild "${PROJECT_FILE}" /p:Configuration=Release /verbosity:minimal
else
    echo "Neither msbuild nor xbuild is installed." >&2
    exit 127
fi

if ! command -v mono >/dev/null 2>&1; then
    echo "mono is not installed." >&2
    exit 127
fi

cd "${EXECUTABLE_DIRECTORY}"
export MONO_IOMAP=all
export POKEEMERALD_ROGUE_ROOT="${PROJECT_ROOT}"
exec mono PokemonDataGenerator.exe profiles "$@"
