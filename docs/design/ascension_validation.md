# Ascension implementation validation

The approved contract is [ascension.md](ascension.md). This file records checks
and remaining acceptance work; passing compilation does not imply balance approval.

## Automated checks

Validated on 2026-09-11. Final per-suite results include the corrected Unique
Ability N-Z batch rerun:

| Suite | Passed | Existing known failures | TODO | Unexpected failures |
| --- | ---: | ---: | ---: | ---: |
| Core | 23 | 3 | 0 | 0 |
| AI | 93 | 0 | 0 | 0 |
| Abilities | 517 | 4 | 59 | 0 |
| Unique Abilities A-M | 1,027 | 0 | 0 | 0 |
| Unique Abilities N-Z | 925 | 0 | 0 | 0 |
| Moves | 717 | 6 | 29 | 0 |
| Items | 404 | 2 | 10 | 0 |
| Forms | 145 | 1 | 0 | 0 |
| Rogue | 453 | 0 | 0 | 0 |
| **Total** | **4,304** | **16** | **98** | **0** |

- Ran `./scripts/launch_build_test.sh --check-all-suites`. It completed every
  suite with one unexpected failure in the existing Canopy Stalker fixture.
  After giving its targets explicit survival stats, reran the entire affected
  batch with `./scripts/launch_build_test.sh --check --suite ability_unique_n_z`:
  all 925 tests passed, including the original failure. No remaining unexpected
  failures are hidden by the totals above.
- All 35 `Ascension:` tests passed in the 453-test Rogue suite. Coverage includes
  generated opponents across stages 0, 7, 8, 12 and 13, independent profile
  properties, A19 filtering, stable aces, Gauntlet gates, service exclusions,
  save round trips, separate ladders, reward retry, payouts, Trial final
  conditions and GBA rule-text widths.
- `make -j4 RELEASE=0 TEST=0 pokeemerald.gba` passed. Link usage: EWRAM
  261,522 / 262,144 bytes (622 bytes free), IWRAM 25,620 / 32,768 bytes, ROM
  31,764,572 / 33,554,432 bytes. The output ROM is padded to 32 MiB. EWRAM
  headroom is tight and should be checked when adding persistent state.
- `make check-species-report` and the offline profile pipeline `verify` passed.
  The tracked profile header remains deterministic and unchanged.
- Quest C data, constants and generated Poryscript were regenerated into temporary
  files and byte-compared with all four build artifacts: passed.
- The full Unique Ability test ROM exceeded 32 MiB. The launcher now splits it
  into A-M / remaining filename batches; GNU Make's selected-source lists cover
  all 599 files exactly once. Mock launcher checks verified the full and public
  Unique Ability flows, focused filters, and failure propagation. The summary
  parser also accepts spacing before the failed-test count's colon.
- Existing AI defensive-switch, signature critical-item and Chaos Charm fixtures
  now isolate their intended behavior from species defaults or uncontrolled
  called moves. All three focused checks and their full suites passed. The
  Canopy Stalker fixture now keeps both targets alive to test forced switching.
  No AI, item, Ability or species runtime behavior changed to satisfy fixtures.
- The island layout matrix now has a 180-second budget for its 12 full layouts
  and passes both focused and full-suite checks. Its former timeout left
  temporary test settings active for later tests. The Dewgong Defense expectation
  now matches the approved manifest value of 80. No island generation or species
  stats changed.
- `git diff --check`: passed.

## Setup disclosure follow-up (2026-09-12)

- `./scripts/launch_build_test.sh --check --suite rogue --filter "Adventure setup:"`:
  both tests passed. They cover the initial six-row list, individual unlocks,
  shared first-clear visibility across formats, entrance starter-bag availability,
  Trial-fixed choices and full read-only inspection.
- `make -j4 RELEASE=0 TEST=0 pokeemerald.gba`: passed. EWRAM and IWRAM usage
  are unchanged; ROM usage is now 31,765,276 bytes.
- `git diff --check`: passed. The full-suite results above precede this scoped UI
  follow-up; the full launcher was not repeated.
- This follow-up has not been manually inspected in the emulator.

## Postgame introduction follow-up (2026-09-12)

- The same focused `Adventure setup:` filter passed both tests after extending
  coverage to require both a full clear and the entrance introduction, hide
  Ascension in read-only setup and base-rule notes before then, and skip the
  early Trial level prompt. Other format ladders share the reveal.
- Regenerated the authoritative opening and entrance Poryscript sources. Inspected
  the generated branches: the entrance checks champion status and an unset
  introduction flag, displays the explanation, then sets that persistent flag.
  The call precedes setup; cancelling setup does not clear the flag. The generated
  opening script contains no Ascension mention.
- The incremental debug ROM build passed: ROM 31,765,960 bytes; EWRAM/IWRAM
  unchanged. `git diff --check` passed. No full-suite rerun for this scoped change.
- The dialogue flow has not been manually exercised in the emulator.

## Mode availability follow-up (2026-09-12)

- The focused `Adventure setup:` filter passed all three tests. The new case
  verifies Gauntlet's existing postgame unlock and final-quest shortcut, hiding
  the Adventure row while Standard is the only choice, safe fallback from old
  remembered modes, and rejection of locked Gauntlet/disabled Slow Path at start.
- The initial editable setup now has five rows. Slow Path cannot be selected for
  new runs; existing active/replay/record identities remain supported.
- The incremental debug ROM build passed: ROM 31,766,124 bytes; EWRAM/IWRAM
  unchanged. `git diff --check` passed. No full-suite rerun for this scoped change.
- No new manual emulator checks were performed.

## Trainers menu follow-up (2026-09-12)

- Renamed the editable row/submenu to Trainers and restored its postgame gate,
  shared with Gauntlet. Read-only rules still show the configured trainers.
- The focused `Adventure setup:` filter passed all three tests, including the
  fresh four-row list, trainer visibility after meeting Pokabbie, the legacy
  final-quest shortcut, and Trial/read-only behavior.
- The incremental debug ROM build and `git diff --check` passed. ROM usage is
  31,766,124 bytes; EWRAM/IWRAM are unchanged. No full-suite or manual emulator
  rerun for this scoped change.

## Shared Pokemon Pool UI Follow-up

- The intro now stages its pool and initial gimmick in the shared adventure UI.
  Regional/ National/ Special fields filter the existing allowed rosters; Hisui
  has its own region and Legends Z-A belongs to Kalos. Single-choice fields are
  hidden, and the same picker serves hub editing, the professor and Trials.
- All eight `Pokemon pool:` focused tests passed: reachability of every roster,
  curated/restricted cycling, single-choice behavior, National gimmick retention,
  initial selection persistence, discarded previews, Trial legality, and GBA
  text widths for every generated roster name and gimmick summary.
- The incremental debug ROM build passed. ROM usage is 31,769,208 bytes;
  EWRAM remains 261,522 bytes and IWRAM 25,620 bytes.
- Generated intro/common scripts were checked for the new specials and final
  confirmation flow. `git diff --check` passed. No full-suite rerun for this
  scoped menu change; manual emulator layout/input checks remain pending.

## Intro Pool and Base Rules Cleanup

- Intro selection excludes Special pools; later editing keeps them. The two
  AI/weather and gimmick-item notes were removed from Base Rules, and the early
  six-line page has no empty second page or paging hint.
- All eight `Pokemon pool:` checks and all three `Adventure setup:` checks passed.
  The incremental debug ROM build and `git diff --check` passed. ROM usage is
  31,769,308 bytes; EWRAM/IWRAM are unchanged. Manual emulator checks remain pending.

## Pokedex Naming and Hisui Follow-up

- Adventure setup now uses Pokédex consistently for the row, picker title,
  type, confirmation and messages. Intro filtering also excludes Hisui; later
  editing retains Hisui and Special rosters.
- All eight `Pokemon pool:` checks and three `Adventure setup:` checks passed,
  including intro navigation excluding Hisui and the renamed menu row.
- The incremental debug ROM build and `git diff --check` passed. ROM usage is
  31,769,304 bytes; EWRAM/IWRAM are unchanged. Manual checks remain pending.

## Adventure Overview Follow-up

- Begin Adventure is first and initially highlighted at the entrance. The chosen
  Pokédex remains visible before its editing unlock; opening it is blocked with
  first-clear help. The first clear or existing Pokabbie unlock enables editing.
- All three `Adventure setup:` checks passed, covering ordering, visible locked
  Pokédex, its edit guard, and unlocking after a clear. The incremental debug
  ROM build and `git diff --check` passed. ROM usage is 31,769,424 bytes;
  EWRAM/IWRAM are unchanged. Manual emulator checks remain pending.

## Manual acceptance

Pending. Windows Computer Use was stopped by the user with Escape before mGBA
layout inspection. No manual checks below are claimed as passed.

- Intro: Regional/National pool pages (Special and Hisui excluded), game/generation changes, National
  gimmicks, cancellation, Continue, and the professor's initial item handoff.
- Reopen the pool picker from the hub, professor, Trials and active-run rules.
- A0/A5/A10/A17/A20: cumulative rules, new addition, base pages and readable layout.
- Browse locked levels; reject selection; restore remembered level after format change.
- Trial minimum A10 / forced Mixed, fixed teams, valid dex choices and fresh-start rules.
- Cancel setup without saving edits; reject invalid teams and Day Care immediately at start.
- C-Gear and Config Lab in the hub; read-only rules during active runs.
- Replay start/return flow on actual hardware or emulator UI.
- Multiplayer is unsupported and disabled; its dormant client review flow is not
  acceptance coverage for this release.
- Hall of Fame identity, Trial records and one-time reward claim status.

## Compatibility and balance

Save schema 10 requires a new save; no old save files are deleted or migrated.
The serialized round-trip check exercises the bounded writer and reader. The
Hall of Fame metadata and Rogue save block have compile-time capacity assertions.

Trial records update only on the matching quest success, including its final
party/capture conditions; beating the final boss alone is insufficient.

Balance risks remain those in the design document: A7/A8 full boss upgrades,
A16/A17 remaining IV impact, preparation variance before A19, and viable opening
matchups at A20. No extra balance changes are implied by this implementation.


## Review fixes (2026-09-12)

- Incremental debug ROM build passed (`make -j4 RELEASE=0 TEST=0 pokeemerald.gba`).
  Link usage: ROM 31,769,456 bytes; EWRAM 261,522; IWRAM 25,620.
  Both staged and unstaged `git diff --check` passed.

- Focused `--check --suite rogue --filter "Run review:"`: 19/19 passed.
  The Trial replay regression reaches the production reset phase and checks the
  recorded seed, retained Trial/replay identity, disabled quests and unchanged
  Trial best. Additional checks cover remembered Trial selection and nurse
  reminders before introduction, at A0, and at the top of the ladder.
- Regenerated entrance script reserves 22 entries for the ascension list and
  routes Choose Trial back to selection. The obsolete edit submenu is removed.
- Trial identity/full-rules presentation is an explicit UX TODO in the design
  document. Multiplayer remains unsupported and disabled; no client UI validation
  is claimed.
- Manual follow-up: navigate A20 Trial selection, change a remembered Trial,
  cancel without starting, and inspect the Choose Trial row on the GBA screen.
