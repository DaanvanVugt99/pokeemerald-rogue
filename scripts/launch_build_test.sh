#!/usr/bin/env bash
set -u
set -o pipefail

script_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"
cd "$repo_root" || exit 1

mode="check" # build | check | ui
test_to_run_prefix="${TESTS:-}"
test_suite="${TEST_SUITE:-}"
check_all_suites=0
all_suites=(core ai ability ability_unique moves items forms rogue)

usage() {
    echo "Usage: $0 [--check-all-suites|--check|--build|--ui] [--suite SUITE] [--filter \"Test name prefix\"]"
    echo "  --check-all-suites"
    echo "            Release/CI validation: run all split test suites sequentially"
    echo "            This is intentionally explicit because it can take 30+ minutes locally."
    echo "  --check   Build and run headless tests via 'make check'"
    echo "            Requires --suite, --filter, or both for focused validation."
    echo "            Example: --check --suite ability --filter \"Some Test\""
    echo "  --suite   Compile only one test suite: core, ai, ability, ability_unique, moves, items, forms, rogue"
    echo "  --filter  Set TESTS prefix filter and compile only matching test files"
    echo "  --build   Legacy: build the monolithic all-in-one pokeemerald-test.elf only"
    echo "            This target may fail near the 32 MiB test ROM linker limit; use --check-all-suites for full validation."
    echo "  --ui      Build and launch the monolithic pokeemerald-test.elf in mGBA"
}

if [ $# -eq 0 ]; then
    echo "No validation mode selected. Refusing to start the full suite implicitly."
    echo
    usage
    exit 2
fi

while [ $# -gt 0 ]; do
    case "$1" in
        --build)
            mode="build"
            check_all_suites=0
            ;;
        --check)
            mode="check"
            check_all_suites=0
            ;;
        --ui)
            mode="ui"
            check_all_suites=0
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
        --suite)
            shift
            if [ $# -eq 0 ]; then
                echo "Error: --suite requires a value."
                usage
                exit 2
            fi
            test_suite="$1"
            ;;
        --check-all-suites)
            mode="check"
            check_all_suites=1
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

if [ "$mode" = "check" ] && [ "$check_all_suites" -eq 0 ] && [ -z "$test_suite" ] && [ -z "$test_to_run_prefix" ]; then
    echo "Error: --check requires --suite, --filter, or both."
    echo "Use --check-all-suites explicitly for release/CI validation."
    exit 2
fi

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
echo "Mode: $mode"
if [ -n "$test_suite" ]; then
    echo "TEST_SUITE: $test_suite"
fi
if [ -n "$test_to_run_prefix" ]; then
    echo "TESTS filter: $test_to_run_prefix"
fi

exit_code=0

strip_ansi() {
    perl -pe 's/\e\[[0-9;]*[[:alpha:]]//g'
}

parse_summary_count() {
    local label="$1"
    local log_file="$2"

    strip_ansi < "$log_file" \
        | awk -v label="$label" '
            $0 ~ ("^[[:space:]]*(-[[:space:]]*)?" label ":[[:space:]]*") {
                for (i = NF; i >= 1; i--) {
                    if ($i ~ /^[0-9]+$/) {
                        value = $i
                        break
                    }
                }
            }
            END {
                if (value == "")
                    value = 0
                print value
            }
        '
}

print_split_suite_summary() {
    local total_failed=0
    local total_known_failing_passed=0
    local total_passed=0
    local total_known_failing=0
    local total_todo=0
    local total_assumption_failed=0
    local total_tests=0
    local total_duration=0
    local failed_suites=0
    local i

    echo
    echo "Split-suite summary:"
    printf "  %-10s %8s %8s %8s %8s %8s %8s %8s %8s\n" "Suite" "Failed" "KF Pass" "Passed" "Known" "TODO" "Assume" "Total" "Time(s)"

    for i in "${!suite_names[@]}"; do
        printf "  %-10s %8s %8s %8s %8s %8s %8s %8s %8s\n" \
            "${suite_names[$i]}" \
            "${suite_failed[$i]}" \
            "${suite_known_failing_passed[$i]}" \
            "${suite_passed[$i]}" \
            "${suite_known_failing[$i]}" \
            "${suite_todo[$i]}" \
            "${suite_assumption_failed[$i]}" \
            "${suite_total[$i]}" \
            "${suite_durations[$i]}"

        total_failed=$((total_failed + suite_failed[$i]))
        total_known_failing_passed=$((total_known_failing_passed + suite_known_failing_passed[$i]))
        total_passed=$((total_passed + suite_passed[$i]))
        total_known_failing=$((total_known_failing + suite_known_failing[$i]))
        total_todo=$((total_todo + suite_todo[$i]))
        total_assumption_failed=$((total_assumption_failed + suite_assumption_failed[$i]))
        total_tests=$((total_tests + suite_total[$i]))
        total_duration=$((total_duration + suite_durations[$i]))

        if [ "${suite_exit_codes[$i]}" -ne 0 ]; then
            failed_suites=$((failed_suites + 1))
        fi
    done

    printf "  %-10s %8s %8s %8s %8s %8s %8s %8s %8s\n" \
        "TOTAL" \
        "$total_failed" \
        "$total_known_failing_passed" \
        "$total_passed" \
        "$total_known_failing" \
        "$total_todo" \
        "$total_assumption_failed" \
        "$total_tests" \
        "$total_duration"

    if [ "$failed_suites" -gt 0 ]; then
        echo
        echo "Suites with command failures:"
        for i in "${!suite_names[@]}"; do
            if [ "${suite_exit_codes[$i]}" -ne 0 ]; then
                echo "  - ${suite_names[$i]} exited with ${suite_exit_codes[$i]}"
            fi
        done
    fi

    if [ "$total_assumption_failed" -gt 0 ]; then
        echo
        echo "Assumption failures found; treating split-suite validation as failed."
        if [ "$exit_code" -eq 0 ]; then
            exit_code=1
        fi
    fi
}

run_make() {
    local target="$1"
    local suite="$2"

    if [ -n "$suite" ]; then
        make -j"$num_cores" "$target" RELEASE=0 TESTS="$test_to_run_prefix" TEST_SUITE="$suite"
    else
        make -j"$num_cores" "$target" RELEASE=0 TESTS="$test_to_run_prefix"
    fi
}

if [ "$check_all_suites" -eq 1 ]; then
    suite_logs_dir="$(mktemp -d "${TMPDIR:-/tmp}/pokeemerald-rogue-tests.XXXXXX")"
    trap 'rm -rf "$suite_logs_dir"' EXIT
    suite_names=()
    suite_exit_codes=()
    suite_failed=()
    suite_known_failing_passed=()
    suite_passed=()
    suite_known_failing=()
    suite_todo=()
    suite_assumption_failed=()
    suite_total=()
    suite_durations=()

    for suite in "${all_suites[@]}"; do
        suite_log="$suite_logs_dir/$suite.log"
        echo "Running headless tests.. [make -j$num_cores check RELEASE=0 TEST_SUITE=\"$suite\" TESTS=\"$test_to_run_prefix\"]"
        suite_started_at=$SECONDS
        run_make check "$suite" 2>&1 | tee "$suite_log"
        suite_exit_code=$?
        suite_duration=$((SECONDS - suite_started_at))
        if [ $suite_exit_code -ne 0 ] && { [ $exit_code -eq 0 ] || [ $suite_exit_code -gt $exit_code ]; }; then
            exit_code=$suite_exit_code
        fi

        suite_names+=("$suite")
        suite_exit_codes+=("$suite_exit_code")
        suite_failed+=("$(parse_summary_count "Tests FAILED" "$suite_log")")
        suite_known_failing_passed+=("$(parse_summary_count "KNOWN_FAILING_PASSED" "$suite_log")")
        suite_passed+=("$(parse_summary_count "Tests PASSED" "$suite_log")")
        suite_known_failing+=("$(parse_summary_count "Tests KNOWN_FAILING" "$suite_log")")
        suite_todo+=("$(parse_summary_count "Tests TO_DO" "$suite_log")")
        suite_assumption_failed+=("$(parse_summary_count "ASSUMPTIONS_FAILED" "$suite_log")")
        suite_total+=("$(parse_summary_count "Tests TOTAL" "$suite_log")")
        suite_durations+=("$suite_duration")
    done

    print_split_suite_summary
elif [ "$mode" = "check" ]; then
    echo "Running headless tests.. [make -j$num_cores check RELEASE=0 TEST_SUITE=\"$test_suite\" TESTS=\"$test_to_run_prefix\"]"
    run_make check "$test_suite"
    exit_code=$?
else
    echo "Running test build.. [make -j$num_cores pokeemerald-test.elf TEST=1 RELEASE=0 TEST_SUITE=\"$test_suite\" TESTS=\"$test_to_run_prefix\"]"
    if [ -n "$test_suite" ]; then
        make -j"$num_cores" pokeemerald-test.elf TEST=1 RELEASE=0 TESTS="$test_to_run_prefix" TEST_SUITE="$test_suite"
    else
        make -j"$num_cores" pokeemerald-test.elf TEST=1 RELEASE=0 TESTS="$test_to_run_prefix"
    fi
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

    if [ $exit_code -ne 0 ] && [ "$mode" = "build" ]; then
        echo
        echo "Note: --build is the legacy monolithic all-in-one test ROM target."
        echo "It can fail when the combined test ROM exceeds the 32 MiB linker layout,"
        echo "especially with tests/dacs overlap errors. For normal full validation, run:"
        echo "  $0 --check-all-suites"
    fi
fi

if [ $exit_code -ne 0 ]; then
    echo "Error! [Exit code $exit_code]"
else
    echo "Success!"
fi

exit $exit_code
