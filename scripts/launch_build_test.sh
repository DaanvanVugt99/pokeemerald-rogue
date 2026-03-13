#!/usr/bin/env bash
set -u

script_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"
cd "$repo_root" || exit 1

mode="build" # build | check | ui
test_to_run_prefix="${TESTS:-}"

usage() {
    echo "Usage: $0 [--build|--check|--ui] [--filter \"Test name prefix\"]"
    echo "  --build   Build pokeemerald-test.elf only (default)"
    echo "  --check   Build and run headless tests via 'make check'"
    echo "  --ui      Build and launch pokeemerald-test.elf in mGBA"
    echo "  --filter  Set TESTS prefix filter (same behavior as make TESTS=...)"
}

while [ $# -gt 0 ]; do
    case "$1" in
        --build)
            mode="build"
            ;;
        --check)
            mode="check"
            ;;
        --ui)
            mode="ui"
            ;;
        --filter)
            shift
            if [ $# -eq 0 ]; then
                echo "Error: --filter requires a value."
                usage
                exit 2
            fi
            test_to_run_prefix="$1"
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Error: unknown argument '$1'"
            usage
            exit 2
            ;;
    esac
    shift
done

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
echo "Mode: $mode"
if [ -n "$test_to_run_prefix" ]; then
    echo "TESTS filter: $test_to_run_prefix"
fi

exit_code=0

if [ "$mode" = "check" ]; then
    echo "Running headless tests.. [make -j$num_cores check RELEASE=0 TESTS=\"$test_to_run_prefix\"]"
    make -j"$num_cores" check RELEASE=0 TESTS="$test_to_run_prefix"
    exit_code=$?
else
    echo "Running test build.. [make -j$num_cores pokeemerald-test.elf TEST=1 RELEASE=0 TESTS=\"$test_to_run_prefix\"]"
    make -j"$num_cores" pokeemerald-test.elf TEST=1 RELEASE=0 TESTS="$test_to_run_prefix"
    exit_code=$?

    if [ $exit_code -eq 0 ] && [ "$mode" = "ui" ]; then
        if command -v mgba >/dev/null 2>&1; then
            echo "Launching mGBA with pokeemerald-test.elf..."
            mgba pokeemerald-test.elf
        else
            echo "mGBA not found in PATH; build succeeded but ROM not launched."
            exit_code=127
        fi
    fi
fi

if [ $exit_code -ne 0 ]; then
    echo "Error! [Exit code $exit_code]"
else
    echo "Success!"
fi

exit $exit_code
