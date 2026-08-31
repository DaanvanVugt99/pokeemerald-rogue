#!/usr/bin/env bash
set -u

script_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"
cd "$repo_root" || exit 1

if [ -n "${BUILD_JOBS:-}" ]; then
    num_cores="$BUILD_JOBS"
else
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
    if [ "$num_cores" -gt 4 ]; then
        num_cores=4
    fi
fi

case "$num_cores" in
    ''|*[!0-9]*|0)
        echo "Error: BUILD_JOBS must be a positive integer." >&2
        exit 2
        ;;
esac

echo "Working Directory: $PWD"
echo "Running Debug Build.. [make -j$num_cores RELEASE=0]"
make -j"$num_cores" RELEASE=0
exit_code=$?

if [ $exit_code -ne 0 ]; then
    echo "Error! [Exit code $exit_code]"
else
    echo "Success!"
fi

exit $exit_code
