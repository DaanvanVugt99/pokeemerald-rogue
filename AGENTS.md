# AGENTS.md

- Work on `main` by default. Check `git status --short --branch` before editing
  and preserve user changes. Stage, commit, push, or switch branches only when
  asked. Treat `pokabbie` and `upstream` as read-only.
- Use [README.md](README.md) for setup and test commands. Run `git diff --check`,
  an incremental ROM build when compiled inputs change, and the narrowest
  relevant test. Confirm the intended test prints `PASS`. Reserve full split
  suites for releases, CI, cross-cutting changes, or explicit requests.
- Avoid `make clean` and `make mostlyclean` for stale builds. They remove tracked
  and ignored graphics that incremental builds may be unable to recreate.
  Rebuild the affected object or use `make tidycheck` for stale test objects.
- Edit `.pory` sources when present. Preserve Porymap-authored layouts and borders
  when running hub generators. Use the relevant generator's `--check` mode for
  hub changes. Keep every walkable exit lane warped and center corridor arrivals
  in `RogueHub_GetWarpArrivalPosition`.
- Edit Pokémon profile inputs in `tools/Pokabbie/PokemonDataGenerator/`. Run
  `./scripts/generate_pokemon_profiles.sh generate` and `verify`. Use `refresh`
  when updating upstream inputs or transformation match counts in the lock.
- Regenerate species stat, type, or regular Ability changes with
  `make species-report` and verify with `make check-species-report`. Keep
  approved rebalance changes aligned with `tools/data/rebalance_manifest.json`
  and run `python3 scripts/check_rebalance_manifest.py --require-complete`.
  Edit authoritative inputs for generated headers and reports.
- Put ordinary Ability tests in `ability`, unique Ability tests in
  `ability_unique`, and Rogue integration tests in `rogue`.
- Record meaningful changes in `CHANGELOG_ROGUE.md`. Preserve released save
  compatibility. Use [VERSIONING.md](VERSIONING.md) for release work.
