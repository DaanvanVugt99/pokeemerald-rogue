# Ascension Difficulty Design

Status: implementation in progress, 2026-09-11. The decisions below are the
approved replacement contract. Balance remains subject to playtesting; validation
results are recorded in `docs/design/ascension_validation.md`.

## Purpose

Replace freely configured difficulty as the main adventure progression with
Ascension 0 and 20 cumulative difficulty levels, inspired by Slay the Spire.
Each level adds a clear rule; earlier rules remain active.

Current Standard is already challenging, particularly around gyms 7-8.
A0 should remain challenging for a learner but be very consistently winnable
by a good player without extensive competitive knowledge or exceptional luck.
A20 should demand excellent play, preparation, and resource management while
preserving a broad range of viable Pokemon.

The central presentation is three kinds of changes:

- Tiered Pokemon upgrades: boss aces, then all boss Pokemon, then all trainer
  Pokemon receive appropriate natures, competitive movesets, and held items.
- IV progression: clearly distinguish bonuses, minimums, and fixed values.
- Milestones: larger boss parties, diverse trainers, and restricted supplies.

Do not return to player-facing rules such as "one additional competitive
slot." The affected group should be immediately understandable. A meaningful
rule must affect a substantial part of the run; do not spend an ascension
merely enabling something two gyms earlier. Major jumps belong at deliberate
milestones rather than being treated as equivalent to small stat increases.

## Scope and Fixed Rules

- Bosses: gyms, rivals, team bosses, Elite Four, champions, final-node bosses,
  and optional Frontier Brains. All receive the same quality and party-size rules.
- Boss ace: the designated final party Pokemon, independent of any later
  battle-time switching or party ordering.
- There is no ordinary-trainer ace tier. The three distribution labels are
  `Aces only` (boss aces), `Boss Pokemon` (all boss Pokemon), and
  `Trainer Pokemon` (all trainer Pokemon, including bosses).
- All trainers: ordinary trainers and bosses, including optional adventure
  trainers. Trials use ascension plus explicit Trial overrides. Service-generated
  parties, including battle-simulator loans and EXP services, are excluded.

At every ascension, retain:

- Permadeath, Set mode, level caps, and the balls-only battle bag clause.
- Standard's species, held-item, and legendary entry clauses.
- Player EVs; no enemy EVs. Convenient hub EV configuration is a future
  preparation improvement, not an ascension penalty or a change implemented
  by this document.
- Standard's AI behavior and progression. Ascensions do not make the AI
  progressively smarter or make the baseline AI less capable.
- Standard's species/evolution eligibility, legendary allowances, weather,
  and battle-gimmick progression, except for the explicit A15 diversity rule.
- Normal shop prices, rewards, catching, encounter variety, and EXP conveniences.

Hub resources remain available until A19. Do not combine a late supply
restriction with accumulated shop-price penalties. Prepared Pokemon remain
useful throughout the ladder.

## A0: Base Adventure

| System | Baseline |
| --- | --- |
| Boss party sizes | Standard's current encounter-specific progression |
| Ordinary trainer parties | Standard's current progression |
| Enemy movesets | Viable basic movesets; no curated competitive sets |
| Enemy ordinary held items | None |
| Enemy natures | Neutral |
| Boss IVs | Standard's stage progression: 0 during gyms, 5 during Elite Four, 10/15 during championship stages |
| Ordinary trainer IVs | 0 |
| Resources | Normal hub preparation, starting resources, prices, and rewards |
| Weather and gimmicks | Standard progression; required gimmick items are an explicit exception to the ordinary-item restriction |

For reference, Standard gym/league party sizes are three at gyms 1-2, four at
gyms 3-6, five at gyms 7-8 and Elite Four, and six during championships.
Rivals and team bosses have encounter-specific sizes; preserve those as the
A0 baseline rather than assuming every boss follows the gym table.

Removing late-game competitive sets and ordinary items is a substantial
baseline reduction. Validate A0 before treating its difficulty as settled.
Basic movesets must give species sensible offensive and support tools; they
must not produce dysfunctional Pokemon just to create room for ascensions.

## Cumulative Additions: A1-A20

| Level | Name | New rule |
| --- | --- | --- |
| A1 | Better Trained | All boss Pokemon gain +5 IVs in every stat over A0. |
| A2 | Focused Aces | Boss aces have natures from their selected competitive profiles. |
| A3 | Competitive Aces | Boss aces use competitive movesets. |
| A4 | Equipped Aces | Boss aces carry suitable held items. |
| A5 | Further Training | Boss Pokemon gain another +5 IVs: cumulative bonus +10 over A0. |
| A6 | Specialized Bosses | All boss Pokemon have natures from their selected competitive profiles. |
| A7 | Competitive Bosses | All boss Pokemon use competitive movesets. |
| A8 | Equipped Bosses | All boss Pokemon carry suitable held items. |
| A9 | Trained Opposition | Ordinary trainer Pokemon have 10 IVs in every stat. |
| A10 | Reinforcements | Every boss gains one Pokemon over its A0 party size, capped at six. |
| A11 | Specialized Opposition | All trainer Pokemon have natures from their selected competitive profiles. |
| A12 | Competitive Opposition | All trainer Pokemon use competitive movesets. |
| A13 | Advanced Training | Boss Pokemon gain another +10 IVs: cumulative bonus +20 over A0, capped at 31. |
| A14 | Equipped Opposition | All trainer Pokemon carry suitable held items. |
| A15 | Broader Specialties | Enable diverse trainers: broader, themed type specialties. |
| A16 | Elite Opposition | All trainer Pokemon have at least 20 IVs in every stat; preserve higher boss values. |
| A17 | Perfect Aces | Boss aces have 31 IVs in every stat. |
| A18 | Perfect Opposition | All trainer Pokemon have 31 IVs in every stat. Enemy EVs remain disabled. |
| A19 | Limited Supplies | Replace imported bag supplies and starting money with a fixed expedition kit and fund. Preserve prepared Pokemon, their EVs, and equipped held items. |
| A20 | Full Opposition | Every boss fields six Pokemon. |

All IV bonuses apply to each stat and cap at 31. Higher-level overrides must
never lower a value established by an earlier rule.

| Upgrade | Aces only | Boss Pokemon | Trainer Pokemon |
| --- | --- | --- | --- |
| Appropriate natures | A2 | A6 | A11 |
| Competitive movesets | A3 | A7 | A12 |
| Suitable held items | A4 | A8 | A14 |

Each Pokemon selects an existing competitive profile. Its nature, moves with
matching regular Ability, and item are applied independently at their distribution
thresholds. Nature-only upgrades use that exact profile nature even when basic
moves remain active; there is no separate nature-selection heuristic. Species-default
unique Abilities remain unchanged. Preserve move-level legality, missing-move filling,
item restrictions, and required gimmick-item precedence. A missing profile leaves a
legal basic Pokemon and emits a development diagnostic.

Determine the ace after baseline ordering and retain that identity through upgrades.
A17 affects boss aces only. Apply final IV rules after rival-specific adjustments.
Calculate expansion from the A0 encounter size, capped by engine party capacity.
Gauntlet retains level 100 and full parties, with the same ascension distributions.

## Ascension Rules Window

Show the effective cumulative rules for the selected level, grouped by knob,
alongside a clear indication of what the newly selected level adds. Do not
require players to mentally add twenty historical descriptions.

Use consistent labels: `Aces only`, `Boss Pokemon`, `Trainer Pokemon`.
Explain that `Aces only` always refers to boss aces; ordinary trainers receive
these upgrades only when the distribution reaches `Trainer Pokemon`.
Use a separate baseline/rules view for fixed rules. Unchanged baseline rows
can be omitted from a compact summary, provided that baseline remains accessible.
Names, exact wording, pagination, and GBA layout are provisional.

Conceptual summaries:

```text
ASCENSION 1
Boss IVs:       Base +5
```

```text
ASCENSION 5
Boss IVs:       Base +10
Natures:        Aces only
Competitive:    Aces only
Held items:     Aces only
```

```text
ASCENSION 10
Boss IVs:       Base +10
Other IVs:      10
Natures:        Boss Pokemon
Competitive:    Boss Pokemon
Held items:     Boss Pokemon
Boss parties:   Base +1 (max 6)
```

```text
ASCENSION 15
Boss IVs:       Base +20 (max 31)
Other IVs:      10
Natures:        Trainer Pokemon
Competitive:    Trainer Pokemon
Held items:     Trainer Pokemon
Boss parties:   Base +1 (max 6)
Specialties:    Diverse
```

```text
ASCENSION 17
Boss IVs:       Base +20 (max 31)
Other IVs:      20
Boss ace IVs:   31
Natures:        Trainer Pokemon
Competitive:    Trainer Pokemon
Held items:     Trainer Pokemon
Boss parties:   Base +1 (max 6)
Specialties:    Diverse
```

```text
ASCENSION 20
All trainer IVs: 31
Natures:         Trainer Pokemon
Competitive:     Trainer Pokemon
Held items:      Trainer Pokemon
Boss parties:    6 Pokemon
Specialties:     Diverse
Starting supply: Expedition kit + fund
```

`Base` means the A0 value for that encounter. Explain this in the detail view.
Do not shorten A10 to "All trainers +10": ordinary trainers have a fixed 10,
while late bosses retain their stage baseline plus 10. At A18+, replace the
obsolete IV bonus/floor/ace rows with the single effective value of 31.

## A19 Supplies: Deliberate New Rule

The intended milestone limits imported supplies and money, not prepared Pokemon.
The existing bag-wipe toggle is not a drop-in implementation: current run setup
also clears daycare access, omits the adventure starting fund, filters carried
items, and gives five Poke Balls and one Potion.

A19 replaces imported supplies and the adventure fund with exactly 15 Poke Balls,
10 Potions, 3 Full Heals, and 10,000 money. Retain key items, prepared Pokemon,
their EVs, equipped items, and Day Care access. Filter imported bag charms/curses;
items and charms obtained during the run remain available. Determine imported-charm
eligibility after the supply policy. Normal prices and in-run rewards remain active.
Trial fresh-start rules explicitly override this kit and keep their own restrictions.

## Setup, Unlocks and Records

The entrance opens Set Up Adventure: Ascension, mode, format, Pokédex,
opponent regions/order, encounter style, starting team, rules, and Begin Adventure.
C-Gear and Config Lab open the same editor in the hub and a read-only rules view
during a run. Only the entrance starts adventures. Keep presentation settings in
Options. Remember the last accepted ascension separately for Singles, Doubles and
Mixed. Trial setup uses the same editor and enforces its fixed choices. Locked levels can be browsed but not selected. Revalidate setup, unlocks,
party, Day Care and Trial legality immediately before departure.

Only A0 starts unlocked. A qualifying Standard clear of An unlocks A(n+1), capped
at A20, on that format's ladder. Fresh solo non-Trial non-replay runs qualify if
started without imported active charms or gameplay-altering debug overrides.
Snapshot eligibility at run start, show exclusions, and award advancement once
before hub restoration. Charms earned within the run are allowed; victory laps
do not advance again. Region/order/dex/encounter choices do not create ladders.
Alternate modes, Trials and replays retain records without advancement.
Multiplayer is unsupported and disabled in current builds.

Trials default to minimum A0. Insane Mode requires minimum A10 and forces Mixed.
The effective format's unlocked maximum applies; definitions can also fix a level.
Record best Trial clears independently per format and permit repeat attempts.
A Trial record requires its matching quest success, including final team or
capture conditions; beating the final boss alone is insufficient.
Keep normal first-clear rewards, remove Hard-only shiny bonuses, and move Orre's
custom non-shiny Lugia into the normal reward package. One claimed flag replaces
reward tiers. Failed inventory delivery remains refundable and retryable.

The post-run room/victory-lap calculation uses a rate of `250 + 5 * ascension`.
Record mode, format, ascension and special-run identity, including Hall of Fame.
Slow Path and Gauntlet have independent mode/format best-clear records.

## Persistence Contract

Remembered hub setup and immutable active configuration are separate. Format
progress, selected levels, mode/Trial records and reward claims are saved.
Save format 10 deliberately requires a new save. No legacy migration is required;
never delete a local save file. Verify in-memory and serialized storage bounds.

Replay snapshots store the seed, complete configuration and ascension rules version.
Snapshots also preserve Trial and special-campaign identity. Incompatible versions
are rejected; replay setup is read-only. Returning to the hub restores remembered
setup instead of overwriting it with special-run settings.

Multiplayer is unsupported and disabled by `ROGUE_FEATURE_MULTIPLAYER`. Retained
network code is dormant compatibility scaffolding, not a supported adventure flow.
Before any future re-enable, design and validate client rule refresh/confirmation
and host authority; the current setup screen does not refresh a waiting client.

Hub EV editing, new rewards, map redesign and further balance changes are deferred.

## Balance Review and Open Work

- TODO (UX design): decide how setup and in-run Adventure Rules show the selected
  Trial's identity and complete effective restrictions, including fixed teams,
  battle rules, and success conditions. The current view only shows ascension/base
  rules and the Trial supply override. Reuse `RogueTrial_GetRuleText` once the
  presentation is agreed; do not imply that the current view is exhaustive.
- Trial setup has a Choose Trial action that returns to the full list even when
  the attendant has loaded a remembered selection. Back still cancels setup.

- A0: check whether removing competitive sets and ordinary held items makes
  the late game too forgiving. Preserve coherent basic sets and existing AI.
- A7-A8: full competitive boss teams followed by full equipment are larger
  changes than a small IV increment; assess their combination explicitly.
- Held items: a berry and a powerful offensive item have very different impact.
  Inspect actual item distributions before assigning confidence to A4/A8/A14.
  A restricted early item pool is an option only if testing justifies it.
- A9/A12/A14: route improvements accumulate across many encounters. Track
  resource expenditure and permanent losses, not just boss win rates.
- A16-A18: verify that successive IV steps feel distinct and worthwhile.
  A17 perfects only boss aces; it does not invent an ordinary-trainer ace role.
  Some late boss aces already reach 31 at A13, so assess A17's remaining
  early/mid-run impact explicitly.
- A19-A20: six fully upgraded opening-boss Pokemon after supply restriction
  must allow multiple plausible answers. Do not accept unavoidable matchup
  losses merely because this is A20.
- Evaluate ordinary hub preparation and heavily stocked preparation separately
  before A19. Those levels cannot have one resource-independent difficulty.
- Compare adjacent levels using representative encounters and varied teams;
  then examine whole runs for loss stage, resource use, rebuilding options,
  strategy diversity, and excessive battle length. Equal win-rate drops are
  not required; accidental cliffs are undesirable.

The abandoned slot-count progression, shop-price ladder, escalating AI,
enemy EVs, and double-championship capstone are not part of this saved design.

## Relevant Existing Code

- [Adventure configuration](../../src/rogue_settings.c): fixed battle rules and remembered setup.
- [Ascension resolver](../../src/rogue_ascension.c): cumulative rules, display text and progression.
- [Trainer generation](../../src/rogue_trainers.c): party sizes, IVs, competitive
  sets, item/nature gates, and existing AI behavior.
- [Run controller](../../src/rogue_controller.c): bag setup and run resources.
- [Settings UI](../../src/rogue_settings_ui.c): existing rule descriptions.
- [Run-start context](../../src/rogue_run_start.c): effective configuration.
- [Save serialization](../../src/rogue_save.c): compatibility-sensitive storage.

## Progressive Setup Disclosure

Editable setup shows only choices that are available. Ascension appears after
the first adventure clear and the entrance introduction (across all formats).
Pokédex is always visible, with editing unlocked by the first clear (or the
existing Pokabbie unlock); the one-time intro selection is always editable.
Starting team appears when both Current party and Starter bag are available.
Trial-fixed choices are omitted, including fixed rows within Trainers, while
the selected Pokédex remains visible. The read-only Adventure Rules view retains all other settings for inspection. Base
rules remain accessible without exposing Ascension.

Ascension is a postgame reveal. The opening text does not mention it. After a
full clear, the entrance attendant explains it immediately before the next setup
visit and records that introduction in a persistent flag, even if setup is then
cancelled. Until then, all setup views hide the selector, ladder help, advancement
status and Ascension-specific base-rule notes. Trials use A0 without a level
prompt until the introduction. C-Gear and Config Lab respect the same reveal.

## Available Adventure Modes

Standard is the only starting mode. Gauntlet retains its previous postgame gate:
meeting Pokabbie (or the final-quest reward shortcut). The Adventure row is hidden
until Gauntlet is available. Slow Path is disabled for new adventures for now;
its existing run, replay and record data remain readable. New setup falls back to
Standard when a remembered mode is no longer available, and run-start validation
rejects newly submitted locked or disabled modes.

The Trainers entry (formerly Opponents) uses the same postgame settings unlock as
Gauntlet. Both its list label and submenu title use Trainers. Trial-forced regions
and order remain effective before that unlock. Read-only Adventure Rules can
still display them. After the intro, a fresh save's editable list contains Battle format,
Pokédex, Encounters and Base rules. At the entrance, Begin Adventure is first
and initially highlighted; Save and close remains last in the hub editor. The
locked Pokédex row shows the chosen variant and explains the first-clear unlock.

## Shared Pokédex Picker

The intro uses Set Up Adventure for Battle format, Pokédex, Encounters,
Base rules, and Continue. Continue commits these choices together, including the
initial pool and starting gimmick. Intro setup never exposes Ascension, Trainers,
or locked modes. Its pool determines encounters and initial partner choices;
trainer regions retain the existing automatic mapping from that pool.

The same picker opens from later adventure setup, the professor's curated pool
selection, and Trial setup. It preserves the existing allowed pools for each
entry point and filters Trial choices using the effective Trial configuration.
The intro excludes Special pools and Hisui, offering Regional and National
Pokédexes. Later editing retains its existing Special rosters and Hisui. The hierarchy is represented by
dependent fields on one page:

- Regional: Region, then Game. Hisui is its own region; Legends Z-A is in Kalos.
- National: through Generation 1-9.
- Special: named roster (Colosseum + XD, Conquest, or Rogue where available).

Hide any field with only one valid choice. Always show the selected roster's
full name, species count, and a short explanation of its scope. Directional
controls cycle valid choices; A also changes the highlighted field. Use this
pool stages the selection, while B discards the picker edits. The final setup
confirmation commits the staged configuration. Active-run inspection is read-only.

During the intro, regional/special choices automatically use their existing
native gimmick (including None); National choices expose Starting gimmick with
None, Mega Evolution, Z-Moves, Dynamax, and Terastallization. Show the effective
starting gimmick in the picker and intro summary. Preserve the staged National
gimmick while previewing other categories. Later pool editing does not grant or
replace gimmick items. The existing professor handoff grants the initial item.

Base Rules omit the implementation notes about Standard AI/weather/gimmicks and
required gimmick items. Before the Ascension introduction, the remaining six
lines fit on one page; no empty second page or page-navigation hint is shown.
