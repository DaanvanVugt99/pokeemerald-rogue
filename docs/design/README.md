# Design Docs

This directory holds project design references for Pokemon Emerald Rogue:
Divergence.

## Pokemon Romhack Tracker

[pokemon_romhack_tracker.xlsx](pokemon_romhack_tracker.xlsx) is a working
design notebook for Pokemon identity work, especially unique abilities and
per-Pokemon notes.

Use it as the planning source for:

- Unique ability ideas, status, complexity, and effect notes.
- Species-level typing, stat, ability, movepool, and role notes.
- Form and variant notes, including regional forms and Mega Evolutions.
- Lightweight implementation tracking before changes move into code and tests.

Current workbook structure:

- `Summary`: workbook purpose and navigation notes.
- `Lookups`: shared status, tier, complexity, and yes/no values.
- `Species`: base National Dex entries and per-species planning fields.
- `Abilities`: ability identifiers, change flags, and changed-effect notes.
- `Forms`: form and variant entries such as regional forms and Mega Evolutions.

Treat the workbook as a sketch space for intent, experiments, and notes. Code
and tests remain the source of truth for shipped behavior, and tracker cleanup
can be handled with manual review passes when useful.
