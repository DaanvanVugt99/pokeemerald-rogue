"""Import intact merchandise foregrounds into unused static hub graphics padding."""
from generate_portal_room import Source

FIRST_TILE = 643
LIMIT_TILE = 1008  # Safari animation begins here; never allocate into it.
SOURCES = {
    'DrawerTop': ('market',0x37F), 'DrawerBase': ('market',0x387),
    'Crate': ('market',0x38F), 'RedBasket': ('market',0x38E),
    'GreenBasket': ('market',0x386), 'Bottles': ('market',0x36E),
    'Goods': ('market',0x376), 'BookDisplay0': ('shop',0x228),
    'BookDisplay1': ('shop',0x229), 'MedicineDisplay0': ('shop',0x22B),
    'MedicineDisplay1': ('shop',0x22C),
}


def append_merchandise(cat):
    assert len(cat.metas)==899, 'Merchandise IDs moved; review the authored map'
    sources={'market':Source('general_hub','slateport'),'shop':Source('building','shop')}
    used={v&1023 for meta in cat.metas for v in meta}
    assert not used.intersection(range(FIRST_TILE,LIMIT_TILE)), 'Static padding is now occupied'
    assert all(not any(cat.tiles[i]) for i in range(FIRST_TILE,LIMIT_TILE)), 'Padding contains source art'
    cursor=FIRST_TILE
    reserved=set(range(512,548))|set(range(1008,1012))
    cat.merchandise_foregrounds={}
    for name,(key,mid) in SOURCES.items():
        source=sources[key];foreground=[]
        for word in source.meta[mid//512][mid%512*8+4:mid%512*8+8]:
            raw=source.tile(word);pal=source.pal(word>>12)
            if not any(raw):foreground.append(0);continue
            colors={pal[v] for v in raw if v}
            bank=min(range(11),key=lambda b:sum(min(sum((c[k]-p[k])**2 for k in range(3)) for p in cat.pals[b][1:]) for c in colors))
            # Cabinet greys must stay neutral: unconstrained matching chose red
            # frames to accommodate the source's green labels in a warm palette.
            if name in ('DrawerTop','DrawerBase','Crate'):bank=3
            convert={v:min(range(1,16),key=lambda i:sum((pal[v][k]-cat.pals[bank][i][k])**2 for k in range(3))) for v in set(raw) if v}
            pixels=bytes(convert[v] if v else 0 for v in raw)
            idx=next((i for i,tile in enumerate(cat.tiles) if i not in reserved and tile==pixels),None)
            if idx is None:
                assert cursor<LIMIT_TILE, 'Merchandise exceeded static padding'
                idx=cursor;cursor+=1;cat.tiles[idx]=pixels
            foreground.append(idx|(word&0xC00)|(bank<<12))
            assert [v==0 for v in raw]==[v==0 for v in pixels], 'Merchandise silhouette changed'
        cat.merchandise_foregrounds[name]=foreground
        for suffix,back in [('', 'Floor'),('Shadow','FloorShadow')]:
            bg=cat.mapping['lab_junction'][cat.config['rooms']['lab_junction']['names'][back]]
            cat.supply_depot['Market'+name+suffix]=len(cat.metas)
            cat.metas.append(cat.metas[bg][:4]+foreground)
            cat.attrs.append(0x1000)
    cat.merchandise_tile_range=(FIRST_TILE,cursor)
