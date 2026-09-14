#!/usr/bin/env python3
"""Validate/preview the authored Supply Depot. Normal runs never write its map/border.

--init-layout is the guarded one-time conversion of the original outdoor mart.
"""
import argparse
import itertools
import json
import re
from collections import deque
from PIL import Image
from generate_portal_room import ROOT, words, packed
from generate_hub_tiles import Catalogue
from generate_berry_lab import tiles as shared_tiles

ROOM = 'Rogue_Area_Marts'
OUT = ROOT/'build/supply_depot'
W, H = 37, 31
DIRECTIONS = ['NORTH','EAST','SOUTH','WEST']
VECTORS = [(0,-1),(1,0),(0,1),(-1,0)]
CENTERS = [(18,1),(34,15),(18,29),(2,15)]
UPGRADES = ['GENERAL_STOCK','POKE_BALLS','POKE_BALLS_STOCK','TMS','TMS_STOCK',
            'TRAVELER_BATTLE_ENCHANCERS','TRAVELER_HELD_ITEMS']
POSITIONS = [(10,22),(10,7),(26,7),(29,22),(24,22),(14,25),(22,25)]
STOCK_DISPLAYS = [(6,24),(13,9),(29,9)]


def place_merchandise(a,t):
    """Explicit authoring step, never called by normal preview/verification."""
    def put(x,y,n=None,shadow=False):
        name=('Market'+n+('Shadow' if shadow else '')) if n else ('FloorShadow' if shadow else 'Floor')
        a[y*W+x]=t[name]|0x3000|(0xC00 if n else 0)
    for x,y in [(6,5),(14,5),(22,5),(30,5),(6,21),(14,21)]:
        for yy in range(3):
            for xx in range(2):put(x+xx,y+yy,shadow=y==5 and yy==0)
    for x,y in [(6,5),(30,6)]:
        put(x,y,'DrawerTop',y==5);put(x,y+1,'DrawerBase')
    for x,y,n in [(7,5,'Bottles'),(14,5,'RedBasket'),(15,5,'GreenBasket'),(14,6,'Crate'),(15,6,'Goods')]:put(x,y,n,y==5)
    for x,y,kind in [(22,5,'BookDisplay'),(30,5,'BookDisplay'),(6,21,'MedicineDisplay'),(14,21,'MedicineDisplay'),(28,21,'BookDisplay')]:
        for xx in range(2):put(x+xx,y,kind+str(xx),y==5)
    for x,y,n in [(6,23,'Bottles'),(7,23,'Goods'),(14,23,'Bottles'),(15,23,'Crate'),(23,21,'Goods'),(24,21,'RedBasket')]:put(x,y,n)
    for x,y in STOCK_DISPLAYS+[(6,9),(22,9)]:
        for xx in range(2):
            put(x+xx,y,'DrawerTop');put(x+xx,y+1,'DrawerBase')
            put(x+xx,y+2,'Crate' if xx==0 else 'Goods')
    # Keep the room-side approaches to the northern signs clear.
    for x in (6,7,22,23):
        put(x,8,'Goods' if x%2 else 'Crate');put(x,11)


def tiles(cat):
    t=shared_tiles(cat)
    for n,old in cat.config['rooms']['lab_junction']['names'].items():
        t[n]=cat.mapping['lab_junction'][old]
    t.update(cat.supply_depot)
    return t


def finish_initial_assemblies(a,t):
    """One-time authoring finish only; never applied during preview/generation."""
    def put(x,y,n,solid=False):a[y*W+x]=t[n]|0x3000|(0xC00 if solid else 0)
    for x in (16,20):
        put(x,3,'PillarJoin',True);put(x,17,'PillarCapFloor',True)
        for y in (4,5,27,28,29,30):put(x,y,'Pillar',True)
    for x in (4,32):put(x,14,'InnerFoot')
    # Remove the extra pillar columns formerly pasted beside existing posts.
    for x in (5,15,21,31):
        for y in (12,18):
            put(x,y,'Wall',True);put(x,y+1,'WallBase',True);put(x,y+2,'FloorShadow')
        put(x,17,'Floor')
    # Both edges of every department opening get cap/join/base/foot assemblies.
    for x in (8,12,24,28):
        for y in (12,18):
            put(x,y-1,'PillarCapFloor',True);put(x,y,'PillarJoin',True)
            put(x,y+1,'InnerBase',True);put(x,y+2,'InnerFoot')
    for x in (5,17,21):
        for y in list(range(6,12))+list(range(21,27)):
            if not a[y*W+x]&0xC00:put(x,y,'FloorShadow')
    # North computers attach to the wall. Southern computers are freestanding.
    for x,y in [(8,5),(24,5),(8,20),(23,20),(28,20)]:
        for xx in (x,x+1):
            put(xx,y,'FloorShadow' if y==5 or xx not in (9,25) else 'Floor')
            put(xx,y+1,'Floor')
        if y==5:
            for xx in range(2):
                put(x+xx,4,f'ComputerBench_{xx}_0',True)
                put(x+xx,5,f'ComputerShadow_{xx}',True)
    for x in (10,25,29):
        for xx in range(2):
            put(x+xx,25,f'ComputerFloorTop_{xx}',True)
            put(x+xx,26,f'ComputerBench_{xx}_1',True)
    # Reinstate jamb feet after clearing the old southern furniture footprints.
    for x in (8,12,24,28):put(x,20,'InnerFoot')
    for x,y in [(6,5),(14,5),(22,5),(30,5),(6,21),(14,21)]:
        for yy in range(3):
            for xx in range(2):
                n=f'SpecimenPlainTop_{xx}' if y==21 and yy==0 else f'SpecimenFloor_{xx}_{yy}'
                put(x+xx,y+yy,n,True)


def initialize(cat):
    path=ROOT/'data/layouts/layouts.json';registry=json.loads(path.read_text())
    layout=next(l for l in registry['layouts'] if l['id']=='LAYOUT_ROGUE_AREA_MARTS')
    assert layout['primary_tileset']=='gTileset_GeneralHub', 'Already converted: edit in Porymap'
    t=tiles(cat);a=[t['Void']|0x3C00]*(W*H)
    def fill(x1,y1,x2,y2,n,solid=False):
        for y in range(y1,y2+1):
            for x in range(x1,x2+1):a[y*W+x]=t[n]|0x3000|(0xC00 if solid else 0)
    def put(x,y,n,solid=True):fill(x,y,x,y,n,solid)
    def wall(x1,x2,y):
        fill(x1,y,x2,y,'Wall',True);fill(x1,y+1,x2,y+1,'WallBase',True)
        fill(x1,y+2,x2,y+2,'FloorShadow')
    def assembly(x,y,name,w,h):
        for yy in range(h):
            for xx in range(w):put(x+xx,y+yy,f'{name}_{xx}_{yy}')
    fill(5,5,31,27,'Floor');wall(5,31,3)
    for x in (4,32):
        fill(x,2,x,27,'Pillar',True)
        for y,n in [(2,'PillarCap'),(3,'PillarJoin'),(12,'InnerJoin'),(13,'InnerBase'),(14,'InnerFoot'),(17,'PillarCap'),(18,'PillarJoin'),(27,'PillarEnd')]:put(x,y,n,y!=14)
        fill(x,15,x,16,'Floor')
    # Exterior cross and facades, with the same wall grammar as Research Junction.
    fill(0,14,36,16,'Floor');fill(17,0,19,30,'Floor')
    for x in (16,20):
        fill(x,0,x,27,'Pillar',True)
        for y,n in [(3,'InnerJoin'),(4,'InnerBase'),(5,'InnerFoot'),(12,'InnerJoin'),(13,'InnerBase'),(14,'InnerFoot'),(17,'PillarCap'),(18,'PillarJoin'),(27,'PillarCap'),(28,'PillarJoin'),(29,'Pillar'),(30,'Pillar')]:put(x,y,n,y in (3,4,5,12,13,17,18,27,28,29,30))
        fill(x,15,x,16,'Floor')
    for x1,x2 in ((0,3),(33,36)):wall(x1,x2,12)
    for x,n in [(0,'FloorDim20'),(1,'FloorDim20'),(2,'WarpWest'),(3,'FloorFadeWest'),(33,'FloorFadeEast'),(34,'WarpEast'),(35,'FloorDim20'),(36,'FloorDim20')]:fill(x,14,x,16,n)
    for x,n in [(0,'FloorShadowDim20'),(1,'FloorShadowDim20'),(2,'WarpWestUpper'),(3,'FloorShadowFadeWest'),(33,'FloorShadowFadeEast'),(34,'WarpEastUpper'),(35,'FloorShadowDim20'),(36,'FloorShadowDim20')]:put(x,14,n,False)
    for y,n in [(0,'FloorDim20'),(1,'WarpNorth'),(2,'FloorFadeNorth'),(28,'FloorFadeSouth'),(29,'WarpSouth'),(30,'FloorDim20')]:fill(17,y,19,y,n)
    # Solid partitions and three-wide department doorways.
    for x1,x2,door in ((5,15,10),(21,31,26)):
        for y in (12,18):
            wall(x1,x2,y)
            fill(door-1,y,door+1,y+2,'Floor')
        for x in (x1,x2):
            put(x,12,'InnerJoin');put(x,13,'InnerBase');put(x,14,'InnerFoot',False)
            put(x,17,'PillarCap');put(x,18,'PillarJoin');put(x,19,'InnerBase');put(x,20,'InnerFoot',False)
    # Distinct staffed counters, backed by complete equipment assemblies.
    for x,y in POSITIONS[:5]:
        for xx in (x-1,x):put(xx,y+1,'CounterLeft' if xx==x-1 else 'CounterRight')
    for x,y in [(6,5),(14,5),(22,5),(30,5),(6,21),(14,21)]:
        assembly(x,y,'SpecimenBank',2,3)
    for x,y in [(8,5),(24,5),(8,20),(23,20),(28,20)]:assembly(x,y,'ComputerBench',2,2)
    for x,y in [(6,9),(13,9),(22,9),(29,9),(6,24)]:assembly(x,y,'Decor_Cargo',2,3)
    for x,y in [(12,4),(28,4)]:
        a[y*W+x]=111|0x3C00;put(x,y+1,'PlantBaseShadow')
    # Readable department signs on the partition faces, away from doorways.
    for x,y in [(7,12),(23,12),(7,18),(23,18)]:put(x,y,'SignTop');put(x,y+1,'SignBase')
    finish_initial_assemblies(a,t)
    place_merchandise(a,t)
    layout.update(width=W,height=H,primary_tileset='gTileset_HubArchitecture',secondary_tileset='gTileset_HubFurnishings')
    (ROOT/layout['blockdata_filepath']).write_bytes(packed(a))
    (ROOT/layout['border_filepath']).write_bytes(packed([t['Void']|0x3C00]*4))
    mp=ROOT/f'data/maps/{ROOM}/map.json';m=json.loads(mp.read_text());m['map_type']='MAP_TYPE_INDOOR'
    for e,(x,y) in zip(m['object_events'],POSITIONS):e.update(x=x,y=y,elevation=3,movement_type='MOVEMENT_TYPE_FACE_DOWN')
    m['object_events'][3]['script']='SupplyDepot_HeldItems'
    m['object_events'][4]['script']='SupplyDepot_BattleItems'
    m['object_events'][6]['graphics_id']='OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE'
    warp=lambda x,y:dict(x=x,y=y,elevation=0,dest_map='MAP_ROGUE_AREA_MARTS',dest_warp_id='0')
    m['warp_events']=[warp(18,1),warp(19,1),warp(34,15),warp(34,16),warp(18,29),warp(19,29),warp(2,15),warp(2,16),warp(18,23),warp(17,1),warp(34,14),warp(17,29),warp(2,14)]
    m['bg_events']=[dict(type='sign',x=x,y=y,elevation=3,player_facing_dir='BG_EVENT_PLAYER_FACING_ANY',script='SupplyDepot_'+name+'Sign') for x,y,name in [(7,13,'Balls'),(23,13,'TMs'),(7,19,'General'),(23,19,'Equipment')]]
    m['bg_events'] += [dict(e,y=e['y']-1,player_facing_dir='BG_EVENT_PLAYER_FACING_SOUTH') for e in m['bg_events']]
    gp=ROOT/'data/maps/map_groups.json';groups=json.loads(gp.read_text());m['connections']=[]
    for suffix,direction,offset,cw,ch in [('Horizontal','left',12,16,8),('Horizontal','right',12,16,8),('North','up',15,8,12),('South','down',15,8,12)]:
        name='Rogue_SupplyDepotCorridor_'+suffix;lid='LAYOUT_ROGUE_SUPPLY_DEPOT_CORRIDOR_'+suffix.upper();mid='MAP_ROGUE_SUPPLY_DEPOT_CORRIDOR_'+suffix.upper()
        m['connections'].append(dict(map=mid,offset=offset,direction=direction))
        if any(l['id']==lid for l in registry['layouts']):continue
        registry['layouts'].append(dict(id=lid,name=name+'_Layout',width=cw,height=ch,primary_tileset='gTileset_HubArchitecture',secondary_tileset='gTileset_HubFurnishings',border_filepath=f'data/layouts/{name}/border.bin',blockdata_filepath=f'data/layouts/{name}/map.bin'))
        folder=ROOT/f'data/layouts/{name}';folder.mkdir(parents=True,exist_ok=True);(folder/'border.bin').write_bytes(packed([t['Void']|0x3C00]*4))
        folder=ROOT/f'data/maps/{name}';folder.mkdir(parents=True,exist_ok=True)
        facade=json.loads((ROOT/'data/maps/Rogue_PortalCorridor_Horizontal/map.json').read_text());facade.update(id=mid,name=name,layout=lid)
        (folder/'map.json').write_text(json.dumps(facade,indent=2)+'\n');(folder/'scripts.pory').write_text(f'mapscripts {name}_MapScripts {{}}\n')
        groups['gMapGroup_RogueHubAreas'].append(name)
    path.write_text(json.dumps(registry,indent=2)+'\n');gp.write_text(json.dumps(groups,indent=2)+'\n');mp.write_text(json.dumps(m,indent=2)+'\n')


def patched(base,t,connections,upgrades):
    code=(ROOT/'src/rogue_hub.c').read_text().split('static void RogueHub_UpdateMartsAreaMetatiles()\n{',1)[1].split('\n}\n',1)[0]
    a=base[:]
    blocks=re.findall(r'if\((.*?)\)\s*\{([^}]+)\}',code,re.S)
    assert len(blocks)==12, 'Expected four exits, four department states, three stock displays and equipment bay'
    for condition,body in blocks:
        if 'HUB_AREA_CONN_' in condition:
            d=re.search(r'HUB_AREA_CONN_(\w+)',condition)[1];active=not connections[DIRECTIONS.index(d)]
        else:
            keys=re.findall(r'HUB_UPGRADE_MARTS_(\w+)',condition)
            active=all(not upgrades[UPGRADES.index(k)] for k in keys)
        if not active:continue
        calls=re.findall(r'Metatile(Set|Fill)_Tile\(([^;]+)\);',body)
        assert calls and not re.sub(r'Metatile(?:Set|Fill)_Tile\([^;]+\);','',body).strip()
        for kind,args in calls:
            args=[v.strip() for v in args.split(',')];coords=list(map(int,args[:-1]));x1,y1=coords[:2];x2,y2=coords[2:] if kind=='Fill' else coords[:2]
            name=re.search(r'METATILE_SupplyDepot_(\w+)',args[-1])[1];v=t[name]|(0xC00 if 'MAPGRID_COLLISION_MASK' in args[-1] else 0)
            for y in range(y1,y2+1):
                for x in range(x1,x2+1):a[y*W+x]=(a[y*W+x]&0xF000)|v
    return a


def main():
    ap=argparse.ArgumentParser();ap.add_argument('--check',action='store_true');ap.add_argument('--init-layout',action='store_true');args=ap.parse_args()
    assert not(args.check and args.init_layout)
    cat=Catalogue();t=tiles(cat)
    if args.init_layout:initialize(cat)
    m=json.loads((ROOT/f'data/maps/{ROOM}/map.json').read_text());ls={l['id']:l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']};layout=ls[m['layout']]
    assert (layout['width'],layout['height'])==(W,H) and m['map_type']=='MAP_TYPE_INDOOR'
    assert layout['primary_tileset']=='gTileset_HubArchitecture' and layout['secondary_tileset']=='gTileset_HubFurnishings'
    base=words(layout['blockdata_filepath']);assert len(base)==W*H
    assert words(layout['border_filepath'])==[t['Void']|0x3C00]*4
    outputs={'include/constants/supply_depot_tiles.h':('// Generated by scripts/generate_supply_depot.py; do not edit.\n'+''.join(f'#define METATILE_SupplyDepot_{n} 0x{v:03X}\n' for n,v in t.items())).encode()}
    maps={json.loads(p.read_text())['id']:json.loads(p.read_text()) for p in (ROOT/'data/maps').glob('Rogue_SupplyDepotCorridor_*/map.json')}
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
    assert len(m['object_events'])==7 and len(m['warp_events'])==13
    assert [(e['x'],e['y']) for e in m['object_events']]==POSITIONS
    assert [e['flag'] for e in m['object_events']]==['0','FLAG_TEMP_1','FLAG_TEMP_2','FLAG_TEMP_4','FLAG_TEMP_3','0','FLAG_TEMP_3']
    assert [e['script'] for e in m['object_events'][:6]]==['Rogue_Shop_Medicine','Rogue_Shop_Balls','Rogue_Shop_TMs','SupplyDepot_HeldItems','SupplyDepot_BattleItems','Rogue_Area_InteractWithWorkbench']
    assert all(e['elevation']==3 for e in m['object_events'])
    assert all(e['movement_type']=='MOVEMENT_TYPE_FACE_DOWN' for e in m['object_events'][:5])
    assert (m['warp_events'][8]['x'],m['warp_events'][8]['y'])==(18,23)
    assert len(m['bg_events'])==8
    for x,y,name in [(7,13,'Balls'),(23,13,'TMs'),(7,19,'General'),(23,19,'Equipment')]:
        for yy,facing in [(y,'BG_EVENT_PLAYER_FACING_ANY'),(y-1,'BG_EVENT_PLAYER_FACING_SOUTH')]:
            assert any(e==dict(type='sign',x=x,y=yy,elevation=3,player_facing_dir=facing,script='SupplyDepot_'+name+'Sign') for e in m['bg_events']), ('Missing two-sided sign',name,yy)
    for n in ('CounterLeft','CounterRight'):assert cat.attrs[t[n]]&255==0x80
    # Recomposition must preserve equipment silhouettes and match its floor.
    for x in range(2):
        for n,source,back in [(f'ComputerShadow_{x}',f'ComputerBench_{x}_1','FloorShadow'),(f'ComputerFloorTop_{x}',f'ComputerBench_{x}_0','Floor'),(f'SpecimenPlainTop_{x}',f'SpecimenBank_{x}_0','Floor')]:
            assert cat.metas[t[n]]==cat.metas[t[back]][:4]+cat.metas[t[source]][4:]
    assert cat.metas[t['PillarCapFloor']]==cat.metas[t['Floor']][:4]+cat.metas[t['PillarCap']][4:]
    for name,foreground in cat.merchandise_foregrounds.items():
        for suffix,back in [('', 'Floor'),('Shadow','FloorShadow')]:
            assert cat.metas[t['Market'+name+suffix]]==cat.metas[t[back]][:4]+foreground
    for x,y in STOCK_DISPLAYS+[(6,9),(22,9)]:
        for xx in range(2):
            assert base[y*W+x+xx]&1023==t['MarketDrawerTop']
            assert base[(y+1)*W+x+xx]&1023==t['MarketDrawerBase']
    count=0
    for connections in itertools.product((False,True),repeat=4):
        for upgrades in itertools.product((False,True),repeat=7):
            a=patched(base,t,connections,upgrades)
            for x,d in ((4,3),(32,1)):
                for y,n in [(2,'PillarCap'),(3,'PillarJoin'),(12,'InnerJoin'),(13,'InnerBase'),(14,'InnerFoot'),(15,'Floor'),(16,'Floor'),(17,'PillarCap'),(18,'PillarJoin'),(27,'PillarEnd')]:
                    expected=n if connections[d] or y not in range(12,19) else 'PillarJoin' if y in (12,18) else 'Pillar'
                    assert a[y*W+x]&1023==t[expected], ('Exterior wall joint',x,y,connections)
            for x in (16,20):
                assert a[3*W+x]&1023==t['PillarJoin']
                assert a[4*W+x]&1023==t['Pillar']
                assert a[17*W+x]&1023==t['PillarCapFloor']
                assert a[27*W+x]&1023==t['Pillar' if connections[2] else 'PillarEnd']
            for door,y,opened in [(10,12,upgrades[1]),(26,12,upgrades[3]),(10,18,True),(26,18,upgrades[5] or upgrades[6])]:
                for x in (door-2,door+2):
                    for yy,n in [(y-1,'PillarCapFloor'),(y,'PillarJoin'),(y+1,'InnerBase'),(y+2,'InnerFoot')]:
                        expected=n if opened else ('FloorDim40' if y==12 else 'Floor') if yy==y-1 else {y:'Wall',y+1:'WallBase',y+2:'FloorShadow' if y==12 else 'FloorShadowDim40'}[yy]
                        assert a[yy*W+x]&1023==t[expected], ('Department jamb',x,yy,opened)
            if not (upgrades[5] or upgrades[6]):
                assert all(a[20*W+x]&1023==t['FloorShadowDim40'] for x in range(21,32)), 'Light strip in closed equipment room'
            for x,opened in ((8,upgrades[1]),(24,upgrades[3])):
                for xx in range(2):
                    assert a[4*W+x+xx]&1023==t[f'ComputerBench_{xx}_0' if opened else 'WallBase']
                    if opened:assert a[5*W+x+xx]&1023==t[f'ComputerShadow_{xx}']
            for x,y,flag,department in [(6,24,0,True),(13,9,2,upgrades[1]),(29,9,4,upgrades[3])]:
                if department:
                    for yy in range(y,y+3):
                        for xx in range(x,x+2):
                            assert a[yy*W+xx]&1023==(base[yy*W+xx]&1023 if upgrades[flag] else t['Floor']), 'Stock display mismatch'
            for x in (12,28):
                assert a[4*W+x]&1023==111 and a[5*W+x]&1023==t['PlantBaseShadow'], 'Incomplete wall plant'
            visible=[True,upgrades[1],upgrades[3],upgrades[6],upgrades[5],True,upgrades[5]]
            occupied={p for p,v in zip(POSITIONS,visible) if v}
            def clear(p):return 0<=p[0]<W and 0<=p[1]<H and not a[p[1]*W+p[0]]&0xC00 and p not in occupied
            seen={(18,23)};q=deque(seen);assert clear((18,23))
            while q:
                x,y=q.popleft()
                for dx,dy in VECTORS:
                    p=x+dx,y+dy
                    if p not in seen and clear(p):seen.add(p);q.append(p)
            # Every sign is readable from the crossing, even with its room locked.
            assert all(p in seen for p in [(7,14),(23,14),(7,17),(23,17)]), 'Blocked aisle-side sign'
            for p,opened in [((7,11),upgrades[1]),((23,11),upgrades[3]),((7,20),True),((23,20),upgrades[5] or upgrades[6])]:
                assert (p in seen)==opened, ('Room-side sign access',p)
            for i,((x,y),v) in enumerate(zip(POSITIONS,visible)):
                if not v:continue
                if i<5:
                    assert cat.attrs[a[(y+1)*W+x]&1023]&255==0x80, ('Missing counter',i)
                    assert (x,y+2) in seen, ('Inaccessible shop',i,connections,upgrades)
                else:assert any((x+dx,y+dy) in seen for dx,dy in VECTORS), ('Inaccessible workbench/cargo',i)
            for d,(cx,cy) in enumerate(CENTERS):
                dx,dy=VECTORS[d]
                for lane in (-1,0,1):
                    p=cx+(lane if dy else 0),cy+(lane if dx else 0)
                    assert (p in seen)==connections[d], ('Exit',d,p,connections)
                    assert any((e['x'],e['y'])==p for e in m['warp_events'])
                    if connections[d]:
                        assert (p[0]-dx,p[1]-dy) in seen
                        qx,qy=p
                        while 0<=qx<W and 0<=qy<H:
                            assert (qx,qy) in seen, 'Blocked facade approach'
                            qx+=dx;qy+=dy
            for x,opened in [(10,upgrades[1]),(26,upgrades[3])]:
                assert ((x,11) in seen)==opened, ('Locked northern department',x)
            assert ((26,21) in seen)==(upgrades[5] or upgrades[6]), 'Locked equipment department'
            count+=1
    OUT.mkdir(parents=True,exist_ok=True);cache={i:cat.render(i) for i in range(len(cat.metas))}
    for name,upgrades in [('locked',(False,)*7),('partial',(True,True,False,False,False,True,False)),('full',(True,)*7)]:
        a=patched(base,t,(True,)*4,upgrades);im=Image.new('RGB',(W*16,H*16))
        for i,v in enumerate(a):im.paste(cache[v&1023],(i%W*16,i//W*16))
        from generate_portal_room import palette
        visible=[True,upgrades[1],upgrades[3],upgrades[6],upgrades[5],True,upgrades[5]]
        for i,((x,y),v) in enumerate(zip(POSITIONS,visible)):
            if not v:continue
            pic='people/mart_employee' if i<3 else 'people/mystery_event_deliveryman' if i<5 else 'rogue/work_table' if i==5 else 'rogue/route_props/supply_crate'
            width=32 if i==5 else 16
            height=16 if i==6 else 32
            indexed=Image.open(ROOT/f'graphics/object_events/pics/{pic}.png').crop((0,0,width,height));colors=palette('graphics/object_events/palettes/npc_2.pal' if i==6 else 'graphics/object_events/palettes/npc_1.pal')
            rgba=Image.new('RGBA',indexed.size);rgba.putdata([(*colors[v],255 if v else 0) for v in list(indexed.get_flattened_data())]);im.paste(rgba,(x*16+8-width//2,(y+1)*16-height),rgba)
        im.save(OUT/f'{name}.png')
        for label,x,y in [('balls',10,9),('equipment',26,23),('crossing',18,15)]:
            camera=im.crop((x*16-120,y*16-80,x*16+120,y*16+80));camera.save(OUT/f'{name}-{label}-native.png');camera.resize((720,480),Image.Resampling.NEAREST).save(OUT/f'{name}-{label}-camera.png')
    from PIL import ImageDraw
    sheet=Image.new('RGB',(W*16*4,(H*16+20)*4));draw=ImageDraw.Draw(sheet)
    for i,connections in enumerate(itertools.product((False,True),repeat=4)):
        a=patched(base,t,connections,(True,)*7);ox=i%4*W*16;oy=i//4*(H*16+20)
        draw.text((ox+4,oy+2),'Open: '+(' '.join(d[0] for d,b in zip(DIRECTIONS,connections) if b) or 'none'),fill='white')
        for j,v in enumerate(a):sheet.paste(cache[v&1023],(ox+j%W*16,oy+20+j//W*16))
    sheet.save(OUT/'wall-states.png')
    print(f'Supply Depot: {count} connection/upgrade states PASS; authored map and border preserved. Previews: {OUT}')


if __name__=='__main__':main()
