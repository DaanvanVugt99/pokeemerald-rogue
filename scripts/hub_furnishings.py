"""Verified Galactic HQ furnishings for the four private underground tilesets.

Only appends metatile IDs. Layouts stay authored in Porymap; callers decide
placement. Retains donor pixels/colors and strips incidental donor behaviors.
"""
import json
from generate_portal_room import ROOT, words

# Complete arrangements, selected from the rendered source map, not atlas order.
ASSEMBLIES = {
    'Table': (46, 39, 2, 2),
    'Stool': (45, 39, 1, 1),
    'Cargo': (37, 44, 2, 3),
    'WallPlate': (50, 35, 1, 1),
}


def append_hub_furnishings(builder):
    b = builder
    layout = next(l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
                  if l['id'] == 'LAYOUT_ROGUE_ENCOUNTER_GALACTIC_HQ')
    cells = words(layout['blockdata_filepath'])
    source = b.sources['galactic']
    # Bank zero was the all-black placeholder. Its unused nonzero entries can
    # hold the furniture accents without allocating a fourteenth BG palette.
    # Preserve index zero, and prove no established tile uses another entry.
    assert all(not any(b.tiles[v & 1023]) for meta in b.metas for v in meta if v >> 12 == 0)
    spare = [(0, 0, 0)]

    def import_tile(word):
        raw = source.tile(word)
        if not any(raw):
            return 0
        source_colors = source.pal(word >> 12)
        colors = {source_colors[c] for c in raw if c}
        # Reuse a full matching palette first, preserving transparent index zero.
        pi = next((i for i, pal in enumerate(b.pals[1:], 1) if colors <= set(pal[1:])), None)
        if pi is None:
            for c in sorted(colors):
                if c not in spare[1:]:
                    spare.append(c)
            assert len(spare) <= 16, ('Furniture accents exceed spare palette', len(spare))
            b.pals[0] = spare + [(0, 0, 0)] * (16 - len(spare))
            pi = 0
        palette = b.pals[pi]
        converted = bytes(0 if c == 0 else palette.index(source_colors[c], 1) for c in raw)
        if converted not in b.tiles:
            b.tiles.append(converted)
        return b.tiles.index(converted) | (word & 0xC00) | (pi << 12)

    for label, (sx, sy, width, height) in ASSEMBLIES.items():
        for y in range(height):
            for x in range(width):
                mid = cells[(sy+y)*layout['width']+sx+x] & 1023
                meta = source.meta[mid//512][mid%512*8:(mid%512+1)*8]
                name = f'Decor_{label}_{x}_{y}'
                assert name not in b.names
                base = 'WallBase' if label == 'WallPlate' else 'Floor'
                b.names[name] = len(b.metas)
                b.metas.append(b.metas[b.names[base]][:4] + [import_tile(v) for v in meta[4:]])
                # Both layers behind actors: no clipped heads along lower edges.
                b.attrs.append(0x1000)
                b.provenance.append({'name': name, 'source': 'galactic', 'metatile': f'0x{mid:03X}',
                                     'reference': {'layout': layout['id'], 'x': sx+x, 'y': sy+y},
                                     'assembly': label, 'background': base, 'behavior': 0,
                                     'palette': 'Exact donor colors; existing banks plus unused bank-0 entries'})
    assert len(b.tiles) <= 512 and len(b.metas) <= 512 and len(b.pals) <= 13


def architecture_collision(cell, names):
    """Furniture may differ by service bay; it is not part of wall symmetry."""
    if cell & 1023 in {v for k, v in names.items() if k.startswith('Decor_') and not k.startswith('Decor_WallPlate_')}:
        return 0
    return cell & 0xC00
