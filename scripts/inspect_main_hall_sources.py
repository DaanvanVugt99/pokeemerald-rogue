"""Read-only donor contact sheets for Main Hall composition."""
import json
from PIL import Image, ImageDraw
from generate_portal_room import ROOT, Builder, Source, words

out=ROOT/'build/main_hall/sources';out.mkdir(parents=True,exist_ok=True)
b=Builder()
b.sources.update(silph=Source('general','facility_rocket','facility'), school=Source('building','pokemon_school'), connect=Source('building','pokemon_center'))
layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
for key,lid in [('galactic','ROGUE_ENCOUNTER_GALACTIC_HQ'),('silph','ROGUE_ENCOUNTER_SILPH_CO'),('school','ROGUE_INTERIOR_SCHOOL'),('connect','ROGUE_INTERIOR_POKE_CONNECT')]:
    l=next(v for v in layouts if v['id']=='LAYOUT_'+lid);w,h=l['width'],l['height'];im=Image.new('RGB',(w*16,h*16))
    for i,v in enumerate(words(l['blockdata_filepath'])):
        name=f'{key}_{v&1023}'
        if name not in b.names:b.add(name,key,v&1023)
        # Builder.render assumes private capacity; donors are rendered individually.
        vals=b.metas[b.names[name]];tile=Image.new('RGB',(16,16))
        for j,t in enumerate(vals):
            for y in range(8):
                for x in range(8):
                    c=b.tiles[t&1023][(7-y if t&2048 else y)*8+(7-x if t&1024 else x)]
                    if j<4 or c:tile.putpixel((j%2*8+x,j%4//2*8+y),b.pals[t>>12][c])
        im.paste(tile,(i%w*16,i//w*16))
    im.save(out/(key+'.png'))
b=Builder();im=Image.new('RGB',(8*48,5*64));d=ImageDraw.Draw(im)
for i,mid in enumerate(range(0x260,0x288)):
    b.add(str(mid),'rock',mid);im.paste(b.render(b.names[str(mid)]).resize((48,48),Image.Resampling.NEAREST),(i%8*48,i//8*64));d.text((i%8*48,i//8*64+48),hex(mid),fill='white')
im.save(out/'stand.png')
