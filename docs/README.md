# Documentation

This directory contains a mixture of current Divergence documentation and
material retained from the projects it builds upon.

## Current Divergence Documentation

- [Design notes](design/README.md) describe local planning resources used while
  developing Divergence. These notes support development, but code and tests
  remain the source of truth for implemented behavior.
- Project-wide setup, release, and change documentation lives at the repository
  root in [README.md](../README.md), [INSTALL.md](../INSTALL.md),
  [VERSIONING.md](../VERSIONING.md), and
  [CHANGELOG_ROGUE.md](../CHANGELOG_ROGUE.md).

## Inherited Upstream Documentation

- [Historical changelogs](changelogs/) record the inherited
  pokeemerald-expansion release history. They are preserved for provenance and
  should not be used for new Divergence release notes. Record Divergence changes
  in [CHANGELOG_ROGUE.md](../CHANGELOG_ROGUE.md) instead.

## Legacy Documentation

- [Legacy WSL1 setup](legacy_WSL1_INSTALL.md) is retained only for repositories
  that require the older WSL1 installation flow linked from
  [INSTALL.md](../INSTALL.md). It is not the preferred setup path for current
  Divergence development.

## Third-Party Notices

- [Random Pokemon nickname data](third_party/pokemon-nicknames.md) records the
  sources, pinned snapshots, licenses, and selection rules for the curated
  nickname pool.
- [Pokemon Showdown attribution](third_party/pokemon-showdown.md) records the
  source and license for adapted canonical ability descriptions.
