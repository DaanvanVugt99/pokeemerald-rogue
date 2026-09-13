#!/usr/bin/env python3
"""Generate private portal assets and preview the Porymap-authored room. Requires Pillow."""
import argparse
import io
import json
import math
import struct
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
PRI = 'data/tilesets/primary/portal_room'
SEC = 'data/tilesets/secondary/portal_room'
OUT = ROOT / 'build/portal_room'

def words(path):
    data = (ROOT / path).read_bytes()
    return list(struct.unpack('<' + 'H' * (len(data)//2), data))

def packed(values):
    return struct.pack('<' + 'H' * len(values), *values)

def palette(path):
    return [tuple(map(int, line.split())) for line in (ROOT/path).read_text().splitlines()[3:19]]

class Source:
    def __init__(self, primary, secondary, graphics=None, metatiles=None):
        self.paths = ['data/tilesets/primary/'+primary, 'data/tilesets/secondary/'+secondary]
        self.images = [Image.open(ROOT/self.paths[0]/'tiles.png'), Image.open(ROOT/('data/tilesets/secondary/'+(graphics or secondary))/'tiles.png')]
        self.meta = [words(self.paths[0]+'/metatiles.bin'), words((metatiles or self.paths[1])+'/metatiles.bin')]
        self.attrs = [words(self.paths[0]+'/metatile_attributes.bin'), words((metatiles or self.paths[1])+'/metatile_attributes.bin')]
    def tile(self, word):
        n=word&1023; image=self.images[n//512]; n%=512
        x=(n%(image.width//8))*8; y=(n//(image.width//8))*8
        return image.crop((x,y,x+8,y+8)).tobytes()
    def pal(self, n):
        folder=self.paths[n>=6]
        if folder.endswith('brown_cave') and n==6: folder='data/tilesets/secondary/unique_den'
        return palette(folder+f'/palettes/{n:02}.pal')

class Builder:
    def __init__(self):
        self.sources={
            'facility':Source('general','facility'),
            'galactic':Source('building','facility_galactic','facility'),
            'factory':Source('general','facility_galactic','facility'),
            'rock':Source('secret_base','secret_base/brown_cave',metatiles='data/tilesets/secondary/secret_base')}
        self.tiles=[bytes(64)]; self.pals=[[(0,0,0)]*16]; self.metas=[]; self.attrs=[]; self.names={}; self.provenance=[]
    def add(self,name,source,mid,behavior=0):
        src=self.sources[source]; vals=[]
        for j,word in enumerate(src.meta[mid//512][(mid%512)*8:(mid%512+1)*8]):
            tile=src.tile(word)
            if not any(tile): vals.append(0); continue
            pal=src.pal(word>>12)
            if pal not in self.pals:self.pals.append(pal)
            if tile not in self.tiles:self.tiles.append(tile)
            vals.append(self.tiles.index(tile)|(word&0xC00)|(self.pals.index(pal)<<12))
        self.names[name]=len(self.metas);self.metas.append(vals)
        self.attrs.append((src.attrs[mid//512][mid%512]&0xF000)|behavior)
        self.provenance.append({'name':name,'source':source,'metatile':f'0x{mid:03X}','behavior':behavior})
        return self.names[name]
    def alias(self,name,base,behavior):
        self.names[name]=len(self.metas);self.metas.append(self.metas[self.names[base]][:]);self.attrs.append(behavior)
    def render(self,mid):
        vals=self.metas[mid] if mid<512 else self.portal_metas[mid-512]
        out=Image.new('RGB',(16,16))
        for j,v in enumerate(vals):
            tile=self.tiles[v&1023] if (v&1023)<512 else self.portal_tiles[(v&1023)-512]
            pal=self.pals[v>>12]
            for y in range(8):
                for x in range(8):
                    c=tile[(7-y if v&2048 else y)*8+(7-x if v&1024 else x)]
                    if j<4 or c:out.putpixel(((j%2)*8+x,((j%4)//2)*8+y),pal[c])
        return out

def png(tiles, count=512):
    image=Image.new('P',(128,((count+15)//16)*8))
    image.putpalette([v for i in range(256) for v in (i,i,i)])
    for i,tile in enumerate(tiles):
        sub=Image.frombytes('P',(8,8),tile);image.paste(sub,((i%16)*8,(i//16)*8))
    buf=io.BytesIO();image.save(buf,format='PNG');return buf.getvalue()

def main():
    # Converted districts share one asset pipeline. Retain the original
    # composition recipes below as provenance for the immutable source art.
    if (ROOT/'tools/data/hub_tiles.json').exists():
        from generate_hub_tiles import main as generate_shared_hub
        return generate_shared_hub()
    ap=argparse.ArgumentParser();ap.add_argument('--check',action='store_true');args=ap.parse_args()
    b=Builder()
    # Reference layout, not atlas adjacency, defines every imported assembly.
    layout=next(l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
                if l['id']=='LAYOUT_ROGUE_ENCOUNTER_GALACTIC_HQ')
    donor=words(layout['blockdata_filepath']);dw=layout['width']
    def at(x,y):return donor[y*dw+x]
    def reference(name,x,y):
        mid=at(x,y)&1023
        result=b.add(name,'galactic',mid)
        b.provenance[-1]['reference']={'layout':layout['id'],'x':x,'y':y}
        return result
    # North wall: cap -> upper wall -> face -> floor shadow. Pillars have
    # distinct start, continuous, and end pieces in the source map.
    for name,x,y in [('Floor',3,4),('Lane',3,4),('FloorShadow',1,4),
        ('Void',1,0),('PillarCap',0,0),('PillarJoin',0,1),('Pillar',0,4),
        ('PillarEnd',0,8),('PillarStart',2,8),('Wall',4,1),('WallBase',4,2),
        ('InnerJoin',5,12),('InnerBase',5,13),('InnerFoot',5,14),
        ('FrameTopLeft',1,12),('FrameTop',2,12),('FrameTopRight',3,12),
        ('FrameLeft',1,13),('FrameRight',3,13),('Shutter',2,13)]:
        reference(name,x,y)
    b.alias('RockFill','Void',0) # Retained name; exterior is uniformly black.
    for name,base,behavior in [('WarpWest','Floor',0x63),('WarpEast','Floor',0x62),('WarpSouth','Floor',0x65)]: b.alias(name,base,behavior)
    b.alias('WarpWestUpper','FloorShadow',0x63);b.alias('WarpEastUpper','FloorShadow',0x62)
    # Native-resolution concentric energy bands contract into a dark center.
    # Red arcs rotate through the purple rings; phase 8 returns exactly to 0.
    portal_pal=[(0,0,0),(16,8,24),(32,16,48),(48,24,72),
        (72,32,112),(96,40,152),(128,56,192),(160,72,216),
        (192,96,232),(224,144,248),(80,16,48),(128,24,56),
        (176,32,64),(224,48,80),(248,88,112),(248,144,160)]
    portal_index=len(b.pals);b.pals.append(portal_pal)
    def vortex_pixel(x,y,phase):
        dx=x-23.5;dy=y-23.5;r=math.hypot(dx,dy)
        if r>23 or r<2:return 1
        angle=math.atan2(dy,dx);t=phase/8
        band=(r/8+t+.04*math.sin(3*angle-math.tau*t))%1
        red=math.sin(2*angle+r*.12-math.tau*t)>.55
        if band<.14:color=13 if red else 8
        elif band<.27:color=12 if red else 6
        elif band<.43:color=10 if red else 4
        else:color=2
        # Fade into the circular rim so new rings emerge without a hard flash.
        if r>21.5:color=min(color,11 if red else 5)
        return color
    phases=[]
    for phase in range(8):
        phases.append([bytes(vortex_pixel(tx*8+x,ty*8+y,phase)
            for y in range(8) for x in range(8)) for ty in range(6) for tx in range(6)])
    assert len({b''.join(frame) for frame in phases})==8
    assert all(vortex_pixel(x,y,0)==vortex_pixel(x,y,8) for y in range(48) for x in range(48))
    assert all(len(frame)==36 and all(len(tile)==64 and max(tile)<16 for tile in frame) for frame in phases)
    b.portal_tiles=phases[0];b.portal_metas=[]
    for my in range(3):
        for mx in range(3):
            b.portal_metas.append([512+my*12+mx*2+(j//2)*6+j%2+(portal_index<<12) for j in range(4)]+[0]*4)
    assert len(b.tiles)<=512 and len(b.pals)<=13
    # Register the existing assemblies in stable ID order. The saved room is
    # Porymap-authored; these source/destination notes document the imported art.
    assemblies=[]
    def stamp(label,sx,sy,w,h,dx,dy):
        assemblies.append({'name':label,'source':[sx,sy,w,h],'destination':[dx,dy]})
        for yy in range(h):
            for xx in range(w):
                v=at(sx+xx,sy+yy);name=f'HQ_{v&1023:03X}'
                if name not in b.names:reference(name,sx+xx,sy+yy)
    stamp('west generator',1,1,2,3,4,1);stamp('east generator',1,1,2,3,13,1)
    stamp('west wall display',11,1,1,2,6,1);stamp('east wall display',11,1,1,2,12,1)
    stamp('console left controls',6,2,1,2,8,5)
    stamp('console right controls',6,2,1,2,10,5)
    b.alias('ControlBase','HQ_375',0)
    b.metas[b.names['ControlBase']][:4]=b.metas[b.names['Floor']][:4]
    factory_layout=next(l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
                        if l['id']=='LAYOUT_ROGUE_ENCOUNTER_POKE_BALL_FACTORY')
    factory_map=words(factory_layout['blockdata_filepath'])
    for name,sx in [('ConsoleMarkLeft',1),('ConsoleMarkRight',12)]:
        cell=factory_map[2*factory_layout['width']+sx]
        assert not cell&0xC00
        b.add(name,'factory',cell&1023)
        b.provenance[-1]['reference']={'layout':factory_layout['id'],'x':sx,'y':2}
    # Outdoor hub facades use an entrance fade followed by a uniformly dark
    # floor (GeneralHub 0x045). Mirror that language with this room's own floor.
    # Four private palette banks preserve the source pattern, in GBA RGB555.
    floor_pal=b.metas[b.names['Floor']][0]>>12
    shade_pals={}
    for percent in (80,60,40,20):
        shade_pals[percent]=len(b.pals)
        b.pals.append([tuple(round(c*percent/100/8)*8 for c in color) for color in b.pals[floor_pal]])
    def shaded(name,base,levels):
        b.alias(name,base,b.attrs[b.names[base]])
        vals=b.metas[b.names[name]]
        for j,v in enumerate(vals):
            if not v:continue
            assert v>>12==floor_pal, ('Unexpected floor palette',base,j)
            vals[j]=(v&0xFFF)|(shade_pals[levels[j%4]]<<12)
        b.provenance.append({'name':name,'derived_from':base,'shadow_percent':levels})
    for base in ('Floor','FloorShadow'):
        shaded(base+'FadeWest',base,(60,80,60,80))
        shaded(base+'FadeEast',base,(80,60,80,60))
        shaded(base+'Dim40',base,(40,40,40,40))
        shaded(base+'Dim20',base,(20,20,20,20))
    shaded('FloorFadeSouth','Floor',(80,80,60,60))
    shaded('FloorDeepSouth','Floor',(40,40,20,20))
    for name,base in [('WarpWest','FloorDim40'),('WarpEast','FloorDim40'),
                      ('WarpWestUpper','FloorShadowDim40'),('WarpEastUpper','FloorShadowDim40'),
                      ('WarpSouth','FloorDeepSouth')]:
        b.metas[b.names[name]]=b.metas[b.names[base]][:]
    # The side controls share the console's dark grey casing. Recolor their private
    # foreground palette only; the floor underneath retains its normal colors.
    console_colors=palette('graphics/object_events/palettes/npc_4.pal')
    controls_pal=b.pals[floor_pal][:]
    for source,target in ((1,12),(2,12),(3,13),(5,7)):
        controls_pal[source]=console_colors[target]
    controls_index=len(b.pals);b.pals.append(controls_pal)
    b.alias('ControlTop','HQ_2B8',b.attrs[b.names['HQ_2B8']])
    for name in ('ControlTop','ControlBase'):
        vals=b.metas[b.names[name]]
        for j in range(4,8):
            if vals[j]:
                assert vals[j]>>12==floor_pal
                vals[j]=(vals[j]&0xFFF)|(controls_index<<12)
    b.provenance.append({'name':'ControlTop/ControlBase','foreground_palette':'NPC_4 dark grey casing at indices 12/13/7; donor indicators and floor unchanged'})
    from hub_furnishings import append_hub_furnishings, architecture_collision
    append_hub_furnishings(b)
    assert len(b.tiles)<=512 and len(b.pals)<=13
    outputs={}
    # Extend the established 16x32 PC icon, preserving its silhouette and NPC_4
    # palette. This is a separate sprite; storage PCs keep their original art.
    console=Image.open(ROOT/'graphics/object_events/pics/rogue/pc_stand.png').copy()
    console.putpalette([v for color in palette('graphics/object_events/palettes/npc_4.pal') for v in color])
    for y in range(6,10):
        for x in range(1,15):
            if console.getpixel((x,y))==14:console.putpixel((x,y),12)
    for y in range(10,20):
        for x in range(1,15):
            if console.getpixel((x,y))==12:console.putpixel((x,y),13)
    for x in range(2,14):console.putpixel((x,26),13)
    # Tiny Adventure Setup preview: black screen, warm heading, pale menu rows,
    # and an accent cursor. Deliberate pixel marks rather than unreadable text.
    draw=ImageDraw.Draw(console);draw.rectangle((3,12,12,17),fill=15)
    draw.line((4,12,10,12),fill=1)
    draw.line((5,14,11,14),fill=14);draw.point((3,14),fill=1)
    draw.line((5,16,9,16),fill=11)
    assert console.size==(16,32) and max(console.tobytes())<16
    assert all((a==0)==(c==0) for a,c in zip(console.tobytes(),Image.open(ROOT/'graphics/object_events/pics/rogue/pc_stand.png').tobytes()))
    buf=io.BytesIO();console.save(buf,format='PNG')
    outputs['graphics/object_events/pics/rogue/adventure_console.png']=buf.getvalue()
    # The main map is now authored in Porymap. Never regenerate or replace it:
    # the assembly above defines stable asset IDs, not the saved room layout.
    authored=words('data/layouts/Rogue_Area_AdventureEntrance/map.bin')
    assert len(authored)==20*16
    blocks=[authored[y*20:(y+1)*20] for y in range(16)]
    for v in authored:
        assert (v&1023)<len(b.metas) or 512<=(v&1023)<521, ('Unknown room metatile',v&1023)
    def connection_blocks(mask):
        result=[row[:] for row in blocks]
        for bit,edges,pillar in [(1,range(2),2),(4,range(17,20),16)]:
            if not mask&bit:
                for edge in edges:
                    for y in range(8,16):result[y][edge]=b.names['Void']|0x3C00
                for y in range(8,12):result[y][pillar]=b.names['Pillar']|0x3C00
                result[12][pillar]=b.names['PillarEnd']|0x3C00
        if not mask&2:
            for x in (7,11):result[12][x]=b.names['Floor']|0x3000
            for x in range(7,12):
                for y in range(13,16):result[y][x]=b.names['Void']|0x3C00
        return result
    outputs[PRI+'/tiles.png']=png(b.tiles)
    outputs[SEC+'/tiles.png']=png(b.portal_tiles,48)
    outputs[PRI+'/metatiles.bin']=packed(sum(b.metas,[]))
    outputs[PRI+'/metatile_attributes.bin']=packed(b.attrs)
    outputs[SEC+'/metatiles.bin']=packed(sum(b.portal_metas,[]))
    outputs[SEC+'/metatile_attributes.bin']=packed([0]*9)
    for i in range(13):
        pal=b.pals[i] if i<len(b.pals) else [(0,0,0)]*16
        data=('JASC-PAL\r\n0100\r\n16\r\n'+'\r\n'.join(' '.join(map(str,c)) for c in pal)+'\r\n').encode()
        outputs[(PRI if i<6 else SEC)+f'/palettes/{i:02}.pal']=data
    for i,phase in enumerate(phases):outputs[SEC+f'/anim/{i}.png']=png(phase,48)
    border=(ROOT/'data/layouts/Rogue_Area_AdventureEntrance/border.bin').read_bytes()
    for facade,w,h,horizontal in [('Rogue_PortalCorridor_Horizontal',16,8,True),('Rogue_PortalCorridor_Vertical',8,12,False)]:
        arr=[]
        for y in range(h):
            for x in range(w):
                walk=(y in (2,3,4)) if horizontal else (x in (2,3,4))
                name='FloorDim20' if walk else 'Void'
                if horizontal and y==0:name='Wall'
                if horizontal and y==1:name='WallBase'
                if horizontal and y==2:name='FloorShadowDim20'
                if horizontal and y==6:name='Wall'
                if horizontal and y==7:name='WallBase'
                if not horizontal and x in (1,5):name='Pillar'
                arr.append(b.names[name]|(0x3000 if walk else 0x3C00))
        outputs[f'data/layouts/{facade}/map.bin']=packed(arr)
        outputs[f'data/layouts/{facade}/border.bin']=border
    header='#ifndef GUARD_PORTAL_ROOM_TILES_H\n#define GUARD_PORTAL_ROOM_TILES_H\n\n// Generated by scripts/generate_portal_room.py.\n'
    for name,n in b.names.items():header+=f'#define METATILE_PortalRoom_{name} 0x{n:03X}\n'
    header+='#define METATILE_PortalRoom_Opening 0x200\n\n#endif\n'
    outputs['include/constants/portal_room_tiles.h']=header.encode()
    outputs['data/tilesets/portal_room_sources.json']=(json.dumps({'metatiles':b.provenance,'primary_tile_count':len(b.tiles),'palette_count':len(b.pals),'portal_texture':'Original procedural 48x48 pixel vortex: concentric contracting purple bands, rotating red arcs, eight cyclic phases','assemblies':assemblies,'composition':'Galactic HQ native wall, floor, pillar and complete machinery assemblies; private floor palettes at 80/60/40/20 percent brightness echo outdoor hub warp shadows'},indent=2)+'\n').encode()
    for path,data in outputs.items():
        target=ROOT/path
        if args.check:
            assert target.exists() and target.read_bytes()==data, f'Stale asset: {path}'
        else:
            if not target.exists() or target.read_bytes()!=data:
                target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
    # Export review artifacts, with services marked independently from tile art.
    OUT.mkdir(parents=True,exist_ok=True)
    out=Image.new('RGB',(320,256))
    for y,row in enumerate(blocks):
        for x,v in enumerate(row):out.paste(b.render(v&1023),(x*16,y*16))
    out.resize((640,512),Image.Resampling.NEAREST).save(OUT/'room.png')
    preview_frames=[]
    for phase in phases:
        b.portal_tiles=phase
        frame=out.copy()
        for y in range(3):
            for x in range(3):frame.paste(b.render(512+y*3+x),((8+x)*16,(1+y)*16))
        preview_frames.append(frame.resize((640,512),Image.Resampling.NEAREST))
    preview_frames[0].save(OUT/'room-animated.gif',save_all=True,append_images=preview_frames[1:],duration=134,loop=0)
    gate_frames=[frame.crop((7*32,0,12*32,4*32)).resize((400,320),Image.Resampling.NEAREST) for frame in preview_frames]
    gate_frames[0].save(OUT/'gate-animated.gif',save_all=True,append_images=gate_frames[1:],duration=134,loop=0)
    strip=Image.new('RGB',(8*96,96))
    for i,frame in enumerate(preview_frames):strip.paste(frame.crop((8*32,32,11*32,4*32)),(i*96,0))
    strip.save(OUT/'gate-phases.png')
    b.portal_tiles=phases[0]
    events=json.loads((ROOT/'data/maps/Rogue_Area_AdventureEntrance/map.json').read_text())
    # Render actual event art with the runtime palettes and GBA feet alignment.
    sprites={
        'OBJ_EVENT_GFX_PC_STAND':('rogue/pc_stand','palettes/npc_4',16,32),
        'OBJ_EVENT_GFX_ADVENTURE_CONSOLE':('rogue/adventure_console','palettes/npc_4',16,32),
        'OBJ_EVENT_GFX_QUEST_BOARD':('rogue/quest_board','palettes/npc_1',32,32),
        'OBJ_EVENT_GFX_WORK_TABLE':('rogue/work_table','palettes/npc_1',32,32),
        'OBJ_EVENT_GFX_UNION_ROOM_NURSE':('people/union_room_attendant','palettes/npc_3',16,32),
        'OBJ_EVENT_GFX_SCIENTIST_2':('people/scientist_2','palettes/npc_1',16,32),
        'OBJ_EVENT_GFX_MISC_PEONIA':('rogue/npc/misc/peonia','pics/rogue/npc/misc/peonia',16,32),
        'OBJ_EVENT_GFX_FLAG_ADVENTURE':('rogue/flag_adventure','palettes/npc_3',32,32)}
    scene=out.copy();fresh_scene=out.copy()
    for e in sorted(events['object_events'],key=lambda e:e['y']):
        if e['graphics_id'] not in sprites:continue
        pic,pal,w,h=sprites[e['graphics_id']]
        indexed=Image.open(ROOT/f'graphics/object_events/pics/{pic}.png').crop((0,0,w,h))
        palpath=f'graphics/object_events/{pal}.pal'
        colors=palette(palpath) if (ROOT/palpath).exists() else [tuple(indexed.getpalette()[i:i+3]) for i in range(0,48,3)]
        sprite=Image.new('RGBA',(w,h))
        for y in range(h):
            for x in range(w):
                c=indexed.getpixel((x,y));sprite.putpixel((x,y),(*colors[c],255 if c else 0))
        position=(e['x']*16+8-w//2,(e['y']+1)*16-h)
        scene.paste(sprite,position,sprite)
        if e['flag']=='0':fresh_scene.paste(sprite,position,sprite)
    scene.save(OUT/'room-native.png')
    scene.resize((960,768),Image.Resampling.NEAREST).save(OUT/'room-inhabited.png')
    fresh_scene.resize((960,768),Image.Resampling.NEAREST).save(OUT/'room-first-visit.png')
    context=Image.new('RGB',(34*16,30*16))
    for yy in range(30):
        for xx in range(34):context.paste(b.render(b.names['RockFill']),(xx*16,yy*16))
    context.paste(scene,(7*16,7*16))
    for facade,w,h,horizontal in [('Rogue_PortalCorridor_Horizontal',16,8,True),('Rogue_PortalCorridor_Vertical',8,12,False)]:
        cells=words(f'data/layouts/{facade}/map.bin')
        if horizontal:
            for yy in range(8):
                for xx in range(7):
                    context.paste(b.render(cells[yy*w+w-7+xx]&1023),(xx*16,(7+8+yy)*16))
                    context.paste(b.render(cells[yy*w+xx]&1023),((7+20+xx)*16,(7+8+yy)*16))
        else:
            for yy in range(7):
                for xx in range(8):context.paste(b.render(cells[yy*w+xx]&1023),((7+6+xx)*16,(7+16+yy)*16))
    context.resize((1088,960),Image.Resampling.NEAREST).save(OUT/'room-context.png')
    variants=Image.new('RGB',(4*320,2*276))
    for mask in range(8):
        variant=scene.copy();closed=connection_blocks(mask)
        for yy in range(16):
            for xx in range(20):
                if closed[yy][xx]!=blocks[yy][xx]:variant.paste(b.render(closed[yy][xx]&1023),(xx*16,yy*16))
        ox=(mask%4)*320;oy=(mask//4)*276
        variants.paste(variant,(ox,oy+20));ImageDraw.Draw(variants).text((ox+4,oy+4),f'West {bool(mask&1)} / South {bool(mask&2)} / East {bool(mask&4)}',fill='white')
        if mask==3:variant.resize((960,768),Image.Resampling.NEAREST).save(OUT/'room-east-closed.png')
    variants.save(OUT/'connection-variants.png')
    # Native 240x160 camera windows expose joins that a zoomed-out plan can hide.
    for label,box in [('console',(40,0,280,160)),('west',(0,96,240,256)),('east',(80,96,320,256))]:
        scene.crop(box).resize((720,480),Image.Resampling.NEAREST).save(OUT/f'camera-{label}.png')
    for label,x,y in [('west-tunnel',0,11),('east-tunnel',19,11),('south-tunnel',9,15)]:
        cx=(x+7)*16+8;cy=(y+7)*16+8
        context.crop((cx-120,cy-80,cx+120,cy+80)).resize((720,480),Image.Resampling.NEAREST).save(OUT/f'camera-{label}.png')
    reference_builder=Builder();reference_image=Image.new('RGB',(15*16,19*16))
    for yy in range(19):
        for xx in range(15):
            mid=at(xx,yy)&1023;name=f'Reference_{mid:03X}'
            if name not in reference_builder.names:reference_builder.add(name,'galactic',mid)
            reference_image.paste(reference_builder.render(reference_builder.names[name]),(xx*16,yy*16))
    reference_image.resize((480,608),Image.Resampling.NEAREST).save(OUT/'reference-hq.png')
    overlay=out.resize((960,768),Image.Resampling.NEAREST);d=ImageDraw.Draw(overlay)
    labels={1:'GUIDANCE',2:'HUB UPGRADES',3:'STORAGE',4:'QUESTS',7:'REPLAY FLAG',8:'PEONIA',10:'TRIALS',11:'ADVENTURE'}
    for i,label in labels.items():
        e=events['object_events'][i-1];x=e['x'];y=e['y']
        d.rectangle((x*48,y*48,x*48+46,y*48+46),outline='#ffda70',width=2);d.text((x*48,y*48+16),label,fill='#ffda70')
    overlay.save(OUT/'room-events.png')
    # All connection masks must preserve console and service access, including
    # the worst case with every optional static service present.
    events=json.loads((ROOT/'data/maps/Rogue_Area_AdventureEntrance/map.json').read_text())
    occupied={(e['x'],e['y']) for e in events['object_events'] if e['script'] != 'NULL'}
    # The ruin maniac is a tutorial guest at an entrance, never the central spawn.
    occupied.discard((9,10))
    # Verify the authored departure movement itself, including every intermediate tile.
    import re
    script=(ROOT/'data/maps/Rogue_Area_AdventureEntrance/scripts.pory').read_text()
    movement=re.search(r'movement Rogue_Adventure_WalkIntoPortal\s*\{([^}]+)\}',script).group(1)
    route=[(9,7)]
    for direction,count in re.findall(r'walk_(right|left|up|down)(?:\s*\*\s*(\d+))?',movement):
        dx,dy={'right':(1,0),'left':(-1,0),'up':(0,-1),'down':(0,1)}[direction]
        for _ in range(int(count or 1)):route.append((route[-1][0]+dx,route[-1][1]+dy))
    assert route[-1]==(9,3)
    for x,y in route:
        assert not blocks[y][x]&0xC00 and (x,y) not in occupied, ('Departure blocked',x,y)
    console=events['object_events'][10]
    assert (console['x'],console['y'])==(9,6)
    for x,y in [(8,6),(10,6),(9,5)]:assert blocks[y][x]&0xC00
    assert events['warp_events'][6]['x']==9 and events['warp_events'][6]['y']==7
    layouts={l['id']:l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
    for connection in events['connections']:
        horizontal=connection['direction'] in ('left','right')
        suffix='Horizontal' if horizontal else 'Vertical'
        layout=layouts['LAYOUT_ROGUE_PORTAL_CORRIDOR_'+suffix.upper()]
        cells=words(layout['blockdata_filepath'])
        # The engine copies these strips using the map connection offset.
        for position in ((11,12) if horizontal else (8,9,10)):
            if horizontal:
                x=layout['width']-1 if connection['direction']=='left' else 0
                y=position-connection['offset']
            else:x=position-connection['offset'];y=0
            assert not cells[y*layout['width']+x]&0xC00, ('Corridor seam',suffix,x,y)
        # Compare the visible join too, not just the two walkable warp cells.
        positions=range(8,16) if horizontal else range(7,12)
        for p in positions:
            if horizontal:
                cx=layout['width']-1 if connection['direction']=='left' else 0
                cy=p-connection['offset'];mx=0 if connection['direction']=='left' else 19;my=p
            else:cx=p-connection['offset'];cy=0;mx=p;my=15
            facade_tile=b.render(cells[cy*layout['width']+cx]&1023)
            map_tile=b.render(blocks[my][mx]&1023)
            if not horizontal and p in (8,9,10):
                # South fade reaches its terminal palette in the lower 8px.
                assert map_tile.crop((0,8,16,16)).tobytes()==facade_tile.crop((0,0,16,8)).tobytes(), ('South fade seam',p)
            elif horizontal and connection['direction']=='left' and p in (10,11,12):
                # The shorter west buffer ends one fade step before the east.
                # Its facade supplies the final dark band at the mirrored distance.
                base='FloorShadow' if p==10 else 'Floor'
                assert map_tile.tobytes()==b.render(b.names[base+'Dim40']).tobytes()
                assert facade_tile.tobytes()==b.render(b.names[base+'Dim20']).tobytes()
            else:
                assert facade_tile.tobytes()==map_tile.tobytes(), ('Visible corridor seam',suffix,p)
    from collections import deque
    for mask in range(8):
        a=connection_blocks(mask)
        # Equal side connectivity must give symmetric structural collision about
        # the portal/console center x=9, including the centered three-wide exit.
        if bool(mask&1)==bool(mask&4):
            for yy in range(16):
                for xx in range(2,10):
                    assert bool(architecture_collision(a[yy][xx], b.names))==bool(architecture_collision(a[yy][18-xx], b.names)), ('Asymmetric room boundary',mask,xx,yy)
        seen={(9,7)};q=deque(seen)
        while q:
            x,y=q.popleft()
            for nx,ny in ((x-1,y),(x+1,y),(x,y-1),(x,y+1)):
                if 0<=nx<20 and 0<=ny<16 and not a[ny][nx]&0xC00 and (nx,ny) not in occupied and (nx,ny) not in seen:seen.add((nx,ny));q.append((nx,ny))
        for i in labels:
            e=events['object_events'][i-1];p=(e['x'],e['y']+1)
            assert p in seen,('Front interaction unreachable',mask,labels[i],p)
        assert (9,3) in seen
        # Keep the established story crossing and central arrival aisle clear.
        assert all((x,10) not in occupied and not a[10][x]&0xC00 for x in range(4,16))
        assert all((9,y) not in occupied and not a[y][9]&0xC00 for y in range(7,13 if not mask&2 else 14))
    print(f'Portal room {"verified" if args.check else "generated"}: {len(b.tiles)} primary tiles, {len(b.pals)} palettes; all 8 connection masks reachable.')

if __name__=='__main__':main()
