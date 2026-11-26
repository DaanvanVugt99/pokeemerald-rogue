#!/usr/bin/env python3
"""Check the authored Market and generate only its private corridor facades."""
import argparse
from collections import deque
import itertools
import json
import re

from PIL import Image
from generate_hub_tiles import Catalogue
from generate_portal_room import ROOT, packed, words

MARKET = 'Rogue_Area_Market'
DIRECTIONS = ('NORTH', 'EAST', 'SOUTH', 'WEST')
STEPS = ((0, -1), (1, 0), (0, 1), (-1, 0))
LANES = ((0, 1, 10), (2, 3, 11), (4, 5, 12), (6, 7, 13))


def read(path):
    return json.loads((ROOT / path).read_text())


def point(event):
    return event['x'], event['y']


def reachable(start, blocked, width, height):
    seen = {start}
    pending = deque(seen)
    while pending:
        x, y = pending.popleft()
        for dx, dy in STEPS:
            neighbor = x + dx, y + dy
            nx, ny = neighbor
            if 0 <= nx < width and 0 <= ny < height and neighbor not in seen and neighbor not in blocked:
                seen.add(neighbor)
                pending.append(neighbor)
    return seen


def runtime_patches():
    """Read the collision-changing patches used by the ROM."""
    source = (ROOT / 'src/rogue_hub.c').read_text().split(
        'static void RogueHub_UpdateMarketAreaMetatiles()\n{', 1)[1].split('\n}\n', 1)[0]
    patches = {}
    for condition, body in re.findall(
        r'if \(([^\n]+)\)\n(.*?)(?=\n    if |\Z)', source, re.S
    ):
        key = re.search(r'(?:HUB_AREA_CONN_|HUB_UPGRADE_MARKET_)(\w+)', condition)[1]
        blocked = set()
        for args in re.findall(r'Metatile(?:Fill|Set)_Tile\(([^)]+)\)', body):
            *coords, tile = args.split(', ')
            if 'MAPGRID_COLLISION_MASK' in tile:
                x1, y1, x2, y2 = map(int, coords if len(coords) == 4 else coords * 2)
                blocked.update(itertools.product(range(x1, x2 + 1), range(y1, y2 + 1)))
        patches[key] = blocked
    return patches


def movement(scripts, name, start):
    body = scripts.split(f'movement Movement_Market{name}\n{{', 1)[1].split('}', 1)[0]
    x, y = start
    path = [start]
    for direction, count in re.findall(r'walk_faster_(up|right|down|left)(?: \* (\d+))?', body):
        dx, dy = STEPS[('up', 'right', 'down', 'left').index(direction)]
        for _ in range(int(count or 1)):
            x, y = x + dx, y + dy
            path.append((x, y))
    return path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', help='Compare generated facades without writing them')
    args = parser.parse_args()
    cat = Catalogue()
    layouts = {row['id']: row for row in read('data/layouts/layouts.json')['layouts']}
    market = read(f'data/maps/{MARKET}/map.json')
    layout = layouts[market['layout']]
    width, height = layout['width'], layout['height']
    assert market['map_type'] == 'MAP_TYPE_INDOOR'
    assert (layout['primary_tileset'], layout['secondary_tileset']) == (
        'gTileset_HubArchitecture', 'gTileset_HubFurnishings')
    base = words(ROOT / layout['blockdata_filepath'])
    assert len(base) == width * height
    collision = {(i % width, i // width) for i, tile in enumerate(base) if tile & 0xC00}
    warps = market['warp_events']
    assert len(warps) == 14 and all(warp['dest_map'] == market['id'] for warp in warps)
    assert point(warps[8]) in collision
    arrival = point(warps[9])
    assert arrival not in collision
    lane_tiles = [{point(warps[i]) for i in lanes} for lanes in LANES]
    expected_behaviors = (0x64, 0x62, 0x65, 0x63)
    for lanes, behavior in zip(lane_tiles, expected_behaviors):
        assert len(lanes) == 3 and not lanes & collision
        for x, y in lanes:
            assert cat.attrs[base[y * width + x] & 1023] & 255 == behavior, (x, y)

    outputs = {}
    for connection in market['connections']:
        suffix = connection['map'].rsplit('_', 1)[1].title()
        facade = read(f'data/maps/Rogue_MarketCorridor_{suffix}/map.json')
        assert facade['id'] == connection['map']
        target = layouts[facade['layout']]
        fw, fh = target['width'], target['height']
        direction = connection['direction']
        offset = connection['offset']
        if direction in ('left', 'right'):
            x = 0 if direction == 'left' else width - 1
            profile = [base[(offset + y) * width + x] for y in range(fh)]
            cells = [profile[y] for y in range(fh) for _ in range(fw)]
        else:
            y = 0 if direction == 'up' else height - 1
            profile = base[y * width + offset:y * width + offset + fw]
            cells = profile * fh
        path = ROOT / target['blockdata_filepath']
        data = packed(cells)
        assert outputs.setdefault(path, data) == data, 'Shared facade has inconsistent seams'

    patches = runtime_patches()
    objects = market['object_events']
    # Check the worst case with all authored templates, including the staged pie.
    assert len(objects) + 2 <= 16, 'Object event capacity exceeded'
    always = {'0'}
    for links, bank, bakery, shop in itertools.product(range(16), (False, True), (False, True), range(3)):
        open_flags = always | ({'FLAG_TEMP_1'} if bank else set()) | (
            {'FLAG_TEMP_2'} if shop else set()) | ({'FLAG_TEMP_3'} if bakery else set())
        active = [e for e in objects if e['flag'] in open_flags]
        occupied = {point(e) for e in active}
        blocked = collision.copy()
        for index, direction in enumerate(DIRECTIONS):
            if not links & (1 << index):
                blocked.update(patches[direction])
        for unlocked, key in ((bank, 'BANK'), (bakery, 'BAKERY'), (shop, 'TREAT_SHOP')):
            if not unlocked:
                blocked.update(patches[key])
        assert arrival not in blocked | occupied
        seen = reachable(arrival, blocked | occupied, width, height)
        for index, lanes in enumerate(lane_tiles):
            if links & (1 << index):
                assert lanes <= seen
            else:
                assert lanes <= blocked
        for event in active + market['bg_events']:
            x, y = point(event)
            approaches = {(x + dx, y + dy) for dx, dy in STEPS} & seen
            assert approaches, event
            if event['script'].endswith('Chef'):
                assert approaches == {(x, y + 1)}, event
        assert (23, 21) in seen, 'Restaurant recovery position is obstructed'

    scripts = (ROOT / f'data/maps/{MARKET}/scripts.pory').read_text()
    staff = {point(event) for event in objects[:-1]}
    for kind, x in (('Normal', 25), ('Shiny', 31)):
        path = movement(scripts, kind + 'ToTable', (x, 20))
        back = movement(scripts, kind + 'ToCounter', path[-1])
        assert path[-1] == (28, 24) and path[-2] == (28, 23)
        assert back == path[::-1]
        assert not set(path) & (collision | staff | {(31, 24), point(objects[-1])})

    for path, data in outputs.items():
        if args.check:
            assert path.read_bytes() == data, f'Stale Market facade {path}'
        elif not path.exists() or path.read_bytes() != data:
            path.write_bytes(data)
    preview = Image.new('RGB', (width * 16, height * 16))
    tiles = {tile & 1023: cat.render(tile & 1023) for tile in set(base)}
    for i, tile in enumerate(base):
        preview.paste(tiles[tile & 1023], (i % width * 16, i // width * 16))
    out = ROOT / 'build/market'
    out.mkdir(parents=True, exist_ok=True)
    preview.save(out / 'room.png')
    # Check the kitchen composition with sprites at the GBA camera size.
    kitchen = preview.crop((21 * 16, 16 * 16, 36 * 16, 26 * 16))
    sprites = [(point(e), 'people/cook', 0) for e in objects if e['script'].endswith('Chef')]
    sprites.append(((25, 20), 'people/brendan/walking', 1))
    for (x, y), name, frame in sorted(sprites, key=lambda entry: entry[0][1]):
        with Image.open(ROOT / f'graphics/object_events/pics/{name}.png') as sheet:
            sprite = sheet.crop((frame * 16, 0, frame * 16 + 16, 32))
            sprite.info['transparency'] = 0
            rgba = sprite.convert('RGBA')
            kitchen.paste(rgba, ((x - 21) * 16, (y - 16) * 16 - 16), rgba)
    kitchen.save(out / 'kitchen-240x160.png')
    print('Market PASS: 192 connection and service states, chef routes, facade seams and object budget')


if __name__ == '__main__':
    main()
