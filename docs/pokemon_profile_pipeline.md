# Divergence Pokémon Profile Pipeline

`src/data/rogue_pokemon_profiles.h` is generated from current PokeAPI and
Showdown data, followed by a small, project-owned Divergence overlay. Ordinary
game builds use the committed header and never contact upstream services.

## Commands

Run these commands from the repository root:

```sh
./scripts/generate_pokemon_profiles.sh refresh
./scripts/generate_pokemon_profiles.sh refresh --accept-removals
./scripts/generate_pokemon_profiles.sh generate
./scripts/generate_pokemon_profiles.sh verify
```

- `refresh` downloads current PokeAPI data plus Showdown's Pokédex and Gen 6-9
  competitive sets. It generates a candidate header and a readable drift
  report under `build/profile_pipeline/`.
- When a previous local cache is available, refresh compares the two normalized
  bundles. If it would remove profiles, moves, sets, redirects, or a locked
  semantic transformation match, it stops before changing tracked files.
  Review `build/profile_pipeline/refresh_report.md`, then rerun with
  `--accept-removals` only when those removals are intentional.
- A successful refresh writes the generated header and a small tracked source
  lock. The normalized upstream bundle stays in the ignored
  `build/profile_pipeline/` cache; it is not committed.
- `generate` rebuilds the header from that local cache and the tracked
  Divergence rules without network access.
- `verify` regenerates twice and checks deterministic output against the
  committed header. Normal profile validation runs during every command.

A fresh checkout can build the game from the committed header immediately. It
only needs `refresh` before using `generate` or `verify`, because the upstream
cache is intentionally local. That first refresh has no local bundle to compare
against, so review the generated-header diff directly before committing it.

## Tracked inputs

Files under
`tools/Pokabbie/PokemonDataGenerator/PokemonDataGenerator/Resources/PokemonProfiles/Pipeline/`
are the reviewable inputs:

- `profile_sources.lock.json`: source URLs and hashes for the local normalized
  bundle used to generate the committed header.
- `divergence_learnsets.json`: explicit level-up, tutor, and legacy move
  compatibility rules. This is where project-owned move distribution belongs.
- `divergence_competitive_sets.json`: intentional Rogue sets with stable IDs
  and roles. Upstream sets should not be copied here wholesale.
- `za_mega_expected_bases.json`: the 49 protected Legends Z-A Mega forms and
  the profile each form must inherit while upstream has no dedicated set.

Named C# transformations remain appropriate for semantic changes such as
typing-sensitive set rewrites, regular-Ability replacements, or complete
profile redesigns. Avoid positional set indexes: selectors must describe the
set they intend to change.

## Review workflow

After `refresh`, review both the concise report and the generated-header diff.
Upstream churn is expected; new content can flow through normally, while
removals require an explicit decision. If a removed upstream move or set is
important to Divergence, represent it as a learnset rule or curated set instead
of retaining an opaque historical snapshot.

Showdown metadata identifies Z-A Mega forms and PokeAPI provides their
learnsets. A few form-specific inheritance adapters are explicit where
Showdown's generic `baseSpecies` would erase a meaningful form distinction,
including female Meowstic, Eternal Flower Floette, and Tatsugiri patterns.
