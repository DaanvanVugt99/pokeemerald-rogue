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
- Added Hisuian Samurott's Momentum unique ability, raising a random stat after
  it lands a critical hit.
- Added a new GEEF logo splash before the upstream boot credit screen.
- Added delayed slash, sparkle, and sound effects to the GEEF boot splash.
- Added an expanded Pokémon summary Info view for reading wrapped ability
  descriptions from the ability panel, which now stays open while switching
  between Pokémon.
- Added expanded canonical ability descriptions adapted from Pokémon Showdown
  for unchanged inherited abilities, with local expanded text for
  Divergence-modified abilities.
- Tightened Divergence-modified ability short descriptions so they fit the
  collapsed Pokémon Info ability panel.
- Corrected Frisk and Infiltrator summary ability descriptions for the expanded
  ability view.
- Updated Frisk and Omnisense battle messaging and Omnisense copy to communicate
  that the revealed item is disabled.
- Ported upstream battle sprites and palettes for newly added Mega Evolutions
  that were still using placeholder battle art.
- Added a once-per-run Unique Den map node with a custom tinted habitat icon
  that uses the Den encounter map for a high-quality random Unique Pokémon
  encounter before lab rarity and typing unlocks.
- Expanded the dynamic Unique Pokémon regular and unique ability roll pools with
  additional broadly useful and expressive options.
- Added the Trial Attendant as an early Adventure Entrance hub upgrade, with V1
  trial contracts for Type Trials, Little Cup, BST Crown, Randoman Roulette,
  Equalized, Region Style runs, Z-A Royale, and Orre Style.
- Added a Quest Log reward-details view so quest rewards can be read as text
  instead of relying only on icons.
- Added Berry Field helper options to harvest all ripe berries and bulk plant
  whole empty patches with 5, 10, or 20 selected berries.
- Added a richer Pokémon Safari offer preview with a Pokémon portrait and exact
  Pokéblock owned/required costs before buying or dismissing an offer.
- Added evil-team outfit disguises: matching team trainers react with a
  question-mark emote instead of automatically approaching, while direct
  interaction still starts their normal battle.

### Changed

- Made the Quest Board open the quest menu when there are no pending quest
  rewards to collect.
- Made challenge quest money rewards more generous, increasing total challenge
  quest money from 182,000 to 552,000 by adding a 10,000 money tier for planned
  full-run restrictions and reserving 20,000 for capstone challenges.
- Increased Main quest money rewards for later progression, rare discoveries,
  and full-run variant goals, raising total main quest money from 96,000 to
  190,000.
- Split obvious run-contract challenges into a dedicated trial challenge display
  group while keeping the existing Challenge quest category.
- Converted Type Master, Regional Style, Z-A Royale, Orre Style, BST Crown,
  and Randoman Roulette rewards to complete through enforced Trial Attendant
  runs instead of self-policed challenge rules.
- Converted Roguelocke, Can't Pick!?, Cursed Body, Pro-Building, Insane Mode,
  Iron Mono, and Iron Kaizo into enforced Trial Attendant runs with granular
  party, starter, battle-format, bag, Legendary, and Curse rules.
- Converted Chaos Master into a pre-Trainer-battle party-reroll Trial,
  Apotheosis into a Legendary-only Trial with Legendary starter choices, and
  Limited Capture into a Fresh Start Trial built around exactly five guaranteed
  captures.
- Added a Trial Attendant Pokédex selection step so flexible trials can choose
  their dex, while Region Style and Z-A Royale restrict choices to matching
  variants.
- Grouped Type Trials and Regional Styles into chevron-marked Trial Attendant
  submenus so related trial families no longer crowd the top-level list.
- Blocked Poké Ball throws against Pokémon that are illegal for the active
  trial, reusing the existing no-catch battle item flow instead of waiting for
  the post-catch release flow.
- Added Trial Attendant start validation for party, Day Care, and chosen partner
  Pokémon so illegal trial teams are caught before entering the run.
- Made Trial Attendant starter choices respect pending type, Little Cup, and
  low-BST legality so the three offered partner Pokémon are valid for the
  selected trial.
- Grouped Trial Attendant Pokédex choices by Pokédex region, with National
  generation options and other multi-variant dexes moved into submenus.
- Made Trial Attendant difficulty selection start on Average by default.
- Kept Trial Attendant runs from using the normal nurse's entrance movement.
- Blocked Trainer battles when the current Trial party is illegal, with the
  offending Pokémon shown, and prevented evolutions into Trial-illegal species.
- Kept the battle Poké Ball shortcut visible but grayed out when catching is
  temporarily blocked, while still hiding it when the Bag has no Poké Balls.
- Added the active Trial name to the start-menu run summary for saved and
  resumed Trial runs.
- Reordered Trial startup validation to catch illegal party or Day Care Pokémon
  before saving or opening the optional starter selection.
- Made the Trial Attendant upgrade become purchasable only after Challenges are
  introduced in the Challenge Area.
- Made the Trial Attendant appear at the Adventure Entrance only after their
  hub upgrade has been built.
- Placed the Trial Attendant beside the normal Adventure nurse, replacing the
  fence behind them once the upgrade is built.
- Replaced the Trial startup text sequence with a single overview panel showing
  the selected Trial, difficulty, Pokédex, and paginated exhaustive rules before
  the native Yes/No confirmation prompt.
- Made Z-A Royale enforce Double Battles alongside its Z-A Pokédex, Rainbow
  Trainer order, and all-region Trainer pool.
- Reworked the Snover and Abomasnow line's White Canopy unique ability to set
  Snow and Grassy Terrain together on switch-in.
- Reworked the Hoothoot and Noctowl line around Psychic/Flying Noctowl,
  Eclipse-accurate Hypnosis, and Eclipse Dark immunity, and removed Techno
  Blast from the Porygon line's level-up learnsets.
- Matched the GEEF boot splash hold duration to the upstream boot credit screen.
- Retinted the title main menu, start menu, option and difficulty menu
  backdrops/window frames, dialogue prompt border, option menu label text, and
  party, summary, move relearner, bag, player outfit, naming screen, and
  yes/no prompt panels toward a darker charcoal UI style.
- Updated generic intro, credits, and system-facing title copy to use the
  Emerald Rogue: Divergence title while preserving Pokabbie's Emerald Rogue
  attribution.
- Softened the Pokémon summary screen charcoal ramp so panels and headers read
  lighter against the white information boxes.
- Recolored the Rogue Pokédex page background stripes from blue to the same
  dark grey stripe pair used by the summary screen.
- Made A Unique Discovery active from the start with discovery rewards, and
  moved the Unique Scanner into Bill's first Unique Lab handoff.
- Smoothed boss team sizes so Standard Elite Four battles use five Pokémon and
  Hard mode reaches six-Pokémon boss teams starting at the Elite Four.
- Reworked the Bank into one-time Adventure Fund upgrades that set absolute
  run-start money tiers up to ¥50000, while Bag Wipe rules still force runs
  to start with no money.
- Reworked Hub Safari acquisition into a Pokéblock purchase economy: Safari
  sign and legendary cave Pokémon now show typed Pokéblock costs, preserve
  stored run traits, and grant low-level egg/baby species to the party or PC.
- Added an affordability marker to purchasable Hub Safari and legendary cave
  Pokémon in the Safari selection grid.
- Added a shiny marker to shiny Hub Safari and legendary cave Pokémon in the
  Safari selection grid.
- Added a unique marker to unique Hub Safari and legendary cave Pokémon in the
  Safari selection grid.
- Added a combined shiny unique marker for Hub Safari and legendary cave
  Pokémon that are both shiny and unique.
- Split the Safari selection grid markers into left caught, middle
  shiny/unique, and right affordability slots for clearer purchase scanning.
- Added an option to dismiss only the selected Hub Safari or legendary cave
  Pokémon after choosing it from the Safari selection grid.
- Changed Safari purchase pricing to use refined final-BST cost tiers, split
  dual-type costs across both matching types, remove hidden individual
  variation, and charge extra Shiny Pokéblocks for shiny Legendary/Mythical
  Pokémon.
- Added random Pokéblock drops to run boss rewards, with mostly typed
  Pokéblocks, occasional stat Pokéblocks, and rare Shiny Pokéblocks.
- Replaced Market Berry Blender minigames with direct Pokéblock crafting that
  uses the Mart-style quantity UI, dynamic recipe lists, and matching berries
  as item currency.
- Made Pokéblock recipe selection open the crafting item list directly instead
  of first showing a redundant Craft/Quit menu.
- Removed the Berry Blender hub upgrade path and made Pokéblock crafting
  available through the Market mixer without unlocking duplicate NPCs.
- Added bundled Pokéblock quest rewards, usually in 5-count chunks, including
  progression bundles, Safari-themed bundles, and 10 matching typed Pokéblocks
  from each Type Master quest.
- Removed Shiny Pokéblocks from Treat Shop stock so they remain tied to run
  drops, quest rewards, and Starf Berry crafting.
- Allow standard adventures to start with up to six party Pokémon, while
  blocking duplicate species from entering the same run-start party.
- Added a visible Legendary Clause difficulty toggle: Easy leaves it off,
  Average and above enable it to limit runs to one Legendary/Mythical in the
  party, and the old early enemy Legendary pressure is now Brutal-only.
- Weighted route and Team Hideout item drops toward Pokémon-linked special
  and evolution items when the matching evolution family is in the party, while
  preventing duplicate linked special-item drops on the same route.
- Reduced Pokémon-linked special item drops so matching party items are only
  favored after a rare special-item roll instead of increasing route special
  item frequency.
- Removed battle-only Bag Clause dead items from generated item economies and
  replaced the route shop vending machine's Poké Doll reward.
- Added guaranteed small Pokéblock Bundle drops on normal routes for Bag
  Clause runs, cashing small bundles into two random Pokéblocks and rare big
  bundles into five when the run ends.
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

### Removed

- Removed the placeholder Solo Contract, Hoenn Circuit, and Cursed Circuit trial
  definitions and quest entries while the real Trial Attendant set is being
  designed.

### Fixed

- Corrected the battle-sprite positioning of newly added Z-A Mega Evolutions,
  including visible gaps beneath Mega Staraptor and other backsprites.
- Made individual Trials appear only after their corresponding Challenge quest
  has unlocked, preserving chained unlocks such as Chaos Master and Iron Kaizo.
- Made Equalized apply a proportional 500-point base-stat spread throughout the
  run, including wild battles, party stats, and the active Pokédex display.
- Restored the gate-opening walk sequence when starting a Trial through the
  Trial Attendant.
- Made completed or retired Trial runs return the player in front of the Trial
  Attendant instead of the normal Adventure nurse.
- Fixed Little Cup treating non-evolving species such as Bruxish and Klawf as
  legal, and made its opponent teams follow the same Little Cup format before
  level-5 battle scaling is applied.
- Disabled ordinary Trainer-battle Exp. Points during Little Cup Trials so
  Pokémon remain at level 5, while leaving PokéAssistant EXP battles unaffected.
- Preserved strong trainer evolution lines when adapting opponent teams to
  restricted formats, allowing choices such as Aggron to resolve into legal
  Little Cup forms such as Aron instead of falling into unrelated type pools.
- Optimized route special item weighting to avoid long pauses when entering
  route, hideout, boss, or miniboss nodes.
- Fixed Shuffle Curse held item restoration deleting items when a shuffled party
  member was restored after battle or when the temporary Dark Deal curse expired.
- Forced EXP trainer helper battles to use 4x battle speed regardless of the
  trainer battle speed option.
- Applied wild form-family encounter rules to catching contest spawns so they
  match normal route encounter filtering.
- Fixed run boss Pokéblock rewards being added during the adventure, causing
  them to be lost when the run bag was wiped on return to the hub.
- Removed stale legacy Safari entrance and shiny-list text now that Safari
  acquisition uses Pokéblock offers.
- Fixed Ariados and Spinarak's Web Trap unique ability so any opposing Speed
  stage drop can trigger a queued random web move.
- Fixed attacker-side move-end unique abilities, including Vicejaw and Handywork,
  failing after the target's contact ability retargeted battle script state first.
- Fixed Unique Den encounters with custom typing showing base species type icons
  and untinted overworld preview sprites.
- Fixed purchasing Unique Pokémon from the Hub Safari so their custom identity
  is preserved when sent to the party or PC.
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
- Fixed Migration setting rain through Rain Dance and interrupting the switch-out
  sequence; it now sets rain directly before switching.

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
