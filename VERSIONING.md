# Pokemon Emerald Rogue: Divergence Versioning

Pokemon Emerald Rogue: Divergence should use a project-owned release tag:

- `DIV-vX.Y.Z` for Divergence releases.

Existing `EX-v*` and `vanilla-v*` tags are inherited release history from the
project's earlier branch structure. Keep them for provenance, but prefer
`DIV-v*` for new standalone releases.

Use semantic-version-style numbers for project releases:

- Increment `X` for save-breaking releases, large engine migrations, or major
  content milestones.
- Increment `Y` for user-facing features, major balance passes, new systems,
  or substantial content drops.
- Increment `Z` for bug fixes, asset fixes, small balance updates, or docs-only
  release housekeeping.

Minor `.0` patch numbers may be omitted only for old compatibility tags. For
new tags, prefer the full `X.Y.Z` shape so sorting and release notes stay clear.

## Current Baseline

- `main` is the authoritative standalone development branch.
- `DIV-v0.1.0` is the first standalone Divergence baseline, tagged on 2026-05-24.
- Existing `EX-v*` and `vanilla-v*` tags are inherited history only.
- Upstream work should be manually ported or cherry-picked deliberately, then recorded in `CHANGELOG_ROGUE.md`.

## Source of Truth

- Git tags are the source of truth for released builds.
- `CHANGELOG_ROGUE.md` is the source of truth for project-specific release
  notes.
- Upstream `pokeemerald-expansion` version constants remain upstream metadata
  and should not be used as this fork's project version.
- The public project name is Pokemon Emerald Rogue: Divergence.

## Routine Workflow

1. Before starting a focused work item, check:

   ```sh
   git status --short --branch
   git describe --tags --always --dirty
   ```

2. During development, keep commits focused and write commit messages in
   imperative mood, for example `Fix evil team hideout palettes`.

3. For every meaningful user-facing change, add one bullet to
   `CHANGELOG_ROGUE.md` under `Unreleased`.

4. Before release, replace `Unreleased` entries with a tagged section:

   ```md
   ## DIV-vX.Y.Z - YYYY-MM-DD
   ```

5. Validate based on risk:

   - Docs-only changes: `git diff --check`.
   - Focused code or test changes: run the relevant filtered suite.
   - Broad battle, content, engine, or release changes: run the full split-suite launcher.

   ```sh
   ./scripts/launch_build_test.sh --check-all-suites
   ```

6. Create annotated tags for release builds:

   ```sh
   git tag -a DIV-vX.Y.Z -m "DIV-vX.Y.Z"
   git push origin DIV-vX.Y.Z
   ```

## Pre-Release Checklist

- Working tree is clean, except for intentional generated artifacts.
- `CHANGELOG_ROGUE.md` has a dated release section.
- The release tag points at the exact commit used to build the ROM.
- Test results are recorded in the release notes.
- Known manual-only checks are called out explicitly.
