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
- Added the Lechonk line's Aroma Trail unique ability.
- Added Eelevate and Fire Mane for the 1.1 champion Mega Evolution ability set.
- Added the Hisuian Voltorb line's Rolling Start unique ability.
- Ported upstream battle sprites and palettes for newly added Mega Evolutions
  that were still using placeholder battle art.

### Changed

- Reworked Hub Safari acquisition into a Pokéblock purchase economy: Safari
  sign and legendary cave Pokémon now show typed Pokéblock costs, preserve
  stored run traits, and grant low-level egg/baby species to the party or PC.
- Added an affordability marker to purchasable Hub Safari and legendary cave
  Pokémon in the Safari selection grid.
- Changed Safari purchase pricing to use matching typed Pokéblocks, split
  dual-type costs across both types, add Shiny Pokéblocks for shiny Pokémon,
  and mildly scale prices by BST, evolution, legendary status, and individual
  variation.
- Added random Pokéblock drops to run boss rewards, with mostly typed
  Pokéblocks, occasional stat Pokéblocks, and rare Shiny Pokéblocks.
- Replaced Market Berry Blender minigames with direct Pokéblock crafting that
  uses the Mart-style quantity UI, dynamic recipe lists, and matching berries
  as item currency.
- Removed the Berry Blender hub upgrade path and made Pokéblock crafting
  available through the Market mixer without unlocking duplicate NPCs.
- Added bundled Pokéblock quest rewards, usually in 5-count chunks, including
  progression bundles, Safari-themed bundles, and 10 matching typed Pokéblocks
  from each Type Master quest.
- Removed Shiny Pokéblocks from Treat Shop stock so they remain tied to run
  drops, quest rewards, and Starf Berry crafting.
- Allow standard adventures to start with up to six party Pokémon, while
  blocking duplicate species from entering the same run-start party.
- Reworked Krookodile's Bounty unique ability so opposing switch-outs are
  punished with Pursuit while keeping the KO money payout.
- Reworked Dondozo's Swallowed unique ability to trigger Stockpile after its
  first successful Rest each switch-in instead of after biting moves.
- Ported upstream battle heap optimization so battler sprite graphics allocate
  two mon frames per battler instead of four.
- Fast Path segments with a pending rival battle now guarantee at
  least one starting Route or Honey Tree option for pre-rival leveling.
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
- Renamed the Bulbasaur line's Rootsnare unique ability to Vine Lash.
- Replaced the Charizard line's Updraft unique ability with Thermal Lift, which
  makes its first Fire-type move each battle also use Sky Attack.
- Reworked the Clefable line's Moonlight unique ability to restore HP when any
  Pokemon is healed.
- Reworked the Alolan Meowth line's unique ability to Royal Treatment, healing
  incoming allies after Parting Shot.
- Changed Trash Alchemy to only trigger in trainer battles, and reduced its
  healing from 1/8 HP to 1/16 HP.
- Added clearer healing animations to Trash Alchemy and shared ability HP
  restoration feedback.
- Reworked Alolan Raichu's unique ability to Brain Surf, letting Electric and
  Psychic moves pivot into the other terrain while raising Sp. Atk once per
  switch-in.
- Reworked the Alolan Sandshrew line's unique ability to Snowplow, making its
  first Ice or Steel move each switch-in start Snow and follow with Rapid Spin.
- Reworked the Alolan Diglett line's unique ability to Fault Finder, making its
  first Ground move each switch-in use Magnet Rise and its first Steel move set
  Stealth Rock.
- Reworked the Alolan Rattata line's unique ability to Nest Boss, making its
  STAB moves never miss and failed Sucker Punch use Swords Dance.
- Reworked the Alolan Vulpix line's unique ability to Lanakila Law, giving
  Encore and Hypnosis priority and perfect accuracy in Snow.
- Made Exotic Unique Pokemon reward-only, removing them from tracker spawns
  and removing the Exotic tracker upgrade from lab progression.
- Made tracked Unique Pokemon guaranteed to appear as Unique when their species
  is encountered in a valid room.
- Reworked the Snubbull line's Brutal Charge unique ability to follow foe
  Defense drops with a 40 BP Headbutt while keeping its first-turn Speed boost.
- Reworked the Pidgeot line's Strong Winds unique ability to set Tailwind when
  a foe is forced out instead of on switch-in.
- Reworked the Litleo line's Battlecry unique ability to use an empowered Growl
  only after an ally fainted last turn.
- Added random unique abilities to Legendary dynamic Unique Pokemon, with the
  ability encoded in the existing custom mon ID.
- Updated the 1.1 champion Mega Evolution ability assignments.
- Retired the obsolete Embargo Curse and Species Curse items now that Bag
  Clause and Species Clause are handled by run config toggles.
- Fixed Rotom form appliance abilities being assigned as normal abilities
  instead of hidden abilities.

### Fixed

- Forced EXP trainer helper battles to use 4x battle speed regardless of the
  trainer battle speed option.
- Applied wild form-family encounter rules to catching contest spawns so they
  match normal route encounter filtering.
- Fixed run boss Pokéblock rewards being added during the adventure, causing
  them to be lost when the run bag was wiped on return to the hub.
- Fixed Ariados and Spinarak's Web Trap unique ability so any opposing Speed
  stage drop can trigger a queued random web move.
- Fixed attacker-side move-end unique abilities, including Vicejaw and Handywork,
  failing after the target's contact ability retargeted battle script state first.
- Fixed attacker-side move-end unique abilities, including Windsurge, being
  attributed to the target when move-end script state changed the attacker first.
- Fixed the Game Modes submenu being hidden on early saves, which made Fast Path
  unavailable from the Config Lab.
- Ported upstream item-PC deposit fix so Rogue-generated normal items such as
  Pokeblocks are not incorrectly treated as important items.
- Ported upstream shiny palette fixes for Mothim and Galarian Cursola.
- Ported upstream shop new-stock indicator fix so General, Ball, Rare Held Item,
  and Treat shops can show the sparkle icon when their stock expands.
- Ported upstream Item Courier seeding fix so courier inventory stays stable
  within the same adventure room.
- Ported upstream tutor move progression fix so badge rewards unlock run tutor
  move buckets at the intended cap.
- Ported upstream auto-pickup fix for Rare Candy and Master Ball overworld
  pickups that could be missed when object events overlapped.
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
