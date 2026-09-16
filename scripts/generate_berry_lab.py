#!/usr/bin/env python3
"""Verify/preview the Porymap-authored Berry Lab; only private facades are generated.

--init-layout is a guarded, one-time outdoor-field conversion. Never regenerate
an already converted layout. Normal generation preserves the room and border.
"""
import argparse
import itertools
import json
import re
from collections import deque
from PIL import Image
from generate_portal_room import ROOT, words, packed
from generate_hub_tiles import Catalogue

ROOM = 'Rogue_Area_FarmingField'
OUT = ROOT/'build/berry_lab'
W, H = 29, 28
OLD_W, OLD_H = 40, 25
# Clockwise, retaining the historical berry tree IDs and upgrade flags.
BEDS = [(7,7,11,'0'), (17,7,6,'FLAG_TEMP_1'),
        (17,22,1,'FLAG_TEMP_2'), (7,22,16,'FLAG_TEMP_3')]
# The table is the single shared furnishing retained in the current room.
# Coordinates are the prefab's top-left tile.
DECORATIONS = [
    ('Hub / Table and four stools', 8, 11),
]
DIRECTIONS = ['NORTH','EAST','SOUTH','WEST']
VECTORS = [(0,-1),(1,0),(0,1),(-1,0)]
CENTERS = [(14,1),(26,11),(14,26),(2,11)]


def tiles(cat):
    def old(key,name): return cat.mapping[key][cat.config['rooms'][key]['names'][name]]
    names = {n:old('lab_junction',n) for n in ('Floor','FloorShadow','Void','Pillar','PillarCap','PillarEnd',
        'PillarJoin','InnerJoin','InnerBase','InnerFoot','Wall','WallBase','FloorShadowFadeWest','FloorShadowFadeEast','FloorFadeWest','FloorFadeEast','FloorFadeNorth','FloorFadeSouth',
        'FloorDim40','FloorDim20','FloorShadowDim20','WarpWest','WarpEast','WarpNorth','WarpSouth',
        'WarpWestUpper','WarpEastUpper')}
    names.update(cat.berry_lab)
    return names


def apply_decorations(a):
    """Stamp the shared lab furnishings into the authored room."""
    prefabs = {p['name']: p for p in json.loads((ROOT/'data/tilesets/hub_prefabs.json').read_text())}
    for name, x, y in DECORATIONS:
        for cell in prefabs[name]['metatiles']:
            a[(y + cell['y']) * W + x + cell['x']] = cell['metatile_id'] | 0x3C00


def initialize(cat):
    path = ROOT/'data/layouts/layouts.json'; registry = json.loads(path.read_text())
    layout = next(l for l in registry['layouts'] if l['id']=='LAYOUT_ROGUE_AREA_FARMING_FIELD')
    assert (layout['width'],layout['height'],layout['primary_tileset']) == (40,12,'gTileset_GeneralHub'), 'Already converted; edit the room in Porymap'
    OUT.mkdir(parents=True,exist_ok=True)
    for f in ('map.bin','border.bin'):
        (OUT/('original-'+f)).write_bytes((ROOT/f'data/layouts/{ROOM}/{f}').read_bytes())
    t=tiles(cat); a=[t['Void']|0x3C00]*(W*H)
    def fill(x1,y1,x2,y2,n,solid=False):
        v=t[n] if isinstance(n,str) else n
        for y in range(y1,y2+1):
            for x in range(x1,x2+1):a[y*W+x]=v|0x3000|(0xC00 if solid else 0)
    fill(5,5,23,24,'Floor')
    fill(4,3,24,3,'Wall',True);fill(5,4,23,4,'WallBase',True)
    for x in (4,24):
        fill(x,4,x,20,'Pillar',True);fill(x,21,x,21,'PillarEnd',True)
    # Main crossing and three-wide exterior fade strips.
    fill(0,10,28,12,'Floor');fill(13,0,15,27,'Floor')
    for x,n in [(0,'FloorDim20'),(1,'FloorDim20'),(2,'WarpWest'),(3,'FloorFadeWest'),
                (25,'FloorFadeEast'),(26,'WarpEast'),(27,'FloorDim20'),(28,'FloorDim20')]:
        fill(x,10,x,12,n)
    for y,n in [(0,'FloorDim20'),(1,'WarpNorth'),(2,'FloorFadeNorth'),
                (25,'FloorFadeSouth'),(26,'WarpSouth'),(27,'FloorDim20')]:fill(13,y,15,y,n)
    # Side upper lane keeps the same terminal shade as the shared facade rules.
    fill(0,10,1,10,'FloorShadowDim20');fill(27,10,28,10,'FloorShadowDim20')
    fill(2,10,2,10,'WarpWestUpper');fill(26,10,26,10,'WarpEastUpper')
    def habitat(x,y):
        # Stretch the back pane of the existing glass habitat to seven tiles.
        for i,sx in enumerate((0,1,2,3,4,4,5)):
            mid=cat.berry_lab[f'GlassBack{sx}']
            fill(x+i,y,x+i,y,mid,True)
    for x,y,_,_ in BEDS:
        habitat(x-1,y-1)
        fill(x,y,x+4,y,'Soil',True)
        # Low glass side posts frame each open-front growing bay.
        for px,sx in ((x-1,0),(x+5,5)):
            mid=cat.mapping['safari_lab'][cat.config['rooms']['safari_lab']['names'][f'Habitat_{sx}_1']]
            fill(px,y,px,y,mid,True)
    plant=next(p for p in json.loads((ROOT/'data/tilesets/hub_prefabs.json').read_text()) if p['name']=='Hub / Plant')
    for x,y in [(5,5),(23,5),(5,21),(23,21),(17,19),(22,19)]:
        for cell in plant['metatiles']:
            fill(x+cell['x'],y+cell['y'],x+cell['x'],y+cell['y'],cell['metatile_id'],True)
    # Finish the initial assembly using the same wall grammar as Research Junction.
    # This runs only during the guarded first conversion, never normal generation.
    def put(x,y,n,solid=True):fill(x,y,x,y,n,solid)
    for x,y in [(5,5),(23,5),(5,21),(23,21),(17,19),(22,19)]:fill(x,y,x,y+1,'Floor')
    fill(4,2,24,5,'Void',True)
    fill(5,3,23,3,'Wall',True);fill(5,4,23,4,'WallBase',True);fill(5,5,23,5,'FloorShadow')
    for x in (4,24):
        fill(x,2,x,24,'Pillar',True)
        for y,n in [(2,'PillarCap'),(3,'PillarJoin'),(8,'InnerJoin'),(9,'InnerBase'),(10,'InnerFoot'),(13,'PillarCap'),(14,'PillarJoin'),(24,'PillarEnd')]:put(x,y,n,y!=10)
        fill(x,11,x,12,'Floor')
    for x in (12,16):
        fill(x,0,x,2,'Pillar',True)
        for y,n in [(3,'InnerJoin'),(4,'InnerBase'),(5,'InnerFoot'),(24,'PillarCap'),(25,'PillarJoin'),(26,'Pillar'),(27,'Pillar')]:put(x,y,n,y!=5)
    fill(13,0,15,5,'Floor')
    for y,n in [(0,'FloorDim20'),(1,'WarpNorth'),(2,'FloorFadeNorth')]:fill(13,y,15,y,n)
    for x1,x2 in [(0,3),(25,28)]:fill(x1,8,x2,8,'Wall',True);fill(x1,9,x2,9,'WallBase',True)
    fill(5,6,5,9,'FloorShadow');fill(5,13,5,20,'FloorShadow')
    put(3,10,'FloorShadowFadeWest',False);put(25,10,'FloorShadowFadeEast',False)
    for x,y,_,_ in BEDS:
        fill(x,y,x+4,y,'SoilRim',True);put(x-1,y,'BedEndLeft');put(x+5,y,'BedEndRight')
    for x in (6,11,17,22):put(x,4,111);put(x,5,'PlantBaseShadow')
    for x in (7,8,9,19,20,21):put(x,4,'GrowLight')
    for x,y in [(5,7),(23,7),(5,21),(23,21),(16,23),(22,23),(17,19),(22,19)]:put(x,y,'PlantTopShadow' if x==5 else 'PlantTop');put(x,y+1,'PlantBaseShadow' if x==5 else 'PlantBase')
    for x in range(18,22):put(x,18,'SeedTray');put(x,19,'PropagationBase')
    for y in range(3):
        for x in range(2):put(5+x,17+y,cat.mapping['lab_junction'][cat.config['rooms']['lab_junction']['names'][f'Decor_Cargo_{x}_{y}']])
    apply_decorations(a)
    (ROOT/layout['blockdata_filepath']).write_bytes(packed(a))
    (ROOT/layout['border_filepath']).write_bytes(packed([t['Void']|0x3C00]*4))
    layout.update(width=W,height=H,primary_tileset='gTileset_HubArchitecture',secondary_tileset='gTileset_HubFurnishings')
    mpath=ROOT/f'data/maps/{ROOM}/map.json';m=json.loads(mpath.read_text())
    m['map_type']='MAP_TYPE_INDOOR'
    for e in m['object_events']:
        ident=e['trainer_sight_or_berry_tree_id']
        if ident.startswith('BERRY_TREE_HUB_'):
            bid=int(ident.rsplit('_',1)[1]);x,y,start,flag=next(b for b in BEDS if b[2]<=bid<b[2]+5)
            e.update(x=x+bid-start,y=y,elevation=3)
        elif e['graphics_id']=='OBJ_EVENT_GFX_WORK_TABLE': e.update(x=20,y=24,elevation=3)
        elif e['graphics_id']=='OBJ_EVENT_GFX_LINK_RECEPTIONIST': e.update(x=14,y=11,elevation=3,movement_type='MOVEMENT_TYPE_FACE_DOWN')
        else: e.update(x=15,y=11,elevation=3)
    warp=lambda x,y:dict(x=x,y=y,elevation=0,dest_map='MAP_ROGUE_AREA_FARMING_FIELD',dest_warp_id='0')
    m['warp_events']=[warp(14,1),warp(15,1),warp(26,11),warp(26,12),warp(14,26),warp(15,26),warp(2,11),warp(2,12),warp(14,18),warp(13,1),warp(26,10),warp(13,26),warp(2,10)]
    groups_path=ROOT/'data/maps/map_groups.json';groups=json.loads(groups_path.read_text())
    m['connections']=[]
    for suffix,direction,offset,cw,ch in [('Horizontal','left',8,16,8),('Horizontal','right',8,16,8),('North','up',11,8,12),('South','down',11,8,12)]:
        name='Rogue_BerryLabCorridor_'+suffix; lid='LAYOUT_ROGUE_BERRY_LAB_CORRIDOR_'+suffix.upper();mid='MAP_ROGUE_BERRY_LAB_CORRIDOR_'+suffix.upper()
        m['connections'].append(dict(map=mid,offset=offset,direction=direction))
        if any(l['id']==lid for l in registry['layouts']):continue
        registry['layouts'].append(dict(id=lid,name=name+'_Layout',width=cw,height=ch,primary_tileset='gTileset_HubArchitecture',secondary_tileset='gTileset_HubFurnishings',border_filepath=f'data/layouts/{name}/border.bin',blockdata_filepath=f'data/layouts/{name}/map.bin'))
        folder=ROOT/f'data/layouts/{name}';folder.mkdir(parents=True,exist_ok=True);(folder/'border.bin').write_bytes(packed([t['Void']|0x3C00]*4))
        folder=ROOT/f'data/maps/{name}';folder.mkdir(parents=True,exist_ok=True)
        facade=json.loads((ROOT/'data/maps/Rogue_PortalCorridor_Horizontal/map.json').read_text());facade.update(id=mid,name=name,layout=lid)
        (folder/'map.json').write_text(json.dumps(facade,indent=2)+'\n');(folder/'scripts.pory').write_text(f'mapscripts {name}_MapScripts {{}}\n')
        groups['gMapGroup_RogueHubAreas'].append(name)
    path.write_text(json.dumps(registry,indent=2)+'\n');groups_path.write_text(json.dumps(groups,indent=2)+'\n');mpath.write_text(json.dumps(m,indent=2)+'\n')


def reflow_layout():
    """Compact the temporary wide room while preserving its authored tiles.

    The temporary 40x25 layout solved the sprite cap by adding a large empty
    central aisle. Fold that aisle back out, then move the lower growing banks
    seven tiles south. The lower boundary follows three tiles south as well,
    leaving the compact room at 29x28 while keeping each camera window to one
    five-berry bank per side.
    """
    layouts_path = ROOT/'data/layouts/layouts.json'
    registry = json.loads(layouts_path.read_text())
    layout = next(l for l in registry['layouts'] if l['id']=='LAYOUT_ROGUE_AREA_FARMING_FIELD')
    assert (layout['width'], layout['height']) == (OLD_W, OLD_H), 'Berry Lab already reflowed'
    map_path = ROOT/f'data/layouts/{ROOM}/map.bin'
    raw = map_path.read_bytes()
    wide = list(words(layout['blockdata_filepath']))
    assert len(wide) == OLD_W * OLD_H and len(raw) == OLD_W * OLD_H * 2
    # Recover the original 29x25 authored room from the temporary wide copy.
    old = []
    for y in range(OLD_H):
        row = wide[y * OLD_W:(y + 1) * OLD_W]
        old.extend(row[:17] + row[28:])
    void = old[0]
    floor = old[8 * 29 + 14]
    new = [void] * (W * H)
    for y in range(OLD_H):
        # Keep the side-pillar end at y=21; only the bottom boundary rows move.
        dest_y = y if y <= 21 else y + 3
        for x in range(29):
            new[dest_y * W + x] = old[y * 29 + x]
    # Restore the compact south aisle and its side-pillar ends below the moved
    # lower beds. The original y=21 row supplies the authored floor/pillar
    # tiles, while the old boundary remains three tiles farther south.
    for x in range(5, 24):
        new[24 * W + x] = old[21 * 29 + x]
    for x in (4, 24):
        new[24 * W + x] = old[21 * 29 + x]
    # Move the complete lower habitat assembly into the lower section. Its
    # seven-tile shift keeps the upper and lower berry rows fifteen tiles apart.
    for y in range(14, 17):
        for x in range(5, 24):
            new[y * W + x] = floor
            new[(y + 7) * W + x] = old[y * 29 + x]
    # The lower habitat's hanging plant bases extend one row below the bed.
    # Move that row for the habitat span while leaving the nearby cargo stack
    # in the service aisle.
    for x in range(7, 24):
        value = old[17 * 29 + x]
        if value & 1023 != floor:
            new[17 * W + x] = floor
            new[24 * W + x] = value
    map_path.write_bytes(packed(new))
    layout.update(width=W, height=H)
    for m in (ROOT/f'data/maps/{ROOM}/map.json',):
        data = json.loads(m.read_text())
        for event in data['object_events']:
            ident = event['trainer_sight_or_berry_tree_id']
            if ident.startswith('BERRY_TREE_HUB_'):
                bid = int(ident.rsplit('_', 1)[1])
                x, y, start, _ = next(b for b in BEDS if b[2] <= bid < b[2] + 5)
                event.update(x=x + bid - start, y=y)
            elif event['graphics_id'] == 'OBJ_EVENT_GFX_WORK_TABLE':
                event.update(x=20, y=24)
        for warp in data['warp_events']:
            if warp['y'] == 23:
                warp['y'] = 26
            elif warp['x'] > 30:
                warp['x'] = 26
        m.write_text(json.dumps(data, indent=2) + '\n')
    layouts_path.write_text(json.dumps(registry, indent=2) + '\n')


def patched(base, names, connections, upgrades):
    """Interpret the actual runtime tile patches, so validation cannot drift from C."""
    code=(ROOT/'src/rogue_hub.c').read_text().split('static void RogueHub_UpdateFarmingAreaMetatiles()\n{',1)[1].split('\n}\n',1)[0]
    a=base[:]
    blocks=re.findall(r'if\((.*?)\)\s*\{([^}]+)\}',code,re.S)
    assert len(blocks)==7, 'Expected four closures and three bed upgrades'
    for condition,body in blocks:
        if 'HUB_AREA_CONN_' in condition:
            d=re.search(r'HUB_AREA_CONN_(\w+)',condition)[1]
            active=not connections[DIRECTIONS.index(d)]
        else:
            i=int(re.search(r'EXTRA_FIELD(\d)',condition)[1]);active=not upgrades[i]
        if not active:continue
        calls=re.findall(r'Metatile(Set|Fill)_Tile\(([^;]+)\);',body)
        assert calls
        assert not re.sub(r'Metatile(?:Set|Fill)_Tile\([^;]+\);','',body).strip(), 'Unsupported runtime patch'
        for kind,args in calls:
            args=[v.strip() for v in args.split(',')];coords=list(map(int,args[:-1]));x1,y1=coords[:2];x2,y2=coords[2:] if kind=='Fill' else coords[:2]
            name=re.search(r'METATILE_BerryLab_(\w+)',args[-1])[1]
            v=names[name]|(0xC00 if 'MAPGRID_COLLISION_MASK' in args[-1] else 0)
            for y in range(y1,y2+1):
                for x in range(x1,x2+1):a[y*W+x]=(a[y*W+x]&0xF000)|v
    return a


def check_wall_joins(a,t,connections):
    """Independent assembly checks for the same cap/join/base/foot grammar as Labs."""
    def expect(x,y,name):
        expected = name if isinstance(name, int) else t[name]
        assert a[y*W+x]&1023==expected, ('Broken wall assembly',x,y,name,connections)
    for x,d in ((4,3),(24,1)):
        expect(x,2,'PillarCap');expect(x,3,'PillarJoin');expect(x,21,'PillarEnd')
        for y,name in [(8,'InnerJoin'),(9,'InnerBase'),(10,'InnerFoot'),(11,'Floor'),(12,7),(13,'PillarJoin'),(14,'Pillar')]:
            expect(x,y,name if connections[d] else 'Pillar')
        for xx in (range(0,4) if x==4 else range(25,29)):
            expect(xx,8,'Wall' if connections[d] else 'Void')
            expect(xx,9,'WallBase' if connections[d] else 'Void')
    for x in (12,16):
        for y in range(3):expect(x,y,'Pillar' if connections[0] else 'Void')
        for y,opened,closed in [(3,'InnerJoin','Wall'),(4,'InnerBase','WallBase'),(5,'InnerFoot','FloorShadow')]:
            expect(x,y,opened if connections[0] else closed)
        for y,name in [(24,7),(25,'PillarJoin'),(26,'Pillar'),(27,'Pillar')]:
            expect(x,y,name if connections[2] else 'Floor' if y==24 else 'Void')


def main():
    ap=argparse.ArgumentParser();ap.add_argument('--check',action='store_true');ap.add_argument('--init-layout',action='store_true');ap.add_argument('--reflow-layout',action='store_true');args=ap.parse_args()
    assert sum((args.check,args.init_layout,args.reflow_layout)) <= 1
    cat=Catalogue();t=tiles(cat)
    if args.reflow_layout:reflow_layout()
    if args.init_layout:initialize(cat)
    m=json.loads((ROOT/f'data/maps/{ROOM}/map.json').read_text())
    ls={l['id']:l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
    layout=ls[m['layout']];assert (layout['width'],layout['height'])==(W,H)
    assert layout['primary_tileset']=='gTileset_HubArchitecture' and layout['secondary_tileset']=='gTileset_HubFurnishings'
    assert m['map_type']=='MAP_TYPE_INDOOR'
    base=words(layout['blockdata_filepath']);assert len(base)==W*H
    header='// Generated by scripts/generate_berry_lab.py; do not edit.\n'
    header+=''.join(f'#define METATILE_BerryLab_{n} 0x{v:03X}\n' for n,v in t.items())
    outputs={'include/constants/berry_lab_tiles.h':header.encode()}
    maps={json.loads(p.read_text())['id']:json.loads(p.read_text()) for p in (ROOT/'data/maps').glob('Rogue_BerryLabCorridor_*/map.json')}
    for c in m['connections']:
        l=ls[maps[c['map']]['layout']];horizontal=c['direction'] in ('left','right');span=l['height'] if horizontal else l['width'];profile=[]
        for i in range(span):
            x,y=((0 if c['direction']=='left' else W-1),c['offset']+i) if horizontal else (c['offset']+i,0 if c['direction']=='up' else H-1)
            profile.append(base[y*W+x])
        data=packed([profile[y if horizontal else x] for y in range(l['height']) for x in range(l['width'])])
        if l['blockdata_filepath'] in outputs:assert outputs[l['blockdata_filepath']]==data
        outputs[l['blockdata_filepath']]=data
    for path,data in outputs.items():
        p=ROOT/path
        if args.check:assert p.read_bytes()==data,('Stale output',path)
        elif not p.exists() or p.read_bytes()!=data:p.write_bytes(data)
    OUT.mkdir(parents=True,exist_ok=True)
    for i,(x,y,start,flag) in enumerate(BEDS):
        assert base[y*W+x-1]&1023==t['BedEndLeft'] and base[y*W+x+5]&1023==t['BedEndRight'], 'Unfinished growing-bed sides'
        for j in range(5):
            e=next(e for e in m['object_events'] if e['trainer_sight_or_berry_tree_id']==f'BERRY_TREE_HUB_{start+j}')
            assert (e['x'],e['y'],e['flag'])==(x+j,y,flag)
            assert cat.attrs[base[y*W+x+j]&1023]&255==0xA0
            assert base[y*W+x+j]&0xC00, 'Preserve the original impassable planting spots'
            assert cat.attrs[base[(y-1)*W+x+j]&1023]>>12==1, 'Glass back must not cover berry sprites'
    for i,v in enumerate(base):
        mid=v&1023
        if mid==111:
            assert i>=W and base[i-W]&1023==t['Wall'] and base[i+W]&1023==t['PlantBaseShadow'], 'Wall-backed plant used on open floor'
        if mid==t['PlantTop']:assert base[i+W]&1023==t['PlantBase'], 'Incomplete floor plant'
        if mid==t['PlantTopShadow']:assert base[i+W]&1023==t['PlantBaseShadow'], 'Incomplete shaded plant'
        if mid==t['SeedTray']:assert base[i+W]&1023==t['PropagationBase'], 'Propagation tray has no cabinet base'
    assert cat.metas[t['PlantTop']][:4]==cat.metas[t['Floor']][:4]
    for name,donor in [('PlantBaseShadow',112),('PlantTopShadow',111)]:
        assert cat.metas[t[name]][:4]==cat.metas[t['FloorShadow']][:4]
        assert cat.metas[t[name]][4:]==cat.metas[donor][4:]
    prefabs = {p['name']: p for p in json.loads((ROOT/'data/tilesets/hub_prefabs.json').read_text())}
    for name,x,y in DECORATIONS:
        for cell in prefabs[name]['metatiles']:
            assert base[(y + cell['y']) * W + x + cell['x']] & 1023 == cell['metatile_id'], ('Missing lab decoration', name, x, y)
    expected = base[:]
    apply_decorations(expected)
    for x,y in [(6,5),(11,5),(17,5),(22,5),(5,8),(5,22)]:
        assert base[y*W+x]&1023==t['PlantBaseShadow'], 'Wall pot breaks the floor shadow'
    assert cat.metas[t['SoilRim']][4:]==cat.metas[t['InactiveBed']][4:]
    assert len(m['object_events'])==23 and len(m['warp_events'])==13
    assert m['object_events'][0]['script']=='Rogue_Area_InteractWithWorkbench'
    assert m['object_events'][21]['script']=='Rogue_Area_FarmingField_SpeakBerryMerchant'
    assert m['object_events'][22]['script']=='Rogue_Area_FarmingField_MakePotions'
    for j,e in enumerate(m['object_events'][1:21],1):assert e['trainer_sight_or_berry_tree_id']==f'BERRY_TREE_HUB_{j}'
    for index,p in [(0,(20,24)),(21,(14,11)),(22,(15,11))]:
        e=m['object_events'][index];assert (e['x'],e['y'],e['elevation'])==(*p,3)
    assert m['object_events'][21]['movement_type']=='MOVEMENT_TYPE_FACE_DOWN'
    assert m['object_events'][22]['flag']=='FLAG_TEMP_4'
    assert (m['warp_events'][8]['x'],m['warp_events'][8]['y'])==(14,18)
    berry_positions=[(e['x'],e['y']) for e in m['object_events']
                     if e['trainer_sight_or_berry_tree_id'].startswith('BERRY_TREE_HUB_')]
    for py in range(H):
        for px in range(W):
            if base[py*W+px]&0xC00:
                continue
            visible=sum(px-2 <= x <= px+17 and py <= y <= py+14 for x,y in berry_positions)
            assert visible <= 10, ('Too many berry sprites in one camera window', px, py, visible)
    count=0
    for connections in itertools.product((False,True),repeat=4):
        for upgrades in itertools.product((False,True),repeat=3):
            a=patched(base,t,connections,upgrades)
            check_wall_joins(a,t,connections)
            for brewing in (False,True):
                occupied={(20,24),(14,11)}|({(15,11)} if brewing else set())
                for i,(x,y,_,_) in enumerate(BEDS):
                    if i==0 or upgrades[i-1]:occupied.update((x+j,y) for j in range(5))
                    else:assert all(cat.attrs[a[y*W+x+j]&1023]&255!=0xA0 for j in range(5))
                def clear(p):return 0<=p[0]<W and 0<=p[1]<H and not a[p[1]*W+p[0]]&0xC00 and p not in occupied
                seen={(14,18)};q=deque(seen);assert clear((14,18))
                while q:
                    x,y=q.popleft()
                    for dx,dy in VECTORS:
                        p=x+dx,y+dy
                        if p not in seen and clear(p):seen.add(p);q.append(p)
                for p in occupied:assert any((p[0]+dx,p[1]+dy) in seen for dx,dy in VECTORS),('Inaccessible service/berry',p,connections,upgrades,brewing)
                for d,(cx,cy) in enumerate(CENTERS):
                    dx,dy=VECTORS[d]
                    for lane in (-1,0,1):
                        p=cx+(lane if dy else 0),cy+(lane if dx else 0)
                        assert (p in seen)==connections[d],('Exit',d,p,connections)
                        assert any((e['x'],e['y'])==p for e in m['warp_events'])
                        if connections[d]:
                            assert (p[0]-dx,p[1]-dy) in seen
                            # Full lane reaches the exterior through the facade.
                            qx,qy=p
                            while 0<=qx<W and 0<=qy<H:
                                assert (qx,qy) in seen
                                qx+=dx;qy+=dy
                count+=1
    def sprite(im,pic,x,y,width=16,frame=0,pal=None):
        from generate_portal_room import palette
        indexed=Image.open(ROOT/f'graphics/object_events/pics/{pic}.png').crop((frame*width,0,(frame+1)*width,32))
        colors=palette(f'graphics/object_events/palettes/{pal}.pal') if pal else [tuple(indexed.getpalette()[i:i+3]) for i in range(0,48,3)]
        rgba=Image.new('RGBA',indexed.size)
        rgba.putdata([(*colors[indexed.getpixel((px,py))],255 if indexed.getpixel((px,py)) else 0)
            for py in range(indexed.height) for px in range(indexed.width)])
        im.paste(rgba,(x*16+8-width//2,(y+1)*16-32),rgba)
    for state,upgrades in [('locked',(False,False,False)),('empty',(True,True,True)),('mature',(True,True,True))]:
        a=patched(base,t,(True,)*4,upgrades);im=Image.new('RGB',(W*16,H*16))
        for i,v in enumerate(a):im.paste(cat.render(v&1023),(i%W*16,i//W*16))
        sprite(im,'people/link_receptionist',14,11,pal='npc_3')
        sprite(im,'rogue/work_table',20,24,width=32,pal='npc_1')
        if state!='locked':sprite(im,'rogue/decor_cauldron',15,11,pal='npc_3')
        if state=='mature':
            for x,y,_,_ in BEDS:
                for j,berry in enumerate(['cheri','chesto','pecha','rawst','oran']):sprite(im,'berry_trees/'+berry,x+j,y,frame=4)
        im.save(OUT/f'{state}.png')
        for label,x,y in [('beds',14,8),('services',20,21)]:
            im.crop((x*16-120,y*16-80,x*16+120,y*16+80)).resize((720,480),Image.Resampling.NEAREST).save(OUT/f'{state}-{label}-camera.png')
    from PIL import ImageDraw
    sheet=Image.new('RGB',(W*16*4,(H*16+20)*4));draw=ImageDraw.Draw(sheet)
    cache={mid:cat.render(mid) for mid in range(len(cat.metas))}
    for i,connections in enumerate(itertools.product((False,True),repeat=4)):
        a=patched(base,t,connections,(True,)*3);ox=i%4*W*16;oy=i//4*(H*16+20)
        draw.text((ox+4,oy+2),'Open: '+(' '.join(d[0] for d,b in zip(DIRECTIONS,connections) if b) or 'none'),fill='white')
        for j,v in enumerate(a):sheet.paste(cache[v&1023],(ox+j%W*16,oy+20+j//W*16))
    sheet.save(OUT/'wall-states.png')
    print(f'Berry Lab: {count} connection/bed/brewing states PASS; authored room preserved. Previews: {OUT}')


if __name__=='__main__':main()
