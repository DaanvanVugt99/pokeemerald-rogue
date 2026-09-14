# Pokemon Nursery

The 37x29 indoor nursery retains Rogue_Area_DayCare, its layout/hub identity,
nine object slots, storage-slot associations, and all existing progression.
The Supply Depot is independent and is not modified by this conversion.

## Authored geometry

The central spine is x17-19 and the east-west crossing is y13-15. Exterior
walls are x4/32 and the main floor ends at y25. The occupied footprint is L-shaped: a broad upper care gallery and a narrower
lower-left service wing. The southeast cutaway (x21-36,y16-28) is solid void;
its inner vertical wall runs at x20. The nursery is not four shop departments. The northwest coffee corner has a six-tile-wide couch at (7-12,6-7),
a sideways Rocket couch at (12,9-11), and a low table at (9-10,9-10).
The right couch faces inward toward the table. A drinks cabinet at (6,9-10)
and bottles at (7,9) balance the left side, with one wall-side pot and clear
space south of the table. Pokemon occupy
(8,7), (11,7), and (12,10), with approaches at (8,8), (11,8), and (11,10).
The first capacity upgrade activates the right half of the long couch; the
second activates the sideways couch. Inactive upholstery is dimmed while its
complete silhouette remains. Furniture stays solid at every capacity, and
all approach aisles stay open. Capacity uses Rogue_GetCurrentDaycareSlotCount:
CAPACITY1 enables three even if CAPACITY0 is absent.

The egg nursery occupies x23-31, y5-10, with a three-tile-high west doorway at
x22,y7-9. The smaller tea room occupies x5-11,y19-25, opening east at
x12,y20-22. Reception fits beside it in the lower wing with a stepped desk,
two terminals and maintenance workbench. There are no full-length central partitions.

Object identities and anchors:

| Slot | Service | Position |
|---|---|---|
| 1 | Workbench | 15,24 |
| 2 | Tea attendant (variable graphics) | 8,20 |
| 3 | Reception | 14,20 |
| 4 | Egg breeder | 27,7 |
| 5 | Stored Pokemon 0 | 8,7 |
| 6 | Stored Pokemon 2 | 12,10 |
| 7 | Stored Pokemon 1 | 11,7 |
| 8 | Pokégear terminal | 13,18 |
| 9 | PC | 15,18 |

Couch seats remain solid to players and render below Pokemon. The long couch has southern interaction approaches; the sideways couch
has a western approach.

Counter customers stand two tiles south of staff; MB_COUNTER bridges the gap.
Both terminals have a free southern approach. The Pokégear terminal retains
its original north-facing interaction and item delivery. Signs have separate
north/south-facing events on the two wall rows, with clear approaches on both
sides. Locked departments remain labelled from the crossing.

Original warp pairs 0-7 retain N/E/S/W ordering. Teleport 8 is (18,21).
Third lanes 9-12 append N/E/S/W. Centered arrivals are (18,1), (34,14),
(18,27), (2,14). Each exit uses the second shaded tile and private facades.

## Editing workflow

Edit layout and border in Porymap. Back up current authored files before moving
any selection, distinguish intentional geometry changes from accidental shifts,
and update events, runtime patches, validation anchors and facade offsets together.
Never restore an earlier design automatically over later user edits.

```sh
python3 scripts/generate_pokemon_nursery.py
python3 scripts/generate_pokemon_nursery.py --check
python3 scripts/generate_hub_tiles.py --check
```

Normal generation writes only the nursery alias header, its private facades,
and previews under build/pokemon_nursery. It never writes the room or border.
The guarded --init-layout converter is only for the original outdoor map and
backs up that map, border, and event file before converting. Do not rerun it.
Existing shared IDs and palettes remain unchanged. Nursery-specific imports
append twelve metatiles at IDs 921-932 and use previously empty static tile
slots 679-691. No sprite slots, palette banks or animation allocations are added.

`nursery_decorations.py` imports GeneralHub + Mauville foregrounds actually used
by the original Day Care: flowers 0x004, shrubs 0x00D, fences 0x291/0x28A, and
the complete tea flower-box pair 0x2B4/0x2BC. Floor/shadow variants preserve
transparent silhouettes while mapping colors into existing banks. Authored
Nursery prefabs keep these assemblies available in Porymap, but the current
room uses indoor shop pots, a drawer cabinet and bottles instead. Retain the
unused imports to preserve stable shared IDs.

## Runtime and compatibility

Connection closures, inactive upholstery and locked rooms live in the existing
Day Care runtime patch function. Closed room floors are consistently dark,
including the full top shadow row. Upgrades do not change costs, eligibility,
experience sharing, storage, tea effects, eggs, or remote services.

A continue warp refreshes old coordinates/elevations and invalid positions,
including positions inside now-locked rooms. Dynamic graphics for object 2 and
objects 5-7 are exempt from graphics comparisons, while their coordinates and
elevations are still checked. Static object graphics remain checked. Deposited
Pokemon, egg requests/cycles, inventory and upgrade data are not migrated or reset.

## Validation and manual follow-up

The validator reads actual C patches and checks 4,096 states: 16 connections,
all 32 bit patterns of capacity0/capacity1/breeder/phone0/tea, and eight occupancy
patterns. Other phone, breeder and experience upgrades retain their existing
logic and do not affect geometry. Checks cover collisions, all visible objects,
counters, terminals, signs, couch layering, doorway assemblies and warp lanes.
Previews cover locked, partial, full and empty rooms; native 240x160 camera crops
include representative 32-pixel Pokemon sprites.

Run the focused `Hub hallways:` test for centered arrivals, special-warp exclusion,
legacy save recovery, deposited Pokemon and egg-progress retention, and valid
dynamic graphics. Then run an incremental ROM build and git diff --check.

Interactive checks still required: deposits/withdrawals in all three slots,
Pokemon interactions, egg requests/collection, Pokégear delivery and remote
upgrades, tea, PC, repeated conversations and silent refreshes, and a real
old-room save. Automated checks do not replace those emulator checks.

## Tile assembly cleanup

Nursery-only shadow compositions at IDs 933-937 retain the original
glass/sofa foregrounds and consume no graphics. Glass variants remain allocated
for stable IDs; the room now uses only the sofa variants.
North posts and side-room doorway posts finish with bases/feet; exposed floors
and indoor pots use matching shadow backgrounds beside walls.
The closed egg nursery retains a continuous dark top-floor shadow.
The validator checks these assemblies across upgrade/connection states.
`finish_tile_assemblies` and `finish_seating` are explicit authoring helpers
only, never normal regeneration steps; later Porymap edits remain authoritative.

## Coffee corner furniture

Rocket tiles 0x3F5/0x3FD supply the sideways couch, with a repeatable padded
middle. Shared IDs 938-947 append these compositions and dim upholstery;
existing graphics, metatiles, palettes and animations retain their allocations.
New static graphics use spare slots after the original nursery imports.
`finish_coffee_corner` is an explicit scoped authoring helper, never invoked
by normal validation or preview commands. Preserve later Porymap edits.
