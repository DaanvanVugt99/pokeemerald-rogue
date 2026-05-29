# Pokemon Emerald Rogue: Divergence Changelog

This file tracks project-specific work on this fork. Keep upstream
`pokeemerald-expansion` changelog entries in `CHANGELOG.md` and
`docs/changelogs/`.

Format follows Keep a Changelog loosely:

- `Added` for new features, content, assets, and tooling.
- `Changed` for behavior, balancing, flow, or data changes.
- `Fixed` for bug fixes.
- `Removed` for deleted features, content, or compatibility.
- `Docs` for documentation-only changes.

## Unreleased

### Added

- Ported upstream Fast Path game mode, replacing the old Experimental pacing
  experiment with shorter alternating adventure segments and stricter economy
  pressure.
- Ported upstream C-Gear/Config Lab sparkle highlights for newly unlocked or
  newly added settings.
- Added Iron Crown's Singularity Array unique ability.
- Added Gouging Fire's Primal Crest and Raging Bolt's Primal Thunder unique
  abilities.
- Added Terapagos's World Prism unique ability.
- Added Pecharunt's Puppet Master unique ability.

### Changed

- Ported upstream Gauntlet pacing so Gauntlet and Rainbow Gauntlet runs start
  incoming Pokemon at level 100.
- Ported upstream Average trainer pacing so the late Gyms and Rival use larger
  teams, and all Elite Four fights use full teams.
- Ported upstream battle economy and charm balance updates: Amulet Coin battle
  winnings are capped at 15,000, Endure Charm is 60%, and Adaptability Curse is
  30%.
- Ported upstream Potion Cauldron crafting to the Mart UI flow, allowing bulk
  potion crafting with Oran Berries.
- Assigned implemented design-doc unique abilities to additional Mega forms.

### Fixed

- Ported upstream regional-dex display fix so seen regional forms only replace
  base species entries when that form is active in the selected dex.
- Ported upstream riding stability fixes for ride-mon sprite allocation,
  remount state reset, and safer ride cycling.
- Ported upstream unique-mon lab pool fix so exotic unique Pokemon must opt in
  to default spawning.
- Fixed Gauntlet entry accepting the full six-Pokemon Hub party through the
  active adventure entrance flow.
- Fixed Fast Path sometimes skipping the early evil-team encounter when all
  reset-path candidate difficulties were occupied by legends.
- Fixed trainer team generation assertions when recoverable species-form fallbacks
  are selected.
- Fixed Colony Guardian freezing when switch-in Intimidate triggered it.

### Docs

- Added the Pokemon romhack tracker as the general design reference for unique
  abilities and Pokemon notes.

## DIV-v0.1.0 - 2026-05-24

First standalone Divergence baseline.

### Changed

- Kept diverse boss teams anchored to their primary type.
- Updated held-item flow to allow going to held items from give item.

### Fixed

- Fixed evil team hideout tiles and grunt palettes.

### Docs

- Added project-specific versioning and changelog guidance.
- Reframed the project README and marketing overview around Pokemon Emerald
  Rogue: Divergence as a standalone project identity.

### Validation

- `./scripts/launch_build_test.sh --check-all-suites`
  - Completed successfully.
  - Existing suite output still included known failing, TODO, and one assumption-failed count in `test/battle/move_effect/clanging_scales.c`.

## Release Notes Template

Copy this section when cutting a new release.

## DIV-vX.Y.Z - YYYY-MM-DD

### Added

- ...

### Changed

- ...

### Fixed

- ...

### Removed

- ...

### Docs

- ...

### Validation

- `./scripts/launch_build_test.sh --check-all-suites`
- Manual checks: ...
