#!/usr/bin/env python3
"""Generate private Park entrance art; normal runs never rewrite the Park map."""
import argparse
import hashlib
import itertools
import json
from collections import deque
from PIL import Image, ImageDraw
from generate_portal_room import ROOT, Source, words, packed, png, palette
from generate_hub_tiles import render

OUT = ROOT/'build/park_tunnels'
ASSET = 'data/tilesets/secondary/park_tunnels'
MAP = 'data/layouts/Rogue_Area_RideTraining/map.bin'
DIRECTIONS = ('NORTH', 'EAST', 'SOUTH', 'WEST')
VECTORS = ((0,-1),(1,0),(0,1),(-1,0))


class Park:
    def __init__(self):
        self.config = json.loads((ROOT/'tools/data/park_tunnels.json').read_text())
        self.base = Source('general_hub', 'mauville')
        self.original = self.config.get('generation_base',words(MAP))[:]
        self.inside = set()
        for d, (x1,y1,x2,y2) in self.config['footprints'].items():
            for i, v in enumerate(self.config['original_footprints'][d]):
                x,y=x1+i%(x2-x1+1),y1+i//(x2-x1+1)
                self.original[y*34+x]=v; self.inside.add(y*34+x)
        self.used = {v&1023 for v in self.original+words('data/layouts/Rogue_Area_RideTraining/border.bin')}
        self.metas = [self.base.meta[1][i:i+8] for i in range(0,len(self.base.meta[1]),8)]
        self.attrs = self.base.attrs[1][:]
        self.tiles = [self.base.tile(512+i) for i in range(512)]
        refs=[v for mid in self.used for v in self.base.meta[mid//512][mid%512*8:mid%512*8+8]]
        assert sorted(m for m in self.used if m>=512)==self.config['original_secondary_ids']
        reserved=set(range(608,672))|{v&1023 for v in refs}
        assert not any(v>>12==6 for v in refs), 'Park palette 6 is now in use'
        self.free_tiles=iter(i for i in range(512,1024) if i not in reserved)
        self.free_metas=iter(i for i in range(512,1022) if i not in self.used)
        self.tile_ids={};self.meta_ids={};self.allocations={}
        self.colors=[(0,0,0),(115,197,164),(164,213,197),(65,180,131),
            (49,49,74),(82,82,106),(139,148,164),(213,222,246),
            (255,255,255),(106,123,123),(164,139,115),(131,106,98),
            (98,74,82),(65,57,65),(205,180,156),(24,32,41)]
        donor=self.config['donors']['rocks']
        self.cliff={n:render(self.base,n) for n in donor['metatiles']}
        donor=self.config['donors']['lab']
        self.lab=Source(donor['primary'],donor['secondary'])
        self.names=donor['metatiles']
        self.floor=render(self.lab,self.names['Floor'])
        self.images={};self.open=self.original[:];self.closed={}
        for d in DIRECTIONS:self.entrance(d)
        if 'authored_map' in self.config:
            self.open=self.config['authored_map'][:]
            # Preserve authored artwork while assigning behaviors by event position.
            thresholds={}
            for d,(dx,dy) in zip(DIRECTIONS,VECTORS):
                x,y=self.config['warp_centers'][d]
                for lane in (0,1):thresholds[(y+(lane if dx else 0))*34+x+(lane if dy else 0)]={'NORTH':0x64,'EAST':0x62,'SOUTH':0x65,'WEST':0x63}[d]
            for i,v in enumerate(self.open):
                mid=v&1023
                if mid>=512 and (self.attrs[mid-512]&255 in (0x62,0x63,0x64,0x65) or i in thresholds):
                    behavior=thresholds.get(i,0)
                    if self.attrs[mid-512]&255!=behavior:
                        mid=self.store_meta(self.metas[mid-512],(self.attrs[mid-512]&0xff00)|behavior,f'AuthoredBehavior_{mid:03X}_{behavior:02X}')
                        self.open[i]=(v&0xfc00)|mid

        self.add_metal_surrounds()
        # The front-facing door fills the whole opening, including the row
        # above the warp strip; a closed door must not retain a black window.
        door=Image.new('RGB',(32,32),(49,49,74));pen=ImageDraw.Draw(door)
        for l,r in ((1,14),(17,30)):
            pen.rectangle((l,1,r,30),fill=(82,82,106))
            pen.line((l,1,r,1),fill=(139,148,164))
            pen.line((l,1,l,29),fill=(139,148,164))
            pen.line((r,2,r,30),fill=(49,49,74))
            pen.line((l,30,r,30),fill=(49,49,74))
        self.closed['NORTH']={}
        for y in range(2):
            for x in range(2):
                mid=self.store_image(door.crop((x*16,y*16,x*16+16,y*16+16)),0,f'NorthDoorPanel_{x}_{y}')
                self.closed['NORTH'][(7+y)*34+21+x]=0x3c00|mid
        self.add_tree_rock_joins()
        for edit in self.config.get('authored_overrides',[]):
            self.open[edit['y']*34+edit['x']]=edit['tile']
        self.facades={}
        border=words('data/layouts/Rogue_Area_RideTraining/border.bin')
        for d in DIRECTIONS:
            horizontal=d in ('WEST','EAST');off=16 if horizontal else 18
            edge=[self.open[(off+i)*34+(0 if d=='WEST' else 33)] if horizontal else self.open[(0 if d=='NORTH' else 35)*34+off+i] for i in range(8)]
            # The first connected row/column continues the mouth; the rock roof
            # covers the passage behind it. Never stretch the threshold to infinity.
            cells=[]
            for y in range(8):
                for x in range(8):
                    depth=(7-x if d=='WEST' else x) if horizontal else (7-y if d=='NORTH' else y)
                    across=y if horizontal else x
                    v=edge[across]
                    if d!='NORTH' and depth>0:
                        if horizontal:
                            # Plateau continues off-screen, but the exposed lane
                            # ends under its roof after the first facade tile.
                            mid=0x69 if across==0 else (0x79 if across==7 else 0x71)
                            if across==7:mid=0x1f0 if x%2==0 else 0x1f1
                            v=0x3c00|mid
                        elif 0<=across<=7:
                            v=0x3c00|({0:0x70,7:0x72}.get(across,0x71))
                        else:
                            v=0x3c00|border[(y%2)*2+x%2]
                    if horizontal and across==7 and depth==0:
                        v=(v&0xfc00)|(0x1f1 if d=='WEST' else 0x1f0)
                    lane=d!='NORTH' and across in (3,4)
                    if depth==0 and not lane and not v&0xc00:
                        v|=0xc00  # Permanent scenery must not expose a walkable border bypass.
                    mid=v&1023
                    if mid>=512 and self.attrs[mid-512]&255:
                        mid=self.store_meta(self.metas[mid-512],self.attrs[mid-512]&0xff00,f'{d}_Facade')
                    cells.append((v&0xfc00)|mid)
            self.facades[d]=cells
        # Continue the newly authored western cliff above the tunnel as well.
        upper=[]
        for y in range(16):
            for x in range(8):
                if x==7:
                    v=self.open[y*34]|0xc00
                else:
                    v=0x3c00|(0x7c if y==9 else 0x71)
                upper.append(v)
        self.facades['WEST']=upper+self.facades['WEST']
        self.make_rock_closures()


    def facade_origin(self,d):
        return {'NORTH':(18,-8),'EAST':(34,16),'SOUTH':(18,36),'WEST':(-8,0)}[d]

    def make_rock_closures(self):
        # Replace complete openings and metal collars with their continuous
        # native rock cross-section. Keep the outer authored corners intact.
        regions={'NORTH':(20,6,23,9),'EAST':(30,18,33,21),'SOUTH':(20,33,23,35),'WEST':(0,18,3,21)}
        self.closed={};self.closed_facades={}
        for d,(x1,y1,x2,y2) in regions.items():
            patch={}
            for y in range(y1,y2+1):
                for x in range(x1,x2+1):
                    mid=(0x79 if y==9 else 0x71) if d=='NORTH' else ((0x69 if y==33 else 0x71) if d=='SOUTH' else ((0x70 if x==30 else 0x71) if d=='EAST' else (0x72 if x==3 else 0x71)))
                    i=y*34+x;v=0x3c00|mid
                    if self.open[i]!=v:patch[i]=v
            self.closed[d]=patch
            cells=self.facades[d][:]
            ox,oy=self.facade_origin(d)
            for i in range(len(cells)):
                fx,fy=i%8,i//8;wx,wy=ox+fx,oy+fy
                if d=='WEST' and fx==7 and wy in range(18,22):cells[i]=0x3c71
                elif d=='EAST' and fx==0 and wy in range(18,22):cells[i]=0x3c71
                elif d=='SOUTH' and fy==0 and wx in range(20,24):cells[i]=0x3c71
            self.closed_facades[d]=cells

    def add_tree_rock_joins(self):
        # Preserve both native palettes: tree pixels occupy the lower layer,
        # with matching holes cut in the rock overlay above them.
        def pixels(word):
            raw=self.base.tile(word)
            return bytes(raw[(7-y if word&2048 else y)*8+(7-x if word&1024 else x)] for y in range(8) for x in range(8))
        def tile(raw,bank):
            if raw not in self.tile_ids:
                n=self.allocate(self.free_tiles,'tree/rock join');self.tile_ids[raw]=n;self.tiles[n-512]=raw
            return self.tile_ids[raw]|(bank<<12)
        for join in self.config.get('tree_rock_joins',[]):
            mid=join['rock'];cap=join['cap'];vals=self.base.meta[0][mid*8:mid*8+8][:]
            canopy=self.base.meta[0][cap*8+4:cap*8+8]
            for q,word in enumerate(canopy):
                if not word:continue
                mask=pixels(word);low=pixels(vals[q]);high=pixels(vals[q+4])
                assert vals[q]>>12==word>>12, 'Tree overlay must use the native grass palette'
                vals[q]=tile(bytes(c or low[i] for i,c in enumerate(mask)),word>>12)
                vals[q+4]=tile(bytes(0 if c else high[i] for i,c in enumerate(mask)),vals[q+4]>>12)
            new=self.store_meta(vals,self.base.attrs[0][mid],f'TreeRock_{mid:03X}_{cap:03X}')
            if 'x' in join:
                i=join['y']*34+join['x'];self.open[i]=(self.open[i]&0xfc00)|new

    def add_metal_surrounds(self):
        # Overlay substantial steel collars on the reviewed terrain, outside
        # the two walkable lanes. Keep the original art allocations stable.
        source=Source('general_hub','mauville')
        source.meta=[self.base.meta[0],sum(self.metas,[])]
        source.tile=lambda word: self.base.tile(word) if word&1023<512 else self.tiles[(word&1023)-512]
        source.pal=lambda bank: self.colors if bank==6 else self.base.pal(bank)
        before=Image.new('RGB',(544,576))
        for i,v in enumerate(self.open):before.paste(render(source,v&1023),(i%34*16,i//34*16))
        after=before.copy();draw=ImageDraw.Draw(after)
        def steel(box,horizontal):
            l,t,r,b=box
            shades=[(49,49,74),(213,222,246),(139,148,164)]+[(139,148,164)]*6+[(82,82,106),(82,82,106),(49,49,74)]
            for n,color in enumerate(shades):
                if horizontal:draw.line((l,t+n,r-1,t+n),fill=color)
                else:draw.line((l+n,t,l+n,b-1),fill=color)
            # Recessed fasteners break up the broad steel face at tile spacing.
            for q in range((l if horizontal else t)+6,(r if horizontal else b)-3,16):
                x,y=(q,t+5) if horizontal else (l+5,q)
                draw.rectangle((x,y,x+2,y+2),fill=(49,49,74));draw.point((x,y),fill=(213,222,246))
        steel((324,100,380,112),True)
        steel((324,112,336,160),False);steel((368,112,380,160),False)
        for l,r in ((0,64),(480,544)):
            steel((l,292,r,304),True);steel((l,336,r,348),True)
        steel((324,528,336,576),False);steel((368,528,380,576),False)
        def end_cap(box,horizontal,far):
            l,t,r,b=box
            # Return the outer corner to the underlying rock along a chamfer.
            for a in range(3):
                for z in range(3-a):
                    if horizontal:
                        x=(r-1-a if far else l+a);y=t+z
                    else:
                        x=l+z;y=t+a
                    after.putpixel((x,y),before.getpixel((x,y)))
            if horizontal:
                x=r-4 if far else l+3
                draw.line((x,t+3,x,b-2),fill=(213,222,246))
                draw.line((x+(1 if far else -1),t+4,x+(1 if far else -1),b-1),fill=(82,82,106))
                draw.line((l,b-1,r-1,b-1),fill=(49,49,74))
            else:
                draw.line((l+3,t+2,r-3,t+2),fill=(213,222,246))
                draw.line((l+2,t+3,r-2,t+3),fill=(82,82,106))
        for t,b in ((292,304),(336,348)):
            end_cap((0,t,64,b),True,True)
            end_cap((480,t,544,b),True,False)
        # Dedicated projecting end blocks finish the side rails against grass.
        for l,r in ((56,64),(480,488)):
            for t,b in ((288,304),(336,352)):
                draw.polygon([(l+2,t),(r-3,t),(r-1,t+2),(r-1,b-3),(r-3,b-1),(l+2,b-1),(l,b-3),(l,t+2)],fill=(139,148,164),outline=(49,49,74))
                draw.line((l+2,t+1,r-3,t+1),fill=(213,222,246))
                draw.line((r-2,t+3,r-2,b-4),fill=(82,82,106))
                draw.rectangle((l+3,t+6,l+4,t+8),fill=(49,49,74))
        end_cap((324,528,336,576),False,False)
        end_cap((368,528,380,576),False,False)
        # Mitred north corners join the lintel to the posts, with heavy feet.
        for x,flip in ((324,False),(368,True)):
            for n in range(11):
                px=x+(11-n if flip else n)
                draw.point((px,101+n),fill=(82,82,106))
            for a in range(3):
                for z in range(3-a):
                    px=x+(11-a if flip else a);py=100+z
                    after.putpixel((px,py),before.getpixel((px,py)))
            draw.rectangle((x,155,x+11,159),fill=(82,82,106))
            draw.line((x,155,x+11,155),fill=(213,222,246))
            draw.line((x,159,x+11,159),fill=(49,49,74))
        for i,v in enumerate(self.open):
            x,y=i%34*16,i//34*16;box=(x,y,x+16,y+16)
            art=after.crop(box)
            if art.tobytes()!=before.crop(box).tobytes():
                mid=v&1023
                behavior=(self.attrs[mid-512] if mid>=512 else self.base.attrs[0][mid])&255
                self.open[i]=(v&0xfc00)|self.store_image(art,behavior,f'SteelSurround_{i%34}_{i//34}')

    def labpic(self,name):return render(self.lab,self.names[name])

    @staticmethod
    def allocate(slots, kind):
        slot=next(slots,None)
        assert slot is not None, f"Park {kind} capacity exhausted; refusing to overwrite reserved assets"
        return slot

    def store_meta(self,vals,attr,label):
        key=tuple(vals),attr
        if key not in self.meta_ids:
            mid=self.allocate(self.free_metas,"metatile");self.meta_ids[key]=mid
            self.metas[mid-512]=vals;self.attrs[mid-512]=attr
        mid=self.meta_ids[key];self.allocations[label]=mid
        return mid

    def store_image(self,im,behavior,label):
        vals=[]
        for y in (0,8):
            for x in (0,8):
                # Preserve native cliff/grass palettes at 8x8 granularity instead
                # of crushing all terrain colors into the metal palette.
                pixels=list(im.crop((x,y,x+8,y+8)).getdata())
                choices=[]
                for bank in range(7):
                    colors=self.colors if bank==6 else self.base.pal(bank)
                    mapping={c:min(range(1,16),key=lambda i:sum((c[k]-colors[i][k])**2 for k in range(3))) for c in set(pixels)}
                    error=sum(sum((c[k]-colors[mapping[c]][k])**2 for k in range(3)) for c in pixels)
                    choices.append((error,bank,bytes(mapping[c] for c in pixels)))
                _,bank,raw=min(choices)
                if raw not in self.tile_ids:
                    tile=self.allocate(self.free_tiles,"graphics");self.tile_ids[raw]=tile;self.tiles[tile-512]=raw
                vals.append(self.tile_ids[raw]|(bank<<12))
        return self.store_meta(vals+[0]*4,0x1000|behavior,label)

    def entrance(self,direction):
        x1,y1,x2,y2=self.config['footprints'][direction];w,h=(x2-x1+1)*16,(y2-y1+1)*16
        base=Image.new('RGB',(w,h))
        for y in range(y1,y2+1):
            for x in range(x1,x2+1):base.paste(render(self.base,self.original[y*34+x]&1023),((x-x1)*16,(y-y1)*16))
        def mound(im,rows,cols):
            for ty in range(rows):
                for tx in range(cols):
                    row=0 if ty==0 else (2 if ty==rows-1 else 1)
                    col=0 if tx==0 else (2 if tx==cols-1 else 1)
                    mid=((0x68,0x69,0x6a),(0x70,0x71,0x72),(0x78,0x79,0x7a))[row][col]
                    im.paste(self.cliff[mid],(tx*16,ty*16))
        def passage(im,box,direction):
            l,t,r,b=box;draw=ImageDraw.Draw(im)
            # Quiet stone thresholds: no oversized tiled lab-floor checkerboard.
            for y in range(t,b):
                for x in range(l,r):
                    depth=(b-1-y if direction=='NORTH' else y-t) if direction in ('NORTH','SOUTH') else (r-1-x if direction=='WEST' else x-l)
                    colors=((106,123,123),(82,82,106),(49,49,74),(24,32,41))
                    im.putpixel((x,y),colors[min(3,depth//8)])
            if direction in ('NORTH','SOUTH'):
                for x in (l-2,r):
                    draw.line((x,t,x,b-1),fill=(49,49,74))
                    draw.line((x+1,t,x+1,b-1),fill=(139,148,164))
            else:
                for y in (t-2,b):
                    draw.line((l,y,r-1,y),fill=(49,49,74))
                    draw.line((l,y+1,r-1,y+1),fill=(139,148,164))
        for closed in (False,True):
            im=base.copy();draw=ImageDraw.Draw(im)
            if direction=='NORTH':
                mound(im,4,6)
                # A narrow steel reveal is inset into the complete native cliff.
                passage(im,(32,48,64,80),direction)
                draw.rectangle((30,44,65,47),fill=(139,148,164))
                draw.line((31,44,64,44),fill=(213,222,246))
                if closed:
                    for x in (32,48):im.paste(self.labpic('Door'),(x,48))
            elif direction=='SOUTH':
                mound(im,3,6)
                for tx in range(6):im.paste(self.cliff[0x70 if tx==0 else (0x72 if tx==5 else 0x71)],(tx*16,32))
                passage(im,(32,0,64,48),direction)
                if closed:
                    draw.rectangle((32,32,63,47),fill=(82,82,106))
                    for y in (33,38,43):
                        draw.line((32,y,63,y),fill=(139,148,164));draw.line((32,y+2,63,y+2),fill=(49,49,74))
            else:
                mound(im,6,4)
                for ty in range(6):im.paste(self.cliff[0x69 if ty==0 else (0x79 if ty==5 else 0x71)],(0 if direction=='WEST' else 48,ty*16))
                passage(im,(0,32,64,64),direction)
                if closed:
                    left=0 if direction=='WEST' else 48
                    draw.rectangle((left,32,left+15,63),fill=(82,82,106))
                    for x in range(left+1,left+16,5):
                        draw.line((x,32,x,63),fill=(139,148,164));draw.line((x+2,32,x+2,63),fill=(49,49,74))
            cells=self.original[:]
            cx,cy=self.config['warp_centers'][direction]
            for y in range(y1,y2+1):
                for x in range(x1,x2+1):
                    idx=y*34+x;crop=im.crop(((x-x1)*16,(y-y1)*16,(x-x1+1)*16,(y-y1+1)*16))
                    lane=(x in (21,22) and y>=8) if direction=='NORTH' else (x in (21,22) if direction=='SOUTH' else y in (19,20))
                    threshold=(y==cy if direction in ('NORTH','SOUTH') else x==cx) and lane
                    solid=not lane
                    # Preserve the untouched perimeter of the assembly exactly.
                    if crop.tobytes()==render(self.base,self.original[idx]&1023).tobytes():
                        cells[idx]=self.original[idx];continue
                    if closed and threshold:solid=True
                    behavior={'NORTH':0x64,'EAST':0x62,'SOUTH':0x65,'WEST':0x63}[direction] if threshold and not closed else 0
                    mid=self.store_image(crop,behavior,f'{direction}_{"Closed" if closed else "Open"}_{x}_{y}')
                    cells[idx]=0x3000|(0xc00 if solid else 0)|mid
            if closed:self.closed[direction]={i:v for i,v in enumerate(cells) if v!=self.open[i] and i in self.region(direction)}
            else:
                for i in self.region(direction):self.open[i]=cells[i]
            self.images[direction,closed]=im

    def region(self,d):
        x1,y1,x2,y2=self.config['footprints'][d]
        return {y*34+x for y in range(y1,y2+1) for x in range(x1,x2+1)}

    def outputs(self):
        result={ASSET+'/tiles.png':png(self.tiles,512),ASSET+'/metatiles.bin':packed(sum(self.metas,[])),ASSET+'/metatile_attributes.bin':packed(self.attrs)}
        for i in range(16):
            result[f'{ASSET}/palettes/{i:02}.pal']=(ROOT/f'data/tilesets/secondary/mauville/palettes/{i:02}.pal').read_bytes()
        result[ASSET+'/palettes/06.pal']=('JASC-PAL\r\n0100\r\n16\r\n'+'\r\n'.join(' '.join(map(str,c)) for c in self.colors)+'\r\n').encode()
        header='// Generated by scripts/generate_park_tunnels.py; do not edit.\n'
        header+='struct ParkTunnelPatch { s16 x, y; u16 tile; };\n'
        for d,patch in self.closed.items():
            header+=f'static const struct ParkTunnelPatch sParkTunnelClosed{d.title()}[] = {{\n'
            for i,v in sorted(patch.items()):header+=f'    {{{i%34}, {i//34}, 0x{v:04X}}},\n'
            ox,oy=self.facade_origin(d)
            for i,v in enumerate(self.closed_facades[d]):
                if v!=self.facades[d][i]:header+=f'    {{{ox+i%8}, {oy+i//8}, 0x{v:04X}}},\n'

            header+='};\n'
        result['src/data/park_tunnel_patches.h']=header.encode()
        labels='// Generated Park tunnel metatiles.\n'
        for name,mid in self.allocations.items():labels+=f'#define METATILE_ParkTunnel_{name} 0x{mid:03X}\n'
        result['include/constants/park_tunnel_tiles.h']=labels.encode()
        result[ASSET+'/sources.json']=(json.dumps({'manifest':'tools/data/park_tunnels.json','metatiles':self.allocations,'graphics_slots':sorted(self.tile_ids.values()),'reserved_animation_slots':[608,671]},indent=2)+'\n').encode()
        for d,cells in self.facades.items():
            path=f'data/layouts/Rogue_ParkTunnel_{d.title()}'
            result[path+'/map.bin']=packed(cells)
            result[path+'/border.bin']=(ROOT/'data/layouts/Rogue_Area_RideTraining/border.bin').read_bytes()
        return result

    def verify(self):
        current=words(MAP)
        assert hashlib.sha256((ROOT/'data/layouts/Rogue_Area_RideTraining/border.bin').read_bytes()).hexdigest()==self.config['border_sha256']
        assert hashlib.sha256((ROOT/'data/maps/Rogue_Area_RideTraining/scripts.pory').read_bytes()).hexdigest()==self.config['script_sha256']
        assert hashlib.sha256(self.outputs()[ASSET+'/sources.json']).hexdigest()==self.config['allocation_sha256'], 'Allocation changed; review the manifest before accepting new slots'
        layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
        layout=next(l for l in layouts if l['id']=='LAYOUT_ROGUE_AREA_RIDE_TRAINING')
        assert (layout['width'],layout['height'],layout['primary_tileset'],layout['secondary_tileset'])==(34,36,'gTileset_GeneralHub','gTileset_ParkTunnels')
        outside=packed([v for i,v in enumerate(current) if i not in self.inside])
        assert current==self.open, 'Authored Park map differs from its reviewed snapshot; do not overwrite Porymap edits'
        events=json.loads((ROOT/'data/maps/Rogue_Area_RideTraining/map.json').read_text())
        connection_names=dict(zip(DIRECTIONS,('up','right','down','left')))
        src=Source('general_hub','park_tunnels')
        runtime=(ROOT/'src/rogue_hub.c').read_text()
        for d in DIRECTIONS:
            horizontal=d in ('EAST','WEST');off=(0 if d=='WEST' else 16) if horizontal else 18
            connection=next(c for c in events['connections'] if c['direction']==connection_names[d])
            assert connection['map']==f'MAP_ROGUE_PARK_TUNNEL_{d}' and connection['offset']==off
            facade=next(l for l in layouts if l['id']==f'LAYOUT_ROGUE_PARK_TUNNEL_{d}')
            assert (facade['width'],facade['height'],facade['primary_tileset'],facade['secondary_tileset'])==(8,24 if d=='WEST' else 8,'gTileset_GeneralHub','gTileset_ParkTunnels')
            assert f'ApplyParkTunnelClosedPatch(sParkTunnelClosed{d.title()}, ARRAY_COUNT(sParkTunnelClosed{d.title()}))' in runtime
            assert all(v&0xc00 and v&1023<512 for v in self.closed[d].values()), 'Closed entrances must be solid native rock without metal'
            if d!='NORTH':
                for fy in range(len(self.facades[d])//8):
                    for fx in range(8):
                        depth=(7-fx if d=='WEST' else fx) if horizontal else fy
                        if depth>=2:
                            assert self.facades[d][fy*8+fx]&1023<512, 'Only native solid terrain may continue past the covered threshold'
                            assert self.facades[d][fy*8+fx]&0xc00, 'Off-map rock/forest must remain solid'

            for i in range(8):
                off=16 if horizontal else 18
                edge=self.open[(off+i)*34+(0 if d=='WEST' else 33)] if horizontal else self.open[(0 if d=='NORTH' else 35)*34+off+i]
                adjoining=self.facades[d][(i+(16 if d=='WEST' else 0))*8+(7 if d=='WEST' else 0)] if horizontal else self.facades[d][(56 if d=='NORTH' else 0)+i]
                edge_pic=render(src,edge&1023); adjoining_pic=render(src,adjoining&1023)
                if horizontal and i==7:
                    # Adjacent left/right canopy halves differ below the rock seam.
                    edge_pic=edge_pic.crop((0,0,16,8));adjoining_pic=adjoining_pic.crop((0,0,16,8))
                assert edge_pic.tobytes()==adjoining_pic.tobytes(), ('Facade seam',d,i)
                lane=d!='NORTH' and i in (3,4)
                if not lane and not edge&0xc00:
                    assert adjoining&0xc00, ('Walkable facade bypass',d,i)
                else:
                    assert edge&0xfc00==adjoining&0xfc00
        for bank in range(16):
            if bank!=6:
                assert (ROOT/f'{ASSET}/palettes/{bank:02}.pal').read_bytes()==(ROOT/f'data/tilesets/secondary/mauville/palettes/{bank:02}.pal').read_bytes()
        assert hashlib.sha256(json.dumps({k:v for k,v in events.items() if k!='connections'},sort_keys=True).encode()).hexdigest()==self.config['events_sha256'], 'Park events or properties changed'
        assert current==self.open, 'Authored entrances differ; normal generation will not overwrite them'
        warp_cells={w['y']*34+w['x'] for w in events['warp_events'][:8]}
        for i,v in enumerate(current):
            mid=v&1023
            behavior=(self.attrs[mid-512] if mid>=512 else self.base.attrs[0][mid])&255
            assert (behavior in (0x62,0x63,0x64,0x65))==(i in warp_cells), ('Stray or missing directional warp behavior',i%34,i//34)

        bodies={0x1d4,0x1d5,0x1d6,0x1d7}
        bottoms={0x1dc,0x1dd,0x1de,0x1df,0x1e4,0x1e5,0x1e6,0x1e7,0x1f2,0x1f3}
        caps={0x1ce,0x1cf,0x1f0,0x1f1}|{v for k,v in self.allocations.items() if k.startswith('TreeRock_')}
        for i,v in enumerate(current):
            mid=v&1023;y=i//34
            if mid in bodies:
                x=i%34
                if mid in (0x1d4,0x1d6) and x<33:assert current[i+1]&1023 in (0x1d5,0x1d7), ('Missing right tree half',x,y)
                if mid in (0x1d5,0x1d7) and x>0:assert current[i-1]&1023 in (0x1d4,0x1d6), ('Missing left tree half',x,y)
                assert y==0 or current[i-34]&1023 in caps|bottoms, ('Missing tree canopy',i%34,y)
                assert y==35 or current[i+34]&1023 in bottoms, ('Tree cut off by terrain',i%34,y)
            if mid in bottoms:
                assert y==0 or current[i-34]&1023 in bodies, ('Detached tree base',i%34,y)
        for mid in self.used:
            if mid>=512:
                assert self.metas[mid-512]==self.base.meta[1][(mid-512)*8:(mid-512)*8+8]
                assert self.attrs[mid-512]==self.base.attrs[1][mid-512]
        assert not set(self.tile_ids.values()) & set(range(608,672))
        assert all(self.tiles[i-512]==self.base.tile(i) for i in range(512,1024) if i not in self.tile_ids.values())
        for flags in itertools.product((False,True),repeat=4):
            cells=self.open[:]
            for d,opened in zip(DIRECTIONS,flags):
                if not opened:
                    for i,v in self.closed[d].items():cells[i]=v
            # Runtime patches include the first off-map strip: verify each
            # mixed state against the facade actually visible in that state.
            for d,opened in zip(DIRECTIONS,flags):
                facade=self.facades[d] if opened else self.closed_facades[d]
                ox,oy=self.facade_origin(d)
                for i,v in enumerate(facade):
                    if v==self.facades[d][i]:continue
                    wx,wy=ox+i%8,oy+i//8
                    assert -7<=wx<41 and -7<=wy<43, ('Patch outside map grid',d,wx,wy)
                    assert v&0xc00 and v&1023<512, ('Metal or walkable closed facade',d,wx,wy)
                    x,y=min(33,max(0,wx)),min(35,max(0,wy))
                    assert cells[y*34+x]&1023==v&1023, ('Closed rock seam',d,wx,wy)
            seen={(21,19)};queue=deque(seen)
            while queue:
                x,y=queue.popleft()
                for dx,dy in VECTORS:
                    p=x+dx,y+dy
                    if 0<=p[0]<34 and 0<=p[1]<36 and p not in seen and not cells[p[1]*34+p[0]]&0xc00:seen.add(p);queue.append(p)
            for d,opened,(dx,dy) in zip(DIRECTIONS,flags,VECTORS):
                x,y=self.config['warp_centers'][d]
                for n in (0,1):
                    px,py=x+(n if dy else 0),y+(n if dx else 0)
                    assert ((px,py) in seen)==opened,(flags,d,px,py)
                    mid=cells[py*34+px]&1023
                    behavior=(self.attrs[mid-512] if mid>=512 else self.base.attrs[0][mid])&255
                    if opened:assert behavior=={'NORTH':0x64,'EAST':0x62,'SOUTH':0x65,'WEST':0x63}[d]
                    else:assert behavior not in (0x62,0x63,0x64,0x65)
                    if opened:
                        assert (px-dx,py-dy) in seen
                        step=cells[(py-dy)*34+px-dx]&1023
                        attrs=self.attrs[step-512] if step>=512 else self.base.attrs[0][step]
                        assert attrs&255==0
            assert all(cells[i]==self.open[i] for i in range(len(cells)) if i not in self.inside)
        print('Park tunnels: all 16 connection combinations, eight warp lanes, arrivals, unchanged surroundings/events and animation reservations PASS.')

    def previews(self):
        OUT.mkdir(parents=True,exist_ok=True)
        src=Source('general_hub','park_tunnels');cache={}
        def picture(v):
            mid=v&1023
            if mid not in cache:cache[mid]=render(src,mid)
            return cache[mid]
        guide=Image.new('RGB',(660,250),(28,32,39));labels=ImageDraw.Draw(guide)
        groups=[('Grass edge',(0x1ce,0x1cf)),('Straight rock face',(0x1f0,0x1f1)),('Rounded rock corners',(self.allocations['TreeRock_078_1CE'],self.allocations['TreeRock_07A_1CF']))]
        for col,(title,pair) in enumerate(groups):
            labels.text((col*220+12,8),title,fill='white')
            labels.text((col*220+12,24),' / '.join(f'0x{mid:03X}' for mid in pair),fill='white')
            for row,tiles in enumerate((pair,(0x1d4,0x1d5),(0x1e4,0x1e5))):
                for x,mid in enumerate(tiles):guide.paste(picture(mid).resize((64,64),Image.Resampling.NEAREST),(col*220+12+x*64,48+row*64))
        guide.save(OUT/'terrain-join-reference.png')
        for closed in (False,True):
            cells=self.open[:]
            if closed:
                for patch in self.closed.values():
                    for i,v in patch.items():cells[i]=v
            im=Image.new('RGB',(34*16,36*16))
            for i,v in enumerate(cells):im.paste(picture(v),(i%34*16,i//34*16))
            im.save(OUT/('closed.png' if closed else 'open.png'))
            facades=self.closed_facades if closed else self.facades
            context=Image.new('RGB',(50*16,52*16))
            border=words('data/layouts/Rogue_Area_RideTraining/border.bin')
            for y in range(-8,44):
                for x in range(-8,42):
                    v=border[(y%2)*2+x%2]
                    if 0<=x<34 and 0<=y<36:v=cells[y*34+x]
                    else:
                        if -8<=x<0 and 0<=y<24:v=facades['WEST'][y*8+x+8]
                        elif 34<=x<42 and 16<=y<24:v=facades['EAST'][(y-16)*8+x-34]
                        elif -8<=y<0 and 18<=x<26:v=facades['NORTH'][(y+8)*8+x-18]
                        elif 36<=y<44 and 18<=x<26:v=facades['SOUTH'][(y-36)*8+x-18]
                    context.paste(picture(v),((x+8)*16,(y+8)*16))
            context.save(OUT/('context-closed.png' if closed else 'context-open.png'))

            for d,(x1,y1,x2,y2) in self.config['footprints'].items():
                crop=im.crop((x1*16,y1*16,(x2+1)*16,(y2+1)*16))
                crop.resize((crop.width*4,crop.height*4),Image.Resampling.NEAREST).save(OUT/f'{d.lower()}-{"closed" if closed else "open"}.png')


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--check',action='store_true');parser.add_argument('--init-layout',action='store_true');args=parser.parse_args()
    assert not(args.check and args.init_layout)
    park=Park()
    if args.init_layout:
        assert words(MAP)==park.original, 'Initialization requires the original authored entrances'
        (ROOT/MAP).write_bytes(packed(park.open))
    outputs=park.outputs()
    assert hashlib.sha256(outputs[ASSET+'/sources.json']).hexdigest()==park.config['allocation_sha256'], 'Allocation changed; review the manifest before writing assets'
    for path,data in outputs.items():
        p=ROOT/path
        if args.check:assert p.exists() and p.read_bytes()==data,('Stale Park asset',path)
        elif not p.exists() or p.read_bytes()!=data:p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
    park.verify();park.previews()
    print(f'Park art: {len(park.tile_ids)} new graphics tiles, {len(park.meta_ids)} metatiles. Previews: {OUT}')


if __name__=='__main__':main()
