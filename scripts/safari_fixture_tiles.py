"""Paint the Safari Lab's capsule plates and habitat vents at tile resolution."""
from PIL import Image, ImageDraw

INK = (48, 56, 72)
SHADE = (80, 88, 104)
STEEL = (104, 112, 128)
SILVER = (139, 148, 164)
LIGHT = (144, 200, 208)
AMBER = (213, 180, 106)
GLASS_HIGHLIGHT = (232, 240, 240)


def beveled_plate(draw, left, top, right, bottom):
    draw.polygon(
        [(left, top + 1), (left + 1, top), (right - 1, top), (right, top + 1),
         (right, bottom - 1), (right - 1, bottom), (left + 1, bottom), (left, bottom - 1)],
        fill=INK,
    )
    draw.rectangle((left + 1, top + 1, right - 1, bottom - 1), fill=STEEL)
    draw.line((left + 2, top + 1, right - 2, top + 1), fill=SILVER)
    draw.line((left + 1, bottom - 1, right - 1, bottom - 1), fill=SHADE)


def lamp(draw, x, y, color):
    draw.rectangle((x, y, x + 3, y + 2), fill=INK)
    draw.line((x + 1, y + 1, x + 2, y + 1), fill=color)


def capsule_plate():
    image = Image.new('RGBA', (48, 16))
    draw = ImageDraw.Draw(image)
    draw.rectangle((5, 7, 42, 13), fill=SHADE)
    beveled_plate(draw, 6, 7, 41, 13)
    draw.line((10, 10, 29, 10), fill=INK)
    lamp(draw, 33, 9, LIGHT)
    return image


def habitat_vents():
    image = Image.new('RGBA', (96, 16))
    draw = ImageDraw.Draw(image)
    draw.rectangle((5, 3, 90, 11), fill=SHADE)
    draw.line((5, 3, 90, 3), fill=SILVER)
    draw.line((5, 11, 90, 11), fill=STEEL)
    for left, right in ((12, 35), (60, 83)):
        draw.rectangle((left, 5, right, 10), fill=INK)
        for x in range(left + 2, right - 1, 4):
            draw.line((x, 6, x, 9), fill=STEEL)
        draw.line((left + 1, 10, right - 1, 10), fill=SILVER)
    beveled_plate(draw, 40, 5, 55, 10)
    lamp(draw, 43, 6, AMBER)
    lamp(draw, 48, 6, LIGHT)
    return image


def append_safari_fixture_art(cat):
    from hub_medallions import InlayTiles, flipped_tiles

    # Use spare entries in the shared hardware palette for the status lights
    # and base colors. Other quadrants can use the existing steel palette.
    assert cat.pals[11][9:] == [(0, 0, 0)] * 7
    cat.pals[11][9:] = [INK, SHADE, STEEL, LIGHT, AMBER, GLASS_HIGHLIGHT, (0, 0, 0)]
    protected = set(range(512, 548)) | set(range(1004, 1024)) | cat.capsule_original_tiles.keys()
    used = {word & 1023 for meta in cat.metas for word in meta}
    free = sorted(set(range(1, 1024)) - used - protected)
    pool = InlayTiles(cat.tiles, free, 0, sorted(used - protected))
    cat.safari_fixture_originals = {}

    def paint_front(mid, image, x):
        original = cat.metas[mid][:]
        cat.safari_fixture_originals[mid] = original
        for j, word in enumerate(original[4:]):
            gx, gy = x * 16 + j % 2 * 8, j // 2 * 8
            paint = image.crop((gx, gy, gx + 8, gy + 8))
            if not paint.getbbox():
                continue
            raw = dict(flipped_tiles(cat.tiles[word & 1023]))[word & 0xC00]
            source_palette = cat.pals[word >> 12]
            colors = [source_palette[index] if index else None for index in raw]
            for pixel, (r, g, b, alpha) in enumerate(paint.get_flattened_data()):
                if alpha:
                    assert raw[pixel], 'Fixture art must stay inside the original silhouette'
                    colors[pixel] = (r, g, b)
            needed = {color for color in colors if color is not None}
            bank = next(b for b in (word >> 12, 4, 11) if needed <= set(cat.pals[b][1:]))
            pal = cat.pals[bank]
            tile = bytes(pal.index(color, 1) if color is not None else 0 for color in colors)
            cat.metas[mid][4 + j] = pool.word(tile) | (bank << 12)

    capsule = capsule_plate()
    for x, mid in enumerate(cat.capsule_originals):
        paint_front(mid, capsule, x)

    vents = habitat_vents()
    for x in range(6):
        paint_front(cat.terrariums[f'Habitat_{x}_4'], vents, x)
        # The tutorial habitats have two shattered center panels.
        if x not in (2, 3):
            paint_front(cat.terrariums[f'BrokenHabitat_{x}'], vents, x)
