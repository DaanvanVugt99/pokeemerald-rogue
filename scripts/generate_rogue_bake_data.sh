#!/usr/bin/env bash
set -euo pipefail

mode="${1:-generate}"

if [[ "${mode}" != "generate" && "${mode}" != "--check" ]]; then
    echo "Usage: $0 [generate|--check]" >&2
    exit 2
fi

script_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"
tool_dir="${repo_root}/tools/Pokabbie/PokemonQueryBaker/PokemonQueryBaker"
build_dir="${repo_root}/build/rogue_bake_data"
output_path="${repo_root}/src/data/rogue_bake_data.h"
temp_dir="$(mktemp -d "${TMPDIR:-/tmp}/rogue-bake-data.XXXXXX")"
backup_path="${temp_dir}/rogue_bake_data.h"
restore_on_exit=0

cleanup() {
    if [[ "${restore_on_exit}" -eq 1 && -f "${backup_path}" ]]; then
        cp "${backup_path}" "${output_path}"
    fi
}
trap cleanup EXIT

if [[ "${mode}" == "--check" ]]; then
    cp "${output_path}" "${backup_path}"
    restore_on_exit=1
fi

mkdir -p "${build_dir}"

c_compiler="${CC:-}"
if [[ -z "${c_compiler}" ]]; then
    for candidate in clang gcc cc; do
        if command -v "${candidate}" >/dev/null 2>&1; then
            c_compiler="${candidate}"
            break
        fi
    done
fi

cxx_compiler="${CXX:-}"
if [[ -z "${cxx_compiler}" ]]; then
    for candidate in clang++ g++ c++; do
        if command -v "${candidate}" >/dev/null 2>&1; then
            cxx_compiler="${candidate}"
            break
        fi
    done
fi

if [[ -z "${c_compiler}" || -z "${cxx_compiler}" ]]; then
    echo "Error: a host C and C++ compiler are required (Clang or GCC)." >&2
    exit 1
fi

common_flags=(
    -DROGUE_BAKING=1
    -DROGUE_EXPANSION=1
    -Wno-visibility
    -iquote "${tool_dir}/Inc"
    -iquote "${repo_root}/include"
    -iquote "${repo_root}/src"
)

"${c_compiler}" -std=c11 "${common_flags[@]}" \
    -c "${tool_dir}/Src/Data.c" \
    -o "${build_dir}/Data.o"
"${c_compiler}" -std=c11 "${common_flags[@]}" \
    -c "${repo_root}/src/rogue_baked.c" \
    -o "${build_dir}/rogue_baked.o"
"${cxx_compiler}" -std=c++17 -Wno-c++11-narrowing -Wno-deprecated "${common_flags[@]}" \
    -c "${tool_dir}/Main.cpp" \
    -o "${build_dir}/Main.o"
"${cxx_compiler}" -std=c++17 -Wno-c++11-narrowing -Wno-deprecated "${common_flags[@]}" \
    -c "${tool_dir}/Src/BakeHelpers.cpp" \
    -o "${build_dir}/BakeHelpers.o"
"${cxx_compiler}" \
    "${build_dir}/Main.o" \
    "${build_dir}/BakeHelpers.o" \
    "${build_dir}/Data.o" \
    "${build_dir}/rogue_baked.o" \
    -o "${build_dir}/PokemonQueryBaker"

(
    cd "${tool_dir}"
    "${build_dir}/PokemonQueryBaker"
)

if [[ "${mode}" == "--check" ]]; then
    if ! cmp -s "${backup_path}" "${output_path}"; then
        echo "src/data/rogue_bake_data.h is stale. Run make rogue-bake-data or make species-report." >&2
        exit 1
    fi
fi
