#!/usr/bin/env python3
"""Verify exterior hallway seams; --update changes private facades only."""
import argparse
import json
from PIL import Image
from generate_portal_room import ROOT, words, packed


def check_corridors(cat, update=False):
    layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
    layouts={l['id']:l for l in layouts if l['primary_tileset']=='gTileset_HubArchitecture'}
    maps={}
    for l in layouts.values():
        path=ROOT/'data/maps'/l['name'].removesuffix('_Layout')/'map.json'
        if path.exists():
            m=json.loads(path.read_text());maps[m['id']]=m
    cache={i:cat.render(i) for i in range(len(cat.metas))}
    def signature(v):
        return (v&0xFC00,cache[v&1023].tobytes())
    def tile(name):
        return cat.mapping['portal_room'][cat.config['rooms']['portal_room']['names'][name]]
    profiles={};rooms=[];count=0
    for m in maps.values():
        if not m.get('connections'):continue
        l=layouts[m['layout']];w,h=l['width'],l['height'];cells=words(l['blockdata_filepath'])
        assert all(cache[v&1023].getbbox() is None and v&0xC00 for v in words(l['border_filepath'])), ('Nonblack exterior border',m['name'])
        rooms.append(m)
        for cn in m['connections']:
            target=layouts[maps[cn['map']]['layout']]
            assert '_CORRIDOR_' in target['id'] and not maps[cn['map']].get('connections'), ('Only private facade layouts may be updated',target['id'])
            horizontal=cn['direction'] in ('left','right')
            span=target['height'] if horizontal else target['width']
            edge=[]
            for i in range(span):
                x,y=((0 if cn['direction']=='left' else w-1),cn['offset']+i) if horizontal else (cn['offset']+i,0 if cn['direction']=='up' else h-1)
                assert 0<=x<w and 0<=y<h, ('Connection offset outside room',m['name'],cn)
                edge.append(cells[y*w+x])
            lanes=[i for i,v in enumerate(edge) if not v&0xC00]
            assert len(lanes)==3 and lanes==list(range(lanes[0],lanes[0]+3)), ('Hallway must be three wide',m['name'],cn)
            profile=[v if v&0xC00 else ((v&0xF000)|tile('FloorShadowDim20' if horizontal and i==lanes[0] else 'FloorDim20')) for i,v in enumerate(edge)]
            key=target['id']
            if key in profiles:
                assert [signature(v) for v in profiles[key][0]]==[signature(v) for v in profile], ('Shared facade needs separate variants',key,m['name'])
            else:profiles[key]=(profile,horizontal)
            count+=1
    # Compute and validate every profile before making the explicit update.
    updated=[]
    for key,(profile,horizontal) in profiles.items():
        l=layouts[key];w,h=l['width'],l['height']
        expected=[profile[y if horizontal else x] for y in range(h) for x in range(w)]
        current=words(l['blockdata_filepath'])
        assert all(cache[v&1023].getbbox() is None and v&0xC00 for v in words(l['border_filepath'])), ('Nonblack facade border',key)
        if [signature(v) for v in current]!=[signature(v) for v in expected]:
            if not update:raise AssertionError(f'Facade does not match the authored room edge: {key}; run scripts/check_hub_corridors.py --update')
            updated.append((l['blockdata_filepath'],expected))
    for path,values in updated:(ROOT/path).write_bytes(packed(values))
    # Include fieldmap.c's seven-tile strips (eight on the east), so visual QA
    # covers the space outside the authored rectangle as well as the room.
    out=ROOT/'build/hub_tiles/edges';out.mkdir(parents=True,exist_ok=True)
    for m in rooms:
        l=layouts[m['layout']];w,h=l['width'],l['height'];cells=words(l['blockdata_filepath'])
        im=Image.new('RGB',((w+15)*16,(h+14)*16))
        for i,v in enumerate(cells):im.paste(cache[v&1023],((i%w+7)*16,(i//w+7)*16))
        for cn in m['connections']:
            target=layouts[maps[cn['map']]['layout']];tw,th=target['width'],target['height'];a=words(target['blockdata_filepath']);d=cn['direction'];off=cn['offset']
            if d in ('left','right'):
                coords=((x,y,x+7-tw if d=='left' else w+7+x,y+7+off) for y in range(th) for x in (range(tw-7,tw) if d=='left' else range(8)))
            else:
                coords=((x,y,x+7+off,y+7-th if d=='up' else h+7+y) for y in (range(th-7,th) if d=='up' else range(7)) for x in range(tw))
            for x,y,dx,dy in coords:im.paste(cache[a[y*tw+x]&1023],(dx*16,dy*16))
        im.save(out/(m['name']+'.png'))
    print(f'Hub corridor seams: {count} connections checked; {len(updated)} facades updated.')
    from check_hub_hallway_states import check_states
    check_states()


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--update',action='store_true');args=parser.parse_args()
    from generate_hub_tiles import Catalogue
    check_corridors(Catalogue(),args.update)


if __name__=='__main__':main()
