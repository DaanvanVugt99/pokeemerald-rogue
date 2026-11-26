#!/usr/bin/env python3
"""Build the Home and Trial medallion assets and verify authored hub placements."""

import argparse
import json

from PIL import Image, ImageDraw

from generate_hub_tiles import Catalogue, render
from generate_portal_room import ROOT, Source, packed, png, words
from hub_medallions import InlayTiles, SHARED_BANK, home, trial

HOME = 'data/tilesets/primary/home_architecture'
TRIAL = 'data/tilesets/secondary/trial_equipment'
TRIAL_ART = range(0x220, 0x240)
HOME_ART = range(480, 512)
HOME_STYLES = ('red_cave', 'blue_cave', 'brown_cave', 'yellow_cave', 'shrub', 'tree')
HOME_METAS = 'data/tilesets/secondary/secret_base'

# Positions follow the room's floor plan. The Safari stamp is centered on its
# southern corridor and Trial is centered on the gallery north of the basin.
PLACEMENTS = (
    ('Main Hall', 'Rogue_Area_TownSquare', 'MainHall', (18, 15)),
    ('Labs', 'Rogue_Area_Labs', 'Labs', (14, 12)),
    ('Home Base', 'Rogue_Interior_Home', 'Home', (6, 6)),
    ('Berry Lab', 'Rogue_Area_FarmingField', 'Berry', (14, 14)),
    ('Market', 'Rogue_Area_Market', 'Market', (18, 15)),
    ('Supply Depot', 'Rogue_Area_Marts', 'SupplyDepot', (18, 15)),
    ('Pokémon Nursery', 'Rogue_Area_DayCare', 'Nursery', (18, 14)),
    ('Safari Area', 'Rogue_Area_SafariZone', 'Safari', (18, 15)),
    ('Safari tutorial', 'Rogue_Area_SafariZone_Tutorial', 'Safari', (18, 15)),
    ('Trial Area', 'Rogue_Area_TrialFrontier', 'Trial', (23, 15)),
)


def palette_bytes(colors):
    lines = ['JASC-PAL', '0100', '16'] + [' '.join(map(str, c)) for c in colors]
    return ('\r\n'.join(lines) + '\r\n').encode()


def home_assets(colors):
    base = Source('secret_base', 'secret_base/red_cave', metatiles=HOME_METAS)
    existing_words = base.meta[0] + base.meta[1]
    assert not set(HOME_ART) & {word & 1023 for word in existing_words}
    bank_zero = [word for word in existing_words if word >> 12 == 0]
    for style in HOME_STYLES:
        src = Source('secret_base', f'secret_base/{style}', metatiles=HOME_METAS)
        assert not any(any(src.tile(word)) for word in bank_zero), style

    tiles = [base.tile(i) for i in range(512)]
    pool = InlayTiles(tiles, HOME_ART, 0)
    floor = base.meta[1][0x0A * 8:0x0A * 8 + 4]
    assert base.attrs[1][0x0A] == 0
    metas = base.meta[0] + [word for art in pool.pieces(home) for word in floor + art]
    attrs = base.attrs[0] + [0x1000] * 9
    assert len(base.attrs[0]) == 2
    outputs = {
        f'{HOME}/tiles.png': png(tiles),
        f'{HOME}/metatiles.bin': packed(metas),
        f'{HOME}/metatile_attributes.bin': packed(attrs),
        f'{HOME}/palettes/00.pal': palette_bytes(base.pal(0)[:1] + colors[1:]),
    }
    for bank in range(1, 6):
        path = f'palettes/{bank:02}.pal'
        outputs[f'{HOME}/{path}'] = (ROOT / base.paths[0] / path).read_bytes()
    return outputs


def trial_assets(colors):
    base = Source('trial_architecture', 'trial_equipment')
    original_metas = base.meta[1][:11 * 8]
    original_attrs = base.attrs[1][:11]
    assert len(base.attrs[1]) in (11, 20) or len(base.attrs[1]) >= 23
    later_metas = base.meta[1][23 * 8:]
    later_attrs = base.attrs[1][23:]
    existing_words = base.meta[0] + original_metas + later_metas
    assert not set(TRIAL_ART) & {word & 1023 for word in existing_words}
    assert not any(word >> 12 == 12 for word in existing_words)
    tiles = [bytes(64)] * 512 + [base.tile(512 + i) for i in range(512)]
    for idx in TRIAL_ART:
        tiles[idx] = bytes(64)
    pool = InlayTiles(tiles, TRIAL_ART, 12)
    assert original_attrs[1] == 0
    floor = original_metas[8:12]
    metas = original_metas + [word for art in pool.pieces(trial, center_on_seam=True) for word in floor + art]
    return {
        f'{TRIAL}/tiles.png': png(tiles[512:]),
        f'{TRIAL}/metatiles.bin': packed(metas + later_metas),
        f'{TRIAL}/metatile_attributes.bin': packed(original_attrs + [0x1000] * 12 + later_attrs),
        f'{TRIAL}/palettes/12.pal': palette_bytes(colors),
    }


def metatiles(cat, name):
    if name == 'Home':
        return list(range(2, 11))
    if name == 'Trial':
        return list(range(523, 535))
    catalogue = {'Berry': cat.berry_lab, 'Market': cat.market}.get(name, cat.medallions)
    prefix = 'Inlay' if name in ('Berry', 'Market') else name
    return [catalogue[f'{prefix}_{x}_{y}'] for y in range(3) for x in range(3)]


def preview_and_check(cat):
    layouts = {l['id']: l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
    home_src = Source('home_architecture', 'secret_base/red_cave', metatiles=HOME_METAS)
    trial_src = Source('trial_architecture', 'trial_equipment')
    sheet = Image.new('RGB', (4 * 252, 3 * 190), '#141c22')
    labels = ImageDraw.Draw(sheet)

    for index, (name, room, symbol, (x, y)) in enumerate(PLACEMENTS):
        map_data = json.loads((ROOT/f'data/maps/{room}/map.json').read_text())
        layout = layouts[map_data['layout']]
        width, height = layout['width'], layout['height']
        grid = words(layout['blockdata_filepath'])
        art = metatiles(cat, symbol)
        source = {'Home': home_src, 'Trial': trial_src}.get(symbol)
        columns = 4 if symbol == 'Trial' else 3
        left = x - columns // 2
        if symbol == 'Trial':
            # The building uses two adjacent entrance tiles. The center of
            # the stamp is their shared edge, at x tiles from the left wall.
            doors = map_data['warp_events'][8:10]
            assert [door['x'] for door in doors] == [x - 1, x]
        for j, mid in enumerate(art):
            word = grid[(y - 1 + j // columns) * width + left + j % columns]
            assert word & 1023 == mid and not word & 0xC00, (name, x, y, j)
            attrs = source.attrs[mid // 512][mid % 512] if source else cat.attrs[mid]
            assert attrs == 0x1000, (name, mid)
        map_image = Image.new('RGB', (width * 16, height * 16))
        render_tile = (lambda mid: render(source, mid)) if source else cat.render
        cache = {v & 1023: render_tile(v & 1023) for v in grid}
        for tile_index, word in enumerate(grid):
            map_image.paste(cache[word & 1023], (tile_index % width * 16, tile_index // width * 16))
        # Frame the same 240-by-160-pixel view on each inlay.
        cx, cy = left * 16 + columns * 8, y * 16 + 8
        view = map_image.crop((cx - 120, cy - 80, cx + 120, cy + 80))
        ox, oy = index % 4 * 252 + 6, index // 4 * 190
        labels.text((ox, oy + 6), name, fill='white')
        sheet.paste(view, (ox, oy + 22))
        if symbol == 'Trial':
            trial_view = view

    out = ROOT / 'build/hub_medallions'
    out.mkdir(parents=True, exist_ok=True)
    sheet.save(out / 'installed.png')
    trial_view.resize((480, 320), Image.Resampling.NEAREST).save(out / 'trial-stairs.png')
    print(f'Hub medallions: {len(PLACEMENTS)} walkable floor stamps verified; preview: {out / "installed.png"}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    cat = Catalogue()
    colors = cat.pals[SHARED_BANK]
    for filename, data in {**home_assets(colors), **trial_assets(colors)}.items():
        path = ROOT / filename
        if args.check:
            assert path.read_bytes() == data, f'Stale medallion asset: {filename}'
        elif not path.exists() or path.read_bytes() != data:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
    preview_and_check(cat)


if __name__ == '__main__':
    main()
