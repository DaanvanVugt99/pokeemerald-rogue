#!/usr/bin/env python3
"""Build private research-junction assets; preserve the Porymap-authored layout."""
import argparse
import io
import json
import re
from collections import deque
from pathlib import Path
from PIL import Image, ImageDraw
from generate_portal_room import Builder, Source, words, packed, png, palette

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'build/lab_junction'
PRI = 'data/tilesets/primary/lab_junction'
SEC = 'data/tilesets/secondary/lab_junction'
MAP = 'data/layouts/Rogue_Area_Labs'
W, H = 28, 24


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--check', action='store_true')
    ap.add_argument('--init-layout', action='store_true', help='One-time conversion of the outdoor map; never used for regeneration.')
    args = ap.parse_args()
    assert not (args.check and args.init_layout)
    b = Builder()
    b.sources['lab'] = Source('building', 'lab')
    layouts = json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
    sources = {}
    for key, lid in [('galactic', 'LAYOUT_ROGUE_ENCOUNTER_GALACTIC_HQ'), ('lab', 'LAYOUT_ROGUE_ENCOUNTER_LAB'), ('factory', 'LAYOUT_ROGUE_ENCOUNTER_POKE_BALL_FACTORY')]:
        l = next(x for x in layouts if x['id'] == lid)
        sources[key] = (words(l['blockdata_filepath']), l['width'], lid)
    def reference(name, key, x, y, behavior=0):
        cells, width, lid = sources[key]
        mid = cells[y*width+x] & 1023
        b.add(name, key, mid, behavior)
        b.provenance[-1]['reference'] = {'layout': lid, 'x': x, 'y': y}
        return name
    for name,x,y in [('Floor',3,4),('FloorShadow',1,4),('Void',1,0),
                     ('PillarCap',0,0),('PillarJoin',0,1),('Pillar',0,4),('PillarEnd',0,8),
                     ('Wall',4,1),('WallBase',4,2),('InnerJoin',5,12),('InnerBase',5,13),('InnerFoot',5,14),
                     ('FrameTopLeft',1,12),('FrameTop',2,12),('FrameTopRight',3,12),
                     ('FrameLeft',1,13),('FrameRight',3,13),('Door',2,13),('SignTop',11,1),('SignBase',11,2)]:
        reference(name,'galactic',x,y,0x69 if name=='Door' else 0)
    # Import complete assemblies from actual maps, retaining their collision.
    assemblies=[]
    def assembly(label,key,sx,sy,w,h,dx,dy):
        cells,width,lid=sources[key]; result=[]
        for yy in range(h):
            row=[]
            for xx in range(w):
                name=f'{label}_{xx}_{yy}'
                reference(name,key,sx+xx,sy+yy)
                row.append((name,bool(cells[(sy+yy)*width+sx+xx]&0xC00)))
            result.append(row)
        assemblies.append({'name':label,'source_layout':lid,'source':[sx,sy,w,h],'destination':[dx,dy],'cells':result})
    assembly('SpecimenBank','galactic',1,1,2,3,5,3)
    assembly('ComputerBench','lab',3,0,2,2,21,4)
    # Preserve the complete desk foreground and match its original wall/floor
    # background layers to the HQ room. This avoids a beige patch around it.
    for xx in range(2):
        for yy,base in [(0,'WallBase'),(1,'Floor')]:
            b.metas[b.names[f'ComputerBench_{xx}_{yy}']][:4]=b.metas[b.names[base]][:4]
    b.provenance.append({'assembly':'ComputerBench','background':'HQ WallBase above Floor; original monitor/worktop foreground unchanged'})
    reference('ApproachLeft','factory',1,2)
    reference('ApproachRight','factory',12,2)
    # Background replacement leaves unused donor palettes; remove those before
    # reserving the four hallway shades and the door animation palette.
    used=sorted({0}|{v>>12 for meta in b.metas for v in meta if v})
    for meta in b.metas:
        for j,v in enumerate(meta):meta[j]=(v&0xFFF)|(used.index(v>>12)<<12)
    b.pals=[b.pals[i] for i in used]
    floor_pal=b.metas[b.names['Floor']][0]>>12
    shades={}
    for percent in (80,60,40,20):
        shades[percent]=len(b.pals)
        b.pals.append([tuple(round(c*percent/800)*8 for c in color) for color in b.pals[floor_pal]])
    def shaded(name,base,levels,behavior=0):
        b.alias(name,base,behavior)
        for j,v in enumerate(b.metas[b.names[name]]):
            if v:
                assert v>>12==floor_pal
                b.metas[b.names[name]][j]=(v&0xFFF)|(shades[levels[j%4]]<<12)
    for base in ('Floor','FloorShadow'):
        for suffix,levels in [('FadeWest',(60,80,60,80)),('FadeEast',(80,60,80,60)),
                              ('Dim40',(40,)*4),('Dim20',(20,)*4)]:
            shaded(base+suffix,base,levels)
    shaded('FloorFadeNorth','Floor',(60,60,80,80))
    shaded('FloorFadeSouth','Floor',(80,80,60,60))
    for name,base,behavior in [('WarpWest','FloorDim20',0x63),('WarpEast','FloorDim20',0x62),
                              ('WarpNorth','FloorDim20',0x64),('WarpSouth','FloorDim20',0x65),
                              ('WarpWestUpper','FloorShadowDim20',0x63),('WarpEastUpper','FloorShadowDim20',0x62)]:
        b.alias(name,base,behavior)

    # Animate the HQ's existing shutter by retracting its two leaves. The frame
    # stays stationary, and the final dark opening is consistent with hub halls.
    closed=Image.new('RGB',(16,32));closed.paste(b.render(b.names['FrameTop']),(0,0));closed.paste(b.render(b.names['Door']),(0,16))
    door_colors=[(0,0,0)]+sorted(set(closed.get_flattened_data())-{(0,0,0)})
    assert len(door_colors)<=16
    door_colors += [(0,0,0)]*(16-len(door_colors))
    door_pal=len(b.pals);b.pals.append(door_colors)
    anim=Image.new('P',(16,96));anim.putpalette([v for c in door_colors for v in c])
    frames=[]
    for phase,shift in enumerate((2,4,6)):
        frame=closed.copy()
        for y in range(9,32):
            for x in range(2,14):
                src=x+shift if x<8-shift else x-shift if x>=8+shift else None
                frame.putpixel((x,y),closed.getpixel((src,y)) if src is not None else (0,0,0))
        frames.append(frame)
        for y in range(32):
            for x in range(16):anim.putpixel((x,phase*32+y),door_colors.index(frame.getpixel((x,y))))
    assert len(b.tiles)<=512 and len(b.pals)<=13 and len(b.metas)<=512
    outputs={}
    outputs[PRI+'/tiles.png']=png(b.tiles)
    outputs[PRI+'/metatiles.bin']=packed(sum(b.metas,[]))
    outputs[PRI+'/metatile_attributes.bin']=packed(b.attrs)
    outputs[SEC+'/tiles.png']=png([bytes(64)],16)
    outputs[SEC+'/metatiles.bin']=packed([0]*8)
    outputs[SEC+'/metatile_attributes.bin']=packed([0])
    for i in range(13):
        colors=b.pals[i] if i<len(b.pals) else [(0,0,0)]*16
        outputs[(PRI if i<6 else SEC)+f'/palettes/{i:02}.pal']=('JASC-PAL\r\n0100\r\n16\r\n'+'\r\n'.join(' '.join(map(str,c)) for c in colors)+'\r\n').encode()
    buf=io.BytesIO();anim.save(buf,format='PNG');outputs['graphics/door_anims/lab_junction.png']=buf.getvalue()
    header='#ifndef GUARD_LAB_JUNCTION_TILES_H\n#define GUARD_LAB_JUNCTION_TILES_H\n\n// Generated by scripts/generate_lab_junction.py.\n'
    for name,value in b.names.items():header+=f'#define METATILE_LabJunction_{name} 0x{value:03X}\n'
    header+=f'#define LAB_JUNCTION_DOOR_PALETTE {door_pal}\n\n#endif\n'
    outputs['include/constants/lab_junction_tiles.h']=header.encode()
    outputs['data/tilesets/lab_junction_sources.json']=(json.dumps({'metatiles':b.provenance,'assemblies':assemblies,'tiles':len(b.tiles),'palettes':len(b.pals),'door':'HQ shutter with two retracting leaves; frame remains fixed'},indent=2)+'\n').encode()

    def tile(name,solid=False):return b.names[name]|0x3000|(0xC00 if solid else 0)
    def fill(a,x1,y1,x2,y2,name,solid=False):
        for y in range(y1,y2+1):
            for x in range(x1,x2+1):a[y][x]=tile(name,solid)
    def doorway(a,x,y):
        for dx,name in [(-1,'FrameTopLeft'),(0,'FrameTop'),(1,'FrameTopRight')]:a[y-1][x+dx]=tile(name,True)
        for dx,name in [(-1,'FrameLeft'),(0,'Door'),(1,'FrameRight')]:a[y][x+dx]=tile(name,dx!=0)
    if args.init_layout:
        l=next(x for x in layouts if x['id']=='LAYOUT_ROGUE_AREA_LABS')
        assert l['primary_tileset']=='gTileset_GeneralHub', 'Layout already converted; edit it in Porymap.'
        a=[[tile('Void',True)]*W for _ in range(H)]
        fill(a,5,5,23,15,'Floor');fill(a,5,5,23,5,'FloorShadow');fill(a,5,6,5,15,'FloorShadow')
        for y,name in [(3,'Wall'),(4,'WallBase'),(17,'Wall'),(18,'WallBase')]:fill(a,5,y,23,y,name,True)
        for x in (4,24):
            fill(a,x,2,x,18,'Pillar',True)
            for y,name in [(2,'PillarCap'),(3,'PillarJoin'),(6,'InnerJoin'),(7,'InnerBase'),(8,'InnerFoot'),(11,'PillarCap'),(12,'PillarJoin'),(17,'PillarJoin'),(18,'PillarEnd')]:a[y][x]=tile(name,y!=8)
            fill(a,x,9,x,10,'Floor')
        for x in (12,16):
            fill(a,x,0,x,2,'Pillar',True)
            for y,name in [(3,'InnerJoin'),(4,'InnerBase'),(5,'InnerFoot'),(16,'PillarCap'),(17,'PillarJoin')]:a[y][x]=tile(name,y!=5)
            fill(a,x,18,x,23,'Pillar',True)
        fill(a,13,0,15,4,'Floor');fill(a,13,16,15,23,'Floor')
        for x in list(range(4))+list(range(25,28)):
            for y,name in [(6,'Wall'),(7,'WallBase'),(11,'Void'),(12,'Wall'),(13,'WallBase')]:a[y][x]=tile(name,True)
            for y in (8,9,10):
                base='FloorShadow' if y==8 else 'Floor'
                distance=x if x<4 else 28-x
                a[y][x]=tile(base+('FadeWest' if x<4 else 'FadeEast') if distance==3 else base+('Dim40' if distance==2 else 'Dim20'))
        for x in (13,14,15):
            for y,name in [(0,'WarpNorth'),(1,'FloorDim40'),(2,'FloorFadeNorth'),(19,'FloorFadeSouth'),(20,'FloorDim40'),(21,'FloorDim20'),(22,'FloorDim20'),(23,'WarpSouth')]:a[y][x]=tile(name)
        for y in (8,9,10):a[y][0]=tile('WarpWestUpper' if y==8 else 'WarpWest');a[y][27]=tile('WarpEastUpper' if y==8 else 'WarpEast')
        for x1,x2 in ((5,10),(18,23)):
            for y,name in [(11,'Void'),(12,'Wall'),(13,'WallBase')]:fill(a,x1,y,x2,y,name,True)
        for x in (11,17):
            for y,name in [(11,'PillarCap'),(12,'InnerJoin'),(13,'InnerBase'),(14,'InnerFoot')]:a[y][x]=tile(name,y!=14)
        for x in (9,19):
            doorway(a,x,4)
            a[6][x-1]=tile('ApproachLeft');a[6][x+1]=tile('ApproachRight')
        for x in (11,17):a[3][x]=tile('SignTop',True);a[4][x]=tile('SignBase',True)
        for asm in assemblies:
            dx,dy=asm['destination']
            for yy,row in enumerate(asm['cells']):
                for xx,(name,solid) in enumerate(row):a[dy+yy][dx+xx]=tile(name,solid)
        outputs[MAP+'/map.bin']=packed(sum(a,[]));outputs[MAP+'/border.bin']=packed([tile('Void',True)]*4)
    else:
        cells=words(MAP+'/map.bin');assert len(cells)==W*H
        a=[cells[y*W:(y+1)*W] for y in range(H)]
    for suffix,w,h in [('Horizontal',16,8),('Vertical',8,12)]:
        arr=[]
        for y in range(h):
            for x in range(w):
                if suffix=='Horizontal':
                    name=['Wall','WallBase','FloorShadowDim20','FloorDim20','FloorDim20','Void','Wall','WallBase'][y];solid=y not in (2,3,4)
                else:name='FloorDim20' if x in (3,4,5) else 'Pillar' if x in (2,6) else 'Void';solid=x not in (3,4,5)
                arr.append(tile(name,solid))
        outputs[f'data/layouts/Rogue_LabCorridor_{suffix}/map.bin']=packed(arr)
        outputs[f'data/layouts/Rogue_LabCorridor_{suffix}/border.bin']=packed([tile('Void',True)]*4)
    for path,data in outputs.items():
        p=ROOT/path
        if args.check:assert p.exists() and p.read_bytes()==data, f'Stale asset: {path}'
        elif not p.exists() or p.read_bytes()!=data:p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)

    def effective(mask,unique=False,debug=False):
        result=[row[:] for row in a]
        for bit,x1,x2,pillar in [(8,0,3,4),(2,25,27,24)]:
            if not mask&bit:
                fill(result,x1,6,x2,13,'Void',True);fill(result,pillar,6,pillar,11,'Pillar',True)
        if not mask&1:
            fill(result,12,0,16,2,'Void',True);fill(result,12,3,16,3,'Wall',True);fill(result,12,4,16,4,'WallBase',True);fill(result,12,5,16,5,'FloorShadow')
        if not mask&4:
            fill(result,12,16,16,16,'Void',True);fill(result,12,17,16,17,'Wall',True);fill(result,12,18,16,18,'WallBase',True);fill(result,12,19,16,23,'Void',True)
        if unique:
            doorway(result,8,13)
            result[12][10]=tile('SignTop',True);result[13][10]=tile('SignBase',True)
        if debug:doorway(result,20,13)
        return result
    def render(cells):
        im=Image.new('RGB',(W*16,H*16))
        for y,row in enumerate(cells):
            for x,v in enumerate(row):
                assert (v&1023)<len(b.metas)
                im.paste(b.render(v&1023),(x*16,y*16))
        return im
    OUT.mkdir(parents=True,exist_ok=True)
    render(a).save(OUT/'room-native.png')
    events=json.loads((ROOT/'data/maps/Rogue_Area_Labs/map.json').read_text())
    def actors(im,workbench=True,birch=False):
        im=im.copy()
        for event in events['object_events']:
            if event['graphics_id']=='OBJ_EVENT_GFX_WORK_TABLE' and workbench:pic,pal,w,h='rogue/work_table','npc_1',32,32
            elif event['graphics_id']=='OBJ_EVENT_GFX_PROF_BIRCH' and birch:pic,pal,w,h='people/prof_birch','npc_3',16,32
            else:continue
            spr=Image.open(ROOT/f'graphics/object_events/pics/{pic}.png').crop((0,0,w,h));colors=palette(f'graphics/object_events/palettes/{pal}.pal');rgba=Image.new('RGBA',(w,h))
            for yy in range(h):
                for xx in range(w):
                    c=spr.getpixel((xx,yy));rgba.putpixel((xx,yy),(*colors[c],255 if c else 0))
            im.paste(rgba,(event['x']*16+8-w//2,(event['y']+1)*16-h),rgba)
        return im
    for name,unique,debug in [('room',False,False),('room-unlocked',True,False),('room-debug',True,True),('room-intro',False,False)]:
        scene=actors(render(effective(15,unique,debug)),birch=name=='room-intro')
        scene.resize((896,768),Image.Resampling.NEAREST).save(OUT/(name+'.png'))
        for label,x,y in [('labs',14,5),('junction',14,10),('side-bays',14,14)]:
            scene.crop((x*16-120,y*16-80,x*16+120,y*16+80)).resize((720,480),Image.Resampling.NEAREST).save(OUT/f'{name}-{label}.png')
    montage=Image.new('RGB',(4*448,4*404))
    for mask in range(16):
        x=(mask%4)*448;y=(mask//4)*404
        montage.paste(actors(render(effective(mask))),(x,y+20));ImageDraw.Draw(montage).text((x+4,y+4),f'N {bool(mask&1)} E {bool(mask&2)} S {bool(mask&4)} W {bool(mask&8)}',fill='white')
    montage.save(OUT/'connection-states.png')
    door_preview=[closed]+frames+frames[-2::-1]+[closed]
    door_preview[0].resize((128,256),Image.Resampling.NEAREST).save(OUT/'door.gif',save_all=True,append_images=[f.resize((128,256),Image.Resampling.NEAREST) for f in door_preview[1:]],duration=150,loop=0)
    # Compare the actual facade seam pixels, including the backing above/below
    # horizontal halls. The last metatile on the map uses arrow-warp behavior;
    # its facade counterpart uses identical pixels with ordinary floor behavior.
    horizontal=words('data/layouts/Rogue_LabCorridor_Horizontal/map.bin')
    vertical=words('data/layouts/Rogue_LabCorridor_Vertical/map.bin')
    for y in range(8):
        expected=b.render(horizontal[y*16]&1023).tobytes()
        assert b.render(a[y+6][0]&1023).tobytes()==expected
        assert b.render(a[y+6][27]&1023).tobytes()==expected
    for x in range(8):
        expected=b.render(vertical[x]&1023).tobytes()
        assert b.render(a[0][x+10]&1023).tobytes()==expected
        assert b.render(a[23][x+10]&1023).tobytes()==expected
    border=words(MAP+'/border.bin')
    assert len(border)==4 and all(v==tile('Void',True) for v in border)
    # No narrowing within the halls or their visible facade continuations.
    for y in list(range(6))+list(range(16,24)):
        assert all(not a[y][x]&0xC00 for x in (13,14,15)),('Narrow vertical passage',y)
        if y != 5:
            assert a[y][12]&0xC00 and a[y][16]&0xC00
    for x in list(range(5))+list(range(24,28)):
        assert all(not a[y][x]&0xC00 for y in (8,9,10)),('Narrow horizontal passage',x)
    # Verify the authoritative scripted routes, rather than duplicate their
    # lengths in a test. Furniture and the initial player must not block Birch.
    script=(ROOT/'data/maps/Rogue_Area_Labs/scripts.pory').read_text(encoding='utf-8')
    def walk(name,start,grid,occupied):
        body=re.search(r'movement '+name+r'\s*\{([^}]+)\}',script).group(1)
        x,y=start
        for direction,count in re.findall(r'walk_(?:fast_)?(up|down|left|right)(?:\s*\*\s*(\d+))?',body):
            dx,dy={'up':(0,-1),'down':(0,1),'left':(-1,0),'right':(1,0)}[direction]
            for _ in range(int(count or 1)):
                x+=dx;y+=dy
                assert 0<=x<W and 0<=y<H and not grid[y][x]&0xC00 and (x,y) not in occupied,(name,x,y)
        return x,y
    workbench={(e['x'],e['y']) for e in events['object_events'] if e['graphics_id']=='OBJ_EVENT_GFX_WORK_TABLE'}
    workbench_approaches=[(x,y+1) for x,y in workbench]
    for direction,bit,end in [('North',1,(14,0)),('East',2,(27,9)),('South',4,(14,23)),('West',8,(0,9))]:
        assert walk('BirchTutorialGuide'+direction,(9,6),effective(bit),workbench|{(9,5)})==end
    assert walk('BirchStartGame1',(8,7),effective(0),workbench|{(9,7)})==(9,5)
    assert walk('BirchStartGame2',(9,5),effective(0),workbench)==(9,4)
    assert 'opendoor(9, 4)' in script and 'closedoor(9, 4)' in script
    for interior,warp in [('Rogue_Interior_ProfLab',9),('Rogue_Interior_ConfigLab',10),('Rogue_Interior_BillsLab',14),('Rogue_Hub_Debug',12)]:
        returns=json.loads((ROOT/f'data/maps/{interior}/map.json').read_text())['warp_events']
        returns=[w for w in returns if w['dest_map']=='MAP_ROGUE_AREA_LABS']
        assert returns and all(int(w['dest_warp_id'])==warp for w in returns),(interior,returns)
    for left,right in [(4,24),(11,17),(12,16)]:
        for y in range(H):
            assert bool(a[y][left]&0xC00)==bool(a[y][right]&0xC00),('Structural symmetry',left,right,y)
    # Check all 128 combinations; ordinary closed walls may never expose warps.
    for mask in range(16):
        for unique in (False,True):
            for debug in (False,True):
                grid=effective(mask,unique,debug)
                for workbench in (False,True):
                    occupied={(e['x'],e['y']) for e in events['object_events'] if e['graphics_id']=='OBJ_EVENT_GFX_WORK_TABLE' and workbench}
                    seen={(14,11)};q=deque(seen)
                    while q:
                        x,y=q.popleft()
                        for nx,ny in ((x+1,y),(x-1,y),(x,y+1),(x,y-1)):
                            if 0<=nx<W and 0<=ny<H and not grid[ny][nx]&0xC00 and (nx,ny) not in occupied and (nx,ny) not in seen:seen.add((nx,ny));q.append((nx,ny))
                    for point in [(9,4),(19,4),(11,5),(17,5),(9,7),(14,11)]+workbench_approaches:assert point in seen,('Service unreachable',mask,unique,debug,workbench,point)
                    assert ((8,13) in seen)==unique and ((20,13) in seen)==debug
                    for bit,points in [(1,[(x,0) for x in (13,14,15)]),(2,[(27,y) for y in (8,9,10)]),(4,[(x,23) for x in (13,14,15)]),(8,[(0,y) for y in (8,9,10)])]:
                        assert all((point in seen)==bool(mask&bit) for point in points),('Exit',mask,points)
                    boundary={(x,y) for x,y in seen if x in (0,W-1) or y in (0,H-1)}
                    warps={(w['x'],w['y']) for w in events['warp_events']}
                    assert boundary<=warps,('Uncovered exit lane',mask,boundary-warps)
    print(f'Lab junction {"verified" if args.check else "generated"}: {len(b.tiles)} tiles, {len(b.pals)} palettes; 128 connectivity/service states pass.')


if __name__=='__main__':main()
