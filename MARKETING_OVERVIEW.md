# Pokemon Emerald Rogue: Divergence Work Overview

This document summarizes the major work completed for Pokemon Emerald Rogue:
Divergence, with an emphasis on player-facing changes that are useful for forum
posts, Reddit updates, release notes, or progress writeups.

## High-Level Summary

Pokemon Emerald Rogue: Divergence is a large expansion-focused fork of Emerald
Rogue that adds a broad custom ability system, modern battle/UI feedback, new
weather and terrain interactions, additional maps and encounter content,
quality-of-life features, and a large amount of stability/test coverage.

Relative to `upstream/expansion-dev`, the branch currently contains roughly:

- 336 authored commits by Daan on this branch
- 1,223 changed files
- 97,854 insertions and 11,629 deletions
- 660 changed test files
- 542 added or expanded ability constants
- 223 changed graphics/assets files
- 127 changed source files and 78 changed headers

The work is not just new content; it also includes battle-system integration, AI support, generated assets, UI polish, map scripting, test infrastructure, and bug-fix passes.

## Main Feature Pillars

### 1. Large Custom Ability Expansion

The biggest theme of the branch is a large expansion of unique and reworked abilities across many generations of Pokemon.

Highlights include:

- A large catalog of new unique abilities for Gen 6, Gen 7, Gen 8, Gen 9, Ultra Beasts, Paradox Pokemon, legendary Pokemon, champions, and special forms.
- Battle-script implementations for many abilities rather than simple stat edits.
- New ability activation messages and popups.
- Ability assignments to species and forms.
- AI handling for custom ability behavior, including move-absorbing and unique effect checks.
- Fixes for edge cases such as ability popups showing on the wrong battler, multi-target scripts losing the original target, and abilities triggering during unintended move states.

Examples visible in the commit history include:

- Meltdown and Fallen Skies
- Cliffside
- Tidal Dawn, Mind Protocol, and Strange Guest
- Avalanche Hide, Heat Protocol, and Surge Protocol
- Storm Glider, Flux Instinct, and Drive Protocol
- Photokeratitis, Swallowed, and Chef's Pick
- Tripwire and Crossed Wires
- Duality, Dirty Tricks, Tumbleweed, Drop Off, Starmobile, and Carving Rush
- Barbed Monsoon and Spring Command
- Crown of Fangs and Gridlock
- Bountiful, Frozen, Dread, Tundra, and Astral Reign
- Royal Advance and Royal Guard
- Duelist and Sword and Board
- Night Hunter and Wandering Hunter
- Toxisphere and Smog Refinery
- Fossil Memory
- Raid Leader, Formation, and Snowstorm
- Space Rift, Dark Dimension, and Temporal Lock changes
- Ultra Beast and shrine-themed abilities

### 2. Existing Ability Reworks

Alongside the new unique abilities, the branch reworks many existing abilities to make them more interesting in Emerald Rogue's format.

Examples include:

- Suction Cups, Immunity, Insomnia, Levitate, and Shield Dust changes
- Early Bird speed boost behavior
- Water Veil Aqua Ring interaction
- Tiered Swarm/Blaze/Torrent/Overgrow scaling
- Rock Head interactions with Normal-to-Rock conversion
- Updated Klutz, Super Luck, Leaf Guard, Anger Point, Solar Power, Stamina, Hydration, Anticipation, Filter/Solid Rock, and Forewarn behavior
- Booster Energy item mechanics

This is useful to communicate as a design goal: abilities are being treated as meaningful run-defining mechanics, not just passive flavor.

### 3. Battle UI and Feedback Improvements

The branch adds several battle-facing UI improvements intended to make the expanded mechanics easier to read during play.

Highlights include:

- Rogue battle HUD with field effects and stat-view overlay.
- Healthbox type indicator sprites.
- Type effectiveness messaging for 4x and 0.25x damage.
- Fixes for type indicator visibility, doubles positioning, Safari behavior, and frame handling.
- Battle overlay and hitmarker cleanup after confirmed move selection.
- Sprite allocation safety checks for battle UI.
- Unique ability page in the Pokedex entry viewer.
- Summary screen page order improvements and unique ability rendering fixes.

This matters for marketing because it makes the feature work feel usable: players can actually see and understand the new systems in battle.

### 4. Weather, Terrain, and Form Content

The branch adds new environmental mechanics and form support that expand battle variety.

Highlights include:

- Eclipse weather.
- Acid Rain weather.
- Plain Terrain.
- Infested Terrain.
- Acid Rain palettes and drop visuals.
- Eclipse weather interactions, including Dark-type immunity for Ghost-type Pokemon.
- Castform forms for Sandy, Eclipse, and Acid Rain.
- Weather and terrain tests.
- New move/effect interactions for Weather Ball, Moonlight, terrain behavior, and related mechanics.

This gives the project a strong "new run conditions and battle texture" angle for public posts.

### 5. Items, Shops, Rewards, and Quality of Life

There is a substantial amount of work on player convenience and run flow.

Highlights include:

- TM/HM party preview in shop menus.
- TM shop menu with party compatibility preview.
- Tutor move progression system for run and hub modes.
- Dynamic TR item support, pricing, descriptions, icons, and tests.
- Held item swapping from the party menu.
- Ability modifier item support in the party menu.
- Quick Heal menu features.
- Item pickup automation.
- Mega Evolution and Z-Crystal quest rewards.
- Support for item IDs above 1023 in held item storage.
- Run Mega Stone gating by active form items.
- Preserve running-toggle state across rogue run reset.
- Hub area relocation feature.

These are good for forum posts because they show that the work is not only about mechanics; it also improves the day-to-day feel of playing runs.

### 6. Maps, Encounters, Teams, and Visual Content

The branch adds and fixes a significant amount of map and encounter content.

Highlights include:

- Four new encounter maps.
- Team Plasma and Team Flare content.
- Battle Tower encounter room.
- Mountain outline rendering in adventure paths.
- Evil team hideout tile fixes.
- Team Plasma, Flare, Galactic, and Neo Plasma grunt palette and overworld fixes.
- Evil team node sprite fixes.
- New evil front sprites.
- Trainer back pics for Calem and Serena.
- Overworld sprites for Therian genies and Kyurem forms.
- Shared facility asset updates.

This is one of the more visual categories and should be prioritized in posts if screenshots or GIFs are available.

### 7. Balance and Run Structure

The branch includes balancing and systems work that affects how runs feel across difficulties and progression.

Highlights include:

- Trainer difficulty and gimmick item rebalance.
- Boss team diversity fix to keep teams anchored to primary type.
- Shop difficulty tracking for rogue runs.
- Subseed system.
- Roamers moved later in encounter ordering.
- Safari/Pie pokeblock cost adjustments and dismiss options.
- Battle item regression fixes.

This is useful to present as "run variety with more controlled balance," especially if players are worried that many new mechanics could make the game chaotic.

### 8. Stability, Tests, and Maintenance

The branch contains a lot of behind-the-scenes work that makes the feature set more credible.

Highlights include:

- 660 changed test files.
- TEST_SUITE filtering and `--check-all-suites` support.
- Battle tests for new weather, terrain, move effects, item effects, unique abilities, and type effectiveness messaging.
- Form table validation fixes.
- Battle item regression fixes.
- Ported upstream 2.1 stability fixes.
- Fixes from Mocha and Lemon battle-fix batches.
- Sprite allocation safety checks.
- Current local uncommitted fix: stopping lingering low-HP music when beginning a new Rogue run.

This is worth mentioning briefly in public posts, because it shows the branch has been tested and iterated on rather than being only experimental content.

## Suggested Public-Facing Positioning

Possible short positioning:

> I have been working on Pokemon Emerald Rogue: Divergence, a standalone Emerald
> Rogue fork focused on making Pokemon feel more distinct across a run. The main
> hook is a huge custom ability pass, backed by new battle UI feedback, weather
> and terrain mechanics, encounter content, quality-of-life improvements, and a
> lot of test coverage.

Possible longer positioning:

> The goal of Pokemon Emerald Rogue: Divergence is to make Emerald Rogue runs
> feel more varied and expressive without losing readability. A lot of the work
> is centered around unique Pokemon abilities, but I have also added UI support
> so those mechanics are easier to understand mid-battle, new weather and terrain
> conditions, more encounter/map content, better shop and item workflows, balance
> fixes, and a large amount of automated test coverage.

## Good Screenshot/GIF Targets

For marketing posts, the strongest visuals are likely:

- Battle HUD showing field effects and stat overlay.
- Healthbox type indicators.
- Unique ability page in the Pokedex.
- TM shop party compatibility preview.
- New weather visuals: Eclipse and Acid Rain.
- Castform Sandy/Eclipse/Acid Rain forms.
- New encounter maps and Battle Tower room.
- Team Plasma/Flare/Galactic/Neo Plasma NPCs and hideout content.
- Mega/Z reward flow if it is visually clear.
- Any flashy new ability activation with custom popup/message.

## Suggested Forum/Reddit Outline

1. Short intro: what the branch is and why it exists.
2. Main hook: large custom unique ability expansion.
3. Show 3-5 concrete ability examples with screenshots or battle clips.
4. Explain readability improvements: battle HUD, type indicators, Pokedex ability page.
5. Show new weather/terrain and form content.
6. Mention new maps, teams, and encounter content.
7. Mention QoL: TM preview, tutor progression, Quick Heal, item pickup, held item workflows.
8. Close with current status: playable/work-in-progress, looking for feedback, and what kind of feedback would help most.

## Notes Before Posting

- The branch currently has two modified files that are not committed yet:
  - `src/battle_gfx_sfx_util.c`
  - `src/rogue_controller.c`
- The current branch is 3 commits ahead of `origin/expansion-abilities`.
- Before presenting this as a release-ready feature set, it would be worth doing one pass to decide which features are stable, which are experimental, and which should be described as work-in-progress.
