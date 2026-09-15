# Opening and first-adventure audit

Scope: new-game setup through the first return and Main Hall workbench demonstration.
This is the active underground-hub sequence. Intro values 0–8, save structures,
Champion/Pokabbie unlock conditions, and adventure rewards are unchanged.

## Scene and progression ledger

Paths below are under `data/maps/` unless stated otherwise. State names abbreviate
`ROGUE_INTRO_STATE_*`. A message's next step is part of its mandatory dialogue.

| Scene / authoritative source | Trigger and prerequisite | Gifts / transition | Purpose and next step | Repeat and cancellation behaviour |
| --- | --- | --- | --- | --- |
| Setup — `Rogue_Intro`, `src/rogue_settings_ui.c` | New-game frame script, before Birch | Initial Pokédex and gimmick persisted only after confirmation; intro remains SPAWN (0) | Explain encounters/first partners and the Champion requirement; continue to character introduction | Browsing is draft-only. Initial Continue names the pool; Go back is selected by default and retains all drafts. B also goes back. Later setup never uses this confirmation. |
| Birch introduction — `Rogue_Intro` | After setup accepted | Character/hub names; warp to Labs | Birch helped after the fall; routes change and Pokémon appear away from home; the hub is a safe research base | Appearance/name confirmation retains existing retry loop. Light memory framing remains; repeated relief and memory remarks removed. |
| Hub exploration — `Rogue_Area_Labs` | Arrival in SPAWN | SPAWN → EXPLORE (1) | Welcome to the hub; left-hand lab door leads to Birch | Frame and intro guards prevent replay. Birch walks through the authored door at (9,4). |
| Escape report — `Rogue_Interior_ProfLab` | One of three approach triggers in EXPLORE | EXPLORE → LEAVE_LAB (2) | Connect exploration, research requests and hub growth; Pokémon escaped through broken glass in the Safari Lab; help Birch | All three approaches converge in front of Birch. Ruin maniac and Birch leave; trigger is disabled by state. |
| Birch guide — `Rogue_Area_Labs` | Leave lab in LEAVE_LAB | LEAVE_LAB → CATCH_MON (3) | Follow Birch toward the connected Adventure Gate and Safari Lab | Existing directional movements retained and checked. Nurse's catch-stage directions resolve the Safari connection dynamically. |
| Escaped starters — `Rogue_Area_SafariZone_Tutorial` | Enter active starter enclosure in CATCH_MON | Existing single Poké Ball; successful capture sets REPORT_TO_PROF (4) in runtime | Approach one Pokémon and catch it; Birch handles the others | Existing battle/capture recovery retained. Exits remain blocked during capture. Return scene brings player and Birch together and warps back to lab. |
| Partner return / preparations — `Rogue_Interior_ProfLab` | REPORT_TO_PROF frame scene | 10 Poké Balls, 5 Potions, initial gimmick when applicable, Pokédex; → COLLECT_QUEST_LOG (5) | Partner enables research; mandatory fresh-adventure, restoration and fainting consequences; next speak to Adventure Gate nurse | Gifts remain protected by intro/frame progression. Existing Pokédex ownership branch retained. Optional explanation is repeatable through Birch. |
| Assistant — same lab | End of preparation scene | Existing follower preference only | Offer partner following | Nickname is reloaded after follower setup before reuse. No Pokémon name is carried across a menu or special without rebuffering. Repeat conversation toggles following. |
| Quest handoff — `Rogue_Area_AdventureEntrance` and `data/scripts/Rogue/Rogue_Manager.pory` | Nurse in COLLECT_QUEST_LOG | Quest Book if absent; initial quest display; → GO_ON_ADVENTURE (6) | Book tracks requests; walk to board; next use Adventure Console | Nurse remains in place. Player starts east (11,6) or south (10,7), walks to (4,5), faces north. Other actors locked; follower temporarily hidden, previous hide state restored. Unknown position leaves state 5 and directs manual board use. Book ownership gates completion; manual/automatic routes share one guarded completion script. Early board use gives guidance, later use opens ordinary quests. |
| Departure — Adventure Gate console / `Rogue_StartAdventureInternal` | GO_ON_ADVENTURE; committed valid setup, successful save and team preparation | GO_ON_ADVENTURE → LEARN_TO_BUILD (7), existing run-start flags and portal transition | One brief route-choice hint immediately before first departure | Existing validation, save cancellation, starter cancellation, party restoration and retry paths retained. Hint/state change occur after validation; failed or cancelled departures do not consume them. Later departures omit the hint. |
| Route map — `Rogue_AdventurePaths` | Portal arrival | Existing map/route state | Inspect stops and choose a route | Existing contextual preview retained; no second general onboarding lecture. |
| First return — `Rogue_ReturnAdventureInternal` in Adventure Gate | Existing LEFT_ON_RUN return hook, intro 7 | Existing actual money/level/Pokéblock rewards; clears return flag; optional save | Acknowledge return without claiming victory; show actual rewards or no-reward message; explain restoration/lasting quest progress; next visit Main Hall builder | Builder direction is independent of reward amount or victory. State remains 7 until builder tutorial. Later returns keep the console direction. |
| Building — `Rogue_Area_TownSquare` | Speak from above builder in LEARN_TO_BUILD | Existing Town Map and one Building Supplies grant; → COMPLETE (8), reveals workbenches | Map shows rooms; Quests earn Building Supplies; workbench chooses construction; next Adventure Gate console | Gift space checked before progression; already-owned Town Map not duplicated. Builder steps right; player steps down to bench. Purchases are optional; leaving does not claim construction succeeded. Subsequent talks do not repeat gifts. |

## Mechanical and text checks

- `RogueSave_SaveHubStates` / `RogueSave_LoadHubStates` preserve and restore the
  party (including moves), Day Care, money, remembered configuration and Bag.
  `EndRogueRun` pushes returning party Pokémon into the Safari system
  before restoration. Return reward specials provide the displayed amounts.
- Fainting text explicitly describes the **base rules**, rather than saying
  "normally." The optional explanation names Standard Adventures and buffers
  their starting level from `STARTER_MON_LEVEL`; it does not apply that level to
  other modes. Safari retention refers to Pokémon caught and brought back in
  the party, rather than claiming the original hub team is copied there too.
- `RoguePokedex_IsVariantEditUnlocked` retains Champion OR meeting Pokabbie.
  Ordinary context eligibility still uses `RoguePokedex_IsVariantEditEnabled`.
  Locked help distinguishes the Champion milestone from a context restriction.
  The early unlock is not advertised in onboarding.
- Confirmation lives entirely in the existing temporary settings-menu state.
  It precedes `RoguePokedex_StoreInitialSelection`; a per-menu guard rejects a
  second commit. Applying the draft retains the existing initial trainer-pool
  selection semantics. No serialized flag or public gameplay API was added.
- Names standardized in touched text: Quest Book, Quest Board, Adventure
  Console, Safari Lab, Main Hall, Building Supplies. The escaped Pokémon are
  identified before Birch asks for help; no explanation for the portal or route
  mystery is invented.
  The Bag retains its existing compact `Build Parts` item label; no item-name
  storage limit or saved item identity was changed.
- `.pory` formatting uses the repository font configuration, including reserved
  widths for player, hub and Pokémon substitutions. Generated `.inc` files are
  build outputs. Settings use the existing small narrow font and window.
- Dynamic return amounts are buffered immediately before each reward message.
  Assistant nickname is refreshed after the follower special. Setup confirmation
  reads the selected pool directly from its draft, rather than script buffers.
- Mandatory consequences are outside Birch's optional carryover explanation.
  That repeatable explanation covers the Standard starting level, restored
  levels/moves, Safari retention, Bag restoration and separate rewards.

## Clarity follow-up

Reviewed every added or rewritten player-facing string in the onboarding diff.
Replaced uncertain mechanical wording with the rule or action it referred to:

- "Shapes encounters" now states that the Pokédex determines encounter and
  first-partner choices; the Champion restriction stays explicit.
- "Most finds won't come home" and "ordinary finds" now explain that the old
  Bag contents are restored and Adventure rewards are added separately.
- Character creation retains "Do you remember what you look like?" at the
  user's request, keeping the light memory framing.
- Route instructions identify the type/difficulty preview and entry choice.
- Nurse/board instructions name the Quest Book, the board, and collecting Quest
  rewards. The builder handoff states that he gives supplies and teaches room
  construction; Bag-full retries name the missing space and who to return to.
- Context-locked Pokédex help directs the player to their own hub.

Kept natural greetings and the unexplained route mystery: these are character
dialogue, not uncertain instructions or promises about gameplay.

The story introduction explicitly establishes the portal to another realm,
the shifting land and displaced Pokémon beyond it, and the underground
laboratory/hideout used to investigate it. Birch's lab invitation connects
the player's expeditions and Quest supplies to research and expanding the base.
The cause of the shifting realm and the portal's origin remain unexplained.

The opening is kept to a brief premise, using the original researcher's curiosity
and "Let's see what you remember" transition into character creation. It does
not discuss finding the player a partner; that conversation belongs in the lab.

The subsequent voice pass retains explicit mechanics while treating spoken
dialogue as a conversation. Birch describes his own observations and admits what
he has not figured out; the nurse welcomes the player and points out the next
service; the builder expresses enthusiasm for filling the map's empty spaces.
Setting labels and the Champion restriction remain direct. The appearance-memory
question remains intact. No new lore explains the portal's origin, and cancelling
the workbench still receives a neutral farewell rather than congratulations for
a purchase. Dialogue-only edits do not alter gifts, movement or progression.

After the escaped-starter return, Birch gives a mandatory two-message introduction
to Unique Pokémon and Treasure rooms before explaining departure rules. Unique
Pokémon are described as individuals with moves or Abilities others of their
species do not have, rather than conflating them with species-default Unique
Abilities. The Adventure map's Unique habitat label provides a concrete place
to look. Treasure text describes battle items and an actual possible tradeoff
(stronger attacks but restricted moves), then directs players to the pedestal
description before taking one. It does not promise permanent ownership or imply
all treasures have a drawback. Asking Birch about Adventures repeats these
explanations; the initial optional carryover question does not repeat them.

## Active versus inherited content

- `Rogue_Area_TownSquare` is the active Main Hall despite its inherited map ID.
  `Rogue_Area_SafariZone_Tutorial` is the active indoor escaped-starter map;
  its old connection-aware IDs and explicit recovery warp remain intentional.
- The old private portal/lab/Safari generator recipes remain provenance only:
  those launchers delegate to `generate_hub_tiles.py` after the shared-tileset
  migration. The new `check_onboarding.py` reads current authored maps and is
  also called by that shared generator, rather than adding checks to dead recipes.
- `Rogue_Intro` retains the existing non-Rogue truck fallback. Inherited outdoor
  story, advanced-mode explanations, replay dialogue and postgame tutorials were
  not rewritten. Their existing console entry restrictions remain intact.
- Main Hall and lab/Safari stale-save recovery code was not changed. Completed
  saves remain outside the intro guards. No map geometry or object placement
  changed in this pass.

## Validation record

- PASS: nine `Pokemon pool:` headless tests, including confirmation default,
  draft preservation, cancellation without persistence, changed selection,
  commit and duplicate-commit rejection, all supported initial pool/gimmick
  combinations, and existing roster-name/summary widths.
- PASS: focused text-width rerun after adding the confirmation wording and
  button-label checks against the actual GBA small narrow font.
- PASS: `Adventure setup: reveal choices as their unlocks become available`,
  including both Champion and Pokabbie unlocks.
- PASS: `python3 scripts/check_onboarding.py` checks both reachable nurse
  approaches, portal departure, Birch's junction/door paths, three lab
  approaches, assistant movement and builder movement against collision,
  elevation and relevant static blockers in current authored maps.
- PASS: shared-hub asset/corridor validation through the portal-room launcher
  (29 connections, 56 hallway state combinations, Safari containment).
- PASS: normal Poryscript generation and incremental debug playable ROM build,
  producing `pokeemerald.gba` (32 MiB); `git diff --check`. Poryscript emits its
  existing missing-`numLines` warning and uses the two-line default. Logs are
  under `build/onboarding_*.log`.

### Emulator acceptance still required

Headless tests and source/map checks do **not** establish visual choreography.
No interactive disposable-save walkthrough was performed in this session;
native emulator UI control is unavailable through the enabled tools.

All ten requested walkthroughs remain untested interactively: (1) fresh defaults
with follower, (2) alternate pool/starter without follower, (3) Go back/change/
Continue, (4) locked editing and both unlock paths, (5) both nurse approaches,
early/manual/repeat board use, (6) cancelled setup/starter then departure,
(7) early defeat/zero rewards, (8) rewarded return/builder, (9) exit workbench
without purchase and return, (10) save/reload and partial/completed saves.

Pay particular attention to follower disappearance/reappearance, menu focus and
long-name wrapping, sound restoration, and gift-full retry paths. The existing
starter reunion recovery warp is deliberately preserved; the normal new board
walk contains no teleport.
