"""Import curated CC-BY laboratory props into stable shared-hub padding.

Source: Land of Pixels - Laboratory Tileset PixelArt 16px by marceles.
The source coordinates below are 16x16 cells in tiles_stuff_16px.png.
"""
import hashlib

from PIL import Image

from generate_portal_room import ROOT


SOURCE = ROOT / "graphics/tilesets/third_party/land_of_pixels_laboratory/tiles_stuff_16px.png"
FIRST_TILE = 703
LIMIT_TILE = 1008  # Safari animation begins here; never allocate into it.

GROUPS = (
    # name, source x/y, width/height, hub backing, add shadow-backed copy
    ("StatusConsole", 18, 2, 5, 2, "WallBase", False),
    ("DataDisplay", 23, 2, 3, 2, "WallBase", False),
    ("StorageUnit", 30, 3, 3, 3, "Floor", False),
    ("WallControls", 30, 6, 3, 1, "WallBase", False),
    ("SpecimenTank", 31, 7, 2, 4, "Floor", False),
    ("ServerBank", 33, 9, 3, 3, "FloorShadow", False),
    ("SmallMonitor", 26, 11, 3, 2, "Floor", False),
    ("EquipmentTable", 27, 13, 3, 1, "Floor", False),
    ("EquipmentBenchBlue", 27, 14, 3, 1, "Floor", False),
    ("EquipmentBenchWhite", 27, 15, 3, 1, "Floor", False),
    ("AnalysisConsole", 33, 12, 3, 4, "Floor", False),
    ("OrbGreen", 25, 14, 1, 1, "Floor", False),
    ("OrbRed", 25, 15, 1, 1, "Floor", False),
)


def append_lab_decor(cat):
    assert len(cat.metas) == 948, "Lab decoration IDs moved; review prefab IDs"
    assert hashlib.sha256(SOURCE.read_bytes()).hexdigest() == "5ce47fd8afd8d03cb0b3d8d97fa0fd88cabc27bd3863eaa0411b8b5db2f1ec94"
    source = Image.open(SOURCE).convert("RGBA")
    assert source.size == (592, 256), "Unexpected Land of Pixels source revision"

    used = {word & 1023 for meta in cat.metas for word in meta}
    assert not used.intersection(range(FIRST_TILE, LIMIT_TILE))
    assert all(not any(cat.tiles[i]) for i in range(FIRST_TILE, LIMIT_TILE))
    reserved = set(range(512, 548)) | set(range(1008, 1012))
    cursor = FIRST_TILE
    cat.lab_decor = {}
    cat.lab_decor_groups = []

    def store(pixels):
        nonlocal cursor
        index = next((i for i, tile in enumerate(cat.tiles) if i not in reserved and tile == pixels), None)
        if index is None:
            assert cursor < LIMIT_TILE, "Lab decorations exceeded static padding"
            index = cursor
            cursor += 1
            cat.tiles[index] = pixels
        return index

    for group_name, sx, sy, width, height, backing, add_shadow in GROUPS:
        crop = source.crop((sx * 16, sy * 16, (sx + width) * 16, (sy + height) * 16))
        colors = {pixel[:3] for pixel in crop.getdata() if pixel[3]}
        bank = min(
            range(11),
            key=lambda b: sum(
                min(sum((color[k] - candidate[k]) ** 2 for k in range(3)) for candidate in cat.pals[b][1:])
                for color in colors
            ),
        )
        color_map = {
            color: min(
                range(1, 16),
                key=lambda i: sum((color[k] - cat.pals[bank][i][k]) ** 2 for k in range(3)),
            )
            for color in colors
        }
        first_metatile = len(cat.metas)
        foregrounds = []
        for y in range(height):
            for x in range(width):
                cell = source.crop(((sx + x) * 16, (sy + y) * 16, (sx + x + 1) * 16, (sy + y + 1) * 16))
                foreground = []
                for qx, qy in ((0, 0), (8, 0), (0, 8), (8, 8)):
                    tile = list(cell.crop((qx, qy, qx + 8, qy + 8)).getdata())
                    if not any(pixel[3] for pixel in tile):
                        foreground.append(0)
                        continue
                    pixels = bytes(color_map[pixel[:3]] if pixel[3] else 0 for pixel in tile)
                    foreground.append(store(pixels) | (bank << 12))
                foregrounds.append(foreground)
                name = f"{group_name}_{x}_{y}"
                background = cat.mapping["lab_junction"][cat.config["rooms"]["lab_junction"]["names"][backing]]
                cat.lab_decor[name] = len(cat.metas)
                cat.metas.append(cat.metas[background][:4] + foreground)
                cat.attrs.append(0x1000)

        shadow_start = None
        if add_shadow:
            shadow_start = len(cat.metas)
            shadow = cat.mapping["lab_junction"][cat.config["rooms"]["lab_junction"]["names"]["FloorShadow"]]
            for y in range(height):
                for x in range(width):
                    name = f"{group_name}_{x}_{y}Shadow"
                    cat.lab_decor[name] = len(cat.metas)
                    cat.metas.append(cat.metas[shadow][:4] + foregrounds[y * width + x])
                    cat.attrs.append(0x1000)

        cat.lab_decor_groups.append(
            {
                "name": group_name,
                "source": [sx, sy, width, height],
                "size": [width, height],
                "metatile_start": first_metatile,
                "shadow_start": shadow_start,
                "palette": bank,
            }
        )

    assert len(cat.metas) == 1022
    cat.lab_decor_tile_range = (FIRST_TILE, cursor)
