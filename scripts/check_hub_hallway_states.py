#!/usr/bin/env python3
"""Check authored hub hallways against the actual C closure patches; write previews only."""
import itertools
import json
import re
from collections import deque
from PIL import Image, ImageDraw
from generate_portal_room import ROOT, Source, words
from generate_hub_tiles import render

# Room geometry and expected architectural seams, independent of runtime patches.
# Each exit specifies its warp center. South specifies the first closed row and
# corridor sides; side exits specify the wall x and rows that become a pillar.
ROOMS = {
    'AdventureEntrance': ('AdventureEntrance', 'PortalRoom', (9, 10), {
        'WEST': ((0, 11), (2, 8, 11)), 'EAST': ((18, 11), (16, 8, 11)),
        'SOUTH': ((9, 15), (7, 11, 12))}),
    'Labs': ('Labs', 'LabJunction', (14, 9), {
        'WEST': ((2, 9), (4, 5, 11)), 'EAST': ((26, 9), (24, 5, 11)),
        'NORTH': ((14, 1), (12, 16)), 'SOUTH': ((14, 20), (12, 16, 15))}),
    'SafariZone': ('Safari', 'SafariLab', (18, 18), {
        'WEST': ((4, 15), (6, 11, 17)), 'EAST': ((32, 15), (30, 11, 17)),
        'SOUTH': ((18, 27), (16, 20, 22))}),
    'SafariZone_Tutorial': ('Safari', 'SafariLab', (18, 18), {
        'WEST': ((4, 15), (6, 11, 17)), 'EAST': ((32, 15), (30, 11, 17)),
        'SOUTH': ((18, 27), (16, 20, 22))}),
    'TownSquare': ('TownSquare', 'MainHall', (18, 18), {
        'WEST': ((1, 17), (3, 14, 19)), 'EAST': ((35, 17), (33, 14, 19)),
        'NORTH': ((18, 1), (16, 20)), 'SOUTH': ((18, 28), (16, 20, 25))}),
}
VECTORS = {'WEST': (-1, 0), 'EAST': (1, 0), 'NORTH': (0, -1), 'SOUTH': (0, 1)}


def check_states():
    code = (ROOT/'src/rogue_hub.c').read_text()
    labels = {}
    for filename in ('portal_room', 'lab_junction', 'safari_lab', 'main_hall'):
        labels.update({n: int(v, 16) for n, v in re.findall(
            r'#define (METATILE_\w+) (0x[0-9A-Fa-f]+)',
            (ROOT/f'include/constants/{filename}_tiles.h').read_text())})
    source = Source('hub_architecture', 'hub_furnishings')
    behaviors = {n: int(v, 16) for n, v in re.findall(r'#define (MB_\w+) (0x[0-9A-Fa-f]+)',
        (ROOT/'include/constants/metatile_behaviors.h').read_text())}
    def behavior(cell):
        mid = cell & 1023
        return source.attrs[mid//512][mid%512] & 255
    images = {}
    def picture(mid):
        if mid not in images: images[mid] = render(source, mid)
        return images[mid]
    def signature(cell):
        return (cell & 0xFC00, picture(cell & 1023).tobytes())
    layouts = {l['name']: l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
    out = ROOT/'build/hub_tiles/states'; out.mkdir(parents=True, exist_ok=True)
    failures = set(); count = 0
    for room, (function, prefix, anchor, exits) in ROOMS.items():
        name = 'Rogue_Area_' + room
        layout = layouts[name + '_Layout']; w, h = layout['width'], layout['height']
        base = words(layout['blockdata_filepath'])
        events = json.loads((ROOT/f'data/maps/{name}/map.json').read_text())
        body = code.split(f'static void RogueHub_Update{function}AreaMetatiles()\n{{', 1)[1].split('\n}\n', 1)[0]
        patches = dict(re.findall(r'HUB_AREA_CONN_(\w+)\) == HUB_AREA_NONE\)\s*\{([^}]+)\}', body))
        assert set(patches) == set(exits), (room, 'Missing closure')
        sheet = Image.new('RGB', (w*16*4, (h*16+20)*((2**len(exits)+3)//4)))
        draw = ImageDraw.Draw(sheet)
        for index, opened in enumerate(itertools.product((False, True), repeat=len(exits))):
            state = dict(zip(exits, opened)); a = base[:]
            for direction in exits:
                if state[direction]: continue
                patch = re.sub(r'//[^\n]*', '', patches[direction])
                calls = re.findall(r'Metatile(Set|Fill)_Tile\(([^;]+)\);', patch)
                assert calls and not re.sub(r'Metatile(?:Set|Fill)_Tile\([^;]+\);', '', patch).strip(), (room, 'Unsupported closure code')
                for kind, args in calls:
                    args = [v.strip() for v in args.split(',')]; coords = list(map(int, args[:-1]))
                    tile = labels[args[-1].split(' | ')[0]] | (0xC00 if 'MAPGRID_COLLISION_MASK' in args[-1] else 0)
                    x1, y1 = coords[:2]; x2, y2 = coords[2:] if kind == 'Fill' else coords[:2]
                    assert 0 <= x1 <= x2 < w and 0 <= y1 <= y2 < h
                    for y in range(y1, y2+1):
                        for x in range(x1, x2+1): a[y*w+x] = (a[y*w+x] & 0xF000) | tile
            seen = {anchor}; queue = deque(seen)
            while queue:
                x, y = queue.popleft()
                for dx, dy in VECTORS.values():
                    p = x+dx, y+dy
                    if 0 <= p[0] < w and 0 <= p[1] < h and p not in seen and not a[p[1]*w+p[0]] & 0xC00:
                        seen.add(p); queue.append(p)
            for direction, ((cx, cy), geometry) in exits.items():
                dx, dy = VECTORS[direction]
                lanes = [(cx+(i if dy else 0), cy+(i if dx else 0)) for i in (-1, 0, 1)]
                for x, y in lanes:
                    if ((x, y) in seen) != state[direction]: failures.add((room, direction, 'warp reachability', x, y))
                    assert any(e['x'] == x and e['y'] == y for e in events['warp_events']), (room, direction, 'Missing warp lane', x, y)
                    if state[direction]:
                        assert a[y*w+x] == base[y*w+x] and (x-dx, y-dy) in seen, (room, direction, 'Blocked arrival')
                        assert behavior(a[y*w+x]) == behaviors[f'MB_{direction}_ARROW_WARP'], (room, direction, 'Wrong warp behavior', x, y)
                        assert behavior(a[(y-dy)*w+x-dx]) == behaviors['MB_NORMAL'], (room, direction, 'Arrival still on warp')
                    else:
                        assert behavior(a[y*w+x]) == behaviors['MB_NORMAL'], (room, direction, 'Closed warp still active')
                expected = {}
                def expect(x, y, tile, solid=True):
                    expected[x, y] = (base[y*w+x] & 0xF000) | labels[f'METATILE_{prefix}_{tile}'] | (0xC00 if solid else 0)
                if not state[direction]:
                    if direction == 'SOUTH':
                        x1, x2, row = geometry
                        for y in range(row, h):
                            for x in range(x1, x2+1):
                                # The authored floor/void edge beside the opening is the reference.
                                expected[x, y] = base[y*w+x1-2]
                    elif direction == 'NORTH':
                        x1, x2 = geometry
                        for y, tile in enumerate(('Void', 'Void', 'Void', 'Wall', 'WallBase', 'FloorShadow')):
                            for x in range(x1, x2+1): expect(x, y, tile, y != 5)
                    else:
                        wall, y1, y2 = geometry
                        for y in range(y1, y2+1): expect(wall, y, 'Pillar')
                        if room == 'AdventureEntrance':
                            expect(wall, 12, 'PillarEnd')
                            for y in range(13, h): expect(wall, y, 'Void')
                        # No remaining facade artwork or traversable space outside a closed side wall.
                        outer = range(wall) if direction == 'WEST' else range(wall+1, w)
                        start = cy-3
                        for y in range(start, min(start+8, h)):
                            for x in outer: expect(x, y, 'Void')
                    for (x, y), value in expected.items():
                        if signature(a[y*w+x]) != signature(value): failures.add((room, direction, 'closed seam', x, y))
                else:
                    # All three lanes must reach the map edge through unchanged open artwork.
                    for x, y in lanes:
                        while 0 <= x < w and 0 <= y < h:
                            assert (x, y) in seen and a[y*w+x] == base[y*w+x], (room, direction, 'Open corridor changed', x, y)
                            x += dx; y += dy
            ox, oy = index%4*w*16, index//4*(h*16+20)
            draw.text((ox+2, oy+2), 'Open: '+(','.join(d[0] for d in exits if state[d]) or 'none'), fill='white')
            for i, cell in enumerate(a): sheet.paste(picture(cell & 1023), (ox+i%w*16, oy+20+i//w*16))
            count += 1
        sheet.save(out/(name+'.png'))
    for failure in sorted(failures): print('FAIL:', *failure)
    assert not failures, f'{len(failures)} hallway seam/reachability errors; previews: {out}'
    print(f'Hub hallway states: {count} connection combinations across {len(ROOMS)} maps PASS. Previews: {out}')


if __name__ == '__main__':
    check_states()
