# Approved rebalance implementation

## Contract

Implement the final approved decisions in `docs/rebalance_review.md`, including
its later distribution, accumulation, typing, shared-design and combined-power
audits. Do not reintroduce withdrawn suggestions or redesign approved mechanics.
Approval and implementation are separate states. A checked item needs evidence
from runtime data and the relevant verification commands.

Starting checkout: `770f15328229db0b8f35c915e591383a7abaa4a7` (clean `main`).
Upstream comparison: `c0ea02119f73916b391ae7e9482ee44197b42071`.
Canonical stats: the committed `tools/data/canonical_species_data.csv`.
The Pokédex Inspect work is already included in the starting checkout.
Do not stage, commit, push, switch branches or clean the whole repository.

## Checkpoints

- [x] Preparation: verify the profile pipeline; establish a machine-checkable
  decision manifest and coverage report; identify unresolved design details.
- [x] Prerequisites: port Revised Swirling Blade and implement the approved
  regular Ponyta/Rapidash Fire pivot move, keeping existing learnable move IDs
  and serialized save layouts stable.
- [x] Content: normalize and implement complete evolution/form families in
  manageable batches. Record absolute stat targets derived from canonical
  deltas, exact Ability slots, typing, level moves, tutors and exceptions.
- [x] Reconciliation: every ledger row covered, every approved operation
  verified, rejected additions absent unless already available at the starting
  checkout, and all intentional asymmetries documented.
- [x] Final verification: generated profiles/species/bake outputs verify;
  focused mechanic and integration tests pass; playable ROM builds.

## Implementation rules

- Apply later explicit decisions over earlier suggestions. If the ledger and
  a later user correction disagree, record and follow that correction.
- Resolve stat deltas against canonical stats, not the already-buffed current
  table. Preserve current stats when the final decision says keep Divergence.
- Expand families through actual evolution/form relationships, not similar
  names. Include Mega, Gigantamax, gender, seasonal and transformation forms.
  Do not impose identical typing or Abilities where differences were approved.
- Preserve move access unless removal was explicitly approved. A rejected
  proposed addition is not permission to remove existing access. Check direct
  learnability and inherited evolutionary access, with explicit exceptions.
- Change authoritative species definitions and profile inputs. Do not edit
  generated headers. Keep upstream sources pinned; no unrelated live refresh.
- Test new mechanics first. Per content checkpoint, run the manifest checker,
  relevant generators/verification and a narrow integration filter. Avoid full
  test suites or repeated full rebuilds for individual species.
- Progress survives turns in this file and the decision manifest. Never mark
  the goal complete with unnormalized, unverified or unresolved approved work.
- Playtesting recommendations are separate from automated implementation
  checks. Do not claim that passing tests proves competitive balance.

## Implementation choices

- The Ponyta row explicitly delegates the new move's name and remaining details
  to implementation. Implemented **Flame Relay**: physical Fire, 70 BP,
  100 accuracy, 20 PP, normal priority, contact, standard hit-and-switch effect
  and no additional secondary effect. Regular Ponyta/Rapidash learn it at 40;
  the Galarian branch does not. No design decision remains blocked on the
  earlier optional naming question.

## Progress log

### Preparation — 2026-09-10

- Goal started; checked the clean starting checkout and read the pipeline docs.
- Reproduced the existing profile launcher failure: Mono xbuild compiles
  unrelated Windows sprite tools and fails on their local-function syntax and
  unavailable PresentationCore. Added a profile-only build target using
  the same authoritative profile pipeline sources.
- `wsl ./scripts/generate_pokemon_profiles.sh verify`: PASS; deterministic
  output matches the existing generated header, without refreshing upstream.
- Added `tools/data/rebalance_manifest.json` and
  `scripts/check_rebalance_manifest.py`. Every covered species preserves its
  exact starting stats/types/Abilities/level placements/tutors except explicit
  overrides. Canonical deltas use HP/Atk/Def/SpA/SpD/Spe order. Shared profile
  pointers are resolved for forms, and ledger/audit fingerprints detect drift.
- `wsl python3 scripts/test_rebalance_manifest.py`: PASS (nine tooling tests).
- `wsl python3 scripts/check_rebalance_manifest.py`: covered 19/650 decision
  table rows and verified 24 species/forms; zero mismatches, explicitly
  INCOMPLETE. The report at `build/rebalance/verification.json` lists every
  unnormalized decision. `--require-complete` is mandatory for final signoff.

### First content checkpoint — 2026-09-10

- Applied approved stats for Butterfree/Gigantamax, Venonat/Venomoth,
  Azurill/Marill/Azumarill, Bonsly/Sudowoodo, Bunnelby/Diggersby,
  Drampa/Mega Drampa and Goldeen/Seaking. Checked preservation of all other
  species fields and learnset placements/access for these families.
- Clamperl remains unchanged; Huntail gains Psychic Fangs at upstream's level
  55; Gorebyss gains the final approved Regenerator in slot 2. Existing profile
  sets do not require an Ability remap for this slot change.
- Profile generation changed exactly one level-up entry, without a source-lock
  refresh or unrelated compatibility/set churn; deterministic verification PASS.
- Focused `ability_unique` filter `Shell Game: Regenerator`: PASS (two tests),
  covering defensive switching and Shell Smash/terrain/Baton Pass combined.
  Initial fixture issues (overridden maximum HP restored by party processing,
  and missing replacement battler Speed) were corrected; no engine change was
  needed.
- Regenerated species reports and baked runtime data; `make check-species-report`
  PASS. `rogue` filter `Pokedex Inspect`: PASS (four tests). Incremental playable
  debug ROM build PASS (31,753,768 bytes used, padded to 32 MiB); `git diff --check`
  PASS. No files staged or committed.

## Resuming and verification

Run `wsl python3 scripts/check_rebalance_manifest.py` for a partial checkpoint.
Only add a batch after reading its final ledger rows and superseding audits;
include every applicable family/form and explicit preservation exceptions.
Empty species rules mean preserve the pinned starting version, not unreviewed.
Do not refresh fingerprints merely to silence drift without reading the change.
The manifest deliberately stores approved operations, not another broad
compatibility-retention snapshot. The pinned Git source supplies preservation
defaults; temporary baseline extraction never alters the worktree.

Next: expand the remaining complete-family batches, implement the pending-name
Fire pivot, and extend form/shared-design invariants as families are reconciled.
Eevee's family and the initial shared-rule groups are now implemented below.
The checker identifies new evolution move gaps;
it is not yet a certification that all historical evolution gaps are resolved.

### Swirling Blade prerequisite and Honedge family

- Ported the actual upstream `battle_moves_revised.h` definition, not its
  unusable normal-table placeholder: Steel, physical, 70 BP, 100 accuracy,
  15 PP, contact, slicing, increased critical-hit chance. Uses upstream's
  Sacred Sword animation alias and a matching in-game description.
- Assigned new learnable ID 863; upstream's 848 already means Acid Rain here.
  All existing learnable IDs and serialized Pokemon/TR fields remain unchanged.
  Following this repository's move-table layout, the derived battle-only
  Z/Max ranges shift after MOVES_COUNT. They are generated during battle from
  the original move slots (`battle_z_move.c`, `battle_dynamax.c`, `battle_util.c`),
  not stored as newly learned moves. No save-structure size changes.
- Added a checker guard against renumbering or reusing existing explicit
  learnable IDs. Focused `moves` filter `Swirling Blade`: PASS (three tests:
  slicing boost, 1/8 critical-hit chance and Rough Skin contact).
- Honedge and Doublade gain hidden Sharpness and level-26 Swirling Blade;
  Aegislash gains it at level 1 in both stances. Chose upstream's shared level
  26 for Doublade, explicitly omitting its duplicate level 38/tutor entries.
  Generator output changes only the intended three level moves. Deterministic
  profile verification and exact family/form manifest checks PASS.
- Added profile-name alias resolution (e.g. SPECIES_AEGISLASH versus the
  species table's SPECIES_AEGISLASH_SHIELD) without conflating battle forms.
- The checkpoint ROM rebuild after adding the new move passed: 31,753,892
  bytes used, padded to 32 MiB. Species/bake and profile verification passed.
  `rogue` filter `Competitive profile abilities belong`: PASS (one whole-table
  Ability-legality test). No final whole-ledger completion is claimed.

### Starter-family checkpoint — 2026-09-10

- Implemented all 27 starter families and their associated Mega, regional,
  Gigantamax and Greninja transformation forms. Coverage is now 100/650
  decision rows and 129 species/forms; exact normalized target checks pass.
- Kept final typing exceptions: regular Meganium, Sceptile, Feraligatr and
  Serperior retain their approved monotypes; their existing Mega typings stay.
  Pignite/Emboar gain Ground while Mega Emboar retains Fighting. Dewott and
  regular Samurott gain Fighting while Hisuian Samurott stays Water/Dark.
- Applied matching approved Mega deltas to Sceptile, Feraligatr, Chesnaught,
  Emboar and Greninja, plus matching Greninja Battle Bond/Ash Attack increases.
  Unapproved starter and Mega stat increases remain excluded.
- Preserved prior move access and placements outside explicit approved edits.
  Chikorita needed one narrow Aromatherapy tutor-retention rule: promoting
  the move to level-up stopped its competitive set from auto-adding the tutor.
- Added compiled evolution-graph checking for newly acquired move access.
  All current new additions accumulate correctly; 133 historical source/target
  gaps (118 distinct target/move pairs) are reported separately, not silently
  fixed or claimed absent. Full reconciliation and form/shared-rule invariants
  remain pending. Twelve checker unit tests pass.
- Species reports and baked data regenerated and verification passed. The
  initial focused whole-table competitive Ability test identified exactly
  eleven Emboar sets still requesting removed Reckless. Added the named
  Reckless-to-Mold Breaker transformation. The guarded refresh candidate has
  exactly those eleven generated-header replacements and no unrelated churn;
  reviewed its report before invoking `refresh --accept-removals`.
- Refresh promotion completed with only the reviewed Emboar remap. Source URLs
  are unchanged; the source-lock hashes/timestamp and the new 11-match entry
  were generated by the supported refresh workflow. Offline deterministic
  profile verification passes, and the rerun of `Competitive profile abilities
  belong` passes (one whole-table test).
- `--require-complete` correctly exits nonzero because 550 rows remain pending;
  the 100 covered rows and 129 species have zero target mismatches or new
  evolution-access gaps. `git diff --check` passes.
- Playable debug ROM build PASS: 31,754,080 bytes used, padded to 32 MiB.
  EWRAM remains 261,522 bytes (99.76%); IWRAM remains 25,604 bytes (78.14%).
  This completes the starter checkpoint, not the overall ledger or balance
  playtesting. No changes staged, committed, pushed or switched to another branch.

### Eevee and shared-design checkpoint — 2026-09-10

- Applied Eevee's eight approved evolution Ability packages and selective
  partner moves, including Glaceon's canonical -30 SpD/+30 Speed. Eevee and
  Gigantamax Eevee remain unchanged. Focused `ability_unique` filter `Smolder
  activates Flare Boost`: PASS (one test; 1.5x special damage without an actual
  burn). Existing battle logic already supports the intended interaction.
- Volbeat and Illumise now both have 465 BST and their approved special moves;
  no Call/Response/Baton Pass changes. Preserved Volbeat's existing Thunder
  tutor with one narrow rule, since adding the level move otherwise removed
  competitive-set-derived tutor access.
- Both Qwilfish forms have 440 BST; only regular Qwilfish receives the approved
  Defense/Special Attack redistribution and a single late-level Fell Stinger
  placement. Hisuian Qwilfish and Overqwil retain their approved original stats.
- Beautifly has the exact 435-BST spread and Air Slash/four tutors; Dustox gains
  Sludge Bomb and Struggle Bug. Neither receives Hurricane. Beheeyem receives
  only Psycho Boost; the rejected Power Gem/Mystical Fire additions are absent.
- Pidgey's family carries the special progression, with the final reduced
  +15 SpA delta on Pidgeot and Mega Pidgeot. Swoobat uses the reduced +10 SpD
  target and no Hurricane; Whiscash uses +15 Attack/+2 SpD without extra Defense.
- Numel/Camerupt keep Slack Off, Camerupt gains Simple and Power Gem, while
  Mega keeps Sheer Force. Barboach/Whiscash gain Simple and Breaking Swipe.
- All Burmy cloaks gain Battle Armor. Wormadam cloaks use their distinct
  approved 450-BST spreads and second Abilities; Mothim uses the approved
  special spread, Compound Eyes and Hurricane.
- Exact verification now covers 139/650 rows and 169 species/forms, with zero
  target errors or new evolution-access gaps. It separately reports 239
  historical source/target gaps in these expanded families; those are not
  claimed fixed and remain for final reconciliation.
- Added 40 explicit relational invariants: 27 starter second-slot groups,
  Eevee and Huntail/Gorebyss distinct Ability counts, counterpart/cloak BSTs,
  and approved Mega/Gigantamax/transformation deltas. All pass. Seventeen
  checker unit tests pass, including malformed-rule and exception-sensitive
  canonical-delta checks.
- Added named competitive remaps for Camerupt (one Anger Point set), Barboach
  (two Oblivious sets), and Whiscash (eight Oblivious sets). Reviewed the guarded
  refresh report and candidate header before acceptance: exactly those eleven
  replacements, unchanged upstream source aggregate, and no unrelated churn.
  Supported refresh promotion and offline deterministic profile verification
  pass. Species reports/baked runtime verification and the focused whole-table
  `Competitive profile abilities belong` test also pass (one test).
- Reconciled another 24 keep-as-is rows across 46 complete-family/form targets:
  Gigalith, Calyrex, Deoxys, Miltank, Victini, Castform, Eldegoss, Falinks,
  Frosmoth, Hypno, Kecleon, Oinkologne, Orbeetle, Squawkabilly, Stonjourner,
  Trevenant, Watchog and Wugtrio. This includes all seven Divergence Castform
  weather forms and Mega Falinks, not only their canonical forms. No content
  edits were needed; exact preservation checks pass.
- Total coverage is now 163/650 rows, 215 species/forms, and 40 invariants.
  No target errors or new evolution gaps; 285 historical source/target gaps
  are listed separately. `git diff --check` passes.
- Reconciled 21 further keep-as-is rows covering 97 additional targets:
  Absol (including both Megas), the full Ralts branching family, Axew/Haxorus,
  Cranidos/Rampardos, Hydreigon's line, Galvantula's line, Kyogre/Primal,
  Jirachi, Lugia, Manaphy, Phione, Alomomola, Mimikyu forms, Tauros forms,
  and Milcery/Alcremie (all 63 sweets/cream variants plus Gigantamax).
  Their stats, types, Abilities and direct learnsets exactly match the pinned
  starting data. No content changes were needed or invented for these rows.
- Final checkpoint: 184/650 decision rows, 312 species/forms, 40 invariants,
  zero target mismatches and zero new evolution move gaps. The final
  `--require-complete` run correctly exits 1 because 466 decisions remain;
  this is not a failed covered-target check or whole-ledger signoff.
- Playable debug ROM build PASS: 31,754,232 bytes used, padded to 32 MiB.
  EWRAM is 261,522 bytes (99.76%); IWRAM is 25,604 bytes (78.14%).
  `git diff --check` and final worktree inspection pass; no tracked deletions,
  staging, commits, pushes or branch changes. Overall goal remains active.

### Selective family-content checkpoint — 2026-09-11

- Continued from verified 184/650 coverage. Implemented 14 complete families:
  Ekans, Spinarak, Cacnea, Skitty, Seel, Wooloo, Spoink, Finneon, Slugma,
  Nickit, Clobbopus, Paras, Yungoos and Morelull. Final stats/Ability slots and
  moves are explicit in the manifest, with current baseline preservation for
  every other field.
- Ekans/Arbok replace level-up Sludge Bomb at their current levels 33/39 with
  Poison Fang, retaining both existing tutors. Added narrow `removeLevelUp`
  profile rules rather than changing tutor compatibility or wholesale learnsets.
  The generator supports these rules before applying level additions, using
  the existing species/move validation and cross-list unique rule IDs.
- Used pinned upstream levels for selective additions. Cacnea/Cacturne retain
  compatibility Needle Arm while gaining level-up access; all other prior
  tutors/placements remain. Parasect receives Lunge despite Ghost/Grass typing,
  and Grapploct receives Bubble, following the final accumulation audit.
- Seel/Dewgong receive Ice Scales, Finneon/Lumineon receive Dazzling, Magcargo
  receives Weak Armor / Flame Body / Solid Rock, and Cacnea/Cacturne receive
  Rough Skin. Current competitive sets use none of their replaced Abilities,
  so no new remap or network refresh was required. Slugma's own Abilities stay
  unchanged, as approved. Gumshoos Totem is only a placeholder ID, not a separate
  compiled runtime species to buff.
- Added Swirling Blade tutors for Cobalion, Terrakion, Virizion and both Zacian
  forms; Cobalion also receives Rock Polish. Both Keldeo forms and the entire
  Pawniard/Bisharp/Kingambit line are verified as exact keep-as-is decisions.
- Applied selective Carnivine Leech Life, Heatmor Power Whip, Emolga level-up
  Electro Ball/Wild Charge, Komala/Bruxish stats, and Bouffalant Speed/charging
  moves. No discarded coverage or stats were reintroduced.
- Exact checker passes 223/650 rows, 356 species/forms and all 40 shared
  invariants, with no new evolution-access gaps. Offline profile generation
  and deterministic verification pass; generated species/bake outputs updated.
  Generated-data verification and the focused whole-table competitive Ability
  legality test pass (one runtime test); all seventeen checker tests also pass.
  The expanded historical-gap report lists 389 source/target gaps separately.
  Playable debug build PASS: 31,754,392 bytes used, padded to 32 MiB; EWRAM
  261,522 bytes (99.76%) and IWRAM 25,604 bytes (78.14%). `git diff --check`
  passes. Final `--require-complete` correctly returns 1 for 427 pending rows,
  not covered-target failures. No new upstream refresh, staging, commits,
  pushes, branch switches or tracked deletions in this checkpoint.

### Focused Ability families and Disguise — 2026-09-11

- Added explicit targets and authoritative inputs for Farfetch'd (both regional
  lines), Hoppip, Dedenne, Delibird, Klawf, Kricketot/Kricketune, Ledyba/Ledian,
  Luvdisc, Maractus, Tarountula/Spidops, Sunkern/Sunflora and Turtonator.
  The final distribution cuts remain excluded, including Jumpluff Hurricane,
  spider Mach Punch and Sunflora's extra coverage tutors.
- Disguise was hard-coded to Mimikyu. Added battle-only party-slot shield state
  for Tarountula/Spidops, without changing their species or any save layout;
  Mimikyu retains its busted-form and Unspeakable handling. Updated the AI's
  shield check and the in-game long description. Added first-hit, multi-hit,
  switching, passive damage, Mold Breaker and Tripwire-combination tests.
- The first focused run against the old species table passed ten tests,
  including all six Mimikyu regressions; the switch-in case failed because the
  harness's forced initial Ability did not survive switching into a species
  without that actual Ability slot. After applying the actual slots, its scene
  assertions passed; the final benched-HP assertion was invalid because battle
  cleanup recalculates and clamps artificially enlarged party HP. The test now
  checks exact shield damage during the scene and preserved species afterwards.
  All eleven focused Disguise tests pass, including all Mimikyu regressions.
- Reviewed the guarded profile refresh: unchanged upstream aggregate, no
  unrelated source drift, and exactly four new transformation counts covering
  five sets: Jumpluff Chlorophyll -> Wind Rider (2), Klawf Regenerator ->
  Technician (1), Ledyba Rattled -> Iron Fist (1), Ledian Early Bird ->
  Technician (1). Candidate header changes contain only these replacements,
  the approved selected moves and derived move-usage counts. Promotion and
  deterministic profile verification pass. Seventeen checker unit tests pass.
- Reconciled another 31 keep-as-is rows across 69 stages/forms: Altaria,
  Arctovish/Arctozolt, Arrokuda, Basculin/Basculegion, Frillish, Girafarig,
  Kingdra, Lilligant, Magnemite, Manectric, Mawile, Palafin, Regigigas,
  Vanillite, Tyrunt, Larvitar, Shellos/Gastrodon and every Vivillon pattern.
  No gameplay edits were needed for this preservation group.
- Exact manifest checks pass for 274/650 decisions and 445 species/forms,
  with all 40 invariants, zero mismatches and zero new evolution move gaps.
  The separately reported historical source/target gaps now total 501.
  Species-report and baked-data verification pass. Five Tripwire tests pass,
  including Disguise absorbing a hit without disarming the tripwire; both
  Unspeakable tests pass; all three Heartthrob tests pass, including the
  explicitly retained Lovely Kiss -> Attract interaction. The whole-table
  competitive Ability legality test also passes (one test).
  `--require-complete` correctly exits 1 for 376 pending decisions, not covered
  target failures. The overall goal remains active.
- Playable debug ROM build PASS: 31,754,928 bytes used, padded to 32 MiB;
  EWRAM 261,522 bytes (99.76%) and IWRAM 25,604 bytes (78.14%). Final whitespace
  and worktree checks pass, with no tracked deletions. The agent did not stage,
  commit, push or switch branches. The final status check detected that the
  changes had been staged externally during the build/checkpoint; that index
  state is preserved, with the last documentation updates left unstaged.
  This checkpoint is verified, not whole-ledger
  completion; continue with the remaining 376 decisions and final audits.

### Selective move families — 2026-09-11

- Implemented 30 additional decisions across 36 species/forms: Aipom,
  Anorith, Baltoy, Bronzor, Buizel, Rolycoly, Dwebble, Cubone, Cottonee,
  Lileep, Dreepy, Litwick and Beldum families, plus Dhelmise and Druddigon.
  Applied only their approved level/tutor moves, Beldum/Metang/Metagross's
  second Abilities, and Whimsicott/Crustle/Druddigon's approved stat changes.
- Preserved Mega Metagross's Tough Claws, existing moves and rejected coverage.
  Added Headbutt to Beldum's evolutions, Head Smash to Alolan Marowak, and
  shared approved profile access with Mega Chandelure and Gigantamax Coalossal.
  Cradily receives Power Whip once as an evolution move, not duplicate upstream
  level-zero and level-one entries. Buizel/Floatzel do not receive Technician.
- Reviewed the generated profile diff: only approved moves and the derived
  Power Gem special-usage count changed. No competitive Ability remap or
  upstream refresh was needed. Offline deterministic profile verification,
  species/baked-data verification and both staged/unstaged whitespace checks
  pass. Exact checks cover 304/650 decisions and 481 species/forms, with all
  40 invariants, zero target errors and zero new evolution move-access gaps.
  The 547 historical gaps are reported separately, not automatically filled.
- The focused whole-table competitive Ability legality test passes (one test).
  Playable debug ROM build PASS: 31,755,120 bytes used, padded to 32 MiB;
  EWRAM 261,522 bytes (99.76%) and IWRAM 25,604 bytes (78.14%).
  `--require-complete` correctly exits 1 for the 346 pending decisions, not
  covered-target failures. Goal stays active. Existing externally staged
  changes remain staged; this checkpoint adds only unstaged changes. No stage,
  commit, push, branch switch or broad cleanup was performed.

### Bird, beast, chime and symbol families — 2026-09-11

- Implemented another 34 decisions across 80 species/forms. Selected moves
  cover Growlithe/Arcanine, Bagon/Salamence, Beedrill/Mega, Phanpy/Donphan,
  Dragonair, Vullaby/Mandibuzz, Rufflet/both Braviary, the Kantonian birds,
  all Kyurem forms, Volcarona, Boltund and Tropius. Galarian legendary birds
  retain their approved no-change treatment. No generic coverage was restored.
- Bagon's new Wish carries through Salamence and Mega Salamence; Shelgon
  already has it. Dragonite/Mega already have Dragonair's new Fly. Hisuian
  Braviary explicitly receives Rufflet's Steel Wing. Regular Growlithe's
  Hone Claws does not imply changes to the separate Hisuian evolution line.
- Cramorant receives +15 SpA/+20 Speed with no Defense increase, plus Belch
  at level 1 and Surf at 45. Its shared macro/profile covers Gulping/Gorging;
  a new invariant explicitly checks matching deltas across all three forms.
- Applied Chingling's +15 SpA/+10 SpD and Extrasensory/Disarming Voice;
  Chimecho already has both moves and otherwise remains unchanged. Poochyena
  receives only +15 Speed and Mightyena stays unchanged. Meowstic's two
  genders and both Megas retain their differentiated starting designs.
- Mienfoo learns Meditate at 5; Mienshao receives it once at 1 plus Blaze Kick
  as a tutor, avoiding upstream's duplicate Meditate placement. Togedemaru
  receives Rollout at 9 and Zippy Zap at 57, without modifying Zippy Zap itself.
  All 28 Unown forms receive Nature Power at 1 through their shared profile.
- Exact manifest checks pass for 338/650 decisions, 561 species/forms and all
  41 invariants, with zero target errors and zero new evolution-access gaps.
  The 599 historical source/target gaps remain separately reported. Offline
  deterministic profile and species/baked-data verification pass, without a
  source refresh. Both staged and unstaged whitespace checks pass. The focused
  whole-table competitive Ability legality test passes (one test). The playable
  debug ROM builds: 31,755,236 bytes used, padded to 32 MiB; EWRAM 261,522 bytes
  (99.76%), IWRAM 25,604 bytes (78.14%). `--require-complete` correctly exits 1
  for 312 pending decisions, not target errors. No staging, commits, pushes,
  branch changes or tracked deletions. The goal remains active; passing these
  checks is not a competitive-balance/playtesting signoff.

### Final typing families and selected mammals — 2026-09-11

- Implemented 22 more decisions across 35 species/forms. The lake trio now
  uses Psychic/Fairy with Moonblast at 41; Ninetales uses Fire/Fairy with
  evolution Dazzling Gleam and the Vulpix-line Moonblast tutor. Alolan forms
  remain unchanged. Misdreavus retains Ghost and receives its approved Fairy
  moves, Grudge and Hypnosis; Mismagius becomes Ghost/Fairy with matching access.
  Its new Grudge is placed at level 1 as a stone evolution's reminder move.
- Gothita's line remains pure Psychic with the selected Dark attacks. Regular
  Goodra reverts Dragon/Poison to pure Dragon while retaining its approved
  Acid Armor/Recover/Sludge Wave. Tearful Look accumulates across both branches;
  Recover is excluded from Goomy and the Hisuian line. The checker detected
  automatic removal of Goodra's existing Acid Armor tutor after adding the
  level-up placement; an explicit preservation rule restores that access.
- Seviper receives the approved Poison/Dark typing, physical redistribution
  and four level moves. Froslass gains Levitate in slot 2 and the same
  -10 Attack/+30 SpA on both forms, with an explicit Mega-delta invariant.
  Mega's approved 170 SpA remains a balance-watch note, not a reverted decision.
- Furfrou gains +10 Attack/+10 SpD across all ten trims without changing their
  individual typings. Buneary receives its selected moves; regular Lopunny
  becomes Normal/Fighting and shares its chosen kicks with Mega. Glameow and
  Purugly receive their approved stats and Crunch tutors, plus Purugly Slack Off.
- Exact checks cover 360/650 decisions and 596 species/forms: all 42 invariants
  pass, with zero mismatches and zero new evolution move-access gaps. The 646
  historical gaps remain separately reported. Deterministic profile and
  species/baked-data verification pass; no upstream refresh was needed. The
  focused whole-table competitive Ability legality test passes (one test).
  The full-completion gate correctly exits 1 for 290 pending decisions.
  Playable debug ROM build PASS: 31,755,392 bytes used, padded to 32 MiB;
  EWRAM 261,522 bytes (99.76%), IWRAM 25,604 bytes (78.14%). Staged and
  unstaged whitespace checks pass. No staging, commits, pushes, branch changes
  or tracked deletions. The goal stays active; balance playtesting remains
  separate from implementation verification.

### Rage, ghost, electric and armor families — 2026-09-11

- Applied 25 additional decisions across 32 species/forms: Mankey's full
  Defiant/Anger Point/Vital Spirit line, Aron/Lairon/Aggron's hidden Earth Eater,
  Gastly/Haunter/Gengar's Cursed Body/empty/Levitate, Zubat's Sniper/special
  progression, Elekid/Electabuzz's Volt Absorb/Static/Vital Spirit and
  Electivire's Iron Fist with selected punches. Mega Aggron and Mega Gengar
  retain their existing Abilities and stats; Gengar's Nightmare reaches all
  associated forms. Crobat retains the Hurricane access added to earlier stages.
- Applied Magby Scorching Sands, unchanged Magmar and Magmortar's hidden
  Quick Draw; Chinchou/Lanturn's luminous moves and Lanturn stats; Tynamo's
  Nuzzle and the selected physical moves on Eelektrik/Eelektross/Mega; and
  Voltorb-line Bug Buzz plus Electrode's Buzzy Buzz. No rejected projectile
  coverage or extra Zippy Zap distribution was introduced.
- Glalie and Mega Glalie become Ice/Rock with Rock Slide as an evolution
  move and Icicle Crash at 46; regular Glalie replaces Ice Body with Levitate.
  Stats and Mega Refrigerate are preserved; Snorunt remains unchanged.
- Added semantic competitive-set remaps for the Aron line's Heavy Metal,
  Magmortar's Vital Spirit and Glalie's Ice Body. The first guarded refresh
  stopped on exactly three new nonzero transformation counts: Aron 1,
  Glalie 2 and Magmortar 5; Lairon/Aggron record zero matches. The raw source
  aggregate changed, but a direct comparison confirms the normalized upstream
  Bundle is identical. Candidate diff contains only the selected moves and
  eight Ability replacements. Candidate exact-target checks pass with no new
  evolution move gaps; 714 historical gaps remain separately reported.
- Promotion through `refresh --accept-removals` completed with the same
  reviewed source aggregate and transformation changes. Offline deterministic
  profile verification and species/baked-data verification pass. Exact runtime
  checks cover 385/650 decisions, 628 species/forms and all 42 invariants, with
  zero target errors and zero new evolution gaps. Whitespace checks pass.
  The focused whole-table competitive Ability legality test passes (one test),
  including the remapped sets. The full-completion gate correctly exits 1
  for 265 pending decisions. Playable debug ROM build PASS: 31,755,496 bytes
  used, padded to 32 MiB; EWRAM 261,522 bytes (99.76%) and IWRAM 25,604 bytes
  (78.14%). Staged and unstaged whitespace checks pass. No staging, commits,
  pushes, branch changes or tracked deletions; existing index state preserved.
  Goal remains active, with balance playtesting separate from verification.

### Stat progressions, shared transformations and regional mammals — 2026-09-11

- Applied 43 additional decisions across 89 species/forms. Bibarel, Cinccino,
  Cryogonal, Dunsparce, Pyukumuku, Swellow and Talonflame receive their exact
  approved canonical deltas. Lotad/Lombre/Ludicolo receive +5/+10/+20 Speed;
  Wooper and Paldean Wooper match Quagsire's +10 Attack/+10 Defense while
  Clodsire stays unchanged. Charjabug gains only +20 Attack, not HP or Speed;
  Grubbin and Vikavolt remain unchanged. Sizzlipede and both Centiskorch forms
  receive the selected Special Attack-to-Speed redistributions.
- Lunatone and Solrock gain +20 in their respective primary offense. Lunatone
  adds Night Daze/Power Gem at level 1 and Lunar Dance at 55, preserving its
  Power Gem tutor. Wailord gains +30 HP and Bouncy Bubble at 72. All fourteen
  Minior forms gain +20 HP and share Air Slash tutor access. All eighteen
  Silvally types gain +5 in every stat, reaching 600 BST; Type: Null stays
  unchanged. New explicit invariants cover these shared transformations.
- Regular/Gigantamax Meowth, both Persian forms and Perrserker receive the
  selected stats; regular Persian gains Quash, and Galarian Meowth/Perrserker
  gain Bullet Punch tutors. Alolan Meowth stays unchanged. Both Raticate forms
  reach 450 BST. The Alolan Totem identifier is a placeholder with no runtime
  species record in this checkout, so there is no separate form data to edit.
  No new species or save identifiers were introduced to manufacture one.
- Jynx and Smoochum receive the selected stats and Draining Kiss at level 1
  while preserving their existing tutors. Onix gains its approved Attack and
  Speed, and Steelix/Mega share +20 Attack/-10 SpA; all three gain Glare at 30
  and Coil at 50. Noctowl gains its specialized spread. Gogoat receives the
  approved stats but stays pure Grass, matching unchanged Skiddo. Obstagoon
  gains its selected stats; both Galarian pre-evolutions stay unchanged.
  Regular Linoone gains Tail Slap at 38 with its tutor preserved; Zigzagoon's
  current direct access is tutor-only despite the historical row's wording,
  and its approved keep-as-is decision does not add a new placement.
- Also reconciled the separate Goodra and Gothita-line typing-audit rows
  against their already verified runtime targets; these were implemented but
  their audit-row fingerprints were missing from the earlier batch.
- Exact checks cover 430/650 decisions, 717 species/forms and all 49 shared
  invariants: zero mismatches and zero new evolution move gaps. The 826
  historical source/target gaps remain separately reported. Offline profile
  regeneration verifies deterministically; no upstream refresh was needed.
  Species/bake verification and the focused whole-table competitive Ability
  legality test pass (one test). The full-completion gate correctly exits 1
  for 220 pending decisions. Playable debug ROM build PASS: 31,755,548 bytes
  used, padded to 32 MiB; EWRAM 261,522 bytes (99.76%), IWRAM 25,604 bytes
  (78.14%). Staged and unstaged whitespace checks pass. No staging, commits,
  pushes, branch changes or tracked deletions; existing index state preserved.
  The goal remains active, with balance playtesting separate from verification.

### Rolling, punching and selected physical family packages — 2026-09-11

- Applied 38 more decisions across 48 species/forms. Geodude's two lines gain
  Rapid Spin/Accelerock, regular forms also gain Magnitude, and both Golem
  forms gain Steamroller. Machop's entire line gains Mach Punch/Drain Punch
  tutors, including Gigantamax Machamp. Crabrawler/Crabominable gain the three
  selected punches and the final stage gains a Crabhammer tutor. Chewtle and
  both Drednaw forms receive Aqua Jet/Shell Smash, without Scale Shot.
- Applied Seedot/Nuzleaf/Shiftry's +5/+10/+20 Attack progression and Shiftry's
  single Parting Shot evolution placement. Blitzle/Zebstrika gain the selected
  physical Fire/Nuzzle moves; Blitzle gains High Horsepower by level while
  Zebstrika retains its existing tutor. No new Zippy Zap distribution.
- All four Deerling and Sawsbuck forms gain Camouflage, Jump Kick, Nature Power
  and High Horsepower; Sawsbuck's shared definition gains +5 HP/+10 Attack.
  Shinx's line gains the selected utility moves, Shinx gains Thunder Fang by
  level, and Luxray gains Psychic Fangs without Dark typing/Sucker Punch.
  Gible is unchanged, Gabite gains one Dual Chop evolution placement, and
  Garchomp plus both Megas gain Dual Chop at 1 and Fly tutor access.
- Omanyte/Omastar gain Power Gem and Omastar gains Shelter. Remoraid/Octillery
  gain Signal Beam, Icicle Spear and Zap Cannon, with Octillery's approved
  +10 Attack/+10 SpA. Spheal/Sealeo/Walrein gain Ice Ball and Slack Off.
  No rejected replacement learnsets or broad coverage were imported.
- The checker caught tutor removals caused by level-up promotion: Drednaw's
  Shell Smash, both deer species' Jump Kick and Garchomp's Dual Chop. Explicit
  preservation rules restore all four tutors and their associated forms.
- Shieldon/Bastiodon gain Head Smash at 51/66; their already implemented
  Solid Rock/Impenetrable package, existing stats and no-new-recovery decision
  are preserved. Slakoth/Slaking's existing Truant/empty/Slow Start and King's
  Domain regular-slot suppression were checked against current code and the
  existing four focused tests; no engine change was needed. Vigoroth gains
  hidden Sheer Force while retaining Vital Spirit and Fever Pitch. No existing
  regular Ability was removed, so no competitive remap or source refresh was
  needed in this batch.
- Exact checks cover 468/650 decisions and 765 species/forms; all 49 invariants
  pass, with zero target errors and zero new evolution move gaps. The 928
  historical gaps remain separately reported. The full-completion gate exits
  1 for 182 pending decisions. Deterministic profile and species/bake
  verification pass. The focused whole-table competitive Ability legality
  test passes (one test). Playable debug ROM build PASS: 31,755,848 bytes
  used, padded to 32 MiB; EWRAM 261,522 bytes (99.76%), IWRAM 25,604 bytes
  (78.14%). Staged and unstaged whitespace checks pass; no tracked deletions,
  staging, commits, pushes or branch changes. Goal remains active, with
  manual balance playtesting separate from implementation verification.

### Wool, gear, dream, hive and immunity packages — 2026-09-11

- Applied source targets for 29 decisions across 30 species/forms: the full
  Mareep Static/Overcoat/Cotton Down line; Klink's Plus/Minus/Levitate line;
  Lickitung/Lickilicky Unaware/Oblivious/Cloud Nine; Munna/Musharna Comatose;
  Combee Honey Gather/Hustle/Sweet Veil and Vespiquen Pressure/Unnerve/Queenly
  Majesty; Durant Heatproof; Dusknoir Iron Fist; and the legendary beasts'
  hidden Flash Fire/Volt Absorb/Water Absorb. Duskull/Dusclops are preserved.
- Ampharos remains Electric and Mega remains Electric/Dragon with Mold Breaker;
  both gain Signal Beam at 51 and Draco Meteor tutor access, without the rejected
  coverage or stat changes. Oddish/Gloom/Vileplume gain Leaf Guard in slot 2.
  Bellossom keeps Grass, adds Cloud Nine in slot 2, and gains Draining Kiss as
  a single evolution move, Leaf Blade at 1 and Fiery Dance at 79. No Apple Acid.
- Klink's line gains Flame Wheel at 28; Lickilicky gains Curse tutor access;
  Vespiquen gains Fly tutor access. Base Rotom gains +10 Speed. Every Rotom
  form gains Ominous Wind at 25 and its selected Ghost/appliance attack at 45,
  with existing Abilities preserved exactly. No proposed Motor Drive was added.
- Added semantic remaps for removed regular Abilities in competitive sets.
  The guarded refresh stopped on seven new nonzero counts: Dusknoir 5,
  Entei 6, Klang 5, Klink 1, Klinklang 9, Raikou 6 and Suicune 2. The eight
  remaining new remaps correctly record zero current matches. The source hash
  and normalized upstream bundle are unchanged; candidate header review shows
  only the approved moves, their derived usage counts and 34 Ability remaps.
  Candidate checks pass for 497/650 decisions, 795 species/forms and all 49
  invariants, with zero mismatches or new evolution gaps; 944 historical gaps
  remain separately reported. The reviewed candidate was accepted with
  `refresh --accept-removals`; deterministic profile verification and final
  species/bake verification pass. Competitive Ability legality passes (1/1).
  The playable debug ROM builds (31,755,932 bytes used, padded to 32 MiB).
  The final full-manifest check has zero errors/new gaps and is incomplete
  solely because 153 decisions remain. This is a verified partial checkpoint,
  not completed-ledger signoff.

### Apple, sugar, stat and preservation follow-through — 2026-09-11

- Reconciled 20 additional ledger rows against 26 species/forms. Appletun and
  Gigantamax share canonical +5 Defense/SpA/SpD and Superpower tutor access;
  Flapple and Gigantamax share -10 SpA/+25 Speed and Scale Shot tutor access.
  Applin gains Leaf Storm as a tutor; all four evolutions already have access,
  including unchanged Dipplin/Hydrapple. Added two form-delta invariants.
- Swirlix/Slurpuff gain Covet at upstream level 29, and Slurpuff gains +10
  Attack. Galarian Slowbro uses -25 Attack/+10 Defense/+15 SpA; Surskit uses
  -2 Defense/+5 SpA/+3 SpD; Swalot uses +5 Defense/+20 SpA/+5 SpD. Gulpin stays
  unchanged. No Ability or typing changes are part of this batch.
- Trapinch gains Bug Bite at 1 and explicitly retains its existing tutor;
  Vibrava/Flygon already have tutor access. The full line's existing stats,
  Abilities and typing are protected, including pure Ground Trapinch.
- Added exact no-change coverage for Capsakid, Trubbish, Grimer, Koffing and
  regular Weezing, Pelipper, Poliwag, Slowking, Shuppet and Weavile. Rejected
  moves, typings, stats and Abilities are not imported.
- Profiles were regenerated offline from the accepted lock; species reports
  and runtime bake data regenerated. Deterministic profile verification and
  species/bake verification pass. Competitive Ability legality passes (1/1).
  The playable debug ROM builds (31,755,948 bytes used, padded to 32 MiB).
  Staged and unstaged whitespace checks pass; no tracked files were deleted.
- Exact checks cover 517/650 decisions, 821 species/forms and 51 invariants,
  with zero mismatches/new evolution gaps. The 984 historical gaps are reported
  separately. `--require-complete` remains incomplete solely for the 133 pending
  decisions. This is a verified partial checkpoint, not completed-ledger signoff.
- Next batch: selected move-only family packages (including Abra, Clefairy,
  Golett, Meditite and Togepi), keeping later distribution cuts and existing
  tutor access intact. Continue from the report's pending rows; do not replay
  previously verified batches or refresh upstream for overlay-only changes.

### Selected move families and legendary signatures — 2026-09-11

- Implemented 41 ledger decisions covering 81 species/forms, including the
  untouched earlier stages of the selected families. No Ability or typing
  changes; Xatu receives canonical -10 Attack/+10 SpA/+10 SpD, Zangoose +2 HP/
  +5 Speed, and Stoutland +10 HP/+5 Attack. Existing identifiers are unchanged.
- Aerodactyl/Mega gain Brave Bird at 81; Alakazam/Mega gain Miracle Eye 23,
  Telekinesis 33, Aura Sphere 49 and Charge Beam tutor. Every Arceus type gains
  Punishment, Roar of Time, Shadow Force and Spacial Rend at 1. Breloom gains
  Mind Reader 33, Sky Uppercut 39 and Spore 50; Shroomish remains unchanged.
- Cleffa gains Magical Leaf 16 and Draining Kiss 19; Clefairy gains Spotlight 1
  and Draining Kiss 19; Clefable/Mega gain Spotlight 1, retaining existing
  Draining Kiss/Magical Leaf tutors and skipping Power Gem. Golett/Golurk gain
  Magnitude 30 and Focus Punch 61/69; Golurk/Mega gain Power-Up Punch tutor.
- Meditite and Medicham/Mega gain Meditate 4 and Mind Reader 25; Medicham/Mega
  gain Psycho Cut tutor. Meditate is also carried into the final forms under
  the approved accumulation rule: they previously had no direct access.
  Togepi/Togetic gain Draining Kiss 20; Togetic gains one Air Cutter evolution
  placement (0), carried into Togekiss at 1. No duplicate 0/1 additions.
- Natu/Xatu gain Lucky Chant 12, Ominous Wind 20 and Miracle Eye 36/39.
  Nidoking gains Thrash 35; Nidoqueen gains Body Slam 35. Ninjask gains First
  Impression 29; Pinsir/Mega Megahorn 47; Kingler/Gigantamax Aqua Jet 1 and
  Ice Hammer 70; Shuckle Encore 5; Sneasel Ice Shard 50; Snorlax/Gigantamax
  Rollout 36. Meloetta's two forms gain Wake-Up Slap tutor, Milotic Moonblast
  44, and Yanmega Hypnosis/Fly/Hurricane tutors. Zangoose does not gain Lunge.
- Skwovet/Greedent gain Recycle 50/62; Guzzlord Steamroller 19, Wring Out 67
  and Dragon Hammer 73; Pancham/Pangoro Sucker Punch tutor. Both Muk forms gain
  one Venom Drench evolution move, with Parting Shot 1 on Alolan Muk only.
  Galarian Weezing gains Aromatherapy 24. Tapu Bulu gains Rototiller 38 and
  Play Rough 63; Koko Play Rough 63; Fini Refresh 26. Herdier/Stoutland gain
  Odor Sleuth once at 5. Existing moves and Abilities remain intact.
- Added narrow explicit tutor-preservation rules for 14 moves promoted to
  level-up access, avoiding generator-derived compatibility removal. Profiles
  regenerate and verify deterministically offline; species reports and runtime
  bake data regenerate and pass verification. Competitive Ability legality
  passes (1/1). Staged/unstaged whitespace checks pass; no tracked deletions.
- Exact checks cover 558/650 decisions, 902 species/forms and 51 invariants,
  with zero mismatches or new evolution gaps. The 1,091 historical gaps remain
  separately reported. The playable debug ROM builds (31,756,212 bytes used,
  padded to 32 MiB). `--require-complete` remains incomplete solely because
  92 ledger decisions are pending. This is a verified partial checkpoint.
- Next: remaining combined Ability/stat packages and their semantic competitive
  remaps, followed by the final custom-move and whole-ledger reconciliation.
  Keep the pinned cache for overlay-only batches; any new transformation match
  counts must use the guarded refresh/review workflow.

### Remaining Ability and paired-family packages — 2026-09-11

- Applied 55 species-review rows plus the Beartic, Midday Lycanroc and
  Ursaring typing-audit confirmations: 58 decisions, 85 explicit targets.
  Accelgor gains Bug/Dark and Infiltrator. Beartic remains Ice, Midday Lycanroc
  Rock, and Ursaring Normal; their rejected secondary typings stay excluded.
- Added the approved slots/replacements for Beartic, Chatot, Drifloon's line,
  Ducklett/Swanna, Karrablast/Escavalier, Whismur's line, Sentret/Furret,
  Garbodor/Gigantamax, Igglybuff's evolutions, Lucario, all three Lycanroc forms,
  Masquerain, Pachirisu, Pidove's line, the three Regis, Runerigus, Sableye,
  both Sandshrew lines, Spearow/Fearow, both Stunfisk forms, Teddiursa/Ursaring
  and Volcanion. Igglybuff and all Mega Abilities remain as previously designed.
- Natural Cure occupies slot 2 on all five Flabebe, six Floette and five
  Florges forms. Eternal Floette's moves carry into Mega Floette, which keeps
  Fairy Aura. Every Pumpkaboo/Gourgeist size receives Flash Fire and the
  selected shared tutors; their individual stats remain unchanged.
- Applied the final canonical stat deltas to Sandslash, Jigglypuff, Sentret,
  Furret, regular/Galarian Corsola, Cursola, Pachirisu, Pidove/Tranquill/Unfezant,
  Cofagrigus/Runerigus, Swanna and both Stunfisk forms. Regular Corsola's extra
  defenses are set locally, not through the shared macro used by the Galarian
  branch; the latter keeps its explicitly approved weaker defensive spread.
- Added 61 selected level-up rules, 16 selected tutor rules and 19 narrow
  tutor-preservation rules. Evolution moves are added once, not duplicated at
  0 and 1. Existing Ice Fang on Beartic, Draining Kiss on Wigglytuff/Florges,
  Sand Tomb on Sandslash and Ice Shard on Alolan Sandslash satisfy accumulation
  without extra placements. Mega Lucario Z inherits Power-Up Punch while
  retaining Aura Guard. Both Ursaluna forms gain Mega Kick tutor access under
  the approved accumulation rule, without stats or Ability changes.
- Removed only Alolan Sandshrew's explicit Jetstream tutor rule as approved.
  Magnitude stays compatibility-only on regular Sandshrew/Sandslash; no new
  broad coverage packages or rejected Mystical Fire are imported.
- Added semantic competitive-Ability remaps for 26 source species. Reviewed
  and accepted six nonzero match-count changes (19 sets), with unchanged source
  hashes and normalized upstream content. Ursaring's old Quick Feet status-orb
  set uses retained Guts to preserve Facade synergy. Ducklett's pre-existing
  normalization now selects its approved Hydro Pump instead of Surf.
- Profiles verify deterministically; species reports and runtime bake verify.
  Competitive Ability legality passes (1/1). Exact reconciliation passes for
  616/650 decisions, 987 species/forms and 51 invariants, with zero mismatches
  or new evolution gaps; 1,277 historical gaps are reported separately.
  Staged/unstaged whitespace checks pass. The incremental playable-ROM build
  will cover this and the following final data batch together; final signoff
  remains pending.

### Final stat and learnset families — 2026-09-11

- Applied 29 more ledger decisions to 58 explicit species/forms. Banette and
  Mega Banette use the pinned upstream stats (64/125/65/93/63/65 and
  64/175/75/103/83/75); only Mega gains Ghost/Normal. Shared macro typing is
  split into local definitions, avoiding unintended base-form changes.
- Applied canonical deltas for Cherubi/Cherrim, Golduck, Snubbull/Granbull,
  Litleo/Pyroar, Mime Jr./both Mr. Mime forms/Mr. Rime, Plusle/Minun,
  Nosepass/Probopass, Poliwrath and regular Raichu. Mega Pyroar and Mega
  Raichu X/Y are absent from the canonical CSV, so exact manifest targets use
  their unchanged starting spreads plus the approved delta: Pyroar Mega
  86/108/92/129/86/126; Raichu X 60/140/95/95/95/110; Raichu Y
  60/105/55/165/80/130. Their Abilities remain unchanged.
- Added 41 selected level-up rules, ten selected tutor rules and eleven
  narrow tutor-preservation rules. Removed only Exeggcute's Grav Apple.
  Bestow at 50 accompanies Natural Gift at 37 on both Exeggutor forms to
  satisfy the approved accumulation rule; both retain Barrage compatibility
  and gain Soft-Boiled tutor, with Dragon Dance on Alolan Exeggutor only.
- Added 28 explicit rules for Pikachu's independently sourced costume/cap
  profiles after the checker caught their lack of automatic base inheritance.
  Every Pikachu form gains Splishy Splash/Floaty Fall at 61; Raichu forms
  receive tutors. Neither Pichu nor any Pikachu/Raichu form gains Zippy Zap.
- Added independent 450-BST checks for Plusle/Minun and matching canonical
  delta checks for Banette/Mega and Cherrim's weather forms (54 invariants).
  Pure-Fairy Snubbull/Granbull, Ground/Flying Dodrio and Rock/Steel Probopass
  remain intact. Unchanged ancestors and regional branches are explicit.

### Aromatisse and Flame Relay — 2026-09-11

- Aromatisse receives every move and timing from the pinned Revised learnset,
  represented by 16 level overrides and two added tutors (Toxic/Charge Beam),
  with existing tutor access preserved. Upstream's repeated Sweet Kiss and
  Odor Sleuth placements collapse to their earliest level (1), consistent with
  the generator's single-placement model. Its original level moves all remain
  in the upstream set; no bulk removal or competitive-set copy is necessary.
  Spritzee's profile is explicitly unchanged.
- Appended Flame Relay at learnable ID 864, after Swirling Blade 863. No
  pre-existing learnable IDs or save structures change. It uses the shared
  EFFECT_HIT_ESCAPE engine/AI path and Flame Charge's existing fiery-charge
  animation; no new pivot engine or additional damage/status mechanics.
  Added long/short names, description and six focused behavior tests.
- Both regular horses learn Flame Relay at 40 and Jump Kick at 43. Galarian
  Ponyta/Rapidash learn Jump Kick at 43/46, without the Fire pivot move.
- `check_rebalance_manifest.py --require-complete`: PASS, 650/650 decisions,
  1,051 species/forms, 54 invariants, no exact mismatches or new evolution
  move gaps. All 17 checker unit tests pass. Profiles verify deterministically.
  Species/bake verification, Flame Relay tests and the final playable build
  are pending; this is data reconciliation, not final implementation signoff.

### Whole-tree reconciliation and final validation — 2026-09-11

- All 439 species stat/type/regular-Ability changes are covered explicitly.
  Auditing every changed learnset found Mega Crabominable inheriting the
  approved Power-Up Punch/Dizzy Punch/Mach Punch and Crabhammer correctly but
  missing its own manifest target; added the target without changing its data.
  All 514 changed profiles are now covered. No species or profiles were added
  or removed. The checker now enforces this whole-tree coverage automatically;
  two regression tests cover accidental macro and inherited-form changes.
- Final exact-data gate passes: 650/650 ledger decisions, 1,052 species/forms,
  54 invariants, zero mismatches, zero uncovered changes and zero new evolution
  move gaps. The 1,371 historical gaps remain separately reported; this is not
  a claim that all inherited upstream learnsets were already cumulative.
- Flame Relay is learnable only by regular Ponyta/Rapidash. Zippy Zap remains
  restricted to Voltorb, Electrode, Blitzle, Zebstrika and Togedemaru. The
  Pikachu/Raichu family has no direct or inherited Zippy Zap access.
- All six Flame Relay behavior tests pass: successful damage/pivot, Protect,
  Flash Fire, no replacement, contact damage before switching, and battle-end
  handling. Three initial INVALID cases were test setup omissions (reserve
  Speed when explicit Speed was used), corrected before the passing run.
  The shared hit-and-switch engine required no behavioral modification.
- All 19 manifest-tooling tests pass. Deterministic profile verification and
  species/bake verification pass. The latest Ability-legality integration
  test passes (1/1); no regular Ability data changed afterward. Both staged
  and unstaged whitespace checks pass; no tracked files were deleted.
- Final incremental playable debug ROM build PASS: 31,756,936 ROM bytes used,
  padded to 33,554,432 bytes in `pokeemerald.gba`. EWRAM remains 261,522 bytes
  (99.76%); IWRAM 25,604 bytes (78.14%). Existing unused-function warnings do
  not prevent the build. No commit, push, branch switch, staging or unstaging
  was performed by this implementation.
- Independent final Mega sweep: every base with an approved stat target has
  the matching canonical delta on its existing Mega forms. For the three
  Megas absent from the canonical CSV, the exact starting-spread-plus-delta
  targets described above also match. No unexplained Mega stat gaps remain.

## Completion and playtesting handoff

Implementation is complete. The whole-ledger checker requires full coverage,
and the generated-data verifiers plus focused tests and playable build pass.
The ROM is a debug build, not a release-performance benchmark. No manual
emulator playthrough or competitive-balance certification is implied.

Retain the approved designs for playtesting, particularly:

- Raichu/Mega Raichu/Alolan Raichu with Splishy Splash and Floaty Fall.
- Volbeat/Illumise's Call/Response priority Baton Pass interactions.
- Gorebyss's Regenerator with Shell Smash/Baton Pass, including mixed use.
- Shieldon/Bastiodon's special immunity plus Solid Rock, without new recovery.
- Luvdisc's Lovely Kiss/Heartthrob interaction and Camerupt's Slack Off routes.
- Flame Relay's in-game presentation and regular Ponyta/Rapidash pivot role.

These are deliberate balance trials, not missing implementation work. Historical
learnset gaps remain available in `build/rebalance/verification.json` for a
separate review; they were not silently broadened into extra distribution.

### Spider Disguise storage follow-up — 2026-09-11

- Removed the dedicated `disguiseBroken` array. Spider Disguise now reuses
  `transformationAbilityUsed`, alongside the mutually exclusive Zero to Hero
  and Rapid Replica mechanisms. `BattleStruct` has its pre-rebalance layout
  again; no other RAM or save structure grows to replace the removed storage.
- Once-per-battle party-slot persistence and Mimikyu's existing form handling
  are unchanged. Added regressions for independent spider/Palafin/Ditto state
  in one party and for Transform not granting a copied spider's shield.
- All 25 focused tests pass: Disguise (13), Zero to Hero (5), Rapid Replica
  (2), and Tripwire (5). Incremental playable debug ROM build and
  `git diff --check` pass. ROM usage is 31,756,848 bytes, padded to 32 MiB.
  Existing staged work was preserved; this follow-up was not staged or committed.

### Pokédex move markers and display checks — 2026-09-11

- Added generated ROM-only move-addition bitmaps and a star/legend in the
  level-up, evolution, tutor, TM and TR lists. Source comparisons are per
  learning method, ignore level-only timing edits and normalize Hail's rename;
  shared form profiles inherit the same bitmap. No RAM/save structure grows.
- Gogoat's compiled baseline/current comparison yields +0/+8/+10/-30/+6/+6
  despite unchanged BST. Cacturne's current second Ability resolves to Rough
  Skin while its vanilla slot is empty. Both dedicated runtime checks pass;
  the user's reported missing display has not been reproduced. A screenshot
  and loaded-ROM identification were requested. A: Inspect intentionally shows
  original Abilities, while A: Back restores the current Ability list.
- Offline profile verification and the complete 650-decision manifest pass;
  all 11 filtered Pokédex tests pass, including marker provenance, shared forms,
  bounds and text width. Playable debug ROM build and `git diff --check` pass.
  ROM usage is 31,774,664 bytes; static EWRAM/IWRAM usage remains unchanged.
  No staging, commit or push was performed.

### Pokédex marker rendering correction — 2026-09-11

- User screenshots confirmed that the move legend appeared but row stars did
  not. The small font's star slot has no visible star; the previous width test
  did not catch that. Row markers now use a direct 7x7 green pixel star in the
  existing gutter, without changing font assets or adding RAM structures.
- The false exclamation mark on Beautifly's empty Ability slot came from
  `GetAbilityBySpecies` returning Swarm as its battle fallback. The dex now
  reads raw slots, preserving custom-mon overrides and genuine additions or
  removals, without changing the battle helper.
- Added pixel-buffer regression coverage for visible markers and clearing on
  row reuse, plus raw empty-slot comparison checks. All 13 filtered Pokédex
  tests pass. Playable debug ROM build and `git diff --check` pass; ROM usage
  is 31,774,852 bytes, with unchanged static EWRAM/IWRAM usage.
