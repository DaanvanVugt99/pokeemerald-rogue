# Supply Depot

The former Poké Marts retain `ROGUE_AREA_MARTS` and all existing upgrade IDs.
The 37x31 indoor room uses HubArchitecture + HubFurnishings. Northwest houses
Poké Balls, northeast TMs, southwest general supplies, and southeast separate
battle-item and held-item counters. The central crossing has three clear lanes.

## Authoring and assets

Edit the room and border in Porymap. Normal generation never rewrites either:

```sh
python3 scripts/generate_hub_tiles.py --check
python3 scripts/generate_supply_depot.py
python3 scripts/generate_supply_depot.py --check
```

The depot command writes only its alias header, private corridor facades, and
previews in `build/supply_depot`. `--check` verifies the generated outputs and
writes previews. `--init-layout` is a guarded one-time outdoor conversion; it
refuses an already converted map. Do not use it for later edits.

Counter metatiles 0x374-0x375 reuse existing table graphic quadrants and retain
MB_COUNTER so the engine can reach a clerk across the counter. They use covered
layering and allocate no graphics or palettes. Keep complete equipment and plant
assemblies; top-wall plants retain their wall backing and shadowed lower pot.

## Gameplay and compatibility

Object slots 1-5 remain general, Balls, TMs, held items, and battle items; slot 6
remains the builder workbench. Slot 7 retains the truck's visibility flag but uses
the existing supply-crate sprite. Local equipment greetings preserve the shared
travelling-shop scripts and all dynamic shop inventory/menu behavior.

Three departments close when unavailable; equipment opens if either equipment
shop is unlocked, including inconsistent flags. Each equipment counter has its
own inactive state. General, Ball, and TM stock upgrades reveal storage displays.
Runtime patches apply individual displays before whole-department closures so
locked interiors cannot acquire stray bright floor or furniture tiles.

Warp indices 0-7 retain N/E/S/W assignments, index 8 is teleport at (18,23), and
indices 9-12 add the third lanes. Threshold centers are north (18,1), east
(34,15), south (18,29), west (2,15). All lanes use centered hallway arrivals.
Coordinate changes must update events, runtime closures, facade profiles, and
validation expectations together.

Saved stale static object coordinates, elevations, or graphics and invalid
authored-map positions trigger a continue warp to teleport arrival. This rebuilds
objects without altering inventory, money, upgrades, or the serialized save format.

## Validation

The checker interprets the actual C tile patches and checks 2048 combinations:
16 connections times all 128 upgrade bit patterns. It tests shop counter behavior
and customer approaches, workbench/cargo access, locked-department isolation,
warp lanes and inward steps, teleport clearance, and complete wall plants.
The shared checker also verifies all four facade seams and graphics limits.

Previews include locked, partially upgraded and full rooms, plus native 240x160
camera crops and nearest-neighbor enlarged views for counters and the crossing.
Focused `Hub hallways:` tests cover the twelve new lanes, teleport exclusion,
legacy staff recovery, valid-save stability, invalid positions, and retained
money/upgrades. Use the normal incremental ROM build; do not clean the checkout.

Interactive purchase/sell menus, repeated conversations, upgrades in the emulator,
and a real old-mart save still require manual verification. Automated path and
save-recovery tests do not substitute for those checks.

## Tile correctness pass

Department openings now use complete cap/join/base/foot jambs. Interior caps
use floor backings; exterior caps retain void. Continuous divider walls keep
pillar segments instead of premature feet or caps. Closed exits/departments
restore complete wall assemblies without leaving jamb fragments.

Northern computer benches attach to the wall; southern computers use floor
backings in the lower service area. Specimen cabinets retain their foregrounds
and use appropriate floor/shadow backings. Inactive equipment bays retain whole
counters instead of isolated wall-base tiles. Checks cover these assemblies in
every state; wall-states.png previews all sixteen connection configurations.

## Original shop decorations

`scripts/supply_depot_merchandise.py` imports foregrounds from the original
GeneralHub + Slateport mart: 0x37F/0x387 drawers, 0x38F crates, 0x38E/0x386
baskets, and 0x36E/0x376 merchandise. Building + Shop supplies the 0x228/0x229
stocked shelf and 0x22B/0x22C medicine display. Foreground transparency is retained;
colours are mapped to existing hub palettes. Separate floor/shadow variants keep
outdoor ground and indoor-shop wall colours out of the new room.

Imports consume 36 previously blank static padding tiles, 643-678, and never
move animation slots or prior metatiles. The normal shared generator does not
rewrite the authored room or the four new market prefabs. Stock-upgrade footprints
remain unchanged; closed equipment counters also hide their merchandise displays.
