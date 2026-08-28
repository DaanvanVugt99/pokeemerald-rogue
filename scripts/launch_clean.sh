#!/usr/bin/env bash
set -u

script_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"
cd "$repo_root" || exit 1

num_cores=""
if command -v sysctl >/dev/null 2>&1; then
    num_cores="$(sysctl -n hw.ncpu 2>/dev/null || true)"
fi
if [ -z "$num_cores" ] && command -v nproc >/dev/null 2>&1; then
    num_cores="$(nproc)"
fi
if [ -z "$num_cores" ] && command -v getconf >/dev/null 2>&1; then
    num_cores="$(getconf _NPROCESSORS_ONLN 2>/dev/null)"
fi

if [ -z "${num_cores:-}" ]; then
    num_cores=4
fi

echo "Working Directory: $PWD"
echo "Running Clean.. [make clean -j$num_cores]"
make clean -j"$num_cores"
exit_code=$?

if [ $exit_code -ne 0 ]; then
    echo "Error! [Exit code $exit_code]"
else
    echo "Success!"
fi

exit $exit_code
