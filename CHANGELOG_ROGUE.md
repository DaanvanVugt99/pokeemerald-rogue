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

Current local status at the time this changelog was introduced:

- Branch: `expansion-abilities`
- Remote status: 3 commits ahead of `origin/expansion-abilities`
- Nearest reachable release tag: `EX-v2.0`
- `EX-v2.1` exists locally but is not an ancestor of this branch

### Changed

- Kept diverse boss teams anchored to their primary type.
- Updated held-item flow to allow going to held items from give item.

### Fixed

- Fixed evil team hideout tiles and grunt palettes.

### Docs

- Added project-specific versioning and changelog guidance.
- Reframed the project README and marketing overview around Pokemon Emerald
  Rogue: Divergence as a standalone project identity.

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
