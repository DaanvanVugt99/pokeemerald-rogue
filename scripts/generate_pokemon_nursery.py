#!/usr/bin/env python3
"""Pokemon Nursery: validate authored maps, generate only facades/header/previews.
--init-layout is an explicit, guarded, one-time conversion; never use for edits.
"""
import argparse
import itertools
import json
import re
from collections import deque
from pathlib import Path
from PIL import Image
from generate_portal_room import ROOT, words, packed, palette
from generate_hub_tiles import Catalogue
from generate_berry_lab import tiles as shared_tiles

ROOM='Rogue_Area_DayCare'
OUT=ROOT/'build/pokemon_nursery'
W,H=37,29
POSITIONS=[(15,24),(8,20),(14,20),(27,7),(8,7),(12,10),(11,7),(13,18),(15,18)]
CENTERS=[(18,1),(34,14),(18,27),(2,14)]
VECTORS=[(0,-1),(1,0),(0,1),(-1,0)]
DIRECTIONS=['NORTH','EAST','SOUTH','WEST']
SIGNS=[(14,11,'Care'),(27,12,'Eggs'),(14,24,'Reception'),(8,18,'Tea')]
BAYS=[(5,5),(10,5),(5,9)] # Legacy initial authoring recipe only
SEATS=[(8,7),(11,7),(12,10)]
SEAT_APPROACHES=[(8,8),(11,8),(11,10)]

def tiles(cat):
    result=shared_tiles(cat)
    for name,old in cat.config['rooms']['lab_junction']['names'].items():
        result[name]=cat.mapping['lab_junction'][old]
    result.update(cat.supply_depot)
    result.update(cat.nursery)
    return result


def author_layout(cat):
    """Explicit initial authoring recipe; normal commands only read Porymap files."""
    t=tiles(cat);a=[t['Void']|0x3C00]*(W*H)
    def put(x,y,n,solid=False):a[y*W+x]=(t[n] if isinstance(n,str) else n)|0x3000|(0xC00 if solid else 0)
    def fill(x1,y1,x2,y2,n,solid=False):
        for y in range(y1,y2+1):
            for x in range(x1,x2+1):put(x,y,n,solid)
    def wall(x1,x2,y):
        fill(x1,y,x2,y,'Wall',True);fill(x1,y+1,x2,y+1,'WallBase',True);fill(x1,y+2,x2,y+2,'FloorShadow')
    fill(5,5,31,25,'Floor');wall(5,31,3)
    fill(17,0,19,28,'Floor');fill(0,13,36,15,'Floor')
    for x in (4,32):
        fill(x,2,x,25,'Pillar',True)
        for y,n in [(2,'PillarCap'),(3,'PillarJoin'),(11,'InnerJoin'),(12,'InnerBase'),(13,'InnerFoot'),(14,'Floor'),(15,'Floor'),(16,'PillarCap'),(17,'PillarJoin'),(25,'PillarEnd')]:put(x,y,n,y not in (13,14,15))
    for x in (16,20):
        fill(x,0,x,4,'Pillar',True);put(x,3,'PillarJoin',True)
        fill(x,26,x,28,'Pillar',True)
    for x1,x2 in ((0,3),(33,36)):wall(x1,x2,11)
    for x,n in [(0,'FloorDim20'),(1,'FloorDim20'),(2,'WarpWest'),(3,'FloorFadeWest'),(33,'FloorFadeEast'),(34,'WarpEast'),(35,'FloorDim20'),(36,'FloorDim20')]:fill(x,13,x,15,n)
    for x,n in [(0,'FloorShadowDim20'),(1,'FloorShadowDim20'),(2,'WarpWestUpper'),(3,'FloorShadowFadeWest'),(33,'FloorShadowDim20'),(34,'WarpEastUpper'),(35,'FloorShadowDim20'),(36,'FloorShadowDim20')]:put(x,13,n)
    put(33,13,'FloorShadowFadeEast')
    for y,n in [(0,'FloorDim20'),(1,'WarpNorth'),(2,'FloorFadeNorth'),(26,'FloorFadeSouth'),(27,'WarpSouth'),(28,'FloorDim20')]:fill(17,y,19,y,n)
    # The southeast cutaway is real void, making an L-shaped occupied footprint.
    fill(21,16,36,28,'Void',True)
    fill(20,16,20,28,'Pillar',True);put(20,16,'PillarCap',True);put(20,17,'PillarJoin',True)
    put(16,25,'PillarCapFloor',True);put(16,26,'PillarJoin',True)
    # Egg nursery at the end of the upper gallery; tea in the lower left wing.
    wall(23,31,11)
    fill(22,4,22,12,'Pillar',True);put(22,3,'PillarJoin',True)
    fill(22,7,22,9,'Floor');put(22,6,'InnerBase',True);put(22,7,'InnerFoot')
    put(22,10,'PillarCapFloor',True);put(22,11,'PillarJoin',True)
    wall(5,11,17);fill(12,17,12,25,'Pillar',True)
    put(12,16,'PillarCapFloor',True);put(12,17,'PillarJoin',True)
    put(12,19,'InnerBase',True);fill(12,20,12,22,'Floor');put(12,20,'InnerFoot')
    put(12,23,'PillarCapFloor',True);put(12,24,'PillarJoin',True);put(12,25,'PillarEnd',True)
    # Small information plinths serve the open care gallery and reception.
    for x,y,name in SIGNS:put(x,y-1,'SignTop',True);put(x,y,'SignBase',True)
    render(cat,a,OUT/'shell.png')
    for x,y in BAYS:
        for xx in range(3):
            put(x+xx,y,'GlassBack'+str(xx),True)
            put(x+xx,y+1,105+xx,True);put(x+xx,y+2,108+xx,True)
        put(x+3,y+1,'DayCareFenceSide',True);put(x+3,y+2,'DayCareFenceSide',True)
        put(x+3,y,'DayCareFlowers',True)
    for x,y in [(9,5),(15,5),(10,10),(10,11),(15,12),(21,5),(21,12),(15,16)]:put(x,y,'DayCareShrub',True)
    for x,y in [(11,10),(15,6),(21,6),(16,17)]:put(x,y,'DayCareFlowers',True)
    # A compact stepped reception desk follows the narrow lower wing.
    for i in (1,2,3):
        x,y=POSITIONS[i];put(x-1,y+1,'CounterLeft',True);put(x,y+1,'CounterRight',True)
    for x,y in [(15,20),(8,23)]:
        for xx in range(2):
            for yy in range(2):put(x+xx,y+yy,f'Decor_Table_{xx}_{yy}',True)
    put(16,22,'Decor_Stool_0_0',True)
    for x in (24,29):
        for yy in range(3):
            for xx in range(2):put(x+xx,4+yy,f'SpecimenBank_{xx}_{yy}',True)
    for x in (6,10):
        put(x,19,'DayCareFlowerBoxShadow',True);put(x,20,'DayCareFlowerBoxBase',True)
    for x,y in [(5,24),(6,24)]:put(x,y,'DayCareFence',True)
    for x,y in [(5,25),(6,25)]:put(x,y,'DayCareFlowers',True)
    return finish_coffee_corner(finish_seating(finish_tile_assemblies(a,t),t),t)

def finish_tile_assemblies(a,t):
    """Explicit authoring cleanup, never invoked by normal generation/checks."""
    def put(x,y,n):a[y*W+x]=(a[y*W+x]&0xFC00)|t[n]
    for x in (16,20):
        put(x,3,'InnerJoin');put(x,4,'InnerBase')
        a[5*W+x]=t['InnerFoot']|0x3000
    for x,y,n in [(22,5,'InnerJoin'),(22,12,'InnerBase'),(12,18,'InnerJoin'),(12,24,'Pillar'),(20,17,'Pillar')]:put(x,y,n)
    a[13*W+22]=t['InnerFoot']|0x3000
    for x,y in BAYS:
        for xx,part in enumerate((0,1,5)):
            put(x+xx,y,f'GlassShadow{part}' if y==5 or x+xx==5 else f'GlassBack{part}')
        if x==5:
            put(x,y+1,'SofaLeftShadowTop');put(x,y+2,'SofaLeftShadowBase')
    # Shade exposed floor beside walls, and use matching decoration backings.
    cells={(x,5) for x in range(5,32)}|{(5,y) for y in range(6,26)}|{(23,6),(23,10)}|{(13,y) for y in (17,18,19,23,24,25)}
    for x,y in cells:
        mid=a[y*W+x]&1023
        if mid==t['Floor']:put(x,y,'FloorShadow')
        for name in ('Flowers','Shrub','Fence','FenceSide','FlowerBoxBase'):
            if mid==t['DayCare'+name]:put(x,y,'DayCare'+name+'Shadow')
    return a

def finish_seating(a,t):
    """Explicit furnishing edit; previews and validation never apply it."""
    def put(x,y,n,solid=False):a[y*W+x]=t[n]|0x3000|(0xC00 if solid else 0)
    outdoors={mid for name,mid in t.items() if name.startswith('DayCare')}
    shadows={mid for name,mid in t.items() if name.startswith('DayCare') and name.endswith('Shadow')}
    for i,v in enumerate(a):
        if v&1023 in outdoors:
            put(i%W,i//W,'FloorShadow' if v&1023 in shadows or i%W==5 or i//W in (5,19) else 'Floor')
    for x,y in BAYS:
        for xx in range(3):put(x+xx,y,'FloorShadow' if y==5 or x+xx==5 else 'Floor')
    # Complete indoor pots, using the same wall and floor compositions as shops.
    for x in (9,14,21):
        a[4*W+x]=111|0x3C00;put(x,5,'PlantBaseShadow',True)
    for x,y in [(8,10),(16,16),(6,19),(10,19),(5,24)]:
        put(x,y,'PlantTopShadow' if y==19 or x==5 else 'PlantTop',True)
        put(x,y+1,'PlantBaseShadow' if x==5 else 'PlantBase',True)
    put(10,10,'MarketDrawerTop',True);put(10,11,'MarketDrawerBase',True)
    put(11,10,'MarketBottles',True)
    return a

def finish_coffee_corner(a,t):
    """Explicit scoped authoring edit, not part of normal preview generation."""
    def put(x,y,n,solid=False):a[y*W+x]=(t[n] if isinstance(n,str) else n)|0x3000|(0xC00 if solid else 0)
    for y in range(5,13):
        for x in range(5,14):put(x,y,'FloorShadow' if x==5 or y==5 else 'Floor')
    # Long shared couch and a sideways Rocket chair surround the coffee table.
    for xx in range(6):
        put(7+xx,6,105 if xx==0 else 107 if xx==5 else 106,True)
        put(7+xx,7,108 if xx==0 else 110 if xx==5 else 109,True)
    put(12,9,'CoffeeSideTop',True);put(12,10,'CoffeeSideMiddle',True);put(12,11,'CoffeeSideSeat',True)
    for yy in range(2):
        for xx in range(2):put(9+xx,9+yy,f'Decor_Table_{xx}_{yy}',True)
    for x in (9,):put(x,5,'PlantBaseShadow',True)
    for x,y in [(5,7)]:
        put(x,y,'PlantTopShadow' if x==5 else 'PlantTop',True)
        put(x,y+1,'PlantBaseShadow' if x==5 else 'PlantBase',True)
    put(6,9,'MarketDrawerTop',True);put(6,10,'MarketDrawerBase',True)
    put(7,9,'MarketBottles',True)
    return a


def initialize(cat):
    rp=ROOT/'data/layouts/layouts.json';registry=json.loads(rp.read_text())
    l=next(v for v in registry['layouts'] if v['id']=='LAYOUT_ROGUE_AREA_DAY_CARE')
    assert l['primary_tileset']=='gTileset_GeneralHub', 'Already converted; edit the authored room in Porymap'
    OUT.mkdir(parents=True,exist_ok=True)
    for path in [l['blockdata_filepath'],l['border_filepath'],f'data/maps/{ROOM}/map.json']:
        (OUT/('legacy-'+Path(path).name)).write_bytes((ROOT/path).read_bytes())
    t=tiles(cat);a=author_layout(cat)
    l.update(width=W,height=H,primary_tileset='gTileset_HubArchitecture',secondary_tileset='gTileset_HubFurnishings')
    (ROOT/l['blockdata_filepath']).write_bytes(packed(a));(ROOT/l['border_filepath']).write_bytes(packed([t['Void']|0x3C00]*4))
    mp=ROOT/f'data/maps/{ROOM}/map.json';m=json.loads(mp.read_text());m['map_type']='MAP_TYPE_INDOOR'
    for e,(x,y) in zip(m['object_events'],POSITIONS):e.update(x=x,y=y,elevation=3)
    for i in (1,2,3):m['object_events'][i]['movement_type']='MOVEMENT_TYPE_FACE_DOWN'
    m['object_events'][7]['graphics_id']='OBJ_EVENT_GFX_PC_STAND'
    warp=lambda x,y:dict(x=x,y=y,elevation=0,dest_map='MAP_ROGUE_AREA_DAY_CARE',dest_warp_id='0')
    m['warp_events']=[warp(18,1),warp(19,1),warp(34,14),warp(34,15),warp(18,27),warp(19,27),warp(2,14),warp(2,15),warp(18,21),warp(17,1),warp(34,13),warp(17,27),warp(2,13)]
    m['bg_events']=[dict(type='sign',x=x,y=yy,elevation=3,player_facing_dir=facing,script='Nursery_'+name+'Sign') for x,y,name in SIGNS for yy,facing in [(y,'BG_EVENT_PLAYER_FACING_NORTH'),(y-1,'BG_EVENT_PLAYER_FACING_SOUTH')]]
    gp=ROOT/'data/maps/map_groups.json';groups=json.loads(gp.read_text());m['connections']=[]
    for suffix,direction,offset,cw,ch in [('Horizontal','left',11,16,8),('Horizontal','right',11,16,8),('North','up',15,8,12),('South','down',15,8,12)]:
        name='Rogue_NurseryCorridor_'+suffix;lid='LAYOUT_ROGUE_NURSERY_CORRIDOR_'+suffix.upper();mid='MAP_ROGUE_NURSERY_CORRIDOR_'+suffix.upper()
        m['connections'].append(dict(map=mid,offset=offset,direction=direction))
        if any(v['id']==lid for v in registry['layouts']):continue
        registry['layouts'].append(dict(id=lid,name=name+'_Layout',width=cw,height=ch,primary_tileset='gTileset_HubArchitecture',secondary_tileset='gTileset_HubFurnishings',border_filepath=f'data/layouts/{name}/border.bin',blockdata_filepath=f'data/layouts/{name}/map.bin'))
        folder=ROOT/f'data/layouts/{name}';folder.mkdir(parents=True,exist_ok=True);(folder/'border.bin').write_bytes(packed([t['Void']|0x3C00]*4))
        folder=ROOT/f'data/maps/{name}';folder.mkdir(parents=True,exist_ok=True)
        facade=json.loads((ROOT/'data/maps/Rogue_PortalCorridor_Horizontal/map.json').read_text());facade.update(id=mid,name=name,layout=lid)
        (folder/'map.json').write_text(json.dumps(facade,indent=2)+'\n');(folder/'scripts.pory').write_text(f'mapscripts {name}_MapScripts {{}}\n')
        groups['gMapGroup_RogueHubAreas'].append(name)
    rp.write_text(json.dumps(registry,indent=2)+'\n');gp.write_text(json.dumps(groups,indent=2)+'\n');mp.write_text(json.dumps(m,indent=2)+'\n')

def patched(base,t,connections,capacity,breeder,tea):
    code=(ROOT/'src/rogue_hub.c').read_text().split('static void RogueHub_UpdateDayCareAreaMetatiles()\n{',1)[1].split('\n}\n',1)[0]
    a=base[:];blocks=re.findall(r'if\((.*?)\)\s*\{([^}]+)\}',code,re.S)
    assert len(blocks)==8
    for condition,body in blocks:
        if 'HUB_AREA_CONN_' in condition:active=not connections[DIRECTIONS.index(re.search(r'HUB_AREA_CONN_(\w+)',condition)[1])]
        elif 'Rogue_GetCurrentDaycareSlotCount' in condition:active=capacity<int(re.search(r'< (\d)',condition)[1])
        else:active=not (tea if 'TEA_SHOP' in condition else breeder)
        if not active:continue
        calls=re.findall(r'Metatile(Set|Fill)_Tile\(([^;]+)\);',body)
        assert calls and not re.sub(r'Metatile(?:Set|Fill)_Tile\([^;]+\);','',body).strip()
        for kind,args in calls:
            args=[v.strip() for v in args.split(',')];coords=list(map(int,args[:-1]));x1,y1=coords[:2];x2,y2=coords[2:] if kind=='Fill' else coords[:2]
            name=re.search(r'METATILE_Nursery_(\w+)',args[-1])[1];v=t[name]|(0xC00 if 'MAPGRID_COLLISION_MASK' in args[-1] else 0)
            for y in range(y1,y2+1):
                for x in range(x1,x2+1):a[y*W+x]=(a[y*W+x]&0xF000)|v
    return a

def render(cat,a,path,visible=None):
    cache={v&1023:cat.render(v&1023) for v in a};im=Image.new('RGB',(W*16,H*16))
    for i,v in enumerate(a):im.paste(cache[v&1023],(i%W*16,i//W*16))
    if visible:
        for i,pic in visible:
            x,y=POSITIONS[i];im.paste(pic,(x*16+8-pic.width//2,(y+1)*16-pic.height),pic)
    path.parent.mkdir(parents=True,exist_ok=True);im.save(path)
    return im

def preview_sprites(capacity,breeder,tea,occupied=True):
    entries=[(0,'rogue/work_table',32,'npc_1'),(2,'people/old_woman',16,'npc_3'),(8,'rogue/pc_stand',16,'npc_4')]
    if capacity>1:entries.append((7,'rogue/pc_stand',16,'npc_4'))
    if breeder:entries.append((3,'people/old_man_2',16,'npc_4'))
    if tea:entries.append((1,'rogue/npc/misc/young_couple_m',16,'npc_1'))
    if occupied:
        for i,name,slot in [(4,'snorlax',0),(6,'absol',1),(5,'abomasnow',2)]:
            if slot<capacity:entries.append((i,'pokemon_ow/'+name,32,None))
    sprites=[]
    for i,name,width,pal in entries:
        pic=Image.open(ROOT/f'graphics/object_events/pics/{name}.png').crop((0,0,width,32))
        colors=palette(f'graphics/object_events/palettes/{pal}.pal') if pal else palette(f'graphics/object_events/pics/{name}.pal')
        rgba=Image.new('RGBA',pic.size)
        rgba.putdata([(*colors[v],255 if v else 0) for v in pic.get_flattened_data()])
        sprites.append((i,rgba))
    return sprites

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--init-layout',action='store_true');ap.add_argument('--check',action='store_true');args=ap.parse_args()
    assert not(args.init_layout and args.check)
    cat=Catalogue();t=tiles(cat)
    if args.init_layout:initialize(cat)
    m=json.loads((ROOT/f'data/maps/{ROOM}/map.json').read_text());ls={v['id']:v for v in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']};l=ls[m['layout']]
    assert (l['width'],l['height'])==(W,H) and m['map_type']=='MAP_TYPE_INDOOR'
    base=words(l['blockdata_filepath']);assert len(base)==W*H
    assert words(l['border_filepath'])==[t['Void']|0x3C00]*4
    outputs={'include/constants/pokemon_nursery_tiles.h':('// Generated by generate_pokemon_nursery.py; stable shared metatile IDs.\n'+''.join(f'#define METATILE_Nursery_{n} 0x{v:03X}\n' for n,v in t.items())).encode()}
    maps={json.loads(p.read_text())['id']:json.loads(p.read_text()) for p in (ROOT/'data/maps').glob('Rogue_NurseryCorridor_*/map.json')}
    for cn in m['connections']:
        fl=ls[maps[cn['map']]['layout']];horizontal=cn['direction'] in ('left','right');span=fl['height'] if horizontal else fl['width'];profile=[]
        for i in range(span):
            x,y=((0 if cn['direction']=='left' else W-1),cn['offset']+i) if horizontal else (cn['offset']+i,0 if cn['direction']=='up' else H-1)
            profile.append(base[y*W+x])
        assert sum(not v&0xC00 for v in profile)==3
        data=packed([profile[y if horizontal else x] for y in range(fl['height']) for x in range(fl['width'])])
        if fl['blockdata_filepath'] in outputs:assert outputs[fl['blockdata_filepath']]==data
        outputs[fl['blockdata_filepath']]=data
    for path,data in outputs.items():
        p=ROOT/path
        if args.check:assert p.read_bytes()==data,('Stale output',path)
        elif not p.exists() or p.read_bytes()!=data:p.write_bytes(data)
    assert len(m['object_events'])==9 and len(m['warp_events'])==13
    assert [(e['x'],e['y']) for e in m['object_events']]==POSITIONS
    assert [e['flag'] for e in m['object_events']]==['0','FLAG_TEMP_4','0','FLAG_TEMP_1','FLAG_TEMP_5','FLAG_TEMP_7','FLAG_TEMP_6','FLAG_TEMP_2','0']
    assert [e['graphics_id'] for e in m['object_events'][4:7]]==['OBJ_EVENT_GFX_FOLLOW_MON_0','OBJ_EVENT_GFX_FOLLOW_MON_2','OBJ_EVENT_GFX_FOLLOW_MON_1']
    assert all(e['elevation']==3 for e in m['object_events'])
    assert all(m['object_events'][i]['movement_type']=='MOVEMENT_TYPE_FACE_DOWN' for i in (1,2,3))
    assert (m['warp_events'][8]['x'],m['warp_events'][8]['y'])==(18,21)
    for x,y,name in SIGNS:
        for yy,facing in [(y,'BG_EVENT_PLAYER_FACING_NORTH'),(y-1,'BG_EVENT_PLAYER_FACING_SOUTH')]:
            assert any(e['x']==x and e['y']==yy and e['script']=='Nursery_'+name+'Sign' and e['player_facing_dir']==facing and e['elevation']==3 for e in m['bg_events'])
    assert not any(v&1023 in {mid for name,mid in t.items() if name.startswith(('Glass','DayCare'))} for v in base), 'Outdoor plants or glass remain'
    count=0
    for conn in itertools.product((False,True),repeat=4):
      for cap0,cap1,breeder,phone,tea in itertools.product((False,True),repeat=5):
        capacity=3 if cap1 else 2 if cap0 else 1
        a=patched(base,t,conn,capacity,breeder,tea)
        for x in (16,20):
            assert [a[y*W+x]&1023 for y in (3,4,5)]==[t[n] for n in ('InnerJoin','InnerBase','InnerFoot')], 'Incomplete north wall foot'
        if breeder:
            for x,y,n in [(22,5,'InnerJoin'),(22,6,'InnerBase'),(22,7,'InnerFoot'),(22,12,'InnerBase'),(22,13,'InnerFoot')]:assert a[y*W+x]&1023==t[n], ('Egg-room post',x,y)
        if tea:
            for x,y,n in [(12,18,'InnerJoin'),(12,19,'InnerBase'),(12,20,'InnerFoot'),(12,24,'Pillar')]:assert a[y*W+x]&1023==t[n], ('Tea-room post',x,y)
        if not breeder:assert all(a[5*W+x]&1023==t['FloorShadowDim40'] for x in range(23,32)), 'Bright closed-room shadow'
        for slot,(x,y) in enumerate(SEATS):
            expected=109 if slot<2 else t['CoffeeSideMiddle']
            if slot>=capacity:expected=t['CoffeeMiddleSeatDim' if slot==1 else 'CoffeeSideMiddleDim']
            assert a[y*W+x]&1023==expected, 'Incorrect active/inactive couch seat'
            assert cat.attrs[expected]>>12==1, 'Couch must render below Pokemon'
        if not breeder:assert a[12*W+22]&1023==t['InnerBase'], 'Closed egg-room footer'
        if not tea:assert a[18*W+12]&1023==t['Pillar'], 'Closed tea-room pillar seam'
        if not tea:assert all(a[19*W+x]&1023==t['FloorShadowDim40'] for x in range(5,12))
        else:
            for x in (6,10):assert a[19*W+x]&1023==t['PlantTopShadow'] and a[20*W+x]&1023==t['PlantBase'], 'Incomplete tea-room pot'
        assert all(not a[y*W+x]&0xC00 for x in (17,18,19) for y in range(5,26)), 'Blocked gallery spine'

        for name,fg in cat.nursery_foregrounds.items():
            assert cat.metas[t['DayCare'+name]][4:]==fg
        for occupancy in itertools.product((False,True),repeat=3):
            visible=[True,tea,True,breeder,occupancy[0],occupancy[2] and capacity>=3,occupancy[1] and capacity>=2,phone,True]
            occupied={p for p,v in zip(POSITIONS,visible) if v}
            def clear(p):return 0<=p[0]<W and 0<=p[1]<H and not a[p[1]*W+p[0]]&0xC00 and p not in occupied
            seen={(18,21)};q=deque(seen);assert clear((18,21))
            while q:
                x,y=q.popleft()
                for dx,dy in VECTORS:
                    p=x+dx,y+dy
                    if p not in seen and clear(p):seen.add(p);q.append(p)
            for i,((x,y),v) in enumerate(zip(POSITIONS,visible)):
                if not v:continue
                if i not in (4,5,6):assert not a[y*W+x]&0xC00, ('Object in solid tile',i)
                if i in (1,2,3):
                    assert cat.attrs[a[(y+1)*W+x]&1023]&255==0x80
                    assert (x,y+2) in seen,('Counter approach',i)
                elif i in (4,5,6):
                    assert a[y*W+x]&1023==(t['CoffeeSideMiddle'] if i==5 else 109), 'Pokemon is not on its couch'
                    assert ((x-1,y) if i==5 else (x,y+1)) in seen, ('Couch front interaction',i)
                elif i in (7,8):assert (x,y+1) in seen,('Terminal approach',i)
                else:assert any((x+dx,y+dy) in seen for dx,dy in VECTORS),('Object approach',i)
            for p in [(14,9),(14,12),(27,13),(14,22),(14,25),(8,16)]:assert p in seen,('Aisle sign',p)
            for p,opened in [((27,10),breeder),((8,19),tea)]:assert (p in seen)==opened,('Room sign',p)
            assert all(p in seen for p in SEAT_APPROACHES), 'Coffee seating approaches'
            assert ((27,10) in seen)==breeder and ((9,20) in seen)==tea
            for d,(cx,cy) in enumerate(CENTERS):
                dx,dy=VECTORS[d]
                for lane in (-1,0,1):
                    p=cx+(lane if dy else 0),cy+(lane if dx else 0)
                    assert (p in seen)==conn[d],('Exit',d,p)
                    assert any((e['x'],e['y'])==p for e in m['warp_events'])
                    if conn[d]:
                        assert (p[0]-dx,p[1]-dy) in seen
                        qx,qy=p
                        while 0<=qx<W and 0<=qy<H:
                            assert (qx,qy) in seen, 'Blocked facade lane'
                            qx+=dx;qy+=dy
            count+=1
    for name,capacity,breeder,tea in [('locked',1,False,False),('partial',2,True,False),('full',3,True,True),('empty',3,True,True)]:
        a=patched(base,t,(True,)*4,capacity,breeder,tea);im=render(cat,a,OUT/f'{name}.png',preview_sprites(capacity,breeder,tea,name!='empty'))
        for label,x,y in [('care',10,9),('eggs',27,8),('reception',14,20),('tea',8,21)]:
            crop=im.crop((x*16-120,y*16-80,x*16+120,y*16+80));crop.save(OUT/f'{name}-{label}-native.png')
    cache={i:cat.render(i) for i in {v&1023 for v in base}|set(t.values())}
    sheet=Image.new('RGB',(W*16*4,H*16*4))
    for i,conn in enumerate(itertools.product((False,True),repeat=4)):
        a=patched(base,t,conn,3,True,True)
        for j,v in enumerate(a):sheet.paste(cache[v&1023],((i%4*W+j%W)*16,(i//4*H+j//W)*16))
    sheet.save(OUT/'connection-states.png')
    print(f'Pokemon Nursery: {count} connection/upgrade/occupancy states PASS; authored layout and border preserved.')

if __name__=='__main__':main()

