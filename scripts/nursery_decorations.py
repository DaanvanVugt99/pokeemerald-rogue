"""Stable indoor foreground imports from the original GeneralHub + Mauville Day Care."""
from generate_portal_room import Source

SOURCES={'Flowers':0x004,'Shrub':0x00D,'Fence':0x291,'FenceSide':0x28A,'FlowerBox':0x2B4,'FlowerBoxBase':0x2BC}

def append_nursery(cat):
    assert len(cat.metas)==921
    src=Source('general_hub','mauville');cursor=679;limit=1008
    used={w&1023 for m in cat.metas for w in m}
    assert not used.intersection(range(cursor,limit))
    assert all(not any(cat.tiles[i]) for i in range(cursor,limit))
    cat.nursery={};cat.nursery_foregrounds={}
    for name,mid in SOURCES.items():
        fg=[]
        for word in src.meta[mid//512][mid%512*8+4:mid%512*8+8]:
            raw=src.tile(word);pal=src.pal(word>>12)
            if not any(raw):fg.append(0);continue
            colors={pal[v] for v in raw if v}
            bank=min(range(11),key=lambda b:sum(min(sum((c[k]-p[k])**2 for k in range(3)) for p in cat.pals[b][1:]) for c in colors))
            convert={v:min(range(1,16),key=lambda i:sum((pal[v][k]-cat.pals[bank][i][k])**2 for k in range(3))) for v in set(raw) if v}
            pixels=bytes(convert[v] if v else 0 for v in raw)
            idx=next((i for i,t in enumerate(cat.tiles) if i not in range(512,548) and i not in range(1008,1012) and t==pixels),None)
            if idx is None:
                assert cursor<limit
                idx=cursor;cursor+=1;cat.tiles[idx]=pixels
            assert [v==0 for v in pixels]==[v==0 for v in raw]
            fg.append(idx|(word&0xC00)|(bank<<12))
        cat.nursery_foregrounds[name]=fg
        for suffix,back in [('', 'Floor'),('Shadow','FloorShadow')]:
            bg=cat.mapping['lab_junction'][cat.config['rooms']['lab_junction']['names'][back]]
            cat.nursery['DayCare'+name+suffix]=len(cat.metas)
            cat.metas.append(cat.metas[bg][:4]+fg);cat.attrs.append(0x1000)
    cat.nursery_tile_range=(679,cursor)

    # Shadow compositions retain existing foregrounds and allocate no graphics.
    assert len(cat.metas)==933, "Nursery shadow IDs moved"
    shadow=cat.mapping['lab_junction'][cat.config['rooms']['lab_junction']['names']['FloorShadow']]
    for name,source in [('SofaLeftShadowTop',105),('SofaLeftShadowBase',108)]+[(f'GlassShadow{i}',cat.berry_lab[f'GlassBack{i}']) for i in (0,1,5)]:
        cat.nursery[name]=len(cat.metas)
        cat.metas.append(cat.metas[shadow][:4]+cat.metas[source][4:])
        cat.attrs.append(cat.attrs[source])

    # Rocket lounge chair: native sideways seating, composed over hub flooring.
    assert len(cat.metas)==938
    cursor=692
    def store(pixels):
        nonlocal cursor
        idx=next((i for i,t in enumerate(cat.tiles) if i not in range(512,548) and i not in range(1008,1012) and t==pixels),None)
        if idx is None:
            assert cursor<1008 and not any(cat.tiles[cursor])
            idx=cursor;cursor+=1;cat.tiles[idx]=pixels
        return idx
    floor=cat.mapping['lab_junction'][cat.config['rooms']['lab_junction']['names']['Floor']]
    rocket=cat.sources['Rocket']
    for name,mid in [('CoffeeSideTop',0x3F5),('CoffeeSideSeat',0x3FD)]:
        fg=[]
        for word in rocket.meta[1][(mid-512)*8+4:(mid-512)*8+8]:
            raw=rocket.tile(word);pal=rocket.pal(word>>12)
            colors={pal[v] for v in raw if v}
            bank=min(range(11),key=lambda b:sum(min(sum((c[k]-p[k])**2 for k in range(3)) for p in cat.pals[b][1:]) for c in colors))
            convert={v:min(range(1,16),key=lambda i:sum((pal[v][k]-cat.pals[bank][i][k])**2 for k in range(3))) for v in set(raw) if v}
            fg.append(store(bytes(convert[v] if v else 0 for v in raw))|(word&0xC00)|(bank<<12))
        cat.nursery[name]=len(cat.metas);cat.metas.append(cat.metas[floor][:4]+fg);cat.attrs.append(0x1000)
    # Inactive seats keep complete furniture silhouettes without looking usable.
    for name,mid in [('CoffeeMiddleTopDim',106),('CoffeeRightTopDim',107),('CoffeeMiddleSeatDim',109),('CoffeeRightSeatDim',110),('CoffeeSideTopDim',cat.nursery['CoffeeSideTop']),('CoffeeSideSeatDim',cat.nursery['CoffeeSideSeat'])]:
        vals=cat.metas[mid][:4]
        for word in cat.metas[mid][4:]:
            bank=word>>12;pal=cat.pals[bank];raw=cat.tiles[word&1023]
            convert={v:min(range(1,16),key=lambda i:sum((pal[v][k]*0.55-pal[i][k])**2 for k in range(3))) for v in set(raw) if v}
            vals.append(store(bytes(convert[v] if v else 0 for v in raw))|(word&0xFC00))
        cat.nursery[name]=len(cat.metas);cat.metas.append(vals);cat.attrs.append(0x1000)
    cat.nursery_tile_range=(679,cursor)

    # Extend the sideways chair into a couch with a repeatable padded middle.
    for name,source in [('CoffeeSideMiddle','CoffeeSideSeat'),('CoffeeSideMiddleDim','CoffeeSideSeatDim')]:
        vals=cat.metas[cat.nursery[source]]
        cat.nursery[name]=len(cat.metas)
        cat.metas.append(vals[:4]+vals[4:6]+vals[4:6]);cat.attrs.append(0x1000)
