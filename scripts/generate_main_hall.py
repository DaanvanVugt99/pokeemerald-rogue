#!/usr/bin/env python3
"""Generate private Main Hall tiles; read, never overwrite, Porymap layouts.

--init-layout is only for the first conversion of the outdoor Town Square.
See docs/design/main_hall.md for donor assemblies and runtime contracts.
"""
import argparse
import io
import json
from collections import deque
from PIL import Image, ImageDraw
from generate_portal_room import ROOT, Builder, Source, words, packed, png, palette

OUT = ROOT/'build/main_hall'
PRI = 'data/tilesets/primary/main_hall'
SEC = 'data/tilesets/secondary/main_hall'
MAP = 'data/layouts/Rogue_Area_TownSquare'
W, H = 36, 30


def main():
    # Converted districts share one asset pipeline. Retain the original
    # composition recipes below as provenance for the immutable source art.
    if (ROOT/'tools/data/hub_tiles.json').exists():
        from generate_hub_tiles import main as generate_shared_hub
        return generate_shared_hub()
    ap=argparse.ArgumentParser();ap.add_argument('--check',action='store_true');ap.add_argument('--init-layout',action='store_true');args=ap.parse_args()
    assert not(args.check and args.init_layout)
    b=Builder();b.sources['silph']=Source('general','facility_rocket','facility')
    b.sources['town_square']=Source('general_hub','slateport')
    layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
    sources={}
    for key,lid in [('galactic','ROGUE_ENCOUNTER_GALACTIC_HQ'),('silph','ROGUE_ENCOUNTER_SILPH_CO')]:
        l=next(x for x in layouts if x['id']=='LAYOUT_'+lid);sources[key]=(words(l['blockdata_filepath']),l['width'],l['id'])
    def ref(name,key,x,y):
        cells,w,lid=sources[key];b.add(name,key,cells[y*w+x]&1023)
        b.provenance[-1]['reference']={'layout':lid,'x':x,'y':y}
    for name,x,y in [('Floor',3,4),('FloorShadow',1,4),('Void',1,0),('PillarCap',0,0),('PillarJoin',0,1),('Pillar',0,4),('PillarEnd',0,8),('Wall',4,1),('WallBase',4,2),('InnerJoin',5,12),('InnerBase',5,13),('InnerFoot',5,14),('FrameTopLeft',1,12),('FrameTop',2,12),('FrameTopRight',3,12),('FrameLeft',1,13),('FrameRight',3,13),('Door',2,13),('SignTop',11,1),('SignBase',11,2)]:ref(name,'galactic',x,y)
    b.attrs[b.names['Door']]=0x69
    b.alias('ClosedDoor','Door',0)
    assemblies=[]
    def assembly(label,key,sx,sy,w,h):
        for y in range(h):
            for x in range(w):ref(f'{label}_{x}_{y}',key,sx+x,sy+y)
        assemblies.append({'name':label,'layout':sources[key][2],'source':[sx,sy,w,h]})
    assembly('Sofa','silph',9,3,3,2)
    assembly('Plant','silph',5,2,1,2)
    assembly('Monitor','galactic',8,2,1,2)
    # These furnishings use the donor's foreground above its own carpet/floor.
    # Put that intact foreground on HQ flooring. Plant tops remain against walls.
    for label,w,h in [('Sofa',3,2),('Plant',1,2),('Monitor',1,2)]:
        for y in range(h):
            for x in range(w):
                n=b.names[f'{label}_{x}_{y}'];base='WallBase' if y==0 and label in ('Plant','Monitor') else 'Floor'
                b.metas[n][:4]=b.metas[b.names[base]][:4];b.attrs[n]=0x1000
    # The Secret Base stand supplies the tread cadence and vertical metal fascia.
    # Remap only this private copy to the hall's existing neutral wall palette.
    wallpal=len(b.pals)
    b.pals.append([(0,0,0),(32,40,56),(48,56,72),(56,64,80),
                   (80,88,104),(104,112,128),(120,128,144),(144,152,168),
                   (168,176,192),(184,192,200),(208,216,224),(224,232,232),
                   (64,80,96),(80,128,144),(208,176,112),(248,248,248)])
    def raster(name,im,layer=0x1000):
        vals=[];colors=b.pals[wallpal]
        for ty in range(2):
            for tx in range(2):
                tile=bytes(min(range(16),key=lambda k:sum((colors[k][c]-im.getpixel((tx*8+x,ty*8+y))[c])**2 for c in range(3))) for y in range(8) for x in range(8))
                if tile not in b.tiles:b.tiles.append(tile)
                vals.append(b.tiles.index(tile)|(wallpal<<12))
        b.names[name]=len(b.metas);b.metas.append(vals+[0]*4);b.attrs.append(layer)
    def pedestal(name,mid):
        # Original Town Square's complete 3x2 pedestal, preserving its foreground
        # pixels and plaque. Replace only its outdoor background with HQ floor.
        src=b.sources['town_square'];vals=b.metas[b.names['Floor']][:4]
        for word in src.meta[1][(mid-512)*8+4:(mid-512+1)*8]:
            pixels=src.tile(word);colors=src.pal(word>>12)
            recolored=bytes(0 if not c else min(range(1,16),key=lambda k:sum((b.pals[wallpal][k][channel]-colors[c][channel])**2 for channel in range(3))) for c in pixels)
            if recolored not in b.tiles:b.tiles.append(recolored)
            vals.append(b.tiles.index(recolored)|(word&0xC00)|(wallpal<<12))
        b.names[name]=len(b.metas);b.metas.append(vals);b.attrs.append(0x1000)
        b.provenance.append({'name':name,'source':'town_square','metatile':f'0x{mid:03X}','reference':'Original TownSquare (9..11,4..5); intact foreground, HQ floor background, private neutral palette'})
    b.add('StandReference','rock',0x272)
    tread=b.render(b.names['StandReference'])
    for name,edge in [('StairLeft','left'),('StairCenter',None),('StairRight','right')]:
        im=Image.new('RGB',(16,16))
        # Repeat the verified right staircase's horizontal bands across a 3-tile flight.
        for y in range(16):
            for x in range(16):im.putpixel((x,y),tread.getpixel((12,y)))
        d=ImageDraw.Draw(im)
        if edge=='left':d.line((0,0,0,15),fill=(56,64,80));d.line((1,0,1,15),fill=(184,192,200))
        if edge=='right':d.line((15,0,15,15),fill=(56,64,80));d.line((14,0,14,15),fill=(184,192,200))
        raster(name,im)
    b.metas[b.names['StandReference']]=b.metas[b.names['StairCenter']][:]
    b.attrs[b.names['StandReference']]=0x1000
    # Broad, two-row metal frontage, fully solid; its lower edge stays behind actors.
    for name,base in [('GalleryRail','Wall'),('GalleryFace','WallBase')]:b.alias(name,base,0x1000)
    # Private floor inlay and plinth: existing HQ pixels and neutral palette only.
    for suffix,sides in [('N','N'),('S','S'),('W','W'),('E','E'),('NW','NW'),('NE','NE'),('SW','SW'),('SE','SE')]:
        im=b.render(b.names['Floor']);d=ImageDraw.Draw(im)
        for side in sides:
            line={'N':(0,2,15,2),'S':(0,13,15,13),'W':(2,0,2,15),'E':(13,0,13,15)}[side]
            d.line(line,fill=(64,80,96),width=2)
        if len(sides)==2:
            cx=3 if 'W' in sides else 12;cy=3 if 'N' in sides else 12
            d.rectangle((cx-1,cy-1,cx+1,cy+1),fill=(208,176,112))
        raster('Inlay'+suffix,im)
    for name,mid in [('PlinthLeft',0x229),('PlinthCenter',0x228),('PlinthRight',0x22A)]:pedestal(name,mid)
    b.alias('Plaque','PlinthCenter',0)
    # Remove unused donor colors before allocating hallway shade palettes.
    used=sorted({0}|{v>>12 for meta in b.metas for v in meta if v})
    for meta in b.metas:
        for j,v in enumerate(meta):meta[j]=(v&0xFFF)|(used.index(v>>12)<<12)
    b.pals=[b.pals[i] for i in used]
    floorpal=b.metas[b.names['Floor']][0]>>12;shades={}
    for percent in (80,60,40,20):
        shades[percent]=len(b.pals);b.pals.append([tuple(round(c*percent/800)*8 for c in color) for color in b.pals[floorpal]])
    def shade(name,base,levels,behavior=0):
        b.alias(name,base,behavior)
        for j,v in enumerate(b.metas[b.names[name]]):
            if v:assert v>>12==floorpal;b.metas[b.names[name]][j]=(v&0xFFF)|(shades[levels[j%4]]<<12)
    for base in ('Floor','FloorShadow'):
        for suffix,levels in [('FadeWest',(60,80,60,80)),('FadeEast',(80,60,80,60)),('Dim40',(40,)*4),('Dim20',(20,)*4)]:shade(base+suffix,base,levels)
    shade('FloorFadeNorth','Floor',(60,60,80,80));shade('FloorFadeSouth','Floor',(80,80,60,60))
    for name,base,behavior in [('WarpNorth','FloorDim40',0x64),('WarpEast','FloorDim40',0x62),('WarpSouth','FloorDim40',0x65),('WarpWest','FloorDim40',0x63),('WarpEastUpper','FloorShadowDim40',0x62),('WarpWestUpper','FloorShadowDim40',0x63)]:b.alias(name,base,behavior)
    closed=Image.new('RGB',(16,32));closed.paste(b.render(b.names['FrameTop']),(0,0));closed.paste(b.render(b.names['Door']),(0,16))
    doorcolors=[(0,0,0)]+sorted(set(closed.get_flattened_data())-{(0,0,0)});assert len(doorcolors)<=16
    doorcolors += [(0,0,0)]*(16-len(doorcolors));doorpal=len(b.pals);b.pals.append(doorcolors)
    anim=Image.new('P',(16,96));anim.putpalette([v for c in doorcolors for v in c])
    for phase,shift in enumerate((2,4,6)):
        for y in range(32):
            for x in range(16):
                src=x
                if y>=9 and 2<=x<14:src=x+shift if x<8-shift else x-shift if x>=8+shift else None
                anim.putpixel((x,y+phase*32),doorcolors.index(closed.getpixel((src,y))) if src is not None else 0)
    # Append new metatiles so the authored layout's earlier IDs remain stable.
    wallpal=used.index(wallpal)
    for name,mid in [('PlinthTopLeft',0x221),('PlinthTopCenter',0x220),('PlinthTopRight',0x222)]:pedestal(name,mid)
    b.alias('PlaqueFront','PlinthCenter',0x1000)
    from hub_furnishings import append_hub_furnishings
    append_hub_furnishings(b)
    assert len(b.tiles)<=512 and len(b.pals)<=13 and len(b.metas)<=512,(len(b.tiles),len(b.pals),len(b.metas))
    outputs={PRI+'/tiles.png':png(b.tiles),PRI+'/metatiles.bin':packed(sum(b.metas,[])),PRI+'/metatile_attributes.bin':packed(b.attrs),SEC+'/tiles.png':png([bytes(64)],16),SEC+'/metatiles.bin':packed([0]*8),SEC+'/metatile_attributes.bin':packed([0])}
    for i in range(13):
        colors=b.pals[i] if i<len(b.pals) else [(0,0,0)]*16
        outputs[(PRI if i<6 else SEC)+f'/palettes/{i:02}.pal']=('JASC-PAL\r\n0100\r\n16\r\n'+'\r\n'.join(' '.join(map(str,c)) for c in colors)+'\r\n').encode()
    buf=io.BytesIO();anim.save(buf,format='PNG');outputs['graphics/door_anims/main_hall.png']=buf.getvalue()
    header='#ifndef GUARD_MAIN_HALL_TILES_H\n#define GUARD_MAIN_HALL_TILES_H\n\n// Generated by scripts/generate_main_hall.py.\n'
    for name,value in b.names.items():header+=f'#define METATILE_MainHall_{name} 0x{value:03X}\n'
    outputs['include/constants/main_hall_tiles.h']=(header+f'#define MAIN_HALL_DOOR_PALETTE {doorpal}\n\n#endif\n').encode()
    outputs['data/tilesets/main_hall_sources.json']=(json.dumps({'metatiles':b.provenance,'assemblies':assemblies,'stairs':'Secret Base stand 0x272 tread bands; private gray repeatable center and end caps; ordinary behavior','tiles':len(b.tiles),'palettes':len(b.pals)},indent=2)+'\n').encode()
    def tile(name,solid=False,elev=3):return b.names[name]|elev<<12|(0xC00 if solid else 0)
    def fill(a,x1,y1,x2,y2,name,solid=False,elev=3):
        for y in range(y1,y2+1):
            for x in range(x1,x2+1):a[y][x]=tile(name,solid,elev)
    def door(a,x,y):
        for dx,name in [(-1,'FrameTopLeft'),(0,'FrameTop'),(1,'FrameTopRight')]:a[y-1][x+dx]=tile(name,True,5)
        for dx,name in [(-1,'FrameLeft'),(0,'Door'),(1,'FrameRight')]:a[y][x+dx]=tile(name,dx!=0,5)
    def stamp(a,label,x,y,w,h):
        for yy in range(h):
            for xx in range(w):a[y+yy][x+xx]=tile(f'{label}_{xx}_{yy}',True,5 if y<8 else 3)
    def inlay(a,x1,y1,x2,y2):
        for y in range(y1,y2+1):
            for x in range(x1,x2+1):
                suffix=('N' if y==y1 else 'S' if y==y2 else '')+('W' if x==x1 else 'E' if x==x2 else '')
                if suffix:a[y][x]=tile('Inlay'+suffix)
    if args.init_layout:
        l=next(x for x in layouts if x['id']=='LAYOUT_ROGUE_AREA_TOWN_SQUARE');assert l['primary_tileset']=='gTileset_GeneralHub','Already converted: edit map.bin in Porymap.'
        a=[[tile('Void',True)]*W for _ in range(H)]
        fill(a,4,5,32,24,'Floor');fill(a,4,5,32,7,'Floor',elev=5);fill(a,4,5,32,5,'FloorShadow',elev=5)
        for y,name in [(3,'Wall'),(4,'WallBase'),(26,'Wall'),(27,'WallBase')]:fill(a,4,y,32,y,name,True)
        for x in (3,33):
            fill(a,x,2,x,27,'Pillar',True)
            for y,name in [(2,'PillarCap'),(3,'PillarJoin'),(14,'InnerJoin'),(15,'InnerBase'),(16,'InnerFoot'),(19,'PillarCap'),(20,'PillarJoin'),(26,'PillarJoin'),(27,'PillarEnd')]:a[y][x]=tile(name,y!=16)
            fill(a,x,17,x,18,'Floor')
        # Black-backed lower walls, with a three-wide south access tunnel.
        for x in (16,20):
            fill(a,x,0,x,2,'Pillar',True,5)
            for y,name in [(3,'InnerJoin'),(4,'InnerBase'),(5,'InnerFoot')]:a[y][x]=tile(name,y!=5,5)
            fill(a,x,25,x,29,'Pillar',True)
            for y,name in [(25,'PillarCap'),(26,'PillarJoin')]:a[y][x]=tile(name,True)
        fill(a,17,0,19,4,'Floor',elev=5);fill(a,17,25,19,29,'Floor')
        for x in (17,18,19):
            for y,name in [(0,'FloorDim20'),(1,'WarpNorth'),(2,'FloorFadeNorth')]:a[y][x]=tile(name,elev=5)
            for y,name in [(27,'FloorFadeSouth'),(28,'WarpSouth'),(29,'FloorDim20')]:a[y][x]=tile(name)
        for x in (0,1,2,34,35):
            for y,name in [(14,'Wall'),(15,'WallBase'),(19,'Void'),(20,'Wall'),(21,'WallBase')]:a[y][x]=tile(name,True)
            for y in (16,17,18):
                base='FloorShadow' if y==16 else 'Floor'
                if x in (2,34):name=base+('FadeWest' if x<3 else 'FadeEast')
                elif x in (1,35):name=('WarpWest' if x<3 else 'WarpEast')+('Upper' if y==16 else '')
                else:name=base+'Dim20'
                a[y][x]=tile(name)
        fill(a,4,8,32,8,'GalleryRail',True);fill(a,4,9,32,9,'GalleryFace',True)
        for center in (12,24):
            for y in (8,9):
                for dx,name in [(-1,'StairLeft'),(0,'StairCenter'),(1,'StairRight')]:a[y][center+dx]=tile(name,elev=0)
        for x in (9,27):door(a,x,4)
        for x in (6,14,22,30):stamp(a,'Plant',x,4,1,2)
        # Monument island with a clear south plaque, separated from the crossing.
        inlay(a,15,11,21,15)
        for y in (12,13):
            for x in (17,18,19):a[y][x]=tile('PlinthLeft' if x==17 else 'PlinthRight' if x==19 else 'PlinthCenter',True)
        a[14][18]=tile('Plaque',True)
        # Furnished upper bays; keep both stair approaches and side routes open.
        for x in (6,8,28,30):stamp(a,'Monitor',x,9,1,2)
        for x1,x2 in ((4,9),(27,32)):
            fill(a,x1,19,x2,19,'Void',True);fill(a,x1,20,x2,20,'Wall',True);fill(a,x1,21,x2,21,'WallBase',True)
        for x in (6,28):stamp(a,'Sofa',x,22,3,2)
        for x in (4,32):stamp(a,'Plant',x,21,1,2)
        inlay(a,11,21,14,24);inlay(a,22,21,25,24)
        # Workbench niche: its only public approach is from the south.
        for x in (5,7):a[11][x]=tile('PlinthCenter',True)
        outputs[MAP+'/map.bin']=packed(sum(a,[]));outputs[MAP+'/border.bin']=packed([tile('Void',True)]*4)
    else:
        cells=words(MAP+'/map.bin');assert len(cells)==W*H;a=[cells[y*W:(y+1)*W] for y in range(H)]
    for suffix,w,h,elev in [('Horizontal',16,8,3),('North',8,12,5),('South',8,12,3)]:
        arr=[]
        for y in range(h):
            for x in range(w):
                if suffix=='Horizontal':name=['Wall','WallBase','FloorShadowDim20','FloorDim20','FloorDim20','Void','Wall','WallBase'][y];solid=y not in (2,3,4)
                else:name='FloorDim20' if x in (3,4,5) else 'Pillar' if x in (2,6) else 'Void';solid=x not in (3,4,5)
                arr.append(tile(name,solid,elev))
        outputs[f'data/layouts/Rogue_MainHallCorridor_{suffix}/map.bin']=packed(arr);outputs[f'data/layouts/Rogue_MainHallCorridor_{suffix}/border.bin']=packed([tile('Void',True)]*4)
    for path,data in outputs.items():
        p=ROOT/path
        if args.check:assert p.exists() and p.read_bytes()==data,f'Stale asset: {path}'
        elif not p.exists() or p.read_bytes()!=data:p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
    OUT.mkdir(parents=True,exist_ok=True)
    def render(grid):
        im=Image.new('RGB',(W*16,H*16))
        for y,row in enumerate(grid):
            for x,v in enumerate(row):im.paste(b.render(v&1023),(x*16,y*16))
        return im
    render(a).save(OUT/'room-native.png')
    # Further runtime-state checks and actor previews below read authored events.
    validate_and_preview(b,a,tile,fill,render,args.check)
    print(f'Main Hall {"verified" if args.check else "generated"}: {len(b.tiles)} tiles, {len(b.pals)} palettes.')


def validate_and_preview(b,a,tile,fill,render,check):
    events=json.loads((ROOT/'data/maps/Rogue_Area_TownSquare/map.json').read_text())
    def effective(mask,school):
        grid=[row[:] for row in a]
        if not mask&1:
            fill(grid,16,0,20,2,'Void',True,5);fill(grid,16,3,20,3,'Wall',True,5);fill(grid,16,4,20,4,'WallBase',True,5);fill(grid,16,5,20,5,'FloorShadow',elev=5)
        if not mask&2:fill(grid,34,14,35,21,'Void',True);fill(grid,33,14,33,19,'Pillar',True)
        if not mask&4:
            fill(grid,16,25,20,29,'Void',True)
        if not mask&8:fill(grid,0,14,2,21,'Void',True);fill(grid,3,14,3,19,'Pillar',True)
        if not school:grid[4][9]=tile('ClosedDoor',True,5)
        return grid
    def actor_list(stage,tutors,statue):
        result=[]
        for i,e in enumerate(events['object_events'],1):
            if i==1 and stage==0 or i in (2,7) and stage==0 or i==7 and stage==1 or i in (4,5) and not tutors:continue
            x,y=e['x'],e['y']
            pic,pal,w,h={1:('rogue/work_table',1,32,32),2:('rogue/npc/misc/backpacker_f',1,16,32),3:('rogue/npc/misc/kalos_worker',4,16,32),4:('people/black_belt',3,16,32),5:('people/frontier_brains/spenser',1,16,32),6:(f'rogue/statue_level_{statue}',[4,4,1,3][statue],32,32),7:('rogue/npc/misc/ball_guy',2,16,32)}[i]
            if i==3:x,y=[(27,5),(6,12),(7,12)][stage]
            result.append((x,y,pic,pal,w,h,i))
        return result
    def actors(im,stage,tutors,statue):
        im=im.copy()
        for x,y,pic,pal,w,h,i in sorted(actor_list(stage,tutors,statue),key=lambda a:a[1]):
            spr=Image.open(ROOT/f'graphics/object_events/pics/{pic}.png').crop((0,0,w,h));colors=palette(f'graphics/object_events/palettes/npc_{pal}.pal');rgba=Image.new('RGBA',(w,h))
            for yy in range(h):
                for xx in range(w):
                    c=spr.getpixel((xx,yy));rgba.putpixel((xx,yy),(*colors[c],255 if c else 0))
            im.paste(rgba,(x*16+8-w//2,(y+1)*16-h),rgba)
        return im
    views=[('south',18,24),('monument',18,15),('west-stair',12,10),('east-stair',24,10),('gallery',18,5),('school',9,5),('connect',27,5),('builder',7,12),('training',29,12),('bag',10,23),('ball-guy',26,23)]
    for label,stage,school,tutors,statue in [('room',2,True,True,3),('room-locked',0,False,False,0),('room-tutorial',1,True,False,0)]:
        scene=actors(render(effective(15,school)),stage,tutors,statue);scene.save(OUT/(label+'-native.png'));scene.resize((1152,960),Image.Resampling.NEAREST).save(OUT/(label+'.png'))
        for name,x,y in views:
            crop=scene.crop((x*16-120,y*16-80,x*16+120,y*16+80));crop.save(OUT/f'{label}-{name}-240x160.png');crop.resize((720,480),Image.Resampling.NEAREST).save(OUT/f'{label}-{name}.png')
    montage=Image.new('RGB',(4*240,160))
    for statue in range(4):
        scene=actors(render(a),2,True,statue).crop((18*16-120,14*16-80,18*16+120,14*16+80));montage.paste(scene,(statue*240,0))
    montage.save(OUT/'statue-states.png')
    # Read all authored cells, testing actual paths with engine elevation rules.
    assert words(MAP+'/border.bin')==[tile('Void',True)]*4
    horizontal=words('data/layouts/Rogue_MainHallCorridor_Horizontal/map.bin')
    for yy in range(8):
        assert b.render(a[yy+14][0]&1023).tobytes()==b.render(horizontal[yy*16]&1023).tobytes()
        # East is one tile shorter in the even-width buffer: its edge is the
        # second shaded tile, followed by the deepest shade in the facade.
        assert horizontal[yy*16]>>12==3
    for suffix,row,z in [('North',0,5),('South',29,3)]:
        facade=words(f'data/layouts/Rogue_MainHallCorridor_{suffix}/map.bin')
        for x in range(8):
            assert b.render(a[row][x+14]&1023).tobytes()==b.render(facade[x]&1023).tobytes(),('Facade seam',suffix,x)
        assert all(facade[x]>>12==z for x in (3,4,5))
    for row in a:
        for cell in row:
            assert (cell&1023)<len(b.metas)
            assert b.attrs[cell&1023]&0xFF in (0,0x62,0x63,0x64,0x65,0x69)
    for x in range(4,33):
        for y in (8,9):
            stairs=x in (11,12,13,23,24,25)
            assert bool(a[y][x]&0xC00)!=stairs
            if stairs:assert a[y][x]>>12==0
    for y in range(3,28):assert bool(a[y][3]&0xC00)==bool(a[y][33]&0xC00)
    for center in (12,24):
        for x in range(center-1,center+2):assert [a[y][x]>>12 for y in range(7,11)]==[5,0,0,3]
    for name in ('GalleryFace','PlinthLeft','PlinthCenter','PlinthRight','Sofa_0_1','Plant_0_1'):assert b.attrs[b.names[name]]>>12==1,('Foreground obscures actors',name)
    warps={(w['x'],w['y']) for w in events['warp_events']}
    # Exhaust all connection, service and introduction states; statue graphics do
    # not change occupancy, and all four are rendered above with their real palettes.
    for mask in range(16):
        for school in (False,True):
            for tutors in (False,True):
                for stage in range(3):
                    grid=effective(mask,school);occupied={(x,y) for x,y,*_ in actor_list(stage,tutors,0)}
                    seen={(18,19,3)};q=deque(seen)
                    while q:
                        x,y,z=q.popleft()
                        for nx,ny in ((x-1,y),(x+1,y),(x,y-1),(x,y+1)):
                            if not(0<=nx<W and 0<=ny<H) or grid[ny][nx]&0xC00 or (nx,ny) in occupied:continue
                            nz=grid[ny][nx]>>12
                            if z and nz and z!=nz:continue
                            point=(nx,ny,nz)
                            if point not in seen:seen.add(point);q.append(point)
                    reach={(x,y) for x,y,z in seen}
                    assert (18,14) in reach
                    for x,y,pic,pal,w,h,i in actor_list(stage,tutors,0):
                        if i==6:continue # South-facing plaque uses its own approach.
                        if i==1 and stage==1:continue # Builder temporarily occupies this approach.
                        assert any(p in reach for p in [(x-1,y),(x+1,y),(x,y+1),(x,y-1)]),('Unreachable service',mask,school,tutors,stage,i)
                    if stage==1:assert (6,13) in reach and (7,12) not in occupied and not grid[12][7]&0xC00
                    assert ((9,4) in reach)==school
                    assert ((27,4) in reach)==(stage!=0)
                    for bit,points in [(1,[(x,1) for x in (17,18,19)]),(2,[(35,y) for y in (16,17,18)]),(4,[(x,28) for x in (17,18,19)]),(8,[(1,y) for y in (16,17,18)])]:
                        assert all((p in reach)==bool(mask&bit) for p in points),('Exit reachability',mask,stage,points)
                        assert set(points)<=warps
    for interior,index in [('Rogue_Interior_School',8),('Rogue_Interior_PokeConnect',9)]:
        returns=json.loads((ROOT/f'data/maps/{interior}/map.json').read_text())['warp_events']
        returns=[w for w in returns if w['dest_map']=='MAP_ROGUE_AREA_TOWN_SQUARE'];assert returns and all(int(w['dest_warp_id'])==index for w in returns)
    import re
    script=(ROOT/'data/maps/Rogue_Area_TownSquare/scripts.pory').read_text(encoding='utf-8')
    def movement(name,start):
        body=re.search(r'movement '+name+r'\s*\{([^}]+)\}',script).group(1);x,y=start
        for direction in re.findall(r'walk_(up|down|left|right)',body):
            dx,dy={'up':(0,-1),'down':(0,1),'left':(-1,0),'right':(1,0)}[direction];x+=dx;y+=dy
            assert not a[y][x]&0xC00,(name,x,y)
        return x,y
    assert movement('Rogue_Area_TownSquare_WorkbenchBuilder_North',(6,12))==(7,12)
    assert movement('Rogue_Area_TownSquare_WorkbenchPlayer_North',(6,13))==(6,12)
    assert 'setobjectxyperm(OBJ_EVENT_BUILDER, 7, 12)' in script
    print('192 connection/service/intro states: reachability, stairs, doors and all exit lanes pass.')


if __name__=='__main__':main()
