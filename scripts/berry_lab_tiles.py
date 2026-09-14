"""Append Berry Lab soil without moving any shared tile, palette or metatile IDs."""
from generate_portal_room import Source


def append_berry_lab(cat):
    from generate_hub_tiles import render
    assert len(cat.metas) == 866, 'Berry Lab allocation moved; review IDs before generation'
    # The original field soil, mapped into an existing warm neutral palette.
    soil = render(Source('general_hub', 'petalburg'), 0x10C)
    colors = [soil.getpixel((x,y)) for y in range(16) for x in range(16)]
    bank = min(range(11), key=lambda b: sum(min(sum((c[k]-p[k])**2 for k in range(3))
        for p in cat.pals[b][1:]) for c in colors))
    vals = []
    for ty in (0, 8):
        for tx in (0, 8):
            raw = bytes(min(range(1, 16), key=lambda i: sum((soil.getpixel((x,y))[k]-cat.pals[bank][i][k])**2
                for k in range(3))) for y in range(ty,ty+8) for x in range(tx,tx+8))
            # Never reuse animated graphics, even when an initial frame matches.
            idx = next((i for i,t in enumerate(cat.tiles) if t == raw and not 512 <= i < 548 and not 1008 <= i < 1012), None)
            if idx is None:
                idx = len(cat.tiles)
                cat.tiles.append(raw)
            vals.append(idx | bank << 12)
    cat.berry_lab = {'Soil': len(cat.metas)}
    cat.metas.append(vals + [0]*4)
    cat.attrs.append(0xA0)  # MB_BERRY_TREE_SOIL; ordinary two-layer drawing.
    # Back panes must draw below the tall berry sprites, unlike Safari pen fronts.
    for x in range(6):
        old = cat.mapping['safari_lab'][cat.config['rooms']['safari_lab']['names'][f'Habitat_{x}_0']]
        cat.berry_lab[f'GlassBack{x}'] = len(cat.metas)
        cat.metas.append(cat.metas[old][:])
        cat.attrs.append((cat.attrs[old] & 0x0FFF) | 0x1000)
    def old(key,name):
        return cat.mapping[key][cat.config['rooms'][key]['names'][name]]
    def append(name,background,foreground,behavior=0):
        cat.berry_lab[name]=len(cat.metas)
        cat.metas.append(cat.metas[background][:4]+foreground)
        cat.attrs.append(0x1000|behavior) # Fixtures remain behind people/plants.
    floor=old('lab_junction','Floor')
    # The donor plant top contains wall-base artwork in its background. Open-floor
    # plants need a floor backing; never paste the wall-backed assembly mid-room.
    append('PlantTop',floor,cat.metas[111][4:])
    append('PlantBase',floor,cat.metas[112][4:])
    for suffix,x in [('Left',0),('Right',5)]:
        append('BedEnd'+suffix,floor,cat.metas[old('safari_lab',f'Habitat_{x}_4')][4:])
    # A shallow rim leaves soil visible and does not cover the berry's feet.
    rim_tile=len(cat.tiles)
    cat.tiles.append(bytes([0]*40+[7]*8+[3]*8+[1]*8))
    rim=[0,0,rim_tile|0x1000,rim_tile|0x1000]
    append('SoilRim',cat.berry_lab['Soil'],rim,0xA0)
    append('InactiveBed',floor,rim)

    # Native 16px greenhouse fittings. Repeated halves share graphics; existing
    # green/cream palette 1 and all animation slots remain unchanged.
    def fitting(name,background,pixels):
        vals=[]
        for ty in (0,8):
            for tx in (0,8):
                raw=bytes(pixels[y][x] for y in range(ty,ty+8) for x in range(tx,tx+8))
                idx=next((i for i,t in enumerate(cat.tiles) if t==raw and not 512<=i<548 and not 1008<=i<1012),None)
                if idx is None:idx=len(cat.tiles);cat.tiles.append(raw)
                vals.append(idx|0x1000)
        append(name,background,vals)
    light=[[0]*16 for _ in range(16)]
    for x in range(16):
        for y,c in [(5,1),(6,7),(7,14),(8,15),(9,14),(10,2),(11,1)]:light[y][x]=c
        if x%8==0:
            for y in range(6,11):light[y][x]=5
    fitting('GrowLight',old('lab_junction','WallBase'),light)
    tray=[[0]*16 for _ in range(16)]
    for y in range(5,15):
        for x in range(16):tray[y][x]=7 if y in (5,14) else 1 if y in (6,13) else 9
    for x in (2,6,10,14):
        for dx,dy,c in [(0,0,2),(-1,-1,4),(1,-1,4),(0,-2,2)]:tray[9+dy][x+dx]=c
    fitting('SeedTray',floor,tray)
    cabinet=[[0]*16 for _ in range(16)]
    for y in range(16):
        for x in range(16):
            if y<11:cabinet[y][x]=7 if y==0 else 1 if x%8==0 or y==10 else 6
            elif x%8 in (0,1):cabinet[y][x]=3
            if y in (4,6) and x%8 in (2,3,4,5):cabinet[y][x]=2
    fitting('PropagationBase',floor,cabinet)
    # Reuse the hallway shadow backing so wall plants do not cut bright holes in it.
    shadow=old('lab_junction','FloorShadow')
    append('PlantBaseShadow',shadow,cat.metas[112][4:])
    append('PlantTopShadow',shadow,cat.metas[111][4:])
    assert len(cat.tiles) <= 1024, ('Berry Lab graphics budget exceeded',len(cat.tiles))
