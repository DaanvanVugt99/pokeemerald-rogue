# Trial Area

The 46 × 38 underground Trial Area uses the basin, retaining walls, rails,
equipment, and crates from Aqua Hideout. The north landing keeps the existing
Trial building facade and stair artwork. The west and east corridors meet below
the stairs, the north corridor passes the right side of the building, and the
south corridor follows the basin. The workbench is in the upper west recess.

Edit the parent room and border in Porymap. Trial Architecture and Trial
Equipment are private tilesets. Their indexed tile sheets, metatiles, attributes,
and palettes are the editable source assets. The shared hub catalogue is used by
the neighboring districts.

| Connection | Center arrival | Warp IDs |
| --- | --- | --- |
| North | (34, 2) | 0, 1, 11 |
| East | (43, 19) | 2, 3, 12 |
| South | (23, 35) | 4, 5, 13 |
| West | (2, 18) | 6, 7, 14 |

The existing building entrances use indices 8 and 9 and the existing building
interior returns via 8. The hub teleport and recovery arrival uses 10 at
(25, 15). A changed workbench position identifies saves from the outdoor
layout. Current saves in closed passages or on invalid tiles also recover to
the teleport landing. No save layout fields were added.

If the entrance geometry moves, update the warp events and facade connection
offsets in `data/maps/Rogue_Area_TrialFrontier/map.json` and the corresponding
wall closure rectangles in `tools/data/trial_area.json`. Run
`python3 scripts/generate_trial_area.py` to refresh the private facade binaries,
runtime closure table and preview. Add `--check` to verify the generated files
and test player routes in all 16 connection combinations. Both modes preserve
the authored room, border, and tilesets.
