#!/usr/bin/env python3
"""Private Safari lab assets; normal generation never overwrites Porymap maps."""
import argparse
import io
import json
import re
from collections import deque
from pathlib import Path
from PIL import Image, ImageDraw
from generate_portal_room import ROOT, Builder, Source, words, packed, png, palette

OUT=ROOT/'build/safari_lab'
PRI='data/tilesets/primary/safari_lab'
SEC='data/tilesets/secondary/safari_lab'
W,H=38,32
HABITATS=((8,9),(23,9))
CAPSULES=((9,5),(25,5))

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--check',action='store_true');ap.add_argument('--init-layout',action='store_true');args=ap.parse_args()
    assert not(args.check and args.init_layout)
    b=Builder();b.sources['junction']=Source('lab_junction','lab_junction');b.sources['safari']=Source('general_hub','fallarbor');b.sources['lab']=Source('building','lab')
    layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
    refs={l['name'].replace('_Layout',''):l for l in layouts}
    def ref(name,key,mapname,x,y,behavior=0):
        l=refs[mapname];mid=words(l['blockdata_filepath'])[y*l['width']+x]&1023
        b.add(name,key,mid,behavior);b.provenance[-1]['reference']={'map':mapname,'x':x,'y':y}
    ids={n:int(v,16) for n,v in re.findall(r'METATILE_LabJunction_(\w+) (0x\w+)',(ROOT/'include/constants/lab_junction_tiles.h').read_text())}
    for n in ('Floor','FloorShadow','Void','PillarCap','PillarJoin','Pillar','PillarEnd','Wall','WallBase','InnerJoin','InnerBase','InnerFoot','SignTop','SignBase'):
        b.add(n,'junction',ids[n])
    # Record the outdoor donor IDs once; later runs read the manifest rather
    # than treating the now-private Safari map as an outdoor source.
    manifest=ROOT/'data/tilesets/safari_lab_sources.json'
    rock_ids=json.loads(manifest.read_text())['rock'] if manifest.exists() else []
    for y in range(6):
        for x in range(7):
            n=f'Rock_{x}_{y}'
            mid=next((p['metatile'] for p in rock_ids if p['name']==n),None)
            if mid is None:
                mid=words('data/layouts/Rogue_Area_SafariZone/map.bin')[y*W+x+15]&1023
                rock_ids.append({'name':n,'metatile':mid,'behavior':0})
            b.add(n,'safari',mid)
    for name,mid,behavior in [('CaveTop',0x259,0),('CaveDoor',0x261,0x60)]:b.add(name,'safari',mid,behavior)
    # The Safari rock uses its own palette bank; neutralize only those banks.
    rock_pals={v>>12 for n in [p['name'] for p in rock_ids]+['CaveTop','CaveDoor'] for v in b.metas[b.names[n]] if v}
    for pi in rock_pals:
        b.pals[pi]=[tuple([round((r*.3+g*.59+bl*.11)/8)*8]*3) for r,g,bl in b.pals[pi]]
    # Hardware reference: the complete Galactic HQ specimen machine at (1,1).
    tank=Image.new('RGB',(32,48))
    for y in range(3):
        for x in range(2):
            n=f'Tank_{x}_{y}';ref(n,'galactic','Rogue_Encounter_GalacticHQ',1+x,1+y);tank.paste(b.render(b.names[n]),(16*x,16*y))
    # Glass frames use the existing machine's neutral hardware colors, with a
    # restrained cyan bank for reflective edges. Interior stays transparent.
    glass=[(0,0,0),(48,56,72),(80,88,104),(128,144,152),(184,200,200),(232,240,240),(64,104,120),(96,152,168),(144,200,208),(192,232,232),(32,64,80),(112,120,136),(216,104,88),(248,192,120),(72,80,96),(160,168,184)]
    gp=len(b.pals);b.pals.append(glass)
    capsule_bg_pal=len(b.pals)
    b.pals.append(b.pals[b.metas[b.names['Floor']][0]>>12][:])
    b.pals[capsule_bg_pal][0]=(0,0,0)
    def foreground(name,im,bg='Floor'):
        vals=b.metas[b.names[bg]][:4]
        for ty in range(2):
            for tx in range(2):
                raw=bytes(im.crop((tx*8,ty*8,tx*8+8,ty*8+8)).get_flattened_data())
                if not any(raw):vals.append(0);continue
                if raw not in b.tiles:b.tiles.append(raw)
                vals.append(b.tiles.index(raw)|(gp<<12))
        # Covered metatiles put both tile layers behind nearby sprites. A split
        # layer would hide the head of a player standing south of the plinth.
        b.names[name]=len(b.metas);b.metas.append(vals);b.attrs.append(0x1000)
    # A continuous glazed frame: metal plinth, narrow mullions and highlights.
    # Hardware proportions follow HQ machine frames; no opaque blue sheet is
    # laid over the Pokémon. Six by five tiles includes a 4x3 interior.
    habitat=Image.new('P',(96,80));d=ImageDraw.Draw(habitat)
    d.rounded_rectangle((2,2,93,77),radius=5,outline=1,width=4)
    d.line((7,3,88,3),fill=5,width=2);d.line((7,7,88,7),fill=3,width=3)
    for x in (5,89):d.line((x,10,x,65),fill=7,width=3);d.line((x+1,12,x+1,62),fill=9)
    d.rectangle((5,65,90,75),fill=2);d.line((5,65,90,65),fill=5,width=2);d.line((7,72,88,72),fill=1,width=2)
    for x in (22,69):d.line((x,11,x-5,20),fill=9);d.line((x+3,11,x-2,20),fill=7)
    # A capsule with the HQ machine dome above a wider transparent viewing area.
    capsule=Image.new('P',(48,64));d=ImageDraw.Draw(capsule)
    d.rounded_rectangle((3,1,44,61),radius=15,fill=2,outline=1,width=2)
    d.rounded_rectangle((5,3,42,57),radius=13,fill=4)
    d.rounded_rectangle((7,10,40,54),radius=7,fill=0,outline=7,width=2)
    d.arc((5,2,42,23),190,350,fill=5,width=2)
    d.line((7,20,7,47),fill=9);d.line((40,20,40,47),fill=6)
    d.rectangle((5,54,42,61),fill=2);d.line((6,54,41,54),fill=5);d.line((10,58,30,58),fill=1);d.rectangle((34,57,37,59),fill=8)
    # Tile-space foreground frames are authored here; source maps remain untouched.
    for label,im in [('Habitat',habitat),('Capsule',capsule)]:
        for y in range(im.height//16):
            for x in range(im.width//16):
                n=f'{label}_{x}_{y}';foreground(n,im.crop((x*16,y*16,x*16+16,y*16+16)))
                if label=='Capsule':
                    # Outside the rounded housing follows the actual rear wall,
                    # while its window has its own floor. No floating floor box.
                    back=b.render(b.names[('Void','Wall','WallBase','Floor')[y]])
                    floor=Image.new('RGB',(16,16),(106,123,123));pi=capsule_bg_pal
                    mask=Image.new('1',(48,64));ImageDraw.Draw(mask).rounded_rectangle((3,1,44,61),radius=15,fill=1)
                    for ty in range(2):
                        for tx in range(2):
                            raw=[]
                            for yy in range(8):
                                for xx in range(8):
                                    px=tx*8+xx;py=ty*8+yy
                                    color=(floor if mask.getpixel((x*16+px,y*16+py)) else back).getpixel((px,py))
                                    raw.append(min(range(16),key=lambda c:sum((b.pals[pi][c][k]-color[k])**2 for k in range(3))))
                            raw=bytes(raw)
                            if raw not in b.tiles:b.tiles.append(raw)
                            b.metas[b.names[n]][ty*2+tx]=b.tiles.index(raw)|(pi<<12)
    for x in range(6):
        broken=habitat.crop((x*16,64,x*16+16,80));d=ImageDraw.Draw(broken)
        if x in (2,3):d.rectangle((0,0,15,15),fill=0);d.line((0,10,4,7,7,13,11,10,15,14),fill=7)
        foreground(f'BrokenHabitat_{x}',broken)
    warning=Image.new('P',(16,16));d=ImageDraw.Draw(warning);d.rectangle((4,4,11,10),fill=1);d.rectangle((5,5,10,8),fill=12);foreground('Warning',warning,'WallBase')
    floorpal=b.metas[b.names['Floor']][0]>>12
    shades={}
    for percent in (80,60,40,20):shades[percent]=len(b.pals);b.pals.append([tuple(round(c*percent/800)*8 for c in color) for color in b.pals[floorpal]])
    for base in ('Floor','FloorShadow'):
        for suffix,levels in [('West',(60,80,60,80)),('East',(80,60,80,60)),('South',(80,80,60,60)),('Dim40',(40,)*4),('Dim20',(20,)*4)]:
            n=base+suffix;b.alias(n,base,0)
            for j,v in enumerate(b.metas[b.names[n]]):
                if v:b.metas[b.names[n]][j]=(v&0xFFF)|(shades[levels[j%4]]<<12)
    for n,base,mb in [('WarpWest','FloorDim40',0x63),('WarpEast','FloorDim40',0x62),('WarpSouth','FloorDim40',0x65),('WarpWestTop','FloorShadowDim40',0x63),('WarpEastTop','FloorShadowDim40',0x62)]:b.alias(n,base,mb)
    # Remove import-only unused banks and tiles after composing the source pieces.
    used=sorted({0}|{v>>12 for meta in b.metas for v in meta if v})
    for meta in b.metas:
        for i,v in enumerate(meta):meta[i]=(v&0xFFF)|(used.index(v>>12)<<12)
    b.pals=[b.pals[i] for i in used]
    from hub_furnishings import append_hub_furnishings
    append_hub_furnishings(b)
    assert len(b.tiles)<=512 and len(b.pals)<=13,(len(b.tiles),len(b.pals))
    def t(name,solid=False):return b.names[name]|0x3000|(0xC00 if solid else 0)
    def fill(a,x1,y1,x2,y2,n,solid=False):
        for y in range(y1,y2+1):
            for x in range(x1,x2+1):a[y][x]=t(n,solid)
    def stamp(a,label,x,y,w,h):
        for dy in range(h):
            for dx in range(w):a[y+dy][x+dx]=t(f'{label}_{dx}_{dy}',dx in (0,w-1) or dy in (0,h-1))
    maps={}
    if args.init_layout:
        assert refs['Rogue_Area_SafariZone']['primary_tileset']=='gTileset_GeneralHub'
        a=[[t('Void',True)]*W for _ in range(H)]
        fill(a,7,8,29,22,'Floor');fill(a,7,8,29,8,'FloorShadow')
        for y,n in [(6,'Wall'),(7,'WallBase'),(24,'Wall'),(25,'WallBase')]:fill(a,7,y,29,y,n,True)
        for x in (6,30):
            fill(a,x,5,x,25,'Pillar',True)
            for y,n in [(5,'PillarCap'),(6,'PillarJoin'),(12,'InnerJoin'),(13,'InnerBase'),(14,'InnerFoot'),(17,'PillarCap'),(18,'PillarJoin'),(24,'PillarJoin'),(25,'PillarEnd')]:a[y][x]=t(n,y!=14)
            fill(a,x,15,x,16,'Floor')
        for x in list(range(6))+list(range(31,38)):
            for y,n in [(12,'Wall'),(13,'WallBase'),(17,'Void'),(18,'Wall'),(19,'WallBase')]:a[y][x]=t(n,True)
            for y in (14,15,16):
                base='FloorShadow' if y==14 else 'Floor';distance=5-x if x<6 else x-31
                a[y][x]=t(base+('West' if x<6 else 'East') if distance==0 else base+('Dim40' if distance==1 else 'Dim20'))
        for y in (14,15,16):a[y][4]=t('WarpWestTop' if y==14 else 'WarpWest');a[y][32]=t('WarpEastTop' if y==14 else 'WarpEast')
        for x in (16,20):
            fill(a,x,23,x,31,'Pillar',True);a[23][x]=t('PillarCap',True);a[24][x]=t('PillarJoin',True)
        fill(a,17,23,19,31,'Floor')
        for x in (17,18,19):
            for y,n in [(26,'FloorSouth'),(27,'WarpSouth'),(28,'FloorDim20'),(29,'FloorDim20'),(30,'FloorDim20'),(31,'FloorDim20')]:a[y][x]=t(n)
        for y in range(6):
            for x in range(7):a[y+2][x+15]=t(f'Rock_{x}_{y}',True)
        for x in (14,22):
            fill(a,x,1,x,7,'Pillar',True);a[1][x]=t('PillarCap',True)
        for x,y in HABITATS:stamp(a,'Habitat',x,y,6,5)
        for x,y in CAPSULES:stamp(a,'Capsule',x,y,3,4)
        for x1,x2 in ((7,13),(23,29)):
            for y,n in ((17,'Void'),(18,'Wall'),(19,'WallBase')):fill(a,x1,y,x2,y,n,True)
        for x in (14,22):
            for y,n in ((17,'PillarCap'),(18,'InnerJoin'),(19,'InnerBase'),(20,'InnerFoot')):a[y][x]=t(n,y!=20)
        normal=[r[:] for r in a];tutorial=[r[:] for r in a]
        for x,y in HABITATS:
            for dx in range(6):tutorial[y+4][x+dx]=t(f'BrokenHabitat_{dx}',dx not in (2,3))
        tutorial[7][13]=t('Warning',True);tutorial[7][23]=t('Warning',True)
        maps={'Rogue_Area_SafariZone':normal,'Rogue_Area_SafariZone_Tutorial':tutorial}
    else:
        for name in ('Rogue_Area_SafariZone','Rogue_Area_SafariZone_Tutorial'):
            a=words(f'data/layouts/{name}/map.bin');assert len(a)==W*H;maps[name]=[a[y*W:(y+1)*W] for y in range(H)]
    outputs={PRI+'/tiles.png':png(b.tiles),PRI+'/metatiles.bin':packed(sum(b.metas,[])),PRI+'/metatile_attributes.bin':packed(b.attrs),SEC+'/tiles.png':png([bytes(64)],16),SEC+'/metatiles.bin':packed([0]*8),SEC+'/metatile_attributes.bin':packed([0])}
    for i in range(13):
        pal=b.pals[i] if i<len(b.pals) else [(0,0,0)]*16
        outputs[(PRI if i<6 else SEC)+f'/palettes/{i:02}.pal']=('JASC-PAL\r\n0100\r\n16\r\n'+'\r\n'.join(' '.join(map(str,c)) for c in pal)+'\r\n').encode()
    outputs['include/constants/safari_lab_tiles.h']=('#ifndef GUARD_SAFARI_LAB_TILES_H\n#define GUARD_SAFARI_LAB_TILES_H\n\n'+''.join(f'#define METATILE_SafariLab_{n} 0x{v:03X}\n' for n,v in b.names.items())+'\n#endif\n').encode()
    outputs['data/tilesets/safari_lab_sources.json']=(json.dumps({'rock':rock_ids,'hardware':'Galactic HQ complete specimen machinery (1,1), 2x3; lab junction structural metatiles','containment':'Private glass frames and expanded capsule housing; normal floor below transparent windows','pc':'Existing dark Adventure Console sprite','furnishings':[p for p in b.provenance if p['name'].startswith('Decor_')],'tiles':len(b.tiles),'palettes':len(b.pals)},indent=2)+'\n').encode()
    for suffix,w,h in [('Horizontal',16,8),('Vertical',8,12)]:
        arr=[]
        for y in range(h):
            for x in range(w):
                if suffix=='Horizontal':n=['Wall','WallBase','FloorShadowDim20','FloorDim20','FloorDim20','Void','Wall','WallBase'][y];solid=y not in (2,3,4)
                else:n='FloorDim20' if x in (2,3,4) else 'Pillar' if x in (1,5) else 'Void';solid=x not in (2,3,4)
                arr.append(t(n,solid))
        outputs[f'data/layouts/Rogue_SafariCorridor_{suffix}/map.bin']=packed(arr);outputs[f'data/layouts/Rogue_SafariCorridor_{suffix}/border.bin']=packed([t('Void',True)]*4)
    if args.init_layout:
        for name,a in maps.items():outputs[f'data/layouts/{name}/map.bin']=packed(sum(a,[]));outputs[f'data/layouts/{name}/border.bin']=packed([t('Void',True)]*4)
    for path,data in outputs.items():
        p=ROOT/path
        if args.check:assert p.exists() and p.read_bytes()==data,('Stale asset',path)
        elif not p.exists() or p.read_bytes()!=data:p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
    def effective(a,mask,cave):
        a=[r[:] for r in a]
        for bit,x1,x2,pillar in ((1,0,5,6),(4,31,37,30)):
            if not mask&bit:
                fill(a,x1,12,x2,19,'Void',True);fill(a,pillar,12,pillar,17,'Pillar',True)
        if not mask&2:
            for y,n in ((23,'Void'),(24,'Wall'),(25,'WallBase')):fill(a,16,y,20,y,n,True)
            fill(a,16,26,20,31,'Void',True)
        if cave:a[6][18]=t('CaveTop',True);a[7][18]=t('CaveDoor')
        return a
    def render(a):
        im=Image.new('RGB',(W*16,H*16))
        for y,row in enumerate(a):
            for x,v in enumerate(row):
                assert (v&1023)<len(b.metas)
                im.paste(b.render(v&1023),(x*16,y*16))
        return im
    def sprite(im,pic,pal,x,y,w=16,h=32):
        src=Image.open(ROOT/f'graphics/object_events/pics/{pic}.png').crop((0,0,w,h))
        colors=palette(pal);rgba=Image.new('RGBA',(w,h))
        for yy in range(h):
            for xx in range(w):
                c=src.getpixel((xx,yy));rgba.putpixel((xx,yy),(*colors[c],255 if c else 0))
        im.paste(rgba,(x*16+8-w//2,(y+1)*16-h),rgba)
    def actors(a,events,tutorial,populated=True,workbench=True):
        im=render(a)
        for e in sorted(events['object_events'],key=lambda e:e['y']):
            gfx=e['graphics_id'];x,y=e['x'],e['y']
            if gfx=='OBJ_EVENT_GFX_WORK_TABLE' and workbench:sprite(im,'rogue/work_table','graphics/object_events/palettes/npc_1.pal',x,y,32)
            elif gfx=='OBJ_EVENT_GFX_ADVENTURE_CONSOLE':sprite(im,'rogue/adventure_console','graphics/object_events/palettes/npc_4.pal',x,y)
            elif gfx=='OBJ_EVENT_GFX_PROF_BIRCH':sprite(im,'people/prof_birch','graphics/object_events/palettes/npc_3.pal',18,17)
            elif gfx=='OBJ_EVENT_GFX_MISC_BACKPACKER_F':sprite(im,'rogue/npc/misc/backpacker_f','graphics/object_events/palettes/npc_1.pal',x,y)
            elif gfx.startswith('OBJ_EVENT_GFX_FOLLOW_MON_') and populated:
                slot=int(gfx.rsplit('_',1)[1]);mon=('bulbasaur','charmander','squirtle','pichu')[slot]
                sprite(im,'pokemon_ow/'+mon,f'graphics/object_events/pics/pokemon_ow/{mon}.pal',x,y,32)
        # Honor any split-layer metatiles above objects, as the GBA does.
        for y,row in enumerate(a):
            for x,cell in enumerate(row):
                mid=cell&1023
                if b.attrs[mid]&0xF000!=0x2000:continue
                for j,v in enumerate(b.metas[mid][4:]):
                    raw=b.tiles[v&1023];pal=b.pals[v>>12]
                    for yy in range(8):
                        for xx in range(8):
                            c=raw[(7-yy if v&2048 else yy)*8+(7-xx if v&1024 else xx)]
                            if c:im.putpixel((x*16+(j%2)*8+xx,y*16+(j//2)*8+yy),pal[c])
        return im
    OUT.mkdir(parents=True,exist_ok=True)
    checks=0
    for name,base in maps.items():
        tutorial=name.endswith('_Tutorial');events=json.loads((ROOT/f'data/maps/{name}/map.json').read_text())
        assert events['map_type']=='MAP_TYPE_INDOOR' and events['weather']=='WEATHER_NONE'
        assert words(f'data/layouts/{name}/border.bin')==[t('Void',True)]*4
        for y in range(1,26):
            for x in range(6,31):assert (base[y][x]&0xC00)==(base[y][36-x]&0xC00),('Asymmetric structure',name,x,y)
        exit_points={(4,y) for y in (14,15,16)}|{(32,y) for y in (14,15,16)}|{(x,27) for x in (17,18,19)}
        assert exit_points<={(e['x'],e['y']) for e in events['coord_events' if tutorial else 'warp_events']}
        if tutorial:
            assert len(events['coord_events'])==9
            assert all(e['var']=='VAR_ROGUE_INTRO_STATE' and e['var_value']==re.search(r'#define ROGUE_INTRO_STATE_CATCH_MON\s+(\d+)',(ROOT/'include/constants/rogue.h').read_text()).group(1) for e in events['coord_events'])
        for mask in range(8):
            for cave in (False,True):
                for workbench in (False,True):
                    a=effective(base,mask,cave and not tutorial)
                    blocked={(e['x'],e['y']) for e in events['object_events'] if e['graphics_id']=='OBJ_EVENT_GFX_ADVENTURE_CONSOLE' or workbench and e['graphics_id']=='OBJ_EVENT_GFX_WORK_TABLE'}
                    def legal(x,y):return 0<=x<W and 0<=y<H and not a[y][x]&0xC00 and (x,y) not in blocked
                    seen={(18,18)};q=deque(seen)
                    while q:
                        x,y=q.popleft()
                        for p in ((x-1,y),(x+1,y),(x,y-1),(x,y+1)):
                            if p not in seen and legal(*p):seen.add(p);q.append(p)
                    for p in ((14,9),(9,21),(27,21),(18,8)):assert p in seen,(name,mask,p)
                    for bit,lanes in ((1,[(4,y) for y in (14,15,16)]),(4,[(32,y) for y in (14,15,16)]),(2,[(x,27) for x in (17,18,19)])):
                        for p in lanes:assert (p in seen)==bool(mask&bit),(name,mask,p)
                    assert ((18,7) in seen)==bool(cave and not tutorial)
                    for e in events['object_events']:
                        if not e['graphics_id'].startswith('OBJ_EVENT_GFX_FOLLOW_MON_'):continue
                        x,y=e['x'],e['y']
                        assert (x,y) in seen if tutorial else (x,y) not in seen
                        for dx in range(-e['movement_range_x'],e['movement_range_x']+1):
                            for dy in range(-e['movement_range_y'],e['movement_range_y']+1):assert legal(x+dx,y+dy),(name,e['local_id'],dx,dy)
                    checks+=1
        # Verify pixels at each facade join rather than only comparing IDs.
        horizontal=words('data/layouts/Rogue_SafariCorridor_Horizontal/map.bin')
        vertical=words('data/layouts/Rogue_SafariCorridor_Vertical/map.bin')
        for y in range(8):
            for x in (0,37):assert b.render(base[y+12][x]&1023).tobytes()==b.render(horizontal[y*16]&1023).tobytes()
        for x in range(8):assert b.render(base[31][x+15]&1023).tobytes()==b.render(vertical[x]&1023).tobytes()
        render(base).save(OUT/(name+'.png'))
        for label,mask,cave,populated,workbench in [('room',7,False,True,True),('unlocked',7,True,True,True),('empty',7,False,False,False),('west-only',1,False,True,True)]:
            a=effective(base,mask,cave and not tutorial);scene=actors(a,events,tutorial,populated,workbench)
            stem=('tutorial-' if tutorial else '')+label
            scene.save(OUT/(stem+'.png'))
            for crop,x,y in [('habitats',18,10),('pc',11,10),('crossing',18,16),('workbenches',18,21)]:
                native=scene.crop((x*16-120,y*16-80,x*16+120,y*16+80));native.save(OUT/f'{stem}-{crop}-native.png')
                native.resize((720,480),Image.Resampling.NEAREST).save(OUT/f'{stem}-{crop}.png')
    print(f'Safari lab: {checks} layout states checked; {len(b.tiles)} tiles, {len(b.pals)} palettes. Previews: {OUT}')

if __name__=='__main__':main()
