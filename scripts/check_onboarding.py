#!/usr/bin/env python3
"""Check authored onboarding routes without modifying maps or generated assets."""
import json
import re
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LAYOUTS = {x['id']: x for x in json.loads((ROOT / 'data/layouts/layouts.json').read_text())['layouts']}
DELTAS = {'right': (1, 0), 'left': (-1, 0), 'up': (0, -1), 'down': (0, 1)}


def load(name):
    events = json.loads((ROOT / f'data/maps/{name}/map.json').read_text())
    layout = LAYOUTS[events['layout']]
    data = (ROOT / layout['blockdata_filepath']).read_bytes()
    return events, layout, struct.unpack('<' + 'H' * (len(data) // 2), data)


def walk(name, movement, start, blocked=()):
    _, layout, cells = load(name)
    w, h = layout['width'], layout['height']
    text = (ROOT / f'data/maps/{name}/scripts.pory').read_text(encoding='utf-8')
    body = re.search(r'movement\s+' + movement + r'\s*\{([^}]+)\}', text).group(1)
    x, y = start
    elevation = cells[y * w + x] >> 12
    for direction, count in re.findall(r'walk_(?:fast_)?(right|left|up|down)(?:\s*\*\s*(\d+))?', body):
        dx, dy = DELTAS[direction]
        for _ in range(int(count or 1)):
            x, y = x + dx, y + dy
            assert 0 <= x < w and 0 <= y < h, (movement, 'outside map', x, y)
            tile = cells[y * w + x]
            assert not tile & 0xC00 and (x, y) not in blocked, (movement, 'blocked', x, y)
            assert tile >> 12 in (0, elevation), (movement, 'elevation', x, y)
    return (x, y), body


def main():
    name = 'Rogue_Area_AdventureEntrance'
    events, layout, cells = load(name)
    nurse = next(x for x in events['object_events'] if x['script'].endswith('SpeakNurse'))
    board = next(x for x in events['object_events'] if x['script'] == 'Rogue_InteractWithQuestBoard')
    blocked = {(x['x'], x['y']) for x in events['object_events']}
    approaches = {(nurse['x'] + dx, nurse['y'] + dy) for dx, dy in DELTAS.values()
                  if not cells[(nurse['y'] + dy) * layout['width'] + nurse['x'] + dx] & 0xC00
                  and (nurse['x'] + dx, nurse['y'] + dy) not in blocked}
    starts = {'East': (11, 6), 'South': (10, 7)}
    assert approaches == set(starts.values()), ('Uncovered nurse approach', approaches)
    for side, start in starts.items():
        end, body = walk(name, name + '_BoardFrom' + side, start, blocked)
        assert end == (board['x'], board['y'] + 1)
        assert body.strip().endswith('face_up')
    assert walk(name, 'Rogue_Adventure_WalkIntoPortal', (9, 7), blocked)[0] == (9, 3)

    # The initial door choreography has fixed authored positions. Corridor
    # facade/connection combinations are checked by check_hub_corridors.py.
    labs = 'Rogue_Area_Labs'
    assert walk(labs, 'BirchStartGame1', (8, 7), {(19, 10), (9, 7)})[0] == (9, 5)
    assert walk(labs, 'BirchStartGame2', (9, 5), {(19, 10)})[0] == (9, 4)
    for direction, end in [('North', (14, 1)), ('East', (26, 9)), ('South', (14, 20)), ('West', (2, 9))]:
        assert walk(labs, 'BirchTutorialGuide' + direction, (9, 6), {(19, 10), (9, 5)})[0] == end
    lab = 'Rogue_Interior_ProfLab'
    for index, start in enumerate([(2, 7), (3, 7), (4, 7)]):
        assert walk(lab, 'PlayerIntro' + str(index), start, {(3, 4), (7, 10)})[0] == (3, 5)
    assert walk(lab, 'IntroAssistant0', (7, 10), {(3, 4), (3, 5)})[0] == (3, 6)
    assert walk(lab, 'IntroAssistant1', (3, 6), {(3, 4), (3, 5)})[0] == (4, 6)
    hall = 'Rogue_Area_TownSquare'
    events, _, _ = load(hall)
    bench = next(x for x in events['object_events'] if x['graphics_id'] == 'OBJ_EVENT_GFX_WORK_TABLE')
    bx, by = bench['x'], bench['y']
    # PrepareMainHallBuilder places him one tile above the authored workbench.
    assert walk(hall, hall + '_WorkbenchBuilder_North', (bx, by - 1), {(bx, by), (bx, by - 2)})[0] == (bx + 1, by - 1)
    assert walk(hall, hall + '_WorkbenchPlayer_North', (bx, by - 2), {(bx, by), (bx + 1, by - 1)})[0] == (bx, by - 1)
    print('Onboarding routes PASS: both nurse approaches, portal, Birch junction paths, lab approaches, assistant and builder; collision, elevation and static blockers.')


if __name__ == '__main__':
    main()
