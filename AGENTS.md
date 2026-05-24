# AGENTS.md

This file documents practical repo-specific guidance for coding agents and contributors.

## Environment Baseline

- Project: Pokemon Emerald Rogue: Divergence (`pokeemerald-rogue`).
- Primary test launcher: `./scripts/launch_build_test.sh`.
- Toolchain expected: `devkitARM` (`arm-none-eabi-*` in `PATH`).
- For interactive test ROM runs: `mgba` in `PATH`.
- For headless checks: `mgba-rom-test` in `PATH` (used via `make check` + hydra wrapper).

## Project Direction

- Treat `main` as the authoritative standalone Divergence development branch.
- Preserve Pokemon Emerald Rogue: Divergence as its own project identity rather than a temporary feature branch of upstream Emerald Rogue.
- Prioritize expressive run variety, readable battle feedback, and deliberate balance/content choices.
- Keep upstream compatibility where it helps, but do not inherit upstream content or behavior blindly.
- Prefer changes that make runs clearer, more replayable, and more distinct without losing the Emerald Rogue foundation.

## Design References

- Use `docs/design/pokemon_romhack_tracker.xlsx` as the general design tracker for unique abilities, Pokemon identity notes, species/form planning, and implementation status.
- Keep the tracker aligned with meaningful design changes. Code and tests remain the source of truth for shipped behavior, but the tracker should capture design intent before and during implementation.

## Git Workflow

- Work on `main` by default unless the user explicitly asks for a separate branch.
- Before edits, check `git status --short --branch` and preserve unrelated user changes.
- Keep commits focused and use imperative commit messages.
- Do not delete inherited branches or tags such as `origin/expansion-abilities`, `origin/vanilla`, `EX-v*`, or `vanilla-v*` unless explicitly requested.
- Treat `pokabbie` and `upstream` as read-only source remotes. Do not push to them.

## Test Commands

- Full split-suite validation:
  - `./scripts/launch_build_test.sh --check-all-suites`
- Focused suite validation:
  - `./scripts/launch_build_test.sh --check --suite ability --filter "Intimidate"`
- Legacy monolithic test ELF build:
  - `./scripts/launch_build_test.sh --build`
  - This all-in-one test ROM is near the 32 MiB linker layout limit and may fail with `tests`/`dacs` overlap. Do not treat this as the required day-to-day full validation path.
- Run in emulator UI:
  - `./scripts/launch_build_test.sh --ui`

## Risk-Based Validation

- Docs-only changes: run `git diff --check`.
- Focused code or test changes: run the relevant filtered suite first, for example `./scripts/launch_build_test.sh --check --suite ability --filter "Intimidate"`.
- Broad battle, content, engine, or release changes: run `./scripts/launch_build_test.sh --check-all-suites`.
- If validation cannot run because `devkitARM`, `mgba`, or `mgba-rom-test` is missing, record the exact missing dependency and do not report the check as passed.

## Upstream Port Policy

- Treat Pokabbie's Emerald Rogue and RHH's pokeemerald-expansion as source material for deliberate ports.
- Prefer cherry-picking or manually porting understood changes over blind merges.
- Record meaningful upstream ports in `CHANGELOG_ROGUE.md`.
- When upstream assumptions conflict with Divergence direction, preserve Divergence behavior unless the user explicitly chooses otherwise.

## Headless Check Notes

- `make check` will fail with exit `127` if `mgba-rom-test` is missing.
- Expected error text:
  - `Error: mgba-rom-test not found in PATH.`
- Fix: install `mgba-rom-test` and ensure it is resolvable by `command -v mgba-rom-test`.

## Battle Test Gaps

- `Shadow Tag` is currently awkward to validate in the black-box battle DSL.
- `RUN_AWAY(...)`, direct escape checks, and simple trainer-battle probes were not reliable enough for a canonical test.
- Prefer manual in-game verification for `Shadow Tag` interactions until a better harness pattern or AI-side mirror test is added.

## Battle Test Authoring Guidance

- Prefer robust assertions in `SCENE`:
  - Use message/state assertions for order-sensitive interactions.
  - Avoid over-constraining animation sequencing when not behavior-critical.
- Use filtered runs first for rapid iteration:
  - `--check --suite ability --filter "Exact or unique prefix"`.
- Use `--check-all-suites` for full validation instead of the legacy monolithic `--build` target.
- Canon behavior should win over existing test assumptions when they conflict.
- Do not add tests that only assert species -> unique-ability mapping (for example, "`X` is the `Y` line's unique ability"); these are low-value and redundant.

## Known Harness/Output Behavior

- Extra validation logs (e.g. quest/hub/bake verify blocks) are normal during test runs.
- Focus pass/fail decisions on the final per-test summary and failed assertion lines.
- `UniqueAbility(ABILITY_NONE)` does not reliably clear a species-default unique ability in the battle test harness.
- For control cases, prefer a neutral species without a built-in unique ability rather than trying to remove the species-default unique ability in-place.
- For stubborn `SCENE { MESSAGE(...) }` mismatches, a useful temporary debug hook is in `test/test_runner_battle.c` inside the queued-message matcher:
  - `MgbaPrintf_("Looking for: %S Found: %S\\n", event->pattern, string);`
  - Uncomment it to see the expected pattern and each candidate runtime string during headless test runs.
  - Always revert this after debugging so test output stays clean.

## macOS / clangd IDE Diagnostics Caveat

- Host-side clang/clangd may report false positives for GBA attributes/macros (for example section attributes or GBA storage macros) because the project is built for `arm-none-eabi`, not native Mach-O.
- Trust actual project builds/tests over standalone IDE diagnostics when they disagree.

## Troubleshooting Build State

- If dependency state looks stale or odd after branch switches, use tidy/clean targets and rebuild.
- Useful targets:
  - `make tidycheck`
  - `make mostlyclean`
  - `make clean`

## Item Icon Asset Notes

- Item icons are converted to 4bpp for GBA sprites, so only palette indices `0-15` survive conversion.
- Transparent pixels must use palette index `0` in the indexed PNG. Do not leave transparency on index `255`; it can render as a solid square in-game.
- Keep matching JASC `.pal` files CRLF-terminated. `gbagfx` rejects LF-only JASC palettes.

## Large Assembly File Editing

- Large assembly/script files such as `data/battle_anim_scripts.s` can cause `apply_patch` to hang or stall on otherwise small edits.
- If that happens, prefer a narrow scripted replacement with tools like `perl -0pi` after first verifying the exact surrounding text with `sed`/`rg`.
- After scripted edits to assembly animation/script files, always re-open the edited block and run a focused filtered test/build to confirm the replacement was exact.

## Versioning and Change Notes

- Use `CHANGELOG_ROGUE.md` for project-specific work on this fork.
- Leave upstream `pokeemerald-expansion` changelog history in `CHANGELOG.md` and `docs/changelogs/`.
- Treat Git tags as the release source of truth:
  - `DIV-vX.Y.Z` for new Divergence releases.
  - Existing `EX-v*` and `vanilla-v*` tags are inherited history.
- Before release work, check both:
  - `git status --short --branch`
  - `git describe --tags --always --dirty`
- Add a changelog bullet for meaningful user-facing changes, balance changes, asset updates, bug fixes, and docs-only release housekeeping.
- Do not update upstream expansion version constants as a substitute for this fork's project version.
- See `VERSIONING.md` for the full release checklist and tag policy.
