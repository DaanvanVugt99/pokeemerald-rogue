# Adventure node icons

Honey Trees use the golden tree sprite from Pokabbie commit
`c503f32cf0a61d3c77b3cef8cfff29f0c52cff69` (the type-colored den update).
`graphics/object_events/pics/rogue/grass_golden.png` is copied unchanged;
its existing NPC_2 palette and 16x16 object layout are retained.

Unique Dens use a compact stone arch with a solid purple opening and a visible
stone threshold. Its broad outline, simple shading and grounded base match the
physical objects used by other nodes.
The sprite remains at `graphics/object_events/pics/rogue/grass_unique_den.png`
to preserve graphics IDs/references. It uses a 32x32 object layout,
512 bytes per 4bpp frame (three frames), and existing NPC_4 palette. Index zero is transparent.
No shared palette, node behavior, spawn rate, or encounter changes.

## Arch asset provenance

Created with the built-in image_gen tool. The transparent master was trimmed
to its alpha bounds, fitted within 20x20 pixels with nearest-neighbor sampling,
placed on a 32x32 canvas with its base on row 30, and mapped to NPC_4 entries
7, 9, 10, 11, 12 and 13 (zero reserved for transparency).
The final artwork occupies 20x18 pixels, between the previous 14x12 and 24x21
exports. Keeping one transparent row below the base preserves ground alignment.
Three vertically stacked frames change only the purple interior: the existing
highlight expands by zero, one, then two Manhattan-distance pixels, masked to
the opening. The stone and transparency are identical across frames. The native
animation plays 0/1/2/1 for 48/24/48/24 ticks (about 2.4 seconds per pulse).
It follows normal object-animation pausing during locked interactions.

Generation prompt:

> Create a single tiny Pokemon Emerald GBA overworld map-node sprite: a squat weathered stone archway containing a solid muted-purple mysterious opening. It should look like a physical little ruin standing on the ground. The final sprite is ONLY 16x16 game pixels; draw it as an enlarged nearest-neighbor pixel sprite on a TRANSPARENT background, with very coarse chunky pixel clusters and no fine detail. Nearly square silhouette, about 14 pixels wide and 14 pixels tall. Thick curved arch made from just 5 or 6 broad gray stone blocks, sturdy short pillars, a wide flat stone threshold/base clearly touching the ground. Show a small amount of the stones' top surfaces in the usual Pokemon Emerald slightly overhead overworld view. Dark one-pixel outline, upper-left lighting, two flat gray shades and one light gray highlight. Opening is clearly filled with deep plum purple, with one broad muted mauve highlight; no black empty slit. Use only these flat colors: outline #081020, shadow gray #414152, mid gray #8B8B94, light gray #D5D5D5, deep plum #522041, purple #A44173. Opaque physical stone and purple interior; transparent background only outside the object. Consistent restrained Gen 3 pixel art. No floating particles, no glow, no sparkles, no cyan, no pink neon, no swirl, no portal effects, no tiny cracks, no vines, no grass, no writing, no decorations, no surrounding scene, no grid lines, no blur, no gradients, no smooth anti-aliased edges. A solid chunky grounded arch icon, NOT tall and thin. Output exactly one centered sprite.


## Unique Den interior

Unique Den nodes now warp to `Rogue_Encounter_UniqueDen`, appended to the existing
encounter map group so existing map IDs stay stable. Its separate 11x15 layout
retains the ordinary Den's collision, entry/exit, and wild Pokemon placement.
It reuses the Secret Base brown cave tiles/metatiles with a private grey-stone
palette (`data/tilesets/secondary/unique_den/palettes/06.pal`). The exit uses muted
purple highlights matching the arch. Ordinary dens retain their shrub tileset.
Encounter scripts, music, weather, map type and battle settings are unchanged.
Unique Den Pokemon use the same collision and interaction logic as ordinary den
and legendary Pokemon: stepping onto them can trigger their encounter, and A-button
interaction remains available. Shared repel and ride-stealth rules still apply.

Validation: incremental debug ROM build and whitespace checks passed. Source-data
checks confirm stable existing map/layout IDs, identical den collision/event
settings, and a walkable route from entry to encounter. The interior preview is
a tile render; entering, catching and leaving still need an in-game visual check.
