# Shared underground hub tiles

Adventure Gate, Research Junction, Safari (including its tutorial), Main Hall
and their corridor facades use **HubArchitecture + HubFurnishings** in Porymap.
Other districts and donor maps retain their original tilesets.

The conversion preserves every existing metatile's rendered pixels, transparency, layer and
behavior attributes, and every authored map cell's collision/elevation bits.
It includes the user's September 13 Porymap edits. Events, services, coordinates,
save layout and progression do not change.

## Editing in Porymap

Reopen the project after pulling this conversion so Porymap reloads its tileset
and layout caches. Both shared sets are already selected on all four districts.
Primary contains the established architecture and room features. Secondary adds
the donor furnishing catalogue. Labels are in `include/constants/metatile_labels.h`.

In the project's existing `porymap.project.cfg`, set
`prefabs_filepath=data/tilesets/hub_prefabs.json`
if it is not already set. This checkout's local configuration is set; the config
itself is ignored by Git. The prefab file is authored and editable in Porymap:
normal generation does not overwrite it or any map/border.

Sixteen initial prefabs provide tables/stools, crates, machines, worktops,
sofas, plants, habitats, capsules, pedestal, stairs and research doorways.
Stairs have elevation 0; other prefabs default to ground elevation 3. Change
elevation to 5 when placing furniture on a gallery. Doors still require a warp
event and appropriate gameplay setup; placing artwork does not create a service.
The existing PC, workbench and statue are object-event sprites, separate from
background tiles. Their appearance/availability is unchanged. Use **Hub / Sofa
against wall** for a couch against the rear wall; its three top tiles combine
the existing couch foreground with WallBase. The original floor-backed Sofa
remains available.
Use **Hub / Wall panel** for the small two-piece panel: `WallPanelTop` is
0x29C and `WallPanelBase` is 0x039. This is the intact Galactic HQ assembly
at (50,34–35); the upper piece was already in the donor catalogue.
**Hub / Door description sign** uses tiles `DoorSignTop` (0x33C) and
`DoorSignBase` (0x33D). It copies SignTop/SignBase's frame and wall background,
with three light text strokes on a dark face. The original display remains
available. Place it at an existing door-description interaction or add the
appropriate background event separately; the decoration itself has no script.

## Catalogue and provenance

The Adventure Gate nurse uses the Porymap-authored covered ControlBase variant
at 0x085. Its recipe is recorded in `tools/data/hub_tiles.json` under `variants`
so generation preserves it. The ordinary ControlBase at 0x022 is unchanged.

- Exact original room art: `portal_room`, `lab_junction`, `safari_lab`, `main_hall`
  source asset folders under `data/tilesets/{primary,secondary}`. These are now
  immutable composition inputs, not selectable runtime tilesets. The old
  generator recipes remain as provenance and forward normal commands to the
  shared pipeline.
- All 166 secondary metatiles in the existing `lab` tileset, including unused
  alternatives; no lab metatile is selected out of the catalogue.
- All metatiles used by the existing Galactic HQ, Silph Co and Poké Ball Factory
  maps, including their complete walls, equipment, furniture and floor variants.
  Unused tiles elsewhere in the multipurpose Facility sheets are not imported.
- `tools/data/hub_tiles.json` is the explicit source inventory and original
  room-name registry. `data/tilesets/hub_sources.json` records generated mappings.
- Donor puzzle, conveyor, teleporter and other behaviors are stripped from added
  catalogue tiles. The established room variants retain their approved behaviors.
- Original room art keeps every color exactly. Some additional donor art is
  recolored into compatible shared palette banks; no donor pixels are redrawn.

The prefab JSON schema was checked against Porymap's
[prefab reader](https://github.com/huderlem/porymap/blob/master/src/ui/prefab.cpp)
and [Emerald examples](https://github.com/huderlem/porymap/blob/master/resources/text/prefabs_default_emerald.json).

## Capacity and animation

1012/1024 graphics tile slots and 866/1024 metatile slots are addressed. The
metatile count includes unused padding up to the secondary-set boundary; there
are 488 populated catalogue entries. All 13 background palette banks are used.

The portal owns graphics slots 512–547 and palette 12; static tile allocation
skips those slots. Both research-door animations use palette 11 with their
original indexed frames. The shared secondary callback animates the portal
texture in all four rooms, so its tiles can safely be reused elsewhere without
animating unrelated machinery. No shared original-world tilesets are modified.

## Generation and validation

```sh
python3 scripts/generate_hub_tiles.py
python3 scripts/generate_hub_tiles.py --check
```

Both commands preserve authored map/border files and prefabs. `--check` verifies
generated bytes, all original room pixels and attributes, palette/tile limits,
animation isolation, door palettes, and valid layout/prefab references. It also
writes review images under `build/hub_tiles/`: each room, the complete catalogue,
prefab sheet and donor maps rendered from shared tiles.

Safari planted habitats append 36 variants, leaving the original habitat art
and all map collision/elevation bits intact. `generate_terrarium_tiles.py` copies
General grass and its three flower frames into the existing hub palette, clips
the planting beneath the frame, and retains intact/broken glass foregrounds.
Graphics slots 1008–1011 are reserved for flowers; padding below that reservation
remains available for future static imports. The shared secondary callback runs
the four-step flower cycle (0,1,0,2) every 16 frames alongside the portal animation.
No wild-grass behavior or grass movement effect is attached to these decorative
floors. Both Safari maps explicitly place the planted variants; normal generation
still never rewrites a map.

Exterior corridor seams are checked against the authored room edges as well.
After changing the room's edge walls, run
`python3 scripts/check_hub_corridors.py --update` to deliberately synchronize
only the private corridor facades. Room layouts, borders, offsets, warp events
and services are never rewritten by this command. Normal generation verifies
these seams without updating them. `build/hub_tiles/edges/` previews include
the actual connection strips outside the map rectangle (seven tiles, or eight
on the east), with all connections visible. Three-wide terminal floor shadows
remain intact. Shared facade directions must agree or the update is rejected.

Corridor verification also runs `scripts/check_hub_hallway_states.py`. It reads
the actual C closure patches and checks all 56 connection combinations across
Adventure Gate, Labs, Main Hall and both Safari maps: closed architectural seams,
blocked exits, three open warp lanes and reachable inward arrival tiles. It
renders the current on-disk tilesets under `build/hub_tiles/states/`, without
regenerating assets. Run it directly while reviewing local Porymap tileset edits.
The closed south edge is floor over void; remove corridor caps when they overlap
the last floor row. Side closures must join the pillars above and below them.
These checks cover hallway geometry, not NPC/service progression or emulator
transition animations; keep the focused runtime hallway tests for those arrivals.

The one-time `--convert-layouts` option remaps only layouts still using the old
private sets, verifies pixels/collision/elevation before writing, and saves local
backups under `build/hub_tiles/before-conversion/`. It is not a regeneration step.
Already-converted layouts are left alone. Do not use it to rewrite hand edits.

The source inventory records an ID-layout hash. Changes that would shift
existing metatile IDs fail rather than silently corrupt maps, prefabs or runtime
aliases. Extending/repacking this inventory needs an explicit reviewed ID
migration; merely changing the hash is not a migration.
New foreground/background combinations can instead be appended through the
inventory's `compositions` list with explicit IDs, without repacking existing
tiles or changing any map references. Couch wall tops occupy 0x339–0x33B.
Optional `paint` rectangles create native-pixel foreground variants using only
the copied tile's existing palette. Their graphics are appended after the
original catalogue, preserving source graphics and portal-animation slots.

Focused runtime checks use the existing `Hub hallways:` and `Main Hall:` Rogue
tests. Interactive Porymap and emulator validation still needs a human check of
the selector/prefabs, doorway animation and portal animation.
