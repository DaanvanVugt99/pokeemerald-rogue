# Berry Lab

The former Berry Fields is a 29 x 25 indoor greenhouse using
HubArchitecture + HubFurnishings. The existing FarmingField map/layout IDs,
hub upgrade IDs, object local IDs, and berry tree IDs are retained.

## Layout and progression

Four open-front, glass-backed beds surround the central three-wide crossing.
Clockwise from northwest, their five planting spots retain these assignments:

| Bed | Plant row | Berry IDs | Unlock |
| --- | --- | --- | --- |
| Northwest | (7–11, 7) | HUB_11–15 | Initial |
| Northeast | (17–21, 7) | HUB_6–10 | EXTRA_FIELD0 |
| Southeast | (17–21, 15) | HUB_1–5 | EXTRA_FIELD1 |
| Southwest | (7–11, 15) | HUB_16–20 | EXTRA_FIELD2 |

Locked bays keep their frames and replace soil with inactive floor. All plants
are individually approachable from the south aisle, including at maturity.
Active planting spots retain their original impassable collision; inactive
bay flooring is walkable.
The merchant (local ID 22) at (8,20), pot (23) at (7,20), and workbench (1) at
(10,20) share the south service nook. Brewing turns the merchant west toward
the pot. Menus, recipes, berry growth, prices, yields, and upgrades are unchanged.

## Hallways and saves

North/east/south/west threshold centers are (14,1), (26,11), (14,23), and (2,11).
Original warp indices 0–7 remain assigned to those directions; new indices 9–12
add their third lanes. Index 8 remains the teleport arrival, now (14,18).
Every exit has a three-wide fade, a warp on the second shade, and private
facade scenery. Missing connections close with the surrounding wall or void.

The usual centered-arrival helper handles all hallway lanes. On loading a save,
stale static object coordinates/elevations or an invalid player position trigger
a continue warp to the authored teleport arrival. This rebuilds live objects
and elevations without touching berry state or hub upgrade data. Extra follower
templates alone do not cause recovery.

## Porymap and generation

The room map/border are authored in Porymap. Normal generation never rewrites
them or the prefab catalogue:

```
python3 scripts/generate_hub_tiles.py
python3 scripts/generate_berry_lab.py
python3 scripts/generate_hub_tiles.py --check
python3 scripts/generate_berry_lab.py --check
```

The guarded `--init-layout` option is solely for the original outdoor conversion;
it refuses an already converted layout. Do not use it for regeneration.
Coordinate edits must remain synchronized with the runtime closure/bed patches,
map events, and the validator's explicit geometry.

`berry_lab_tiles.py` appends soil at metatile 0x362 and six covered glass-back
variants at 0x363–0x368. Soil reuses the original GeneralHub soil pattern in an
existing palette and preserves MB_BERRY_TREE_SOIL. Glass variants reuse all
original pixels but draw below tall sprites. The initial addition uses two graphics
tiles. The finishing pass appends floor-backed plant tops/bases, capped bed ends,
shallow active/inactive rims, grow lights, seed trays, and propagation cabinets
at 0x369–0x371. These use seven additional graphics tiles, preserving earlier
IDs, palettes, and animation slots (1021/1024 graphics slots total).

Walls follow the Research Junction assemblies: cap/join/pillar at outside
corners, join/base/foot at corridor openings, and matching closure patches.
Wall-backed donor plants are only used beneath actual wall tiles; open-floor
plants use the new floor-backed variants. The northwest/southeast planting
aisles and service approaches remain clear despite the additional greenery.

The Berry Lab validator interprets the actual C patches and checks 256
connection/bed/brewing states, including unreachable exits, service and mature
berry access, inactive soil, tree identity, glass layering, complete plant/bed
assemblies, and the correct wall joins in every connection state. Normal mode
generates only the Berry Lab tile aliases and private facade layouts. Both modes
write locked, empty, and mature previews plus native-camera enlargements under
`build/berry_lab`, including a `wall-states.png` contact sheet. The shared checker also checks every new facade seam.

Focused runtime validation uses the `Hub hallways:` filter in the Rogue suite,
including all Berry Lab lanes and legacy save recovery. Interactive verification
of berry menus, brewing, and a real pre-conversion save is a separate manual
check; static previews and headless tests do not replace that playthrough.

Shadow-backed pot bottoms and plant tops occupy 0x372�0x373. They reuse the
existing FloorShadow backing and plant foregrounds without allocating graphics.
Top-wall pots use the dark bottom; left-wall plants use both shaded halves.
