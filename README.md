 # Pokemon Emerald Rogue: Divergence

Pokemon Emerald Rogue: Divergence is a standalone fork of Pokemon Emerald
Rogue focused on expressive run variety and expanded battle identity through
custom abilities, weather, terrain, items, maps, and quality-of-life systems.

This repository is now treated as its own project. Upstream Emerald Rogue and
pokeemerald-expansion changes may still be ported when useful, but they are
inputs to this project rather than the project identity itself.

## Project Direction

- Preserve a standalone Divergence identity while keeping upstream credits
  explicit.
- Unique abilities and battle feedback should make Pokemon feel meaningfully
  distinct during a run.
- Balance and content rollout should favor readable, replayable runs over
  simply enabling every available upstream feature.
- Upstream changes may be ported deliberately when they strengthen the project.

## Current Development Line

- Active branch: `main`
- Baseline release: `DIV-v0.1.0`
- Project changelog: [CHANGELOG_ROGUE.md](CHANGELOG_ROGUE.md)
- Versioning policy: [VERSIONING.md](VERSIONING.md)
- Documentation index: [docs/README.md](docs/README.md)
- Agent/contributor guidance: [AGENTS.md](AGENTS.md)

Existing `EX-v*` and `vanilla-v*` tags and inherited branches are retained for
historical provenance. New project releases use the `DIV-vX.Y.Z` tag line.

## Build and Test

Expected tools:

- `devkitARM` with `arm-none-eabi-*` in `PATH`
- `mgba` for interactive ROM runs
- `mgba-rom-test` for headless checks

Common validation commands:

```sh
./scripts/launch_build_test.sh --check-all-suites
./scripts/launch_build_test.sh --check --suite ability --filter "Intimidate"
./scripts/launch_build_test.sh --ui
```

On Windows, use the devkitPro/MSYS2 wrappers instead:

```powershell
.\scripts\check_windows_setup.ps1 -InstallPoryscript
.\scripts\launch_build_test.bat --check-all-suites
```

For the faster WSL1 path with this checkout on `C:`, invoke the Linux launcher
directly from PowerShell:

```powershell
wsl ./scripts/launch_build_test.sh --check-all-suites
```

See [docs/windows_setup.md](docs/windows_setup.md) for the one-time setup and
the WSL1, MSYS2, mGBA, and Python configuration.

Use the split-suite validation path for normal full checks. The legacy
monolithic test ROM build can run into the 32 MiB linker layout limit and should
not be treated as the normal full-validation target.

## Release Notes

Record project-specific changes in `CHANGELOG_ROGUE.md`. Keep inherited
pokeemerald-expansion changelog history in `CHANGELOG.md` and `docs/changelogs/`.

Release tags should use:

- `DIV-vX.Y.Z` for Divergence releases

Existing `EX-v*` and `vanilla-v*` tags are inherited history and should be kept
for provenance.

## Upstream Credits

This project builds on:

- pret's `pokeemerald` decompilation project
- RHH's `pokeemerald-expansion`
- Pokabbie's `pokeemerald-rogue`

Keep upstream credits intact when distributing builds or public release notes.
