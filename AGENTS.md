# AGENTS.md

This file documents practical repo-specific guidance for coding agents and contributors.

## Environment Baseline

- Project: `pokeemerald-rogue` (modern + rogue expansion build).
- Primary test launcher: `./scripts/launch_build_test.sh`.
- Toolchain expected: `devkitARM` (`arm-none-eabi-*` in `PATH`).
- For interactive test ROM runs: `mgba` in `PATH`.
- For headless checks: `mgba-rom-test` in `PATH` (used via `make check` + hydra wrapper).

## Test Commands

- Build test ELF only:
  - `./scripts/launch_build_test.sh --build`
- Run headless checks:
  - `./scripts/launch_build_test.sh --check`
- Run only tests matching prefix:
  - `./scripts/launch_build_test.sh --check --filter "Intimidate"`
- Run in emulator UI:
  - `./scripts/launch_build_test.sh --ui`

## Headless Check Notes

- `make check` will fail with exit `127` if `mgba-rom-test` is missing.
- Expected error text:
  - `Error: mgba-rom-test not found in PATH.`
- Fix: install `mgba-rom-test` and ensure it is resolvable by `command -v mgba-rom-test`.

## Battle Test Authoring Guidance

- Prefer robust assertions in `SCENE`:
  - Use message/state assertions for order-sensitive interactions.
  - Avoid over-constraining animation sequencing when not behavior-critical.
- Use filtered runs first for rapid iteration:
  - `--filter "Exact or unique prefix"`.
- Canon behavior should win over existing test assumptions when they conflict.

## Known Harness/Output Behavior

- Extra validation logs (e.g. quest/hub/bake verify blocks) are normal during test runs.
- Focus pass/fail decisions on the final per-test summary and failed assertion lines.

## macOS / clangd IDE Diagnostics Caveat

- Host-side clang/clangd may report false positives for GBA attributes/macros (for example section attributes or GBA storage macros) because the project is built for `arm-none-eabi`, not native Mach-O.
- Trust actual project builds/tests over standalone IDE diagnostics when they disagree.

## Troubleshooting Build State

- If dependency state looks stale or odd after branch switches, use tidy/clean targets and rebuild.
- Useful targets:
  - `make tidycheck`
  - `make mostlyclean`
  - `make clean`

## Large Assembly File Editing

- Large assembly/script files such as `data/battle_anim_scripts.s` can cause `apply_patch` to hang or stall on otherwise small edits.
- If that happens, prefer a narrow scripted replacement with tools like `perl -0pi` after first verifying the exact surrounding text with `sed`/`rg`.
- After scripted edits to assembly animation/script files, always re-open the edited block and run a focused filtered test/build to confirm the replacement was exact.
