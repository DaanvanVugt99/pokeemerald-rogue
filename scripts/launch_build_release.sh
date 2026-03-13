#!/usr/bin/env bash
set -u

script_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"
cd "$repo_root" || exit 1

if command -v sysctl >/dev/null 2>&1; then
    num_cores="$(sysctl -n hw.ncpu 2>/dev/null)"
elif command -v nproc >/dev/null 2>&1; then
    num_cores="$(nproc)"
else
    num_cores="$(getconf _NPROCESSORS_ONLN 2>/dev/null)"
fi

if [ -z "${num_cores:-}" ]; then
    num_cores=4
fi

echo "Working Directory: $PWD"
echo "Running Release Build.. [make -j$num_cores RELEASE=1]"
make -j"$num_cores" RELEASE=1
exit_code=$?

if [ $exit_code -ne 0 ]; then
    echo "Error! [Exit code $exit_code]"
else
    echo "Success!"
fi

exit $exit_code
