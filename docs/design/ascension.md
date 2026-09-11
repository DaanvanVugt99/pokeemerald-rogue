# Ascension Difficulty Design

Status: accepted first design direction, saved 2026-09-11. Not implemented.
The progression and cumulative rules presentation are the working design;
numerical balance, encounter exceptions, and the A19 starting kit need testing.
This document does not describe current shipped difficulty behavior.

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

- Bosses: mandatory gyms, rivals, team bosses, Elite Four, and champions.
- Boss ace: the designated final party Pokemon, independent of any later
  battle-time switching or party ordering.
- There is no ordinary-trainer ace tier. The three distribution labels are
  `Aces only` (boss aces), `Boss Pokemon` (all boss Pokemon), and
  `Trainer Pokemon` (all trainer Pokemon, including bosses).
- All trainers: ordinary trainers and bosses, including optional adventure
  trainers. Standalone facilities and Trials retain their own rules pending
  deliberate integration.
- Optional Frontier Brain encounters need an explicit balance pass; do not
  automatically apply mandatory-boss party expansion to them.

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
| A2 | Focused Aces | Boss aces have natures suited to their roles. |
| A3 | Competitive Aces | Boss aces use competitive movesets. |
| A4 | Equipped Aces | Boss aces carry suitable held items. |
| A5 | Further Training | Boss Pokemon gain another +5 IVs: cumulative bonus +10 over A0. |
| A6 | Specialized Bosses | All boss Pokemon have natures suited to their roles. |
| A7 | Competitive Bosses | All boss Pokemon use competitive movesets. |
| A8 | Equipped Bosses | All boss Pokemon carry suitable held items. |
| A9 | Trained Opposition | Ordinary trainer Pokemon have 10 IVs in every stat. |
| A10 | Reinforcements | Every mandatory boss gains one Pokemon over its A0 party size, capped at six. |
| A11 | Specialized Opposition | All trainer Pokemon have natures suited to their roles. |
| A12 | Competitive Opposition | All trainer Pokemon use competitive movesets. |
| A13 | Advanced Training | Boss Pokemon gain another +10 IVs: cumulative bonus +20 over A0, capped at 31. |
| A14 | Equipped Opposition | All trainer Pokemon carry suitable held items. |
| A15 | Broader Specialties | Enable diverse trainers: broader, themed type specialties. |
| A16 | Elite Opposition | All trainer Pokemon have at least 20 IVs in every stat; preserve higher boss values. |
| A17 | Perfect Aces | Boss aces have 31 IVs in every stat. |
| A18 | Perfect Opposition | All trainer Pokemon have 31 IVs in every stat. Enemy EVs remain disabled. |
| A19 | Limited Supplies | Replace imported bag supplies and starting money with a fixed expedition kit and fund. Preserve prepared Pokemon, their EVs, and equipped held items. |
| A20 | Full Opposition | Every mandatory boss fields six Pokemon. |

All IV bonuses apply to each stat and cap at 31. Higher-level overrides must
never lower a value established by an earlier rule.

| Upgrade | Aces only | Boss Pokemon | Trainer Pokemon |
| --- | --- | --- | --- |
| Appropriate natures | A2 | A6 | A11 |
| Competitive movesets | A3 | A7 | A12 |
| Suitable held items | A4 | A8 | A14 |

Nature, moveset, and held-item upgrades are independent. Applying a competitive
profile must not silently grant its optimized nature or held item before the
corresponding ascension. Item and nature selection must suit the actual
moveset, including basic movesets. Respect item restrictions and give required
gimmick items precedence. Audit other profile effects, such as Ability and
Tera-type selection, before implementation so no undocumented upgrade is bundled
into the moveset step.

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

Before implementation, define the kit contents and fund, retained key items,
held-item handling, and treatment of charms/curses and special run-start items.
The kit must support catching, recovery, and an initial purchase. Preserve
prepared Pokemon and their EVs; do not accidentally import the existing
daycare restriction. Normal in-run earnings and shop prices remain active.

## Balance Review and Open Work

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

Implementation planning must separately resolve unlock/selection flow, legacy
difficulty rewards and quests, Trials, replays, multiplayer, and save migration.
Do not invalidate released saves or equate the existing four-value trainer
difficulty setting with the ascension number. A future implementation should
derive runtime behavior and the cumulative UI from the same effective rules.

## Relevant Existing Code

- [Difficulty presets](../../src/rogue_settings.c): current Standard rules.
- [Trainer generation](../../src/rogue_trainers.c): party sizes, IVs, competitive
  sets, item/nature gates, and existing AI behavior.
- [Run controller](../../src/rogue_controller.c): bag setup and run resources.
- [Settings UI](../../src/rogue_settings_ui.c): existing rule descriptions.
- [Run-start context](../../src/rogue_run_start.c): effective configuration.
- [Save serialization](../../src/rogue_save.c): compatibility-sensitive storage.
