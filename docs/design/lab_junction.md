# Underground Research Junction

`Rogue_Area_Labs` retains its 28 × 24 buffer, district identity, music, movement
permissions, interiors, and progression. Its usable room is a compact junction
inside the same underground base as the Adventure Gate. Only this district uses
the new private tilesets and corridor facades. No save-format change is involved.

## Composition and source artwork

The architecture mirrors around x = 14: outer walls at x = 4/24, matching wing
doors at x = 9/19, and three-tile north/south passages at x = 13–15. The main
east/west crossing occupies y = 8–10. Equipment and the workbench sit outside it.
Lower side bays hold the concealed Unique Lab and debug maintenance access.
Unbuilt passages close with complete walls. Black backing separates horizontal
wall caps from walkable floor; floor colors fade toward each warp boundary.

Assemblies were taken from actual source maps, not guessed from an atlas:

| Assembly | Source map and coordinates | Treatment |
| --- | --- | --- |
| Industrial walls, pillar ends and inward joins | Galactic HQ, x0/y0–8 and x5/y12–14 | Preserve complete joins and black backing |
| Matching shutter frames | Galactic HQ, rectangle (1,12), 3 × 2 | Keep complete frame; animate two retracting leaves |
| Specimen machinery | Galactic HQ, rectangle (1,1), 2 × 3 | Complete machinery bank beside Birch's wing |
| Computer workbench | Encounter Lab, rectangle (3,0), 2 × 2 | Preserve equipment foreground; replace beige background layers with the room wall/floor |
| Approach markings | Poké Ball Factory, (1,2) and (12,2) | Small mirrored yellow markings by both main doors |

Exact metatiles and palette/tile remapping are recorded in
`data/tilesets/lab_junction_sources.json`. Decorative tiles use normal behavior,
not donor puzzle/interaction behavior. Only actual doors and exit lanes receive
their corresponding behaviors. Private door graphics are selected by layout
before metatile ID, so unrelated maps with the same tile number are unaffected.
The asset set uses 77 unique tiles and 12 map palettes, within engine limits.

## Events and preserved warps

Coordinates are zero-based Porymap positions. Existing warp indices 0–14 are
preserved. The old unreachable warp 8 remains sealed in black space.

| Purpose | Warp index | Position |
| --- | --- | --- |
| North passage | 0 / 1 | (13,0) / (14,0) |
| East passage | 2 / 3 | (27,9) / (27,10) |
| South passage | 4 / 5 | (13,23) / (14,23) |
| West passage | 6 / 7 | (0,9) / (0,10) |
| Birch's Lab | 9 | (9,4) |
| Config Lab | 10 | (19,4) |
| Introduction arrival | 11 | (9,7) |
| Debug maintenance access | 12 | (20,13) |
| Hub teleport arrival | 13 | (14,11) |
| Unique Lab | 14 | (8,13) |
| Added upper west/east exit lanes | 15 / 16 | (0,8) / (27,8) |
| Added third north/south exit lanes | 17 / 18 | (15,0) / (15,23) |

All four hub halls stay three tiles wide through their shadow fades and facade
continuations. The added lane warps resolve through the existing connection slots.
Private horizontal/vertical facades retain connection offsets 6/10 and are
appended to map/layout lists without renumbering existing entries.

Birch starts at (8,7), beside the player's (9,7). He walks up and then right to
enter his door without crossing the player. After the lab visit he waits at
(9,6), below the player at (9,5), then walks through the clear crossing toward
the Adventure Gate in whichever direction it was built. Introduction flags and
timing retain their previous meaning.

The workbench remains object 1 at (6,14), against the back wall of the left side
recess, governed by its original hide flag. Its southern approach at (6,15) stays
clear. Prefer builder workbenches against a top wall in future hub interiors;
their sprite reads best with wall backing rather than floating in open floor.
The Unique Lab's existing upgrade adds its frame and door at runtime; before
that, its sign script is silent and the entire recess is ordinary walling.
The maintenance door exists only under `ROGUE_DEBUG`. Lab interiors and their
return warps are unchanged.

On continue, a saved workbench at the old outdoor location or a player standing
on newly solid tiles triggers the existing continue-warp mechanism. The arrival
is (9,7) before the opening scene, (9,5) while leaving the lab, or (14,11)
otherwise. Introduction state is preserved; completed scenes are not replayed.
Birch's scripted positions alone do not trigger recovery.

## Editing and generation

Edit the main `data/layouts/Rogue_Area_Labs/map.bin` and `border.bin` directly in
Porymap. Keep events in `data/maps/Rogue_Area_Labs/map.json` and authoritative
scripts in `scripts.pory` synchronized with changed geometry.

```sh
python3 scripts/generate_lab_junction.py
python3 scripts/generate_lab_junction.py --check
```

Normal generation writes private tileset/door/facade resources and reads the
current main layout for validation and previews. It never rewrites that layout
or border. The guarded `--init-layout` option was used for the initial outdoor
conversion and refuses to run once the private tileset is assigned.

Previews under `build/lab_junction/` include the whole room, unlocked and debug
variants, Birch's initial placement, 240 × 160 camera crops enlarged with nearest
neighbor scaling, all 16 connection masks, and a door animation GIF. Event
overlays use actual game sprites and their palettes.

## Validation

The generator checks all 128 combinations of four connections, Unique Lab
availability, debug access, and workbench visibility. It checks service
reachability, sealed exits, every walkable boundary lane's warp, exact facade
seam pixels, solid black borders, structural collision symmetry, original lab
return indices, and Birch's actual `.pory` walking routes in each direction.

The generated-asset check and both debug/release ROM builds passed on
2026-09-12. Existing map and layout IDs remain unchanged; only the two facade
entries were appended. `git diff --check` also passed.

Focused runtime tests use the `Lab junction:` filter in the Rogue suite. They
exercise all cardinal connection masks, door behavior and sound selection,
upgrade/debug gates, added warp redirection, old-save recovery for every intro
state, and continued loading of current positions.

Interactive emulator verification is still required for door animation timing,
follower movement, the complete opening conversation/entry/return sequence,
teleport animation, and real save/reload. Rendered camera crops and automated
checks do not substitute for those playthrough checks.
