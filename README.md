# Pokemon Emerald Rogue: Divergence

Pokemon Emerald Rogue: Divergence is a standalone fork of Pokemon Emerald
Rogue focused on expressive run variety, Gen 1-3-first content direction, and
expanded battle identity through custom abilities, weather, terrain, items,
maps, and quality-of-life systems.

This repository is now treated as its own project. Upstream Emerald Rogue and
pokeemerald-expansion changes may still be ported when useful, but they are
inputs to this project rather than the project identity itself.

## Project Direction

- First playable milestone scope prioritizes Gen 1-3 content and an
  Emerald/Hoenn feel.
- Engine work may remain compatible with later generations where that keeps
  future options open.
- Unique abilities and battle feedback should make Pokemon feel meaningfully
  distinct during a run.
- Balance and content rollout should favor readable, replayable runs over
  simply enabling every available upstream feature.

## Current Development Line

- Active branch: `expansion-abilities`
- Project changelog: [CHANGELOG_ROGUE.md](CHANGELOG_ROGUE.md)
- Versioning policy: [VERSIONING.md](VERSIONING.md)
- Agent/contributor guidance: [AGENTS.md](AGENTS.md)

At the time project versioning was cleaned up, `expansion-abilities` was ahead
of `origin/expansion-abilities` and described from `EX-v2.0`. The existing
`EX-v2.1` tag is not an ancestor of the active branch, so the next release line
should explicitly decide whether to merge, cherry-pick, or supersede that tag.

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
