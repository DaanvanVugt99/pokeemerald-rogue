# Main Hall — Grand Underground Concourse

`Rogue_Area_TownSquare` retains its map/layout identity and becomes the Main Hall.
Its 36 × 30 buffer contains a 29-tile-wide chamber, centered at x18. The room uses
private `MainHall` tilesets; the Adventure Gate, Labs, Safari and wing interiors
are unchanged.

## Architecture and services

The gallery floor is elevation 5. Two three-wide flights, x11–13 and x23–25,
cross elevation-0 treads at y8–9 to ground elevation 3. The frontage is solid:
there is no underpass. The north corridor and both wing doors retain elevation 5.
School is at (9,4), Poké Connect at (27,4). School's locked door keeps its frame
but loses door behavior and becomes impassable.

The monument reuses the original Town Square's gray pedestal and south plaque, with warm corner
markers in its floor surround. All four original statue appearances and their
progression remain intact. Interact north at (18,14) to open the world map.
The east–west crossing occupies y16–18; the teleport arrival is (18,19).

| Service | Position | Notes |
|---|---|---|
| Workbench | (6,11) | Northwest, backed by the gallery wall |
| Builder | (7,12) | Reserved standing space after the tutorial |
| Bag upgrades | (12,22) | Southwest seating bay |
| Move tutor / deleter | (28,12) / (30,12) | Northeast advice station; both hidden until unlocked |
| Ball Guy | (24,22) | Southeast gathering bay |

Before reporting to Birch, the builder stands at (27,5), elevation 5, blocking
Poké Connect. During preparation for the construction tutorial he stands at
(6,12), elevation 3. The player approaches from (6,13); side interactions direct
the player around without granting gifts or advancing the scene. The builder
steps right, then the player moves north to the workbench. Existing Town Map,
Building Supplies, construction options and one-time progression logic are kept.
The persistent template and elevation are prepared together before objects spawn.

## Tile provenance and editing

`scripts/inspect_main_hall_sources.py` renders the complete Galactic HQ, Silph Co,
School and Poké Connect donor maps plus the Secret Base stand contact sheet.
These informed the composition; the latter interiors were inspected without
copying their brightly colored classroom/reception architecture into the hall.

- Galactic HQ: complete wall/pillar/door assemblies, metal floor, wall signs and
  monitor units. Source coordinates are recorded in
  `data/tilesets/main_hall_sources.json`.
- Silph Co: complete three-tile sofa at (9,3), height two, and planter at (5,2),
  height two. The original foreground is retained over the hall's wall/floor.
- Secret Base stand 0x272: tread cadence. Private neutral-gray left/right ends
  and repeatable centers form the wider stair flights. No decoration, slide,
  puzzle or directional stair behavior is carried over.
- Original Town Square/Slateport: the complete 3 × 2 pedestal (metatiles 0x221,
  0x220, 0x222 above 0x229, 0x228, 0x22A), including its existing plaque. Its
  foreground pixel shapes are unchanged; the outdoor background is replaced by
  HQ floor and colors share the hall's neutral palette. No custom pedestal.
- Private neutral palette: stair/frontage details, floor borders and
  small warm markers. Frontage, pedestal and lower furniture tiles use covered
  layering so adjacent player/follower heads draw in front.
- Doors use the verified HQ shutter animation with retracting leaves and a
  private palette; closed School doors cannot animate.

There are 126 unique 8×8 tiles and 13 background palettes after the
[shared furnishing pass](hub_furnishings.md), within GBA limits.
Palette bank 13 remains free for the engine. Decorative metatiles have ordinary
behavior. Only the wing doors and corridor strips have interaction/warp behavior.

Edit `data/layouts/Rogue_Area_TownSquare/map.bin` and `border.bin` in Porymap.
Normal generation **does not rewrite them**:

```sh
python3 scripts/generate_main_hall.py
python3 scripts/generate_main_hall.py --check
```

The guarded `--init-layout` was used only for the first outdoor-to-indoor
conversion. Its initial draft is not a replacement for the refined authored map.
Asset provenance, private facade resources, door graphics and constants are
generated. Events remain authoritative map JSON; dialogue/movements use `.pory`.

## Connections and save compatibility

All passages are three wide. The warp strip is the second shaded tile, and all
incoming lanes use the shared centered-arrival helper:

| Direction | Center | Original indices | Added lane |
|---|---|---|---|
| North | (18,1), elevation 5 | 0–1 | 11 |
| East | (35,17), elevation 3 | 2–3 | 12 |
| South | (18,28), elevation 3 | 4–5 | 13 |
| West | (1,17), elevation 3 | 6–7 | 14 |

School return 8, Poké Connect return 9 and teleport arrival 10 keep their
indices. Three facade maps/layouts are appended without renumbering existing
entries: horizontal, north (elevation 5), south (elevation 3). Connection offsets
are 14. Missing connections become complete walls with black exterior backing.

Old outdoor object snapshots and newly solid player locations recover via the
existing continue warp. An unfinished construction lesson returns the player to
the south workbench approach; other states return south of the monument. Flags,
gifts and introduction progress remain untouched. Valid current gallery and
ground-floor saves remain in place. Hallway saves beyond a moved threshold use
the shared centered recovery. There is no save format change or save deletion.

## Validation and previews

`build/main_hall/room.png` is the overview with actual event sprites;
`room-locked.png` and `room-tutorial.png` show service/introduction variants.
`room-*-240x160.png` camera views cover the south approach, monument, both stairs,
gallery, doors and each service bay. `statue-states.png` shows all four statues
with their actual palettes.

The asset validator checks 192 combinations (16 connections × School × tutors ×
three introduction stages), elevation-aware reachability, service approaches,
three-lane warp coverage, solid gallery frontage and preserved interior returns.
Focused Rogue tests use `Main Hall:` for runtime closures, arrivals, doors,
stairs, builder elevations and obsolete/current save snapshots. `Hub hallways:`
checks the previously completed rooms as well.

Validation on 2026-09-13: both `Main Hall:` tests and both `Hub hallways:` tests
passed, as did all four rebuilt-room asset verifiers and `git diff HEAD --check`.
The debug ROM was rebuilt incrementally; the final ROM includes the reused
Town Square pedestal. Existing map/layout IDs and wing return indices are unchanged.

Interactive mGBA playthroughs remain required for door timing, staircase/follower
movement, actual tutorial cancellation and gifts, wing returns and real saves on
both levels. Assembled previews and headless checks do not replace those checks.
