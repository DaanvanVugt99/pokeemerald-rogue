"""Market art in unused shared-hub metatile and static graphics slots."""
from PIL import Image, ImageDraw
from generate_portal_room import Source
from berry_lab_tiles import draw_berry_symbol


def floor_medallion(symbol):
    """Draw the shared brass frame and its room-specific symbol."""
    image = Image.new('P', (48, 48))
    pen = ImageDraw.Draw(image)
    pen.ellipse((2, 2, 45, 45), fill=4, outline=12, width=2)
    pen.ellipse((6, 6, 41, 41), fill=5, outline=7)
    symbol(image)
    return image


def draw_market_bowl(image):
    pen = ImageDraw.Draw(image)
    pen.arc((12, 16, 35, 35), 0, 180, fill=13, width=3)
    pen.line((12, 26, 35, 26), fill=12, width=2)
    for x in (18, 24, 30):
        pen.line((x, 23, x - 2, 20, x, 17, x - 1, 14), fill=11, width=1)


def append_market(cat):
    first_tile, limit_tile = 878, 1008
    used = {word & 1023 for meta in cat.metas for word in meta}
    assert not used.intersection(range(first_tile, limit_tile))
    assert not any(any(tile) for tile in cat.tiles[first_tile:limit_tile])

    # Leave the shared stone colors in this bank intact.
    bank = 10
    colors = [(7, 8, 9), (9, 12, 13), (11, 15, 16), (14, 18, 18),
              (10, 7, 6), (14, 9, 7), (17, 11, 8), (20, 14, 10),
              (23, 18, 12), (28, 23, 15), (21, 10, 8), (30, 19, 8)]
    assert not any(any(rgb) for rgb in cat.pals[bank][4:])
    for meta in cat.metas:
        for word in meta:
            if word >> 12 == bank:
                assert max(cat.tiles[word & 1023]) < 4
    cat.pals[bank][4:] = [tuple(value * 255 // 31 for value in rgb) for rgb in colors]
    # A reserved portion of bank 9 carries the Restaurant's steel and oak shades.
    restaurant_bank = 9
    assert not any(any(rgb) for rgb in cat.pals[restaurant_bank][6:])
    for meta in cat.metas:
        for word in meta:
            if word >> 12 == restaurant_bank:
                assert max(cat.tiles[word & 1023]) < 6
    cat.pals[restaurant_bank][6:] = [
        (82, 82, 106), (131, 131, 139), (230, 222, 246), (189, 230, 255),
        (82, 148, 197), (139, 98, 41), (180, 148, 90), (205, 172, 115),
        (246, 213, 172), (205, 106, 90),
    ]
    graphics = {}
    cat.market = {}

    def graphic(raw):
        # Reuse flipped copies of 8×8 graphics in the reserved Market slots.
        rows = [raw[i:i + 8] for i in range(0, 64, 8)]
        for flags, tile in ((0, raw), (0x400, b''.join(row[::-1] for row in rows)),
                            (0x800, b''.join(reversed(rows))),
                            (0xC00, b''.join(row[::-1] for row in reversed(rows)))):
            if tile in graphics:
                return graphics[tile] | flags
        index = graphics.setdefault(raw, first_tile + len(graphics))
        assert index < limit_tile, 'Market art exceeds the reserved graphics padding'
        cat.tiles[index] = raw
        return index

    def metatile(name, tiles, attributes=0x1000):
        mid = 256 + len(cat.market)
        assert cat.metas[mid] == [0] * 8 and not cat.attrs[mid]
        cat.metas[mid] = tiles
        cat.attrs[mid] = attributes
        cat.market[name] = mid

    restaurant = Source('building', 'restaurant')

    def fixture(name, source_id, backing, layer=4, mirror=False):
        start = (source_id - 512) * 8
        words = restaurant.meta[1][start + layer:start + layer + 4]
        if mirror:
            words = [words[i] ^ 0x400 for i in (1, 0, 3, 2)]
        tiles = []
        for word in words:
            raw = restaurant.tile(word)
            if not any(raw):
                tiles.append(0)
                continue
            source_colors = restaurant.pal(word >> 12)
            dest = cat.pals[restaurant_bank]
            remap = {color: min(range(1, 16), key=lambda i: sum(
                (a - b) ** 2 for a, b in zip(dest[i], source_colors[color])))
                for color in set(raw) if color}
            pixels = bytes(remap[pixel] if pixel else 0 for pixel in raw)
            tiles.append((graphic(pixels) ^ (word & 0xC00)) | (restaurant_bank << 12))
        background = cat.metas[cat.market[backing]][:4]
        metatile(name, background + tiles)


    def sheet(width, height, color):
        im = Image.new('P', (width * 16, height * 16), color)
        return im, ImageDraw.Draw(im)

    def match_kitchen_floor(im):
        for y in range(im.height):
            for x in range(im.width):
                if im.getpixel((x, y)) == 6:
                    im.putpixel((x, y), kitchen_floor.getpixel((x % 16, y % 16)))

    def add(name, im):
        width, height = im.size
        for y in range(0, height, 16):
            for x in range(0, width, 16):
                words = []
                for dy, dx in ((0, 0), (0, 8), (8, 0), (8, 8)):
                    raw = im.crop((x + dx, y + dy, x + dx + 8, y + dy + 8)).tobytes()
                    words.append(graphic(raw) | (bank << 12))
                key = name if (width, height) == (16, 16) else f'{name}_{x//16}_{y//16}'
                metatile(key, words + [0] * 4)

    im, d = sheet(1, 1, 6)
    d.line((0, 15, 15, 15), fill=5)
    d.line((15, 0, 15, 14), fill=5)
    d.point((4, 3), fill=7)
    d.point((11, 10), fill=7)
    add('Slate', im)

    im, d = sheet(1, 1, 10)
    for y in (0, 8):
        d.line((0, y, 15, y), fill=8)
        x = 4 if y == 0 else 12
        d.line((x, y + 1, x, y + 7), fill=9)
        d.line((x - 3, y + 3, x + 2, y + 3), fill=11)
    add('Oak', im)

    im, d = sheet(1, 1, 8)
    d.rectangle((0, 0, 7, 7), fill=9)
    d.rectangle((8, 8, 15, 15), fill=9)
    d.point((3, 3), fill=10)
    d.point((11, 11), fill=10)
    add('KitchenTile', im)
    kitchen_floor = im.copy()

    def inlay(floor, symbol):
        base = floor_medallion(symbol)
        for y in range(3):
            for x in range(3):
                words = []
                for dy, dx in ((0, 0), (0, 8), (8, 0), (8, 8)):
                    px, py = x * 16 + dx, y * 16 + dy
                    raw = base.crop((px, py, px + 8, py + 8)).tobytes()
                    words.append((graphic(raw) | (bank << 12)) if any(raw) else 0)
                yield f'Inlay_{x}_{y}', cat.metas[floor][:4] + words

    for name, words in inlay(cat.market['Slate'], draw_market_bowl):
        metatile(name, words)

    im, d = sheet(2, 1, 6)
    d.rectangle((0, 3, 31, 10), fill=12)
    d.rectangle((0, 3, 31, 4), fill=13)
    d.rectangle((0, 10, 31, 14), fill=8)
    d.line((0, 11, 31, 11), fill=9)
    for x in (3, 19):
        d.rectangle((x, 12, x + 9, 13), fill=9)
    add('Counter', im)
    fixture('KitchenCounter_0_0', 0x21B, 'KitchenTile')
    fixture('KitchenCounter_1_0', 0x21B, 'KitchenTile', mirror=True)

    def brickwork(width, height, top_trim=False):
        im, d = sheet(width, height, 8)
        for y in range(0, im.height, 4):
            offset = (y // 4 % 2) * 4
            for x in range(offset - 8, im.width, 8):
                d.rectangle((x + 1, y + 1, x + 7, y + 3), fill=14)
                d.line((x + 2, y + 1, x + 6, y + 1), fill=10)
        if top_trim:
            d.line((0, 0, im.width - 1, 0), fill=7)
            d.line((0, 1, im.width - 1, 1), fill=4)
        return im, d

    im, d = sheet(1, 1, 6)
    d.rectangle((2, 4, 13, 15), fill=8)
    d.rectangle((1, 4, 14, 11), fill=12)
    d.line((2, 4, 13, 4), fill=13)
    d.line((5, 7, 10, 7), fill=15)
    add('MenuBoard', im)
    match_kitchen_floor(im)
    add('KitchenMenuBoard', im)

    im, d = sheet(1, 1, 6)
    d.line((0, 7, 15, 7), fill=12)
    d.line((0, 8, 15, 8), fill=4)
    add('BrassThreshold', im)

    im, d = sheet(2, 1, 6)
    d.rounded_rectangle((0, 2, 31, 14), radius=2, fill=8, outline=12)
    d.rectangle((2, 4, 29, 10), fill=5)
    for x, color in ((4, 14), (11, 7), (18, 15), (25, 11)):
        d.rectangle((x, 6, x + 3, 8), fill=color)
        d.point((x + 1, 6), fill=13)
    d.line((2, 11, 29, 11), fill=7)
    add('DisplayCase', im)

    im, d = sheet(1, 1, 5)
    d.line((0, 0, 15, 0), fill=7)
    d.line((0, 15, 15, 15), fill=4)
    d.point((4, 4), fill=6)
    d.point((12, 10), fill=6)
    add('ServiceMat', im)
    service_mat = im.copy()

    # The original Restaurant's round table and directional chairs.
    for name, donor in [('TableNW', 0x204), ('TableNE', 0x205), ('TableSW', 0x20C),
                        ('TableSE', 0x20D), ('ChairWest', 0x20F), ('ChairEast', 0x20E)]:
        fixture(name, donor, 'Oak')

    # Keep the wall modules after the established Market metatile IDs.
    im, d = brickwork(1, 2, top_trim=True)
    d.line((0, 31, 15, 31), fill=4)
    add('KitchenWall', im)

    im, d = sheet(1, 1, 6)
    d.rounded_rectangle((1, 2, 14, 13), radius=2, fill=4, outline=7)
    for y in (4, 7, 10):
        d.line((3, y, 12, y), fill=5)
        d.line((3, y + 1, 12, y + 1), fill=7)
    for x in (2, 13):
        d.point((x, 3), fill=11)
        d.point((x, 12), fill=11)
    match_kitchen_floor(im)
    add('DrainGrate', im)

    # Keep the complete source assemblies in their native order. The lower
    # sink pieces are authored in the original metatile's background layer.
    for name, donor, layer in [
        ('SinkUpperLeft', 0x228, 4), ('SinkUpperRight', 0x229, 4),
        ('SinkLowerLeft', 0x230, 0), ('SinkLowerRight', 0x231, 0),
        ('CabinetUpperLeft', 0x22A, 4), ('CabinetUpperRight', 0x22B, 4),
        ('CabinetLowerLeft', 0x232, 4), ('CabinetLowerRight', 0x233, 4),
    ]:
        fixture(name, donor, 'KitchenWall_0_0' if 'Upper' in name else 'KitchenWall_0_1', layer)
    fixture('Range', 0x234, 'KitchenTile')
    fixture('PrepCounterMiddle', 0x21C, 'KitchenTile')
    fixture('TableCup', 0x206, 'Oak')
    fixture('TablePlate', 0x207, 'Oak')
    fixture('RangeHood', 0x22C, 'KitchenWall_0_1')
    fixture('RangeBase', 0x23C, 'KitchenTile')

    im = kitchen_floor.copy()
    d = ImageDraw.Draw(im)
    d.line((0, 13, 15, 13), fill=9)
    d.line((0, 14, 15, 14), fill=12)
    d.line((0, 15, 15, 15), fill=4)
    add('DiningThreshold', im)

    im = kitchen_floor.copy()
    d = ImageDraw.Draw(im)
    d.line((0, 0, 0, 15), fill=4)
    d.line((1, 0, 1, 15), fill=12)
    add('KitchenThreshold', im)

    for name, x in (('ServiceMatLeft', 0), ('ServiceMatRight', 15)):
        end = service_mat.copy()
        ImageDraw.Draw(end).line((x, 0, x, 15), fill=7)
        add(name, end)

    floor_name = cat.config['rooms']['lab_junction']['names']['Floor']
    berry_floor = cat.mapping['lab_junction'][floor_name]
    for index, (name, words) in enumerate(inlay(berry_floor, draw_berry_symbol)):
        mid = 0x140 + index
        assert cat.metas[mid] == [0] * 8 and cat.attrs[mid] == 0, mid
        cat.metas[mid] = words
        cat.attrs[mid] = 0x1000
        cat.berry_lab[name] = mid
