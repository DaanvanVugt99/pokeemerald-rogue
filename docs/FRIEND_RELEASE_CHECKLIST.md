# Friend Release Checklist

This checklist defines a small, private preview of Pokemon Emerald Rogue:
Divergence for trusted friends. It is intentionally stricter about crashes,
progression, and save safety than it is about balance or content completeness.

## Ship / No-Ship Gate

Do not send the build until every item in this section is checked.

- [ ] Freeze the candidate commit: no uncommitted source or data changes.
- [ ] Decide whether this is an untagged friend preview or a versioned release.
- [ ] Give the build a visible preview name or build identifier.
- [ ] Confirm the legacy multiplayer feature is disabled cleanly and its hub
      dialogue does not promise functionality that is unavailable.
- [ ] Run `git diff --check`.
- [ ] Run `./scripts/launch_build_test.sh --check-all-suites` successfully.
- [ ] Build the release ROM from the same candidate commit with
      `./scripts/launch_build_release.sh`.
- [ ] Boot that exact release ROM in mGBA.
- [ ] Start a fresh save and reach the first Adventure without a crash,
      softlock, or broken onboarding state.
- [ ] Complete at least one representative Adventure, including saving and
      reloading once.
- [ ] Verify both a won run and a lost or abandoned run return safely to the
      hub.
- [ ] Confirm there are no known data-loss, hardlock, repeatable crash, or
      main-progression bugs.
- [ ] Write a short known-issues list for non-blocking problems.
- [ ] Package the build with setup notes, credits, and a SHA-256 checksum.
- [ ] Tell testers where and how to report feedback.

## 1. Freeze the Scope

- [ ] Write one sentence describing the preview's purpose.
      Suggested wording: "Validate the new Divergence onboarding, run variety,
      Unique Pokemon systems, and core progression with real players."
- [ ] Mark unfinished features as either disabled, clearly labelled, or out of
      scope. Do not leave half-accessible entry points in the player path.
- [ ] Treat multiplayer replacement work as out of scope unless it is promoted
      deliberately into this preview.
- [ ] Stop adding balance and content changes once release validation begins.
- [ ] Put every meaningful player-facing change in `CHANGELOG_ROGUE.md`.
- [ ] Decide and document the save policy. For a pre-release friend build,
      requiring a fresh save is acceptable; supporting development saves should
      be an explicit promise, not an assumption.
- [ ] Decide the candidate label:
  - [ ] Private preview: use a human-readable build label and exact commit,
        without creating a release tag.
  - [ ] Versioned release: choose the next `DIV-vX.Y.Z` version according to
        `VERSIONING.md`.
- [ ] Move desirable but non-blocking work to a post-preview list.

### Release blockers

Treat these as mandatory fixes:

- Repeatable crash, hang, softlock, or save corruption.
- A new player cannot start or finish a normal Adventure.
- A win, loss, reset, or resume leaves progression in an invalid state.
- Required items, encounters, battles, or hub interactions become inaccessible.
- Battle state is materially wrong in a way a player cannot understand or work
  around.
- The distributed artifact cannot be reproduced from the recorded commit.

Balance outliers, awkward text, minor visual defects, and rare non-progression
mechanic errors can be documented for a friend preview if they are honest,
bounded, and do not threaten a save.

## 2. Automated Candidate Validation

Record the candidate before testing:

```sh
git status --short --branch
git describe --tags --always --dirty
git rev-parse HEAD
```

- [ ] Working tree is clean.
- [ ] Local `main` contains everything intended for the preview.
- [ ] The candidate commit is backed up on the intended remote.
- [ ] Formatting and whitespace check passes:

  ```sh
  git diff --check
  ```

- [ ] Full split-suite validation passes:

  ```sh
  ./scripts/launch_build_test.sh --check-all-suites
  ```

- [ ] Release build succeeds:

  ```sh
  ./scripts/launch_build_release.sh
  ```

- [ ] Review the build output for linker errors, memory overflow, and an
      unexpected ROM size.
- [ ] Re-run `git status --short` after building and confirm the build did not
      introduce unreviewed source changes.
- [ ] Record the exact commit and ROM checksum:

  ```sh
  git rev-parse HEAD
  shasum -a 256 pokeemerald.gba
  ```

The legacy monolithic test ELF is not a release gate; use the split-suite
launcher because the all-in-one test ROM is near the 32 MiB layout limit.

## 3. Core Manual Smoke Test

Perform this on the release ROM, not a debug or stale local ROM.

### Boot and onboarding

- [ ] Cold boot reaches the title screen with correct Divergence identity.
- [ ] A fresh save can be created.
- [ ] The favorite-game Pokédex selection is understandable and produces the
      expected first-partner pool.
- [ ] The optional native battle-gimmick gift matches the selected game group.
- [ ] Rogue Modern, Rogue Classic, regional, and National choices are named and
      grouped clearly.
- [ ] The player reaches the hub and can start an Adventure.

### Complete run loop

- [ ] Start an Adventure with ordinary settings.
- [ ] Travel through several route and encounter node types.
- [ ] Catch, level, evolve, teach moves to, and replace party members.
- [ ] Use shops, healing, bag items, and storage.
- [ ] Save and reload in the hub.
- [ ] Save and reload during an Adventure.
- [ ] Finish a won Adventure and return to a valid hub state.
- [ ] Finish a lost or abandoned Adventure and return to a valid hub state.
- [ ] Start a second Adventure and confirm persistent unlocks and resources are
      sensible.

### Battle readability and stability

- [ ] Test singles and doubles.
- [ ] Test at least one Mega Evolution, Z-Move, Dynamax, and Terastallization
      when available.
- [ ] Check Tera selectors, type indicators, and hidden opposing Tera types.
- [ ] Trigger multiple chained Ability popups and confirm they remain readable.
- [ ] Encounter a generated Unique Pokemon and exercise its Ability and exotic
      move set.
- [ ] Exercise at least one Anomalous Ability for several turns and through a
      switch.
- [ ] Exercise Illusion and confirm its typing and UI stay disguised until
      reveal.
- [ ] Confirm accelerated battle settings do not cause healthbox, fade, or
      end-of-battle failures.

## 4. Focused Regression Tour

These checks target prominent changes since `DIV-v0.1.0`. They do not require
exhaustively testing every species or Ability.

- [ ] Pokédex stat increases show the `+` marker in the intended position.
- [ ] A representative buffed species has the documented stats, typing, and
      Abilities.
- [ ] A representative restored or new move can be learned and behaves as
      described.
- [ ] Revival Blessing succeeds only once per team per battle.
- [ ] Reusable Tera Shards remain owned, disappear from route drops, and show
      as sold out in shops.
- [ ] Regional Style Trials temporarily restrict battle-gimmick activators and
      restore the bag correctly afterward.
- [ ] Legendary Clause applies at Adventure entry without blocking later
      Legendary or Mythical recruits.
- [ ] Species Clause permits branched evolutions while blocking direct
      evolution lines.
- [ ] A defeated Legendary-den Pokemon appears in the hub Safari as intended.
- [ ] Unique Safari offers render fully and preserve native Ability choices.
- [ ] Ride Training, whistle replacement, mount cycling, and refreshing Flight
      uses work through a save/reload.
- [ ] Frontier Brains do not replace the starting path column.
- [ ] Trial encounter filters keep forbidden Pokemon out of Honey Trees.
- [ ] The PokéConnect multiplayer interaction gives the work-in-progress
      message and exits safely.
- [ ] Extended Storage still works with the supported Rogue Assistant setup, or
      is listed clearly as unverified for this preview.

## 5. Friend-Test Package

Include:

- [ ] A versioned or preview-labelled ROM artifact.
- [ ] Exact Git commit and SHA-256 checksum.
- [ ] Emulator and setup recommendations.
- [ ] A clear fresh-save or save-compatibility statement.
- [ ] A short summary of what is new and what feedback is most useful.
- [ ] Known issues and explicitly disabled features.
- [ ] Upstream project credits.
- [ ] A single feedback destination.

Ask every tester to include:

- Build label or checksum.
- Emulator, emulator version, and platform.
- Whether the save was fresh or carried forward.
- Steps immediately before the problem.
- Expected and actual result.
- Screenshot or short recording when useful.
- Save file when they are comfortable sharing it.

Suggested triage:

- **Blocker:** crash, data loss, hardlock, or core progression failure.
- **High:** repeatable major mechanic failure with no reasonable workaround.
- **Medium:** mechanic, UI, or balance problem with a workaround.
- **Low:** polish, wording, cosmetic issue, or general suggestion.

## 6. Preview Exit Criteria

The friend milestone is successful when:

- [ ] At least two people other than the developer boot the packaged build.
- [ ] At least two complete Adventures are reported across the group.
- [ ] At least one tester exercises a gimmick-heavy or Unique-heavy run.
- [ ] No open Blocker issue remains.
- [ ] Every High issue is fixed or explicitly accepted for the next preview.
- [ ] Feedback has been converted into a small, prioritized follow-up list.

After fixes, repeat validation in proportion to risk. If the candidate becomes
a versioned release, date the changelog section, build from the final clean
commit, create the annotated `DIV-vX.Y.Z` tag, and ensure the tag points to the
exact commit used for the distributed ROM.

## Current Snapshot (2026-07-29)

- Baseline tag: `DIV-v0.1.0`.
- Current description at checklist creation:
  `DIV-v0.1.0-392-g7e6e9819a3-dirty`.
- `main` is three commits ahead of `origin/main`.
- The working tree contains in-progress changes that disable legacy
  multiplayer while its replacement is developed.
- `devkitARM`, `mgba-rom-test`, and `mgba` are available in `PATH`.
- `git diff --check` passes.
- The full split-suite validation and a fresh release build were not run while
  drafting this checklist and remain release gates.
- Given the substantial user-facing work since `DIV-v0.1.0`, `DIV-v0.2.0` is a
  plausible next version if this is promoted from a private preview to a tagged
  release. The version is not chosen by this checklist.
