#!/usr/bin/env python3
"""Pack the shared hub catalogue; normal generation NEVER writes map/border files.

--convert-layouts performs the explicit, one-time private-to-shared ID conversion.
The original private tilesets are immutable composition inputs, not runtime sets.
"""
import argparse
import hashlib
import json
from functools import lru_cache
from PIL import Image, ImageDraw
from generate_portal_room import ROOT, Source, words, packed, png, palette

PRI = 'data/tilesets/primary/hub_architecture'
SEC = 'data/tilesets/secondary/hub_furnishings'
OUT = ROOT/'build/hub_tiles'
BLACK = (0, 0, 0)


def render(source, mid):
    im = Image.new('RGB', (16, 16))
    for j, word in enumerate(source.meta[mid//512][mid%512*8:mid%512*8+8]):
        raw, pal = source.tile(word), source.pal(word >> 12)
        for y in range(8):
            for x in range(8):
                c = raw[(7-y if word&2048 else y)*8+(7-x if word&1024 else x)]
                if j < 4 or c:
                    im.putpixel((j%2*8+x, j%4//2*8+y), pal[c])
    return im


class Catalogue:
    def __init__(self):
        self.config = json.loads((ROOT/'tools/data/hub_tiles.json').read_text())
        self.sources = {key: Source(key,key) for key in self.config['rooms']}
        self.sources.update({key: Source(d['primary'],d['secondary'],d['graphics'])
                             for key,d in self.config['donors'].items()})
        self.records = []
        for key in self.config['rooms']:
            for mid in range(len(self.sources[key].attrs[0])):
                self.records.append((key,mid,True))
        for key,d in self.config['donors'].items():
            self.records.extend((key,mid,False) for mid in d['metatiles'])
        # Exact existing colors first. Tile-local color sets share palette banks;
        # foreground transparency remains index zero, including opaque black.
        sets = set()
        for key,mid,exact in self.records:
            if exact:
                for j,v in enumerate(self.meta(key,mid)):
                    sets.add(self.colors(key,v,j<4))
        banks = []
        for cs in sorted(sets,key=lambda s:(-len(s),sorted(s))):
            if any(cs <= b for b in banks):
                continue
            fits = [(len(cs-b),i) for i,b in enumerate(banks) if len(cs|b)<=15]
            if fits:
                banks[min(fits)[1]] |= cs
            else:
                banks.append(set(cs))
        assert len(banks)<=11, ('Existing room palette overflow',len(banks))
        # Fill remaining static banks with the most useful full donor color sets.
        candidates = {}
        for key,mid,exact in self.records:
            if not exact:
                for j,v in enumerate(self.meta(key,mid)):
                    cs=self.colors(key,v,j<4)
                    if len(cs)<=15 and not any(cs<=b for b in banks):
                        candidates[cs]=candidates.get(cs,0)+1
        while len(banks)<11:
            chosen=max(candidates,key=lambda c:sum(n for s,n in candidates.items() if s<=c),default=frozenset())
            banks.append(set(chosen))
            candidates={s:n for s,n in candidates.items() if not s<=chosen}
        self.pals=[[BLACK]+sorted(b)+[BLACK]*(15-len(b)) for b in banks]
        door=Image.open(ROOT/'graphics/door_anims/lab_junction.png').getpalette()
        self.pals.append([tuple(door[i:i+3]) for i in range(0,48,3)])
        portal=self.sources['portal_room'];self.pals.append(portal.pal(portal.meta[1][0]>>12))
        self.tiles=[bytes(64)]
        self.metas=[];self.attrs=[];self.mapping={};self.aliases={}
        self.recolored=set()
        donors_started=False
        for key,mid,exact in self.records:
            if not exact and not donors_started:
                assert len(self.metas)<=512
                self.metas.extend([[0]*8 for _ in range(512-len(self.metas))])
                self.attrs.extend([0]*(512-len(self.attrs)))
                donors_started=True
            vals=[self.import_tile(key,v,j<4,exact) for j,v in enumerate(self.meta(key,mid))]
            attr=self.sources[key].attrs[mid//512][mid%512]
            if not exact:
                attr &= 0xF000 # No donor puzzles, teleporters, conveyors or warps.
            signature=(tuple(vals),attr)
            if signature not in self.aliases:
                self.aliases[signature]=len(self.metas)
                self.metas.append(vals);self.attrs.append(attr)
            self.mapping.setdefault(key,{})[mid]=self.aliases[signature]
        assert len(self.metas)<=1015, ('Metatile overflow',len(self.metas))
        # Animation has dedicated VRAM slots at 512..547. Static tile allocation
        # skips them, so the secondary callback cannot overwrite decorations.
        while len(self.tiles)<548:self.tiles.append(bytes(64))
        for i in range(36):self.tiles[512+i]=portal.tile(512+i)
        self.portal_start=len(self.metas)
        for i in range(9):
            self.mapping['portal_room'][512+i]=len(self.metas)
            self.metas.append([(v&0xFFF)|(12<<12) if j<4 else 0 for j,v in enumerate(portal.meta[1][i*8:i*8+8])])
            self.attrs.append(portal.attrs[1][i])
        assert len(self.tiles)<=1024, ('Graphics tile overflow',len(self.tiles))
        digest=hashlib.sha256(json.dumps(json.loads(json.dumps(self.mapping)),sort_keys=True).encode()).hexdigest()
        if 'id_layout_sha256' in self.config:
            assert digest==self.config['id_layout_sha256'], ('Hub IDs changed: a reviewed map/prefab/runtime remap is required; do not regenerate over authored layouts',digest)
        # Append compositions at explicit IDs, without repacking established art.
        self.compositions={}
        for entry in self.config.get('compositions',[]):
            assert entry['id']==len(self.metas), ('Composition IDs must be appended',entry)
            def resolve(ref):
                key,name=ref
                return self.mapping[key][self.config['rooms'][key]['names'][name]]
            background=resolve(entry['background']);foreground=resolve(entry['foreground'])
            self.compositions[entry['name']]=len(self.metas)
            vals=self.metas[background][:4]+self.metas[foreground][4:]
            # Small native-tile variants reuse the original frame and palette.
            # Always append edited graphics; existing source tiles stay intact.
            for j in range(4,8):
                word=vals[j];original=self.tiles[word&1023]
                raw=bytearray(original[(7-y if word&2048 else y)*8+(7-x if word&1024 else x)] for y in range(8) for x in range(8))
                changed=False
                for edit in entry.get('paint',[]):
                    x1,y1,x2,y2=edit['rect']
                    assert 0<=x1<=x2<16 and 0<=y1<=y2<16
                    for y in range(8):
                        for x in range(8):
                            if x1<=j%2*8+x<=x2 and y1<=j%4//2*8+y<=y2:
                                raw[y*8+x]=self.pals[word>>12].index(tuple(edit['color']),1)
                                changed=True
                if changed:
                    vals[j]=len(self.tiles)|(word&0xF000)
                    self.tiles.append(bytes(raw))
            self.metas.append(vals)
            self.attrs.append(self.attrs[foreground])
        # Preserve explicit Porymap variants in unused primary slots without
        # changing established IDs or the immutable donor metatiles.
        for entry in self.config.get('variants', []):
            mid=entry['id'];key,name=entry['source']
            original=self.mapping[key][self.config['rooms'][key]['names'][name]]
            assert 0<=mid<512 and self.metas[mid]==[0]*8 and self.attrs[mid]==0, ('Variant slot is occupied',mid)
            assert entry['layer_type'] in (0,1,2)
            self.metas[mid]=self.metas[original][:]
            self.attrs[mid]=(self.attrs[original]&0x0FFF)|(entry['layer_type']<<12)
        from generate_terrarium_tiles import append_terrariums
        append_terrariums(self)
        from berry_lab_tiles import append_berry_lab
        append_berry_lab(self)
        # Capsules are wall fixtures: replace their light floor backing in place.
        # Preserve IDs, foreground art, attributes, and immutable private sources.
        shadow=self.mapping['lab_junction'][self.config['rooms']['lab_junction']['names']['FloorShadow']]
        self.capsule_originals={}
        self.capsule_original_tiles={}
        for x in range(3):
            mid=self.mapping['safari_lab'][self.config['rooms']['safari_lab']['names'][f'Capsule_{x}_3']]
            assert mid==99+x, 'Capsule IDs moved'
            self.capsule_originals[mid]=self.metas[mid][:]
            # The green capsule interior is part of the background, too.
            # Repaint only its exposed floor, retaining the original silhouette.
            for j,word in enumerate(self.metas[mid][:4]):
                idx=word&1023;raw=self.tiles[idx];pal=self.pals[word>>12]
                sw=self.metas[shadow][j];shade=self.tiles[sw&1023];sp=self.pals[sw>>12]
                pixels=bytes(raw[(7-y if word&2048 else y)*8+(7-x if word&1024 else x)] for y in range(8) for x in range(8))
                replacement=bytes(pal.index((106,123,123)) if pal[pixels[k]]==(106,123,123) else pal.index(sp[shade[k]]) for k in range(64))
                if replacement==pixels:continue
                assert not word&0xC00, 'Review flipped capsule background'
                assert all(m==mid for m,meta in enumerate(self.metas) if any(v&1023==idx for v in meta)), 'Capsule graphics are shared'
                self.capsule_original_tiles[idx]=raw
                self.tiles[idx]=replacement
        assert len(self.metas)==884, "Supply Depot allocation moved; review stable IDs"
        self.supply_depot={}
        for x,side in enumerate(('Left','Right')):
            names=self.config['rooms']['lab_junction']['names']
            top=self.metas[self.mapping['lab_junction'][names[f'Decor_Table_{x}_0']]]
            bottom=self.metas[self.mapping['lab_junction'][names[f'Decor_Table_{x}_1']]]
            self.supply_depot['Counter'+side]=len(self.metas)
            self.metas.append(self.metas[self.mapping['lab_junction'][names['Floor']]][:4]+top[6:8]+bottom[6:8])
            self.attrs.append(0x1080) # MB_COUNTER, below object sprites.
        # Complete equipment assemblies with the correct exposed floor backing.
        names=self.config['rooms']['lab_junction']['names']
        for name,back in [(f'ComputerShadow_{x}', 'FloorShadow') for x in range(2)]+[(f'SpecimenFloor_{x}_{y}', 'FloorShadow' if y==0 else 'Floor') for y in range(3) for x in range(2)]:
            source=f'ComputerBench_{name[-1]}_1' if name.startswith('Computer') else 'SpecimenBank_'+name.removeprefix('SpecimenFloor_')
            foreground=self.metas[self.mapping['lab_junction'][names[source]]][4:]
            background=self.metas[self.mapping['lab_junction'][names[back]]][:4]
            self.supply_depot[name]=len(self.metas)
            self.metas.append(background+foreground)
            self.attrs.append(0x1000)
        for x in range(2):
            for name,source in [(f'ComputerFloorTop_{x}',f'ComputerBench_{x}_0'),(f'SpecimenPlainTop_{x}',f'SpecimenBank_{x}_0')]:
                self.supply_depot[name]=len(self.metas)
                self.metas.append(self.metas[self.mapping['lab_junction'][names['Floor']]][:4]+self.metas[self.mapping['lab_junction'][names[source]]][4:])
                self.attrs.append(0x1000)
        self.supply_depot['PillarCapFloor']=len(self.metas)
        self.metas.append(self.metas[self.mapping['lab_junction'][names['Floor']]][:4]+self.metas[self.mapping['lab_junction'][names['PillarCap']]][4:])
        self.attrs.append(0x1000)
        from supply_depot_merchandise import append_merchandise
        append_merchandise(self)
        from nursery_decorations import append_nursery
        append_nursery(self)
        assert len(self.metas)<=1024 and len(self.tiles)<=1024

    def meta(self,key,mid):
        return self.sources[key].meta[mid//512][mid%512*8:mid%512*8+8]

    @lru_cache(None)
    def colors(self,key,v,base):
        s=self.sources[key];pal=s.pal(v>>12)
        return frozenset(pal[c] for c in s.tile(v) if c or base)

    @lru_cache(None)
    def import_tile(self,key,v,base,exact):
        s=self.sources[key];raw=s.tile(v);pal=s.pal(v>>12)
        colors=self.colors(key,v,base)
        # A transparent foreground tile does not need an opaque color entry.
        if not any(raw):return 0
        costs=[]
        for pi,p in enumerate(self.pals[:11]):
            cost=sum(min(sum((c[k]-d[k])**2 for k in range(3)) for d in p[1:]) for c in colors)
            costs.append((cost,pi))
        cost,pi=min(costs)
        assert not exact or cost==0, ('Lossy existing art',key,v,colors)
        if cost:self.recolored.add(key)
        p=self.pals[pi]
        convert={c:min(range(1,16),key=lambda k:sum((pal[c][channel]-p[k][channel])**2 for channel in range(3))) for c in set(raw)}
        new=bytes(0 if not c else convert[c] for c in raw)
        # Never deduplicate against animation slots, including its initial phase.
        index=next((i for i,t in enumerate(self.tiles) if not 512<=i<548 and t==new),None)
        if index is None:
            if len(self.tiles)==512:self.tiles.extend([bytes(64)]*36)
            index=len(self.tiles);self.tiles.append(new)
        return index|(v&0xC00)|(pi<<12)

    def render(self,mid,vals=None,original_tiles=None):
        im=Image.new('RGB',(16,16))
        for j,v in enumerate(self.metas[mid] if vals is None else vals):
            raw=(original_tiles or {}).get(v&1023,self.tiles[v&1023]);pal=self.pals[v>>12]
            for y in range(8):
                for x in range(8):
                    c=raw[(7-y if v&2048 else y)*8+(7-x if v&1024 else x)]
                    if j<4 or c:im.putpixel((j%2*8+x,j%4//2*8+y),pal[c])
        return im

    def outputs(self):
        outputs={PRI+'/tiles.png':png(self.tiles[:512]),SEC+'/tiles.png':png(self.tiles[512:],512)}
        from generate_terrarium_tiles import animation_header
        outputs['src/data/terrarium_anims.h']=animation_header(self)
        for folder,metas,attrs in [(PRI,self.metas[:512],self.attrs[:512]),(SEC,self.metas[512:],self.attrs[512:])]:
            if folder==PRI:
                metas=metas+[[0]*8]*(512-len(metas));attrs=attrs+[0]*(512-len(attrs))
            outputs[folder+'/metatiles.bin']=packed(sum(metas or [[0]*8],[]))
            outputs[folder+'/metatile_attributes.bin']=packed(attrs or [0])
        for i,p in enumerate(self.pals):
            outputs[(PRI if i<6 else SEC)+f'/palettes/{i:02}.pal']=('JASC-PAL\r\n0100\r\n16\r\n'+'\r\n'.join(' '.join(map(str,c)) for c in p)+'\r\n').encode()
        for key,d in self.config['rooms'].items():
            text=f'#ifndef GUARD_{key.upper()}_TILES_H\n#define GUARD_{key.upper()}_TILES_H\n\n// Generated shared hub aliases; do not edit.\n'
            for name,old in d['names'].items():
                text+=f'#define METATILE_{d["label"]}_{name} 0x{self.mapping[key][old]:03X}\n'
            if key in ('lab_junction','main_hall'):text+=f'#define {key.upper()}_DOOR_PALETTE 11\n'
            outputs[f'include/constants/{key}_tiles.h']=(text+'\n#endif\n').encode()
        header='#ifndef GUARD_HUB_TILES_H\n#define GUARD_HUB_TILES_H\n\n// Generated Porymap catalogue names.\n'
        named=set()
        for key,d in self.config['rooms'].items():
            for n,old in d['names'].items():
                mid=self.mapping[key][old]
                if mid in named:continue
                named.add(mid);prefix='HubArchitecture' if mid<512 else 'HubFurnishings'
                header+=f'#define METATILE_{prefix}_{d["label"]}_{n} 0x{mid:03X}\n'
        for key,d in self.config['donors'].items():
            for old in d['metatiles']:
                mid=self.mapping[key][old]
                if mid in named:continue
                named.add(mid);prefix='HubArchitecture' if mid<512 else 'HubFurnishings'
                header+=f'#define METATILE_{prefix}_{key}_{old:03X} 0x{mid:03X}\n'
        for name,mid in self.compositions.items():
            header+=f'#define METATILE_HubFurnishings_{name} 0x{mid:03X}\n'
        for name,mid in self.terrariums.items():
            header+=f'#define METATILE_HubFurnishings_Planted{name} 0x{mid:03X}\n'
        for name,mid in self.berry_lab.items():
            header+=f'#define METATILE_HubFurnishings_BerryLab_{name} 0x{mid:03X}\n'
        for name,mid in self.supply_depot.items():
            header+=f'#define METATILE_HubFurnishings_SupplyDepot_{name} 0x{mid:03X}\n'
        for name,mid in self.nursery.items():
            header+=f'#define METATILE_HubFurnishings_Nursery_{name} 0x{mid:03X}\n'
        for entry in self.config.get('labels',[]):
            key,old=entry['source'];mid=self.mapping[key][old]
            prefix='HubArchitecture' if mid<512 else 'HubFurnishings'
            header+=f'#define METATILE_{prefix}_{entry["name"]} 0x{mid:03X}\n'
        # Porymap reads labels directly from this file (not C include aliases).
        labels=ROOT/'include/constants/metatile_labels.h'
        content=labels.read_text()
        begin='// BEGIN GENERATED SHARED HUB LABELS'
        end='// END GENERATED SHARED HUB LABELS'
        if begin in content:
            left,rest=content.split(begin,1);_,right=rest.split(end,1)
            content=left+right.lstrip('\n')
        # Porymap can rewrite the label file and remove our block markers.
        # Retain its existing labels rather than emitting duplicate definitions.
        import re
        existing=dict(re.findall(r'^#define\s+(METATILE_\w+)\s+(0x[0-9A-Fa-f]+)',content,re.M))
        generated=[]
        for line in header.split('// Generated Porymap catalogue names.\n',1)[1].splitlines():
            _,name,value=line.split()
            if name in existing:
                assert int(existing[name],16)==int(value,16), ('Conflicting Porymap label',name)
            else:generated.append(line)
        block=begin+'\n'+'\n'.join(generated)+'\n'+end+'\n\n'
        outputs['include/constants/metatile_labels.h']=content.replace('#endif // GUARD_METATILE_LABELS_H',block+'#endif // GUARD_METATILE_LABELS_H').encode()
        outputs['data/tilesets/hub_sources.json']=(json.dumps({'tiles':len(self.tiles),'metatiles':len(self.metas),'palettes':len(self.pals),'mapping':self.mapping,'compositions':self.compositions,'supply_depot_merchandise_tile_range':self.merchandise_tile_range,'nursery_tile_range':self.nursery_tile_range,'recolored_donors':sorted(self.recolored),'portal_tile_range':[512,547],'door_palette':11,'portal_palette':12},indent=2)+'\n').encode()
        return outputs


def convert_layouts(cat):
    path=ROOT/'data/layouts/layouts.json';text=path.read_text();layouts=json.loads(text)['layouts']
    by_label={d['label']:key for key,d in cat.config['rooms'].items()}
    pending={}
    for l in layouts:
        key=by_label.get(l['primary_tileset'].removeprefix('gTileset_'))
        if key is None:continue
        for field in ('blockdata_filepath','border_filepath'):
            p=l[field];old=words(p)
            new=[(v&0xFC00)|cat.mapping[key][v&1023] for v in old]
            assert all((a&0xFC00)==(b&0xFC00) for a,b in zip(old,new))
            assert all(render(cat.sources[key],a&1023).tobytes()==cat.render(b&1023).tobytes() for a,b in set(zip(old,new)))
            pending[p]=packed(new)
    # Validate the entire conversion before touching any authored file.
    for p,data in pending.items():
        backup=OUT/'before-conversion'/p;backup.parent.mkdir(parents=True,exist_ok=True)
        if not backup.exists():backup.write_bytes((ROOT/p).read_bytes())
        (ROOT/p).write_bytes(data)
    for label in by_label:
        for suffix in ('Secondary','Gate',''):
            old='gTileset_'+label+suffix
            new='gTileset_HubFurnishings' if suffix else 'gTileset_HubArchitecture'
            text=text.replace('"'+old+'"','"'+new+'"')
    path.write_text(text)
    print(f'Converted {len(pending)} authored map/border files; pixels, collision and elevation unchanged.')


def previews(cat):
    OUT.mkdir(parents=True,exist_ok=True)
    cache={i:cat.render(i) for i in range(len(cat.metas))}
    sheet=Image.new('RGB',(16*48,((len(cat.metas)+15)//16)*44));draw=ImageDraw.Draw(sheet)
    for mid,im in cache.items():
        x=mid%16*48;y=mid//16*44;sheet.paste(im.resize((32,32),Image.Resampling.NEAREST),(x,y));draw.text((x,y+32),f'{mid:03X}',fill='white')
    sheet.save(OUT/'catalogue.png')
    prefabs_path=ROOT/'data/tilesets/hub_prefabs.json'
    if prefabs_path.exists():
        prefabs=json.loads(prefabs_path.read_text())
        sheet=Image.new('RGB',(720,((len(prefabs)+3)//4)*128),(24,28,36));draw=ImageDraw.Draw(sheet)
        for i,p in enumerate(prefabs):
            x=i%4*180;y=i//4*128
            draw.text((x+4,y+4),p['name'].removeprefix('Hub / ').split(' (')[0],fill='white')
            assert p['width']>0 and p['height']>0
            for v in p['metatiles']:
                assert 0<=v['x']<p['width'] and 0<=v['y']<p['height']
                assert 0<=v['collision']<=3 and 0<=v['elevation']<=15
                sheet.paste(cache[v['metatile_id']],(x+4+v['x']*16,y+24+v['y']*16))
        sheet.save(OUT/'prefabs.png')
    layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
    for l in layouts:
        if l['primary_tileset']!='gTileset_HubArchitecture':continue
        cells=words(l['blockdata_filepath']);assert len(cells)==l['width']*l['height']
        border=words(l['border_filepath'])
        assert all((v&1023)<len(cat.metas) for v in cells+border),l['name']
        im=Image.new('RGB',(l['width']*16,l['height']*16))
        for i,v in enumerate(cells):im.paste(cache[v&1023],(i%l['width']*16,i//l['width']*16))
        im.save(OUT/(l['name']+'.png'))
    # Donor reference rooms keep complete assemblies visible in their original
    # arrangement; all tiles in these sheets are now usable in any hub room.
    for key,d in cat.config['donors'].items():
        for l in d['reference_maps']:
            im=Image.new('RGB',(l['width']*16,l['height']*16))
            for i,v in enumerate(words(l['path'])):im.paste(cache[cat.mapping[key][v&1023]],(i%l['width']*16,i//l['width']*16))
            im.save(OUT/(key+'-assemblies.png'))


def check_safari_enclosures():
    events=json.loads((ROOT/'data/maps/Rogue_Area_SafariZone/map.json').read_text())
    layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
    layout=next(l for l in layouts if l['id']==events['layout'])
    cells=words(layout['blockdata_filepath']);w=layout['width'];h=layout['height']
    for slot,left in ((0,9),(1,24)):
        e=next(e for e in events['object_events'] if e['graphics_id']==f'OBJ_EVENT_GFX_FOLLOW_MON_{slot}')
        assert e['movement_type']=='MOVEMENT_TYPE_WANDER_AROUND', ('Terrarium needs four-direction wandering',slot)
        assert e['movement_range_x'] and e['movement_range_y'], ('Terrarium needs bounded movement',slot)
        allowed=set()
        for y in range(max(0,e['y']-e['movement_range_y']),min(h,e['y']+e['movement_range_y']+1)):
            for x in range(max(0,e['x']-e['movement_range_x']),min(w,e['x']+e['movement_range_x']+1)):
                v=cells[y*w+x]
                if not v&0xC00 and v>>12==e['elevation']:allowed.add((x,y))
        expected={(x,y) for x in range(left,left+4) for y in range(10,13)}
        assert allowed==expected, ('Terrarium movement must cover exactly its 4x3 interior',slot,allowed^expected)
        assert (e['x'],e['y']) in allowed, ('Terrarium spawn outside interior',slot)
    print('Safari terrariums: four-direction movement and both 4x3 containment bounds PASS.')


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--check',action='store_true');parser.add_argument('--convert-layouts',action='store_true')
    args=parser.parse_args();assert not(args.check and args.convert_layouts)
    cat=Catalogue()
    # Exact pixel verification for every original room metatile, including ones
    # unused by the current map but still referenced by runtime upgrade code.
    for key in cat.config['rooms']:
        for old,new in cat.mapping[key].items():
            original=cat.capsule_originals.get(new,cat.metas[new])
            assert render(cat.sources[key],old).tobytes()==cat.render(new,original,cat.capsule_original_tiles).tobytes(),(key,old,new)
            assert cat.attrs[new]==cat.sources[key].attrs[old//512][old%512]
            for a,b in zip(cat.meta(key,old),original):
                before=cat.sources[key].tile(a);after=cat.capsule_original_tiles.get(b&1023,cat.tiles[b&1023])
                assert [c==0 for c in before]==[c==0 for c in after],('Transparency changed',key,old)
    shadow=cat.mapping['lab_junction'][cat.config['rooms']['lab_junction']['names']['FloorShadow']]
    for mid,original in cat.capsule_originals.items():
        assert cat.metas[mid]==original, 'Capsule tile composition changed'
        before=cat.render(mid,original,cat.capsule_original_tiles)
        after=cat.render(mid)
        for y in range(16):
            for x in range(16):
                j=y//8*2+x//8;word=original[j];k=y%8*8+x%8
                raw=cat.capsule_original_tiles.get(word&1023,cat.tiles[word&1023])
                if cat.pals[word>>12][raw[k]]==(106,123,123):
                    assert before.getpixel((x,y))==after.getpixel((x,y)), 'Capsule interior changed'
        assert before.tobytes()!=after.tobytes(), 'Capsule floor was not shaded'
    # Static decorations can never reference the VRAM region animated at runtime.
    for mid,vals in enumerate(cat.metas):
        if not cat.portal_start<=mid<cat.portal_start+9:
            assert not any(512<=v&1023<548 for v in vals),(mid,vals)
    from generate_terrarium_tiles import FLOWER_START
    for mid,vals in enumerate(cat.metas):
        if any(FLOWER_START<=v&1023<FLOWER_START+4 for v in vals):
            assert mid in (cat.terrariums['Habitat_1_1'],cat.terrariums['Habitat_4_2'])
    assert len({b''.join(frame) for frame in cat.terrarium_frames})==3
    for name,mid in cat.terrariums.items():
        old=cat.mapping['safari_lab'][cat.config['rooms']['safari_lab']['names'][name]]
        assert cat.metas[mid][4:]==cat.metas[old][4:] and cat.attrs[mid]==cat.attrs[old]
    for name in ('lab_junction','main_hall'):
        raw=Image.open(ROOT/f'graphics/door_anims/{name}.png').getpalette()
        assert cat.pals[11]==[tuple(raw[i:i+3]) for i in range(0,48,3)],name
    for path,data in cat.outputs().items():
        p=ROOT/path
        if args.check:assert p.exists() and p.read_bytes()==data, f'Stale generated asset: {path}'
        elif not p.exists() or p.read_bytes()!=data:p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
    if args.convert_layouts:convert_layouts(cat)
    from check_hub_corridors import check_corridors
    check_corridors(cat)
    check_safari_enclosures()
    from check_onboarding import main as check_onboarding
    check_onboarding()
    previews(cat)
    print(f'Shared hub: {len(cat.tiles)}/1024 tiles; {len(cat.metas)}/1024 metatiles; 13/13 palettes. Source art verified pixel-for-pixel; capsule shadow replacements verified.')


if __name__=='__main__':main()
