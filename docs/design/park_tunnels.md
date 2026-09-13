# Park tunnel entrances

Ride Training retains GeneralHub and uses a private Mauville-derived secondary
`ParkTunnels` tileset. Entrances use the complete native GeneralHub mountain
family (0x68â€“0x7A: rounded grassy corners, plateau, side faces and foot), matching
the existing Park training cliffs. Original primary terrain palettes are chosen
per graphics tile; rock and grass are not forced into the metal palette.
The lab junction's Galactic HQ assembly supplies the metal donor references.
Twelve-pixel steel collars, beveled faces and recessed fasteners surround each
directional cut. The metal overlays the reviewed rock outside the two walkable
lanes; quiet stone shadow fades remain inside the passage.

Side and south rock formations continue off-screen. Their exposed passages end
under a solid rock roof after the first facade tile, avoiding an endless corridor.
The four Park facade connections remain visible whether a hub link exists or not:
disconnected sides replace the entire opening and steel collar with native rock.
Other districts retain
conditional facade visibility. All state previews include these permanent rocks.

Metal rails have projecting end blocks, chamfered end faces, and north lintel
mitres with post feet. These pieces sit outside the two walkable lanes.

## Authoring and generation

Run `python3 scripts/generate_park_tunnels.py` to regenerate private assets,
rock closure patches, facades, and previews. Normal generation never writes
the Park map or border. `--check` verifies outputs without updating them.
`--init-layout` is a guarded one-time migration requiring the original footprint
cells; it is not a normal regeneration command.

The original entrance footprints contain the dynamic rock patches; terrain
cleanup may extend around them in Porymap. The reviewed Park layout is stored as
`authored_map` in `tools/data/park_tunnels.json`, with subsequent reviewed edits
recorded in `authored_overrides` after the artwork and terrain joins. Preserve new Porymap edits and
review their snapshot changes deliberately. Normal generation checks the authored
map rather than repainting the old entrance rectangles. `generation_base` is the
immutable pre-tunnel composition input used to keep artwork allocations stable.
The original snapshot and `cleanup_edits` retain provenance for terrain changes.

Warp behavior is assigned from the eight fixed event positions, independent of
which visually matching metatile was copied in Porymap. Copied directional tiles
away from a threshold receive equivalent normal metatiles; each threshold gets
its correct direction. Artwork, NPCs, scripts, and teleport coordinates remain
independent of this correction. Off-map rock continuations follow the taller side faces
and the south shoulder. The west facade is 8 by 24 tiles at connection offset 0,
continuing the cliff above the tunnel; the other facades remain 8 by 8.
The south continuation follows the wider shoulder beside the revised shoreline.

The manifest also records border/event/script hashes, donor references and the
allocation digest. Review allocation changes rather than deleting the guard.
`data/tilesets/secondary/park_tunnels/sources.json` lists individual allocations.

Only unused secondary metatiles and graphics are allocated. Original used
Mauville metatiles 616, 617, 624, 625 remain intact. Palette bank 6 is verified
unused by the original Park and supplies the tunnel colors; all other Mauville
palettes remain byte-identical. GeneralHub palettes and primary animation remain
unchanged. Mauville animation destinations 608â€“671 are reserved and its callback
is retained. No new door animation is registered.

## Runtime and validation

`RogueHub_UpdateRideTrainingAreaMetatiles` replaces each disconnected entrance
with a continuous solid rock formation, including both threshold lanes, the full
metal frame, and matching top, side, and foot tiles. Signed patch coordinates
also replace exposed metal in the first off-map facade strip after connections
are loaded. Closed patches use existing native terrain with no arrow behavior;
the authored rounded outer corners remain intact. Older shutter artwork retains
its allocations for stable metatile IDs but is no longer used for closures.
The eight original warp events and teleport warp 8 are unchanged, as are routing,
arrival handling, and serialized structures. Permanent terrain visibility
selects the four Park-only facades in every connection state. Other outdoor districts keep their facades.

The checker exercises all 16 connection masks, reachability of both lanes,
solid non-arrow closed thresholds, inward steps, unchanged surroundings/events,
allocation reservations, palette preservation and all four facade seams. Mixed
state checks include closed off-map strips and their map-grid bounds.
Previews are written to `build/park_tunnels/`: full open/closed maps and eight
directional close-ups, plus `context-open.png` and `context-closed.png` with
off-map scenery. Preview art does not simulate sprite rendering.

Validation: run the checker, `git diff --check`, an incremental non-test ROM
build, and the existing `Hub hallways:` filter in the `rogue` suite when
`mgba-rom-test` is installed. In mGBA, visit the Park with each direction connected
and disconnected, use both lanes, return from underground rooms, inspect sprite
occlusion, and confirm the sealed thresholds cannot be crossed. Static checks
and preview inspection do not replace this final gameplay check.

## Tree and rock transitions in Porymap

Use `0x1CE` / `0x1CF` for tree caps on grass, and `0x1F0` / `0x1F1`
for tree caps in front of a straight rock face. Rounded cliff corners need
`METATILE_ParkTunnel_TreeRock_078_1CE` (left) and
`METATILE_ParkTunnel_TreeRock_07A_1CF` (right); their current numeric IDs are
shown in `build/park_tunnels/terrain-join-reference.png` and the generated labels.
These composites retain both native palettes using the two metatile layers.

A canopy cap sits above each complete tree body and base. When a new cliff
replaces a tree base, remove the entire interrupted tree or provide a proper
composite; do not leave its upper half behind. The verifier now catches missing
canopies, clipped bases, detached bases, and missing horizontal tree halves across the Park. Off-map joins use
correct left/right canopy halves instead of repeating one half across the seam.
