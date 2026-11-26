"""Shared artwork and stable allocation for the hub's floor medallions."""

from PIL import Image, ImageDraw

GOLD, BRASS, SHADE = 13, 12, 11


def hall(image):
    d = ImageDraw.Draw(image)
    d.polygon(((11, 20), (24, 12), (37, 20)), fill=BRASS)
    d.line(((11, 20), (24, 12), (37, 20), (11, 20)), fill=GOLD, width=2)
    d.rectangle((13, 22, 34, 34), fill=SHADE)
    for x in (15, 22, 29):
        d.rounded_rectangle((x, 24, x + 4, 33), radius=2, fill=5)
    d.line((12, 35, 35, 35), fill=GOLD, width=2)


def labs(image):
    d = ImageDraw.Draw(image)
    d.polygon(((20, 13), (27, 13), (27, 22), (34, 33), (32, 36), (15, 36), (13, 33), (20, 22)), fill=5)
    d.polygon(((16, 28), (22, 27), (25, 29), (31, 28), (34, 33), (32, 35), (15, 35), (13, 33)), fill=SHADE)
    d.line(((20, 13), (20, 22), (13, 33), (15, 36), (32, 36), (34, 33), (27, 22), (27, 13)), fill=GOLD, width=2, joint='curve')
    d.line((18, 12, 29, 12), fill=BRASS, width=2)
    for x,y in ((30, 17),(33,13),(22,31)):
        d.rectangle((x,y,x+1,y+1), fill=GOLD)


def home(image):
    d = ImageDraw.Draw(image)
    d.rectangle((15, 22, 32, 35), fill=SHADE)
    d.line(((15, 22), (15, 35), (32, 35), (32, 22)), fill=BRASS, width=2)
    d.line(((10, 23), (24, 12), (37, 23)), fill=GOLD, width=3)
    d.rectangle((22, 27, 26, 35), fill=5)
    d.rectangle((17, 25, 19, 27), fill=GOLD)
    d.rectangle((28, 25, 30, 27), fill=GOLD)


def mirrored_symbol(image, half):
    for left, x in ((half, 0), (half.transpose(Image.Transpose.FLIP_LEFT_RIGHT), 24)):
        image.paste(left, (x, 0), left.point([0] + [255] * 255, 'L'))


def safari(image):
    pen = ImageDraw.Draw(image)
    ball = (12, 12, 35, 35)
    pen.pieslice(ball, 180, 360, fill=BRASS)
    pen.ellipse(ball, outline=GOLD, width=2)
    pen.rectangle((13, 23, 34, 24), fill=GOLD)
    pen.ellipse((19, 19, 28, 28), fill=5, outline=GOLD, width=2)
    pen.ellipse((22, 22, 25, 25), fill=GOLD)


def supplies(image):
    d = ImageDraw.Draw(image)
    d.polygon(((11, 17), (24, 11), (36, 17), (24, 23)), fill=BRASS, outline=GOLD)
    d.polygon(((11, 17), (24, 23), (24, 36), (11, 30)), fill=SHADE, outline=GOLD)
    d.polygon(((24, 23), (36, 17), (36, 30), (24, 36)), fill=5, outline=GOLD)
    d.line(((17, 14), (30, 20), (30, 30)), fill=GOLD, width=2)
    d.line((13, 26, 21, 30), fill=BRASS)


def trial(image):
    d = ImageDraw.Draw(image)
    d.line(((17, 17), (11, 17), (12, 23), (18, 27)), fill=BRASS, width=2)
    d.line(((30, 17), (36, 17), (35, 23), (29, 27)), fill=BRASS, width=2)
    d.polygon(((16, 13), (31, 13), (30, 23), (27, 28), (20, 28), (17, 23)), fill=GOLD)
    d.line((24, 28, 24, 34), fill=BRASS, width=3)
    d.rectangle((18, 34, 29, 36), fill=GOLD)
    d.point((20, 17), fill=BRASS)


def nursery(image):
    half = Image.new('P', (24, 48))
    d = ImageDraw.Draw(half)
    outer = ((23, 12), (19, 13), (16, 17), (13, 24), (13, 30), (16, 34), (21, 36), (23, 36))
    d.polygon(outer, fill=SHADE)
    d.line(outer, fill=GOLD, width=2, joint='curve')
    d.polygon(((18, 24), (21, 21), (23, 24), (23, 32), (18, 27)), fill=BRASS)
    mirrored_symbol(image, half)


# These backings match the nine authored floor cells. The Depot's northwest
# corner sits on the edge of the west partition's existing floor shadow.
SHARED = {
    'MainHall': (0x160, hall, (0,) * 9),
    'Labs': (0x172, labs, (0,) * 9),
    'Safari': (0x17B, safari, (0,) * 9),
    'SupplyDepot': (0x184, supplies, (1,) + (0,) * 8),
    'Nursery': (0x18D, nursery, (0,) * 9),
}
SHARED_BANK = 10


def flipped_tiles(raw):
    rows = [raw[i:i + 8] for i in range(0, 64, 8)]
    return (
        (0, raw),
        (0x400, b''.join(row[::-1] for row in rows)),
        (0x800, b''.join(reversed(rows))),
        (0xC00, b''.join(row[::-1] for row in reversed(rows))),
    )


class InlayTiles:
    def __init__(self, tiles, free, bank, reusable=()):
        self.tiles = tiles
        self.free = iter(free)
        self.bank = bank << 12
        self.words = {}
        for idx in reusable:
            self.remember(idx, tiles[idx])

    def remember(self, idx, raw):
        for flags, variant in flipped_tiles(raw):
            self.words.setdefault(variant, idx | flags)

    def word(self, raw):
        if not any(raw):
            return 0
        if raw not in self.words:
            idx = next(self.free, None)
            assert idx is not None, 'Floor medallions exceeded the graphics budget'
            self.tiles[idx] = raw
            self.remember(idx, raw)
        return self.words[raw] | self.bank

    def pieces(self, symbol, center_on_seam=False):
        from market_tiles import floor_medallion

        image = floor_medallion(symbol)
        if center_on_seam:
            # Eight pixels of padding on each side put the unchanged 48-pixel
            # circle on the seam between the two middle map tiles.
            padded = Image.new('P', (64, 48))
            padded.paste(image, (8, 0))
            image = padded
        for y in range(0, image.height, 16):
            for x in range(0, image.width, 16):
                yield [
                    self.word(image.crop((px, py, px + 8, py + 8)).tobytes())
                    for py in (y, y + 8) for px in (x, x + 8)
                ]


def reclaim_static_graphics(cat):
    """Reclaim exact pixel duplicates, keeping all metatile IDs stable."""
    protected = set(range(512, 548)) | set(range(1008, 1012))
    protected.update(word & 1023 for words in cat.capsule_originals.values() for word in words)
    used = {word & 1023 for words in cat.metas for word in words}
    known = {}
    replacements = {}
    for idx in sorted(used - protected):
        raw = cat.tiles[idx]
        if raw in known:
            replacements[idx] = known[raw]
            continue
        for flags, variant in flipped_tiles(raw):
            known.setdefault(variant, idx | flags)

    for words in cat.metas:
        for i, word in enumerate(words):
            replacement = replacements.get(word & 1023)
            if replacement is not None:
                old_view = dict(flipped_tiles(cat.tiles[word & 1023]))[word & 0xC00]
                new = (word & 0xF000) | (replacement ^ (word & 0xC00))
                new_view = dict(flipped_tiles(cat.tiles[new & 1023]))[new & 0xC00]
                assert old_view == new_view
                words[i] = new
    used = {word & 1023 for words in cat.metas for word in words}
    return sorted(set(range(len(cat.tiles))) - used - protected), used - protected


def append_hub_medallions(cat):
    cat.tiles.extend([bytes(64)] * (1024 - len(cat.tiles)))
    free, reusable = reclaim_static_graphics(cat)
    pool = InlayTiles(cat.tiles, free, SHARED_BANK, sorted(reusable))
    cat.medallions = {}
    for name, (first, symbol, backing) in SHARED.items():
        for index, (floor, art) in enumerate(zip(backing, pool.pieces(symbol))):
            mid = first + index
            assert cat.metas[mid] == [0] * 8 and cat.attrs[mid] == 0, mid
            assert cat.attrs[floor] & 0xFF == 0 and cat.metas[floor][4:] == [0] * 4
            cat.metas[mid] = cat.metas[floor][:4] + art
            cat.attrs[mid] = 0x1000
            cat.medallions[f'{name}_{index % 3}_{index // 3}'] = mid
