# Underground base furnishings

The Adventure Gate, Labs, Safari (including its tutorial variant) and Main Hall
reuse complete Galactic HQ furnishings. This pass adds rooms' everyday uses:
waiting/seating at the Gate, a research table at Labs, Safari preparation bays,
and advice and gathering tables in the Main Hall. Cargo occupies perimeter
storage spaces; wall plates sit in the wall faces. Service events, warp lanes,
progression and scripted routes retain their positions.

## Source assemblies

`scripts/hub_furnishings.py` reads the Galactic HQ layout and appends private
metatiles to each room's existing asset builder:

| Assembly | Galactic HQ origin | Size |
|---|---|---|
| Small table | (46,39) | 2 × 2 |
| Stool | (45,39) | 1 × 1 |
| Complete small cargo stack | (37,44) | 2 × 3 |
| Wall plate | (50,35) | 1 × 1 |

Table settings retain the donor arrangement: the 2 × 2 table has a stool beside
each corner, forming a 4 × 2 group. Cargo uses the complete small stack from the
source map, including its top and bottom. The helper does not invent new shapes
or copy isolated pieces of a larger fixture. All foreground pixels and colors
remain intact; backgrounds use the matching private floor or wall face.

Tables and cargo are solid. Stools retain the source map's walkable behavior.
Wall fixtures remain solid parts of the wall. Decorative metatiles have ordinary
behavior and covered layering, so adjacent player/follower heads appear in front.

## Placement

- **Adventure Gate:** waiting table in the southeast bay; supplies beside the
  storage PC; paired wall plates. The setting starts at y11, keeping the entire
  y10 story crossing and portal approach clear.
- **Labs:** a table in front of the systems worktop, cargo beside specimen
  equipment, and plates above the lower work/service recesses. All of Birch's
  scripted routes, both lab doors and hidden entrances remain accessible.
- **Safari:** paired preparation tables and cargo in the lower workbench bays;
  upper and lower wall plates. The normal and broken-glass tutorial maps use the
  same placements. Enclosures, wandering Pokémon bounds and central exits stay
  clear.
- **Main Hall:** tables below the builder and tutor bays, and beside the bag
  service and Ball Guy. Cargo frames the upper service areas. The former small
  floor outlines around the lower NPCs give way to these table groupings; the
  monument surround, stair approaches and full crossing remain open.

Architecture remains symmetrical. Furniture can differ with each bay's purpose;
the Gate validator distinguishes furnishing footprints from structural walls
when checking symmetry. It still validates every service approach, the story
crossing, departure script and central aisle.

## Palette and editing contracts

Existing metatile IDs are preserved; decorations are appended. No shared donor
tileset is edited. The helper reuses exact matching palette banks, then packs
additional furniture colors into previously unused nonzero entries of bank 0.
It first verifies that no established tile uses those entries. Index zero stays
black/transparent, preserving the void and existing artwork. There are still at
most 13 background palettes per room, including the door/portal resources.

Layouts remain Porymap-authored. Normal asset generation reads their files and
does not stamp furniture or replace hand edits. Run the room generators with
`--check` to validate assets, state-dependent reachability and regenerate previews:

```sh
python3 scripts/generate_portal_room.py --check
python3 scripts/generate_lab_junction.py --check
python3 scripts/generate_safari_lab.py --check
python3 scripts/generate_main_hall.py --check
```

The existing save recovery in Labs, Safari and Main Hall already handles newly
solid floor. The Gate now performs the same check before continuing a save;
standing where its new table/cargo is placed returns the player safely to the
console without changing progress. A focused `Portal room:` test covers both
furniture recovery and continued loading on the clear story crossing.

Overview and native camera previews are under `build/portal_room`,
`build/lab_junction`, `build/safari_lab` and `build/main_hall`. Interactive emulator
checks of furniture layering and walking onto stools remain to be performed.

The combined overview is `build/hub_furnishings/four-rooms.png`. All four asset
verifiers and five focused `Portal room:` tests passed on 2026-09-13. Map/border
binaries are now explicit map-object dependencies, ensuring Porymap edits are
included in incremental builds without touching JSON or cleaning the checkout.
