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

### Fixed

- Restored unique-ability inheritance for costume and cap Pikachu, Spiky-eared
  Pichu, alternate Unown letters, and ten previously unmapped Gigantamax forms.
- Prevented Frontier Brains from replacing routes in the starting column of an
  adventure path, ensuring the player can level before challenging them.
- Moved battle gimmick selectors clear of the battler type indicators.
- Prevented Honey Tree habitats from generating Pokémon forbidden by the active Trial.
- Removed Swimmers and swimming Triathletes from land-based route Trainer
  encounters.
- Prevented the level-up helper Trainer from Terastallizing.
- Clarified that Iron Treads' Singularity Drive clears allied entry hazards and
  sets Gravity on switch-in.
- Made Frontier Brain Pokémon rewards respect the configured nickname mode,
  including skipping prompts for Never and assigning names for Automatic.
- Kept Tera type indicators visible during battle animations whenever their
  healthboxes remain visible.
- Prevented battle-end fades from freeing healthbox backing data while its
  sprites were still active, including during accelerated battle updates.
- Prevented future-only faint effects and subsequent end-of-turn effects from
  activating after either party's final Pokemon faints.

### Changed

- Gave every Minior Meteor colour Blast Shield, which blocks damaging moves
  with at least 120 listed BP, and every Core colour Flak Shield, which blocks
  damaging moves with at most 40 listed BP.
- Gave both Landorus forms Sand Command, which starts Sandstorm after Landorus
  knocks out a foe while backed by another Flying ally and Ground ally.
- Gave Rayquaza Emerald Accord, a once-per-battle switch-in ability that restores
  weather, terrain, rooms, side conditions, and active stat changes to neutral.
- Made the Roguelocke Trial require nicknames for every acquired Pokémon while
  preserving the player's nickname setting outside the Trial.
- Show each battler's Tera type beside its level whenever Terastallization is
  enabled, using a grayscale icon until it Terastallizes.
- Added unique abilities for the Galarian legendary birds: Freezing Glare now
  sets Future Sight, Galarian Zapdos matches the field's fastest raw Speed, and
  contact hits prime Galarian Moltres's next Dark move to drain half its damage.
- Made Jetstream use the user's effective Speed, allowing Tailwind, held items,
  Abilities, weather, status, and other Speed modifiers to affect its damage.
- Made Unique Legendary variants available in legendary nodes from the start,
  and moved alternative Unique Pokémon typings to the final Unique Lab upgrade.
- Made Regional Style Trials exclusively enable and temporarily supply their
  era-appropriate battle gimmick, while legacy styles disable all gimmicks.
- Made Orre Style replace the incoming party with Umbreon and Espeon while
  retaining its requirement to finish the Trial with both partners.
- Layered staggered lightning strikes and electrical flashes through
  Thundercrush's rising stone animation.
- Updated every Poké Ball item description to state its exact catch multiplier
  and conditions while retaining descriptions of secondary capture effects.
- Added independent 1x-4x speed controls for scripted cutscene movement,
  evolution animations, and catching animations.
- Replaced Ditto's Stabilize unique ability with Rapid Replica, which raises its
  Speed after transforming once per battle before adopting the target's unique
  ability.
- Expanded generated Unique Pokemon from a 103-entry unique-ability pool to
  563 audited battle-safe abilities, including Starmobile, and added guaranteed
  synergy moves for abilities that depend on particular moves or move families,
  while retaining both random moves when the species already learns the pairing;
  Final Step now guarantees a sound move instead of a contact move.
- Gave Paldean Wooper and Clodsire Bog Body, which prevents opposing Pokemon
  from restoring HP while either is active.
- Repacked generated Unique Pokemon IDs for the larger ability pool and bumped
  the development save version; existing development saves require a restart.
- Reworked Alolan Ninetales's Lanakila Law so Encore and Disable gain priority
  in Snow, removing priority and perfect accuracy from Hypnosis.
- Rebalanced temporary Dark Deal Curses to match their paired Charms and
  removed economy, recruitment, evolution, and Randoman effects from the
  temporary pool.
- Reduced Reach Charm's contact-move damage bonus from 25% to 20%.
- Made all standard Charms unique and rebalanced older Charms: Crit grants one
  stage, Shed Skin cures status 50% of the time, Strength guarantees perfect
  IVs, Grace doubles secondary-effect chances, Priority activates 25% of the
  time, Endure always protects from full HP, and Pressure consumes two extra PP.
- Made charms, curses, and dynamic TRs versioned dynamic items so their IDs can
  be rearranged without stale saved items becoming unrelated rewards.
- Sacred Ash can now be consumed immediately after any battle with party losses,
  before post-battle dialogue or reward popups, provided the player did not
  white out; declining preserves the Ash and releases fainted Pokémon normally.
- Smoothed boss difficulty progression by delaying competitive movesets across
  Easy, Average, and Hard, and by delaying boss weather until the Elite Four
  on Average and Gym 5 on Hard.
- Made the dark charcoal window border palette the global default so standard
  menus, prompts, shops, and scripted choices use it consistently.
- Added the level-up battle NPC to Mysterious Shrines so parties can reach the
  current level cap before challenging the shrine guardian.
- Changed new hub area construction to use the hub map editor, allowing the
  player to choose any valid connected location instead of only expanding from
  their current area.
- Unified hub upgrades, construction, and rearrangement under one workbench
  session, with the hub refresh occurring once when the player exits.

### Fixed

- Fixed the Trial preview briefly drawing its border around an unloaded gap
  before the complete overview appeared.
- Fixed Infested Terrain damaging Flying and otherwise ungrounded Pokémon on
  switch-in.
- Fixed the storage out-of-Dex warning compressing and overlapping the Pokémon
  information panel.
- Added Galarian Corsola to the Modern Pokedex alongside Cursola.
- Fixed the intended increase in Full Rest Stop frequency during the Elite Four
  being shadowed by the earlier Gym 3 condition.
- Fixed the lone opponent's type badges being vertically misaligned in 2-vs-1
  double battles.
- Fixed evolved Safari catches being shown as nicknames when their hub offers
  display the Pokémon's base species.
- Fixed Fossil Drill activating Rapid Spin when its user had fainted or was
  about to faint from its triggering move's recoil.
- Fixed custom Unique Pokémon types reverting to their species' base types
  when switching into battle, and fixed empty adventure paths appearing after
  defeating a boss.
- Fixed corrupted text and background tiles on the move-replacement summary
  screen opened while relearning a move.
- Fixed Gimmighoul Roaming Form entering the generated Unique Pokemon buffer
  through Gholdengo's egg-species conversion.
- Fixed Gimmighoul Roaming Form being unable to evolve with a Gimmighoul Coin,
  and made the shared evolution data cover both forms without a Rogue override.
- Fixed the Unique Pokemon tracker preview clipping its final detail line when
  displaying every optional field.

### Removed

- Removed the deprecated agbcc build path and its unused fixed-layout linker
  tooling; Divergence now supports the modern devkitARM toolchain exclusively.
- Removed inherited Japanese font glyphs, Mystery Gift and e-Reader support
  (including their save data and link hooks), and the Generation III Berry
  Program Update payload to free ROM space.

### Added

- Ported full battle animations for all five Torque moves from
  `pokeemerald-expansion`.
- Restored Frontier Brain adventure-path encounters with three scheduled
  Frontier Brain battles per run, previewed Pokémon trophies, and canon-inspired
  competitive teams; Brandon can uniquely award one of his Legendary guardians.
  Brains now appear in Trial runs except Little Cup, only offer Trial-legal
  trophies, skip trophies during Limited Capture, and use Snagging instead during
  Orre Style.
- Added Level, Tinted, Reach, Accuracy, Retaliate, and Stand Curses to the
  temporary Dark Deal pool as opponent-side mirrors of their Charms.
- Added the Moody and Eviolite Charms, which adjust friendly Pokémon's stats
  each turn and bolster the defenses of Pokémon that can still evolve.
- Added the Level and Regen Charms, which raise the player's level cap by three
  and restore friendly Pokémon's HP when switching out.
- Added the Prep and Protean Charms, which grant priority to the first status
  move after switching in and change the user's type to match its first
  eligible move respectively.
- Added the Retaliate, Momentum, and Stand Charms, which reward replacing a
  fallen ally, knocking out opponents, and fielding the final party member.
- Added the Recoil, Guard, and Convert Charms, which prevent friendly recoil
  and crash damage, block critical hits, and convert Normal-type moves to the
  user's primary type respectively.
- Added the Reach, Accuracy, and Recovery Charms, which strengthen contact
  moves, prevent friendly non-OHKO moves from missing, and improve move-based
  healing respectively.
- Added the Sharpness, Jaw, and Skill Charms, which strengthen slicing and
  biting moves and improve variable multi-hit moves respectively.
- Added the Tech, Tinted, and Iron Fist Charms, which strengthen low-power,
  resisted, and punching moves respectively.
- Added Hidden Stash for the Gimmighoul line, Last Pour for the Poltchageist
  line, and Flock Step for Flamigo.
- Added Great Tusk's Primal Onslaught and Iron Boulder's Singularity Crash
  unique abilities.
- Implemented Tatsugiri's Commander ability and Dondozo's Order Up interaction,
  including form-specific boosts and animations, switching restrictions, and
  battle gimmick cancellation while Tatsugiri is swallowed.
- Added dimmed storage icons, grayscale previews, and a warning label for
  Pokémon outside the currently selected Pokédex.
- Added Plain and Infested Seeds, which raise accuracy on Plain Terrain and
  critical-hit ratio on Infested Terrain respectively.

- Added an Achievements quest category for optional accomplishments that do not
  gate One Last Quest or the Final Run.
- Ported upstream Fast Path pacing, replacing the old Experimental pacing
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
- Added Eevee's Potential, Tyrogue's Freestyle, and Wynaut's Why Not? unique
  abilities, including early punching and kicking moves for Tyrogue.
- Added Necrozma's Prism Refraction, Marshadow's Living Shadow, and Eternatus's
  Endless Core unique abilities, and restored Eternabeam to Eternatus's learnset.
- Added Breach Point for the Nymble line, Salt Fortress for the Nacli line, and
  Ground Fault for the Tadbulb line.
- Added the Eclipse-consuming Withering Script, Severing Rite, Earthen Seal,
  and Cinder Edict unique abilities for the Treasures of Ruin, and assigned
  Mean Streak to Okidogi.
- Added an unlockable Stealth Poké Ride skill that lets thematically fitting
  mounts avoid automatic wild encounters.
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
- Added matching Day Care node berry helper options to harvest all ripe berries
  and bulk plant its three-tree patches with 3, 6, or 9 selected berries.
- Added a richer Pokémon Safari offer preview with a Pokémon portrait and exact
  Pokéblock owned/required costs before buying or dismissing an offer.
- Added evil-team outfit disguises: matching team trainers react with a
  question-mark emote instead of automatically approaching, while direct
  interaction still starts their normal battle.

### Changed

- Made Morelull and Shiinotic's Sporelight use Spotlight after every successful
  healing move instead of only once per battle.
- Expanded Unique Pokémon move generation to a curated 154-move general pool
  with shared physical and special attacks for every type, added four complementary
  altered-type moves per type, and made the shrine guardian retain a more varied
  pair of moves from its selected competitive set. The Unique tracker now also
  displays each tracked Pokémon's effective typing.
- Made run starts substantially faster by caching repeated species and trainer
  eligibility work, deferring late rival planning,
  and removing unnecessary portal transition frames.
- Made the Starter Bag a prerequisite for constructing the Trial Attendant and
  aligned the player's Trial starter-selection approach with the bag.
- Reworked Zygarde's Edgeproof unique ability into Natural Order, which moves
  every active Pokémon's stat stages one step toward neutral at turn end.

- Reworked Emolga's unique ability into Static Stash, which uses Charge after
  consuming a Berry or terrain seed, and simplified the Wattrel line's Storm
  Glider to only set Tailwind the first time it is damaged each switch-in.
- Gave Type: Null the Control Mask unique ability, which resets its stat changes
  after it completes a damaging move.
- Reworked the original Regi trio's unique abilities into Stone Seal, Frost
  Seal, and Iron Seal, which disable the first qualifying move each switch-in.

- Restored the Pidgey line's Strong Winds unique ability to set Tailwind on
  switch-in instead of when a foe is forced out.
- Replaced Silvally's Overclock unique ability with RKS Relay, making switches
  between Silvally and allies sharing its Memory type transfer Baton Pass state.
- Reworked the Mysterious Shrine into a once-per-run battle against a boosted
  Epic Unique Ho-Oh with randomized moves, regular Ability, and possible
  rainbow typing, a dedicated transition, Ho-Oh's battle theme, and a fiery
  Sacred Ash rebirth animation; victory now awards Sacred Ash without a
  Capacity Curse, and the shrine no longer appears in Gauntlet modes.
- Sacred Ash now activates only as an automatic whiteout rescue during
  Adventures instead of also being manually usable from the party menu.
- Expanded thematically appropriate Main Quest rewards with a balanced spread
  of typed and Shiny Pokéblocks.
- Safari offers now base their Pokéblock types on the displayed base Pokémon
  while retaining the final evolution's BST-based total cost.
- Unique Pokémon Dens now have a 30% chance to appear a second time during a
  run, with both appearances scheduled away from other reserved encounters.
- Reworked the Premier Ball with a 3x catch rate and a random neutral nature
  for successfully caught Pokémon.
- Updated the wild-battle quick-catch shortcut to begin each turn with the
  currently owned Ball that has the highest catch rate against the target,
  randomly selecting among Balls tied for the highest rate.
- Reworked the Cherish Ball to guarantee at least three perfect IVs on a
  successfully caught Pokémon while retaining its standard catch rate.
- Reworked the Luxury Ball to scale with the player's money, granting a 2x
  catch bonus at ¥10,000, 3x at ¥30,000, and 4x at ¥50,000.
- Increased the Repeat Ball's catch bonus for previously caught species from
  3.5x to 4x.
- Reworked the Nest Ball to grant a 4x catch bonus against Grass- and
  Flying-type Pokémon instead of scaling with the target's level.
- Increased the Net Ball's catch bonus against Water- and Bug-type Pokémon from
  3.5x to 4x.
- Reworked the Dive Ball to grant a 4x catch bonus against Water- and
  Flying-type Pokémon instead of depending on the encounter location.
- Reworked the Dream Ball to give caught Pokémon their Hidden Ability instead
  of receiving a catch bonus against sleeping targets.
- Reworked the Moon Ball to grant a 4x catch bonus against Dark- and Fairy-type
  Pokémon.
- Made Pokémon caught in a Friend Ball start at maximum friendship.
- Reworked the Heavy Ball to compare the target's weight with the active
  Pokémon, granting a 2x bonus at 50 kg heavier, 3x at 150 kg, and 4x at 300 kg.
- Reworked the Level Ball to use absolute level leads, granting a 2x bonus at
  10 levels, 3x at 25 levels, and 4x at 50 levels above the target.
- Expanded the Fast Ball's 4x catch bonus to roaming Pokémon.
- Made Fast Path pacing the Standard Adventure format and retained the previous
  Standard format as the optional Slow Path mode.
- Made the selected Pokédex a run-entry legality rule for the party and Day Care,
  with illegal Pokémon marked in the party screen.
- Made a confirmed Trial Pokédex selection immediately become the active hub
  Pokédex so party previews and run validation use the same rules.
- Made Insane Mode force Hard difficulty instead of presenting a difficulty
  selection.
- Added an early Trial Attendant shortcut that previews and repeats the exact
  setup from the most recently started Trial.
- Reduced Trial starter-bag loading time by filtering legal species earlier and
  selecting starters directly from the eligible pool.

- Made the Quest Board prioritize pending rewards, then newly unlocked quests,
  and otherwise open the normal Quest Log.
- Made Trial quest money rewards more generous by adding a 10,000 money tier
  for full-run restrictions and reserving 20,000 for capstone Trials.
- Increased Main quest money rewards for later progression, rare discoveries,
  and full-run variant goals, raising total main quest money from 96,000 to
  190,000.
- Reworked the former run-contract quest category into Trials for enforced,
  repeatable run contracts.
- Moved one-time battle-gimmick, flawless-run, speedrun, team-building, Shiny,
  and evil-team achievements into Main Quests, with their former Hard-only
  Shiny rewards granted on first completion.
- Made each run unlock the Main Quest for its selected evil team before quests
  activate, while postgame progression reveals every remaining team quest.
- Converted Type Master, Regional Style, Z-A Royale, Orre Style, BST Crown,
  and Randoman Roulette rewards to complete through enforced Trial Attendant
  runs instead of self-policed rules.
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
- Made the Trial Attendant upgrade become purchasable only after Trials are
  introduced in the Trial Area.
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
- Standard Adventure segments with a pending rival battle now guarantee at
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
- Changed Headspace and Psionic Paradox to use Trick Room when any terrain is
  active instead of overwriting it with Psychic or Plain Terrain.
- Added random unique abilities to Legendary dynamic Unique Pokemon, with the
  ability encoded in the existing custom mon ID.
- Updated the 1.1 champion Mega Evolution ability assignments.
- Retired the obsolete Embargo Curse and Species Curse items now that Bag
  Clause and Species Clause are handled by run config toggles.
- Fixed Rotom form appliance abilities being assigned as normal abilities
  instead of hidden abilities.
- Increased Short Circuit's Electric-move boost from 20% to 50% per fainted
  Electric-type ally.

### Removed

- Removed the placeholder Solo Contract, Hoenn Circuit, and Cursed Circuit trial
  definitions and quest entries while the real Trial Attendant set is being
  designed.

### Fixed

- Fixed Trial starter generation mishandling move-based evolutions, which could
  pause debug builds before opening the starter bag.
- Reduced the first rival battle's loading pause by planning and caching the
  rival's roster without constructing discarded battle-ready Pokémon.
- Prevented the shrine guardian Ho-Oh from receiving Whirlwind, which could end
  its solo wild battle without resolving the trial.
- Fixed Endless Core allowing Eternatus to switch, use an item, or flee instead
  of selecting a status move during its recharge turn.
- Made automatic Poké Ball selection break equal catch-rate ties by using the
  ball with the lowest maximum potential, and preserve the Master Ball unless
  it is the only ball available.
- Fixed battle animations firing debug assertions when a transformed Pokémon
  was queried for its sprite dimensions, including once per hit of Double Kick,
  and retained Transform's gender-specific sprite across later sprite reloads.
- Prevented the AI from repeatedly using Whirlwind and other phazing status
  moves against Pokémon with Unmovable.
- Made Eevee require Baby-Doll Eyes specifically to evolve into Sylveon, so
  Fairy-type Covet no longer enables the evolution immediately.
- Fixed the Quest Log index failing to redraw scrolled options after adding the
  Achievements category, restoring access to Adventure Stats and Back.
- Added up and down indicators to the Quest Log index when category options are
  hidden beyond the visible list.
- Returned the player and gate NPC to their normal positions when final party
  validation cancels a run after the gate-opening walk.
- Prevented forced-starter Trials from rejecting the Hub party on capacity rules
  before replacing it with the required starter.
- Allowed capacity-restricted Trials to recover an oversized Hub party through
  the starter bag instead of forcing the player to leave the startup flow.
- Made the optional starter choice available to every Trial without requiring
  the separate Starter Bag hub upgrade.
- Prevented flying Ride Pokémon from sliding on ice tiles.
- Corrected the battle-sprite positioning of newly added Z-A Mega Evolutions,
  including visible gaps beneath Mega Staraptor and other backsprites.
- Made individual Trials appear only after their corresponding Trial quest
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
- Fixed the Game Modes submenu being hidden on early saves, which made Slow Path
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
- Fixed Standard Adventures sometimes skipping the early evil-team encounter
  when all reset-path candidate difficulties were occupied by legends.
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
