#!/usr/bin/env python3
"""Generate the indexed Adventure Paths tileset and its binary map data."""

from __future__ import annotations

import binascii
import math
import struct
import zlib
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TILESET = ROOT / "data/tilesets/secondary/adventure_paths"
LAYOUT = ROOT / "data/layouts/Rogue_AdventurePaths"
ITEM_ROOM_LAYOUT = ROOT / "data/layouts/Rogue_Encounter_ItemRoom"
ISLAND_EDGE_SOURCE = TILESET / "island_edges.png"
CAVE_TILE_SOURCE = ROOT / "data/tilesets/secondary/cave/tiles.png"
GENERAL_HUB = ROOT / "data/tilesets/primary/general_hub"

SHEET_WIDTH_TILES = 16
METATILE_COUNT = 512
MAP_WIDTH = 44
MAP_HEIGHT = 44
ITEM_ROOM_WIDTH = 17
ITEM_ROOM_HEIGHT = 17

SECONDARY_TILE_BASE = 512
METATILE_BASE = 0x200
FORMATION_BACKGROUND_COUNT = 15
FORMATION_OVERLAY_A_BASE = 0x250
FORMATION_OVERLAY_B_BASE = FORMATION_OVERLAY_A_BASE + 6 * FORMATION_BACKGROUND_COUNT
MAP_COLLISION = 1 << 10
MAP_ELEVATION = 3 << 12
METATILE_LAYER_COVERED = 1 << 12
METATILE_BEHAVIOR_SOUTH_ARROW_WARP = 0x65

PALETTE_VOID = 6
PALETTE_ROCK = 7
PALETTE_PATH = 8

# Character-for-character transform of the original committed 44x44 Adventure
# Paths composition. Runtime masking decides which cells are visible; this
# authored hierarchy decides whether a visible surface cell is quiet stone,
# fractured stone, a mineral fleck, a low pebble group, or part of a complete
# 2x3 bedrock formation.
SURFACE_TEMPLATE = (
    '....f..*..f..*...........f..*..f....*...*...',
    '.*......of........o..**.......AB...o........',
    '*.f..*.ffff.*AB*.*...*.fo.*..*CD....*.**..*.',
    '....*..ffff..CD.***.....f.....EF...ffff.....',
    '...o*f.fff.f.EF..*...AB*fff..f..fffffff...o.',
    '.*.....ff...o.....ff.CDfffff.....fffff......',
    '.**..o.AB*..*..f.fff.EFffoff..*..ffoff...**.',
    '.**....CD..f.....f......fffff..o...*....*.**',
    '*..f..*EF.ff...f.ff...*.f.f*....f.***.......',
    '..........f..AB..ffo.............AB**...o...',
    'f..*o.*.f...*CD.fff..*.....*.f...CD.*.....*.',
    '....ff....*..EFffff.AB.o...ffffffEF..*......',
    'f.AB.ff...o*......f.CD....fffoff..ff..AB..*.',
    '..CD..f......ff**...EF......fff..ff...CD...f',
    '..EF.ffff..ABff.*..*...*........ffff..GH..ff',
    '....fffff..CD.f.......ff....*.....o.......ff',
    '*.f.fff....EF....*..fffff.*AB...........o.ff',
    '.......o...*........ffff...CD.....AB......ff',
    '*.*.o...f..ff.*......*.*...EFf..f.CD..*...ff',
    '...AB......*fff.....AB.....o......EF.....*..',
    '...CDf...*.*ffff.o*.CD....f...*f.*...f.*....',
    '...EF.ff....*f.f....EF*.......ff....fff.....',
    '**.f.ffo...f...*....fffff..*.ffff..ffoff..f.',
    '*....fff....AB....ffffff.....f**....ffff.*ff',
    '.o...fff....CD.*..ffffff*..*...*...fffff.*f.',
    '.....f...f..EF...off.ffoAB....f...o.fff.....',
    'f.......ff.*..f...fffff.CDf.....*..fff.f.o.*',
    '..*.o...ff.o............EF...*..............',
    '..*........*....*.............o.*.........*.',
    '.......f..*..f.....*.o.f....f..*..f.....*...',
    '....f..*..f..*...........f..*..f....*...*...',
    '.*......of........o..**.......AB...o........',
    '*.f..*.ffff.*AB*.*...*.fo.*..*CD....*.**..*.',
    '....*..ffff..CD.***.....f.....EF...ffff.....',
    '...o*f.fff.f.EF..*...AB*fff..f..fffffff...o.',
    '.*.....ff...o.....ff.CDfffff.....fffff......',
    '.**..o.AB*..*..f.fff.EFffoff..*..ffoff...**.',
    '.**....CD..f.....f......fffff..o...*....*.**',
    '*..f..*EF.ff...f.ff...*.f.f*....f.***.......',
    '..........f..AB..ffo.............AB**...o...',
    'f..*o.*.f...*CD.fff..*.....*.f...CD.*.....*.',
    '....ff....*..EFffff.AB.o...ffffffEF..*......',
    'f....ff...o*......f.CD....fffoff..ff......*.',
    '......f......ff**...EF......fff..ff........f',
)

METATILES = {
    "Void": 0x200,
    "Void_Stars0": 0x201,
    "Void_Stars1": 0x202,
    "Void_Stars2": 0x203,
    "Island_Interior0": 0x204,
    "Island_Interior1": 0x205,
    "Island_Interior2": 0x206,
    "Island_Edge_North": 0x207,
    "Island_Edge_East": 0x208,
    "Island_Edge_South": 0x209,
    "Island_Edge_West": 0x20A,
    "Island_Corner_NorthEast": 0x20B,
    "Island_Corner_SouthEast": 0x20C,
    "Island_Corner_SouthWest": 0x20D,
    "Island_Corner_NorthWest": 0x20E,
    "Island_InnerCorner_NorthEast": 0x20F,
    "Island_InnerCorner_SouthEast": 0x210,
    "Island_InnerCorner_SouthWest": 0x211,
    "Island_InnerCorner_NorthWest": 0x212,
    "Island_CliffFace": 0x213,
    "Island_CliffFace_Left": 0x214,
    "Island_CliffFace_Right": 0x215,
    "Island_CliffGlow": 0x216,
    "Island_CliffGlow1": 0x217,
    "Island_CliffGlow2": 0x218,
    "Island_CliffFace1": 0x219,
    "Island_CliffFace2": 0x21A,
    "Island_Edge_South1": 0x21B,
    "Island_Edge_South2": 0x21C,
    "Trail_Centre": 0x220,
    "Trail_Horizontal": 0x221,
    "Trail_Vertical": 0x222,
    "Trail_Corner_NorthEast": 0x223,
    "Trail_Corner_SouthEast": 0x224,
    "Trail_Corner_SouthWest": 0x225,
    "Trail_Corner_NorthWest": 0x226,
    "Trail_T_MissingNorth": 0x227,
    "Trail_T_MissingEast": 0x228,
    "Trail_T_MissingSouth": 0x229,
    "Trail_T_MissingWest": 0x22A,
    "Trail_Cross": 0x22B,
    "Trail_EndNorth": 0x22C,
    "Trail_EndEast": 0x22D,
    "Trail_EndSouth": 0x22E,
    "Trail_EndWest": 0x22F,
    "Trail_BlockedHorizontal": 0x230,
    "Trail_BlockedVertical": 0x231,
    "Surface_Fractured0": 0x232,
    "Surface_Fractured1": 0x233,
    "Surface_FleckCyan": 0x234,
    "Surface_FleckViolet": 0x235,
    "Surface_Pebbles0": 0x236,
    "Surface_Pebbles1": 0x237,
    "FormationA_TopLeft": 0x238,
    "FormationA_TopRight": 0x239,
    "FormationA_MiddleLeft": 0x23A,
    "FormationA_MiddleRight": 0x23B,
    "FormationA_BottomLeft": 0x23C,
    "FormationA_BottomRight": 0x23D,
    "FormationB_TopLeft": 0x23E,
    "FormationB_TopRight": 0x23F,
    "FormationB_MiddleLeft": 0x240,
    "FormationB_MiddleRight": 0x241,
    "FormationB_BottomLeft": 0x242,
    "FormationB_BottomRight": 0x243,
    "Terrace_Face0": 0x244,
    "Terrace_Face1": 0x245,
    "Terrace_Face2": 0x246,
    "Island_Underside_Left": 0x247,
    "Island_Underside_Middle0": 0x248,
    "Island_Underside_Middle1": 0x249,
    "Island_Underside_Middle2": 0x24A,
    "Island_Underside_Right": 0x24B,
    "Surface_Rock0": 0x24C,
    "Surface_Rock1": 0x24D,
    "Surface_MineralCyan": 0x24E,
    "Surface_MineralViolet": 0x24F,
    "Surface_CrystalClusterBlue0": 0x304,
    "Surface_CrystalClusterBlue1": 0x305,
    "Trail_ExitSouth": 0x306,
    "Item_Pedestal": 0x307,
    "Island_Underside_Shadow0": 0x308,
    "Island_Underside_Shadow1": 0x309,
    "Island_Underside_Shadow2": 0x30A,
    "Terrace_FaceLeftSlant0": 0x30B,
    "Terrace_FaceLeftSlant1": 0x30C,
    "Terrace_FaceLeftSlant2": 0x30D,
    "Terrace_FaceShadow0": 0x30E,
    "Terrace_FaceShadow1": 0x30F,
    "Terrace_FaceShadow2": 0x310,
    "Terrace_FaceRightSlant0": 0x311,
    "Terrace_FaceRightSlant1": 0x312,
    "Terrace_FaceRightSlant2": 0x313,
    "Satellite_SurfaceLeft": 0x314,
    "Satellite_SurfaceMiddle0": 0x315,
    "Satellite_SurfaceMiddle1": 0x316,
    "Satellite_SurfaceMiddle2": 0x317,
    "Satellite_SurfaceRight": 0x318,
    "Debris_Single0": 0x319,
    "Debris_Single1": 0x31A,
    "Debris_Single2": 0x31B,
    "Debris_Single3": 0x31C,
    "Debris_Single4": 0x31D,
    "Debris_Single5": 0x31E,
    "Debris_Single6": 0x31F,
    "Debris_Single7": 0x320,
    "Debris_Single8": 0x321,
    "Debris_Single9": 0x322,
    "Debris_PairLeft": 0x323,
    "Debris_PairRight": 0x324,
}


PALETTES = {
    PALETTE_VOID: [
        (0, 0, 0), (5, 7, 22), (8, 13, 39), (13, 23, 57),
        (20, 27, 58), (29, 39, 76), (43, 55, 95), (62, 77, 116),
        (25, 61, 111), (32, 91, 156), (42, 131, 205), (69, 183, 239),
        (105, 222, 255), (181, 243, 255), (156, 116, 255), (247, 249, 255),
    ],
    PALETTE_ROCK: [
        (0, 0, 0), (5, 7, 22), (8, 13, 39), (13, 23, 57),
        (22, 27, 48), (37, 43, 66), (53, 62, 86), (76, 87, 112),
        (24, 34, 71), (31, 49, 94), (42, 72, 125), (55, 105, 163),
        (50, 126, 165), (138, 221, 239), (104, 81, 158), (220, 250, 255),
    ],
    PALETTE_PATH: [
        (0, 0, 0), (5, 7, 22), (8, 13, 39), (13, 23, 57),
        (22, 27, 48), (37, 43, 66), (53, 62, 86), (76, 87, 112),
        (42, 43, 68), (66, 67, 94), (93, 94, 126), (133, 135, 166),
        (48, 151, 199), (91, 218, 239), (137, 93, 218), (218, 94, 229),
    ],
}


def png_chunk(kind: bytes, payload: bytes) -> bytes:
    return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", binascii.crc32(kind + payload) & 0xFFFFFFFF)


def write_indexed_png(path: Path, width: int, height: int, pixels: list[list[int]], palette: list[tuple[int, int, int]], transparent: bool = False) -> None:
    assert width % 2 == 0
    raw = bytearray()
    for row in pixels:
        raw.append(0)
        for x in range(0, width, 2):
            raw.append((row[x] << 4) | row[x + 1])

    palette_bytes = bytes(channel for color in palette for channel in color)
    palette_bytes += bytes(768 - len(palette_bytes))
    data = bytearray(b"\x89PNG\r\n\x1a\n")
    data += png_chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 4, 3, 0, 0, 0))
    data += png_chunk(b"PLTE", palette_bytes)
    if transparent:
        data += png_chunk(b"tRNS", bytes((0, *([255] * 15))))
    data += png_chunk(b"IDAT", zlib.compress(bytes(raw), 9))
    data += png_chunk(b"IEND", b"")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


def read_indexed_png(path: Path) -> tuple[int, int, list[list[int]]]:
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError(f"{path} is not a PNG")

    width = height = bit_depth = color_type = None
    compressed = bytearray()
    offset = 8
    while offset < len(data):
        length = struct.unpack_from(">I", data, offset)[0]
        kind = data[offset + 4:offset + 8]
        payload = data[offset + 8:offset + 8 + length]
        offset += 12 + length
        if kind == b"IHDR":
            width, height, bit_depth, color_type, compression, filtering, interlace = struct.unpack(">IIBBBBB", payload)
            if (bit_depth, color_type, compression, filtering, interlace) != (4, 3, 0, 0, 0):
                raise ValueError(f"{path} must be a non-interlaced 4-bit indexed PNG")
        elif kind == b"IDAT":
            compressed.extend(payload)
        elif kind == b"IEND":
            break

    if width is None or height is None or width % 2 != 0:
        raise ValueError(f"{path} has invalid dimensions")
    raw = zlib.decompress(bytes(compressed))
    stride = width // 2
    if len(raw) != (stride + 1) * height:
        raise ValueError(f"{path} has unexpected scanline data")

    pixels = []
    for y in range(height):
        scanline = raw[y * (stride + 1):(y + 1) * (stride + 1)]
        if scanline[0] != 0:
            raise ValueError(f"{path} must use PNG filter type 0")
        row = []
        for value in scanline[1:]:
            row.extend((value >> 4, value & 0xF))
        pixels.append(row)
    return width, height, pixels


def write_palette(path: Path, colors: list[tuple[int, int, int]]) -> None:
    lines = ["JASC-PAL", "0100", "16", *(f"{r} {g} {b}" for r, g, b in colors)]
    path.write_bytes(("\r\n".join(lines) + "\r\n").encode("ascii"))


def read_jasc_palette(path: Path) -> list[tuple[int, int, int]]:
    lines = path.read_text().splitlines()
    if lines[:3] != ["JASC-PAL", "0100", "16"]:
        raise ValueError(f"{path} is not a 16-color JASC palette")
    return [tuple(map(int, line.split())) for line in lines[3:19]]


def render_general_hub_metatile(metatile_id: int) -> list[list[tuple[int, int, int]]]:
    width, _, sheet = read_indexed_png(GENERAL_HUB / "tiles.png")
    raw = (GENERAL_HUB / "metatiles.bin").read_bytes()
    entries = struct.unpack(f"<{len(raw) // 2}H", raw)
    palettes = [read_jasc_palette(GENERAL_HUB / f"palettes/{index:02}.pal") for index in range(16)]
    output = [[(0, 0, 0) for _ in range(16)] for _ in range(16)]
    data = entries[metatile_id * 8:metatile_id * 8 + 8]
    tiles_per_row = width // 8

    for layer in range(2):
        for cell in range(4):
            entry = data[layer * 4 + cell]
            tile_id = entry & 0x3FF
            tile_x = (tile_id % tiles_per_row) * 8
            tile_y = (tile_id // tiles_per_row) * 8
            colors = palettes[(entry >> 12) & 0xF]
            for py in range(8):
                for px in range(8):
                    source_x = 7 - px if entry & 0x400 else px
                    source_y = 7 - py if entry & 0x800 else py
                    color_index = sheet[tile_y + source_y][tile_x + source_x]
                    if layer == 0 or color_index != 0:
                        output[(cell // 2) * 8 + py][(cell % 2) * 8 + px] = colors[color_index]
    return output


def render_general_hub_metatile_overlay(metatile_id: int) -> list[list[tuple[int, int, int] | None]]:
    """Render only a GeneralHub metatile's transparent upper layer."""
    width, _, sheet = read_indexed_png(GENERAL_HUB / "tiles.png")
    raw = (GENERAL_HUB / "metatiles.bin").read_bytes()
    entries = struct.unpack(f"<{len(raw) // 2}H", raw)
    palettes = [read_jasc_palette(GENERAL_HUB / f"palettes/{index:02}.pal") for index in range(16)]
    output = [[None for _ in range(16)] for _ in range(16)]
    data = entries[metatile_id * 8 + 4:metatile_id * 8 + 8]
    tiles_per_row = width // 8

    for cell, entry in enumerate(data):
        tile_id = entry & 0x3FF
        tile_x = (tile_id % tiles_per_row) * 8
        tile_y = (tile_id // tiles_per_row) * 8
        colors = palettes[(entry >> 12) & 0xF]
        for py in range(8):
            for px in range(8):
                source_x = 7 - px if entry & 0x400 else px
                source_y = 7 - py if entry & 0x800 else py
                color_index = sheet[tile_y + source_y][tile_x + source_x]
                if color_index != 0:
                    output[(cell // 2) * 8 + py][(cell % 2) * 8 + px] = colors[color_index]
    return output


def astral_recolor_rgb(pixels: list[list[tuple[int, int, int]]]) -> list[list[int]]:
    colors = sorted({color for row in pixels for color in row}, key=lambda color: sum(color))
    ramp = (4, 5, 5, 6, 6, 7)
    lookup = {
        color: ramp[min(len(ramp) - 1, index * len(ramp) // max(1, len(colors)))]
        for index, color in enumerate(colors)
    }
    return [[lookup[color] for color in row] for row in pixels]


def blank(width: int, height: int, color: int = 1) -> list[list[int]]:
    return [[color for _ in range(width)] for _ in range(height)]


def star_tiles(frame: int) -> list[list[list[int]]]:
    positions = ((2, 2), (5, 3), (3, 5), (5, 6))
    tiles = []
    for tile_index, (cx, cy) in enumerate(positions):
        pixels = blank(8, 8)
        phase = (frame + tile_index) % 4
        pixels[cy][cx] = (12, 13, 15, 13)[phase]
        if phase in (1, 2):
            arm = 13 if phase == 1 else 15
            pixels[cy - 1][cx] = arm
            pixels[cy + 1][cx] = arm
            pixels[cy][cx - 1] = arm
            pixels[cy][cx + 1] = arm
        if phase == 2:
            for x, y in ((cx, cy - 2), (cx, cy + 2), (cx - 2, cy), (cx + 2, cy)):
                if 0 <= x < 8 and 0 <= y < 8:
                    pixels[y][x] = 10
        pixels[(cy + 3) % 8][(cx + 2) % 8] = 9
        tiles.append(pixels)
    return tiles


def rock_metatile(
    missing: set[str],
    inner: str | None = None,
    variant: int = 0,
) -> list[list[int]]:
    pixels = blank(16, 16)
    north_jag = (0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0)
    west_jag = (1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0)

    def is_rock(x: int, y: int) -> bool:
        north_edge = 2 + north_jag[x]
        south_edge = 13 - north_jag[15 - x]
        west_edge = 2 + west_jag[y]
        east_edge = 13 - west_jag[15 - y]

        if "N" in missing and y < north_edge:
            return False
        if "S" in missing and y > south_edge:
            return False
        if "W" in missing and x < west_edge:
            return False
        if "E" in missing and x > east_edge:
            return False
        if inner == "NE" and x > 11 and y < 4 and x - y > 10:
            return False
        if inner == "SE" and x > 11 and y > 11 and x + y > 26:
            return False
        if inner == "SW" and x < 4 and y > 11 and y - x > 10:
            return False
        if inner == "NW" and x < 4 and y < 4 and x + y < 4:
            return False
        return True

    for y in range(16):
        for x in range(16):
            if not is_rock(x, y):
                continue
            color = 6
            if y < 15 and not is_rock(x, y + 1):
                color = 4
            elif x > 0 and not is_rock(x - 1, y):
                color = 7
            elif y > 0 and not is_rock(x, y - 1):
                color = 7
            elif ((x * 5 + y * 7 + variant * 11) % 29) == 0:
                color = 5
            elif ((x * 7 + y * 3 + variant * 13) % 37) == 0:
                color = 7
            pixels[y][x] = color
    return pixels


EDGE_CASES = {
    "N": (3, 2),
    "E": (0, 2),
    "W": (2, 2),
    "NE": (6, 3),
    "SE": (2, 4),
    "SW": (3, 4),
    "NW": (5, 3),
    "INNER_NE": (3, 0),
    "INNER_SE": (1, 0),
    "INNER_SW": (2, 0),
    "INNER_NW": (4, 0),
}


def overlay_pixels(base: list[list[int]], source: list[list[int]], source_x: int, source_y: int) -> list[list[int]]:
    result = [row[:] for row in base]
    for y in range(16):
        for x in range(16):
            value = source[source_y + y][source_x + x]
            if value != 0:
                result[y][x] = value
    return result


def round_upper_corner(pixels: list[list[int]], side: str, radius: int = 5) -> list[list[int]]:
    """Trim a small quarter-circle from an exposed north corner."""
    result = [row[:] for row in pixels]

    def is_inside(x: int, y: int) -> bool:
        local_x = x if side == "W" else 15 - x
        if local_x >= radius or y >= radius:
            return True
        return (local_x - radius) ** 2 + (y - radius) ** 2 <= radius ** 2

    for y in range(radius):
        for x in range(16):
            if not is_inside(x, y):
                result[y][x] = 1
            elif result[y][x] != 1 and side == "W" and x > 0 and not is_inside(x - 1, y):
                result[y][x] = 7
            elif result[y][x] != 1 and side == "E" and x < 15 and not is_inside(x + 1, y):
                result[y][x] = 7
    return result


def edge_metatile(source: list[list[int]], case: str, variant: int = 0) -> list[list[int]]:
    missing = set(case) if not case.startswith("INNER_") else set()
    inner = case.removeprefix("INNER_") if case.startswith("INNER_") else None
    pixels = rock_metatile(missing, inner=inner, variant=variant)
    column, row = EDGE_CASES[case]
    pixels = overlay_pixels(pixels, source, column * 16, row * 16)

    # The approved south corners combine the complete island lip with the
    # source sheet's matching side corner, exactly as in the static mockup.
    if case == "SW":
        pixels = overlay_pixels(pixels, source, 0, 96)
    elif case == "SE":
        pixels = overlay_pixels(pixels, source, 5 * 16, 96)
    elif case == "NW":
        pixels = round_upper_corner(pixels, "W")
    elif case == "NE":
        pixels = round_upper_corner(pixels, "E")
    return pixels


def south_edge_metatile(source: list[list[int]], column: int, variant: int = 0) -> list[list[int]]:
    pixels = rock_metatile({"S"}, variant=variant)
    return overlay_pixels(pixels, source, column * 16, 96)


def apply_wall_slant(pixels: list[list[int]], source: list[list[int]], column: int) -> list[list[int]]:
    """Clip a wall face to one of the authored full-height outer slopes."""
    result = [row[:] for row in pixels]

    for y in range(16):
        for x in range(16):
            mask = source[112 + y][column * 16 + x]
            if mask == 0:
                result[y][x] = 1
            elif column == 0 and (x == 0 or source[112 + y][column * 16 + x - 1] == 0):
                result[y][x] = 7
            elif column == 5 and (x == 15 or source[112 + y][column * 16 + x + 1] == 0):
                result[y][x] = 7
    return result


def wall_shadow_metatile(
    surface: list[list[int]],
    face: list[list[int]],
    source: list[list[int]],
) -> list[list[int]]:
    """Join a platform on the left to a wall on the right with a \\ shadow."""
    result = [row[:] for row in surface]
    shadow_ramp = {4: 1, 5: 1, 6: 2, 7: 3}

    for y in range(16):
        for x in range(16):
            mask = source[112 + y][x]
            if mask == 0:
                # Keep just enough stone texture to read as a shadow rather
                # than a missing tile, using the near-black end of the
                # Adventure Paths rock palette.
                result[y][x] = shadow_ramp.get(surface[y][x], 1)
            else:
                result[y][x] = face[y][x]
                if x == 0 or source[112 + y][x - 1] == 0:
                    result[y][x] = 4
    return result


def cobbled_debris_pixels(
    source: list[list[int]],
    width: int,
    spans: tuple[tuple[int, int, int], ...],
    texture_offset: int = 0,
) -> list[list[int]]:
    """Cut a broken silhouette from the authored platform underside."""
    pixels = blank(width, 16)
    occupied = {
        (x, y)
        for y, start_x, end_x in spans
        for x in range(start_x, end_x + 1)
    }

    for x, y in occupied:
        # The source row is the same cobbled cliff face used by the island.
        # Re-light exposed pixels after cutting so mirrored silhouettes keep
        # their top-lit appearance instead of mirroring the lighting too.
        color = source[112 + y][16 + ((x + texture_offset) % 32)]
        if (x, y - 1) not in occupied:
            color = 7
        elif (x, y + 1) not in occupied:
            color = 4
        elif (x - 1, y) not in occupied:
            color = max(color, 6)
        pixels[y][x] = color
    return pixels


def mirror_debris_spans(
    width: int,
    spans: tuple[tuple[int, int, int], ...],
) -> tuple[tuple[int, int, int], ...]:
    return tuple((y, width - 1 - end_x, width - 1 - start_x) for y, start_x, end_x in spans)


def single_debris_metatiles(source: list[list[int]]) -> list[list[list[int]]]:
    """Build five one-cell debris compositions and consistently lit mirrors."""
    silhouettes = (
        # A tall shard with a fractured two-prong underside.
        (
            (2, 7, 9),
            (3, 5, 11),
            (4, 4, 12),
            (5, 3, 12),
            (6, 3, 11),
            (7, 4, 11),
            (8, 4, 7),
            (8, 9, 10),
            (9, 5, 6),
            (9, 9, 9),
            (10, 6, 6),
        ),
        # A broad, low slab that reads as a snapped piece of cliff wall.
        (
            (4, 5, 9),
            (5, 3, 11),
            (6, 2, 12),
            (7, 2, 12),
            (8, 3, 11),
            (9, 4, 7),
            (9, 9, 10),
            (10, 5, 6),
            (10, 9, 9),
        ),
        # An offset chunk with a visible bite and a long broken point.
        (
            (3, 7, 10),
            (4, 5, 11),
            (5, 4, 12),
            (6, 3, 8),
            (6, 10, 12),
            (7, 3, 11),
            (8, 4, 10),
            (9, 5, 7),
            (9, 9, 9),
            (10, 6, 6),
        ),
        # A large shard pushed left of the cell with a detached chip to the
        # right, deliberately disguising the metatile's centre point.
        (
            (3, 3, 6),
            (4, 2, 8),
            (5, 1, 8),
            (5, 12, 13),
            (6, 1, 7),
            (6, 11, 13),
            (7, 2, 6),
            (7, 11, 12),
            (8, 3, 5),
            (8, 12, 12),
            (9, 4, 4),
        ),
        # Three separate fragments at different heights make a loose debris
        # field while still occupying just one map cell.
        (
            (2, 13, 13),
            (3, 12, 14),
            (4, 13, 13),
            (5, 7, 10),
            (6, 6, 11),
            (7, 6, 11),
            (8, 2, 3),
            (8, 7, 10),
            (9, 1, 4),
            (9, 8, 9),
            (10, 2, 3),
        ),
    )
    variants = []
    for index, spans in enumerate(silhouettes):
        for rendered_spans in (spans, mirror_debris_spans(16, spans)):
            variants.append(cobbled_debris_pixels(source, 16, rendered_spans, index * 7))
    return variants


def paired_debris_metatiles(source: list[list[int]]) -> tuple[list[list[int]], list[list[int]]]:
    """Cut an asymmetric broken slab from the authored cliff-face texture."""
    pixels = blank(32, 16)
    row_spans = {
        2: ((8, 13), (19, 23)),
        3: ((5, 16), (18, 25)),
        4: ((3, 27),),
        5: ((2, 29),),
        6: ((1, 30),),
        7: ((2, 29),),
        8: ((4, 28),),
        9: ((6, 13), (15, 26)),
        10: ((7, 12), (17, 25)),
        11: ((9, 11), (19, 23)),
        12: ((10, 10), (21, 21)),
    }
    occupied = {
        (x, y)
        for y, spans in row_spans.items()
        for start_x, end_x in spans
        for x in range(start_x, end_x + 1)
    }

    for x, y in occupied:
        # Columns 1 and 2 of the source's underside strip are complete cobble
        # faces, so the debris genuinely looks broken from the platform wall.
        color = source[112 + y][16 + x]
        if (x, y - 1) not in occupied or (x - 1, y) not in occupied:
            color = 7
        elif (x, y + 1) not in occupied:
            color = 4
        pixels[y][x] = color

    for x, y in ((14, 4), (14, 5), (15, 6), (14, 7), (15, 8), (14, 9),
                 (23, 5), (22, 6), (22, 7), (21, 8)):
        if (x, y) in occupied:
            pixels[y][x] = 4
    pixels[3][1] = 7
    pixels[4][2] = 5
    pixels[9][30] = 5
    pixels[12][4] = 4
    return (
        [row[:16] for row in pixels],
        [row[16:] for row in pixels],
    )


def underside_metatile(source: list[list[int]], column: int) -> list[list[int]]:
    pixels = blank(16, 16)
    return overlay_pixels(pixels, source, column * 16, 112)


def fractured_metatile(variant: int) -> list[list[int]]:
    pixels = rock_metatile(set(), variant=variant)
    cracks = (
        ((2, 4), (4, 5), (5, 7), (8, 7), (9, 10), (12, 11)),
        ((13, 2), (11, 4), (12, 6), (9, 8), (7, 8), (5, 11), (2, 12)),
    )[variant]
    for index, (x, y) in enumerate(cracks):
        pixels[y][x] = 4 if index % 3 else 5
    return pixels


def fleck_metatile(color: int) -> list[list[int]]:
    pixels = rock_metatile(set(), variant=color & 1)
    for x, y, value in ((7, 7, color), (8, 7, 13 if color == 12 else 15), (7, 8, 6), (11, 3, 5)):
        pixels[y][x] = value
    return pixels


def item_pedestal_metatile() -> list[list[int]]:
    """Draw a low crystal plinth without covering the item object above it."""
    pixels = rock_metatile(set(), variant=1)
    shape = (
        (7,  ".....CCCCCC....."),
        (8,  "...CCBBBBBBCC..."),
        (9,  "..CBBBBBBBBBBC.."),
        (10, "...dddddddddd..."),
        (11, "...eFFFFFFFFe..."),
        (12, "....eFFFFFFe...."),
        (13, "....eeFFFFee...."),
        (14, ".....eeeeee....."),
        (15, "....ssssssss...."),
    )
    colors = {
        "C": 13,
        "B": 10,
        "d": 8,
        "e": 7,
        "F": 5,
        "s": 3,
    }
    for y, row in shape:
        for x, symbol in enumerate(row):
            if symbol != ".":
                pixels[y][x] = colors[symbol]
    return pixels


def pebble_metatile(variant: int) -> list[list[int]]:
    pixels = rock_metatile(set(), variant=variant)
    groups = (
        ((4, 10, 8, 13), (10, 12, 13, 14)),
        ((3, 12, 6, 14), (8, 9, 13, 13)),
    )[variant]
    for x0, y0, x1, y1 in groups:
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                dx = min(x - x0, x1 - x)
                dy = min(y - y0, y1 - y)
                pixels[y][x] = 7 if dy > 0 and dx > 0 else 5
        pixels[y0][x0 + 1] = 6
    return pixels


def adapt_surface_rock(
    source: list[list[tuple[int, int, int] | None]],
    variant: int,
) -> list[list[int]]:
    """Adapt the original Adventure Paths rock into a subdued astral overlay."""
    output = blank(16, 16, 0)
    colors = sorted(
        {color for row in source for color in row if color is not None},
        key=lambda color: sum(color),
    )
    ramp = (4, 5, 6, 7)
    lookup = {
        color: ramp[min(len(ramp) - 1, index * len(ramp) // max(1, len(colors)))]
        for index, color in enumerate(colors)
    }

    for y in range(16):
        for x in range(16):
            read_x = 15 - x if variant & 1 else x
            color = source[y][read_x]
            if color is not None:
                output[y][x] = lookup[color]

    # The second plain-rock treatment is slightly cooler and flatter without
    # changing the familiar overworld silhouette.
    if variant == 1:
        for x, y in ((5, 5), (6, 5), (4, 8), (10, 9), (8, 12)):
            if output[y][x] != 0:
                output[y][x] = 5

    mineral_color = 12 if variant == 2 else 14
    mineral_highlight = 13 if variant == 2 else 15
    if variant >= 2:
        seam = ((7, 5), (8, 6), (8, 7), (9, 8), (9, 9))
        for index, (x, y) in enumerate(seam):
            if output[y][x] != 0:
                output[y][x] = mineral_highlight if index == 1 else mineral_color
    return output


def detailed_crystal_cluster(variant: int) -> list[list[int]]:
    """Return the approved clear-blue, detail-preserving 16x16 crystal trace."""
    rows = (
        ".......oo.......",
        "......ohco......",
        ".....ohhbc......",
        "....ohchbbo.....",
        "...ohcchbbco....",
        "...ocbchhcbo....",
        "oooocbbhmmbo....",
        "ohhobmbhmmbooooo",
        "ohcobmmcmmmohhho",
        "obhcommhmdmoccho",
        "obbbcomcddocchbo",
        ".bmhbcocddcbhbbo",
        ".obmcmbodombmmb.",
        "oooddcmbobmcmbo.",
        "ocdmddcobmcdmo..",
        "obcomdobmmddo...",
    )
    colors = {".": 0, "o": 8, "d": 9, "m": 10, "b": 12, "c": 13, "h": 15}
    if variant:
        rows = tuple(row[::-1] for row in rows)
    return [[colors[value] for value in row] for row in rows]


def adapt_geology_formation(source: list[list[int]], source_x: int, source_y: int, mirror: bool) -> list[list[int]]:
    output = blank(32, 48, 0)
    mapping = {1: 5, 2: 7, 3: 7, 4: 6, 5: 5, 6: 5, 7: 4}
    for y in range(48):
        for x in range(32):
            read_x = source_x + (31 - x if mirror else x)
            value = source[source_y + y][read_x]
            if value != 0:
                output[y][x] = mapping.get(value, 5)

    seam = ((21, 15, 12), (22, 16, 13), (21, 17, 12), (20, 18, 14),
            (20, 19, 12), (19, 20, 14), (18, 21, 12), (18, 22, 13))
    if mirror:
        seam = tuple((31 - x, y + 6, value) for x, y, value in seam)
    for x, y, value in seam:
        if 0 <= x < 32 and 0 <= y < 48 and output[y][x] != 0:
            output[y][x] = value
    return output


def formation_metatile(formation: list[list[int]], column: int, row: int, variant: int) -> list[list[int]]:
    pixels = rock_metatile(set(), variant=variant)
    for y in range(16):
        for x in range(16):
            value = formation[row * 16 + y][column * 16 + x]
            if value != 0:
                pixels[y][x] = value
    return pixels


def formation_overlay(formation: list[list[int]], column: int, row: int) -> list[list[int]]:
    return [
        formation[row * 16 + y][column * 16:column * 16 + 16]
        for y in range(16)
    ]


def cliff_metatile(kind: str, variant: int = 0) -> list[list[int]]:
    pixels = blank(16, 16)
    if kind == "underside":
        # Three uneven rock silhouettes keep a long southern edge from reading
        # as a row of identical neon teeth. Accents are mineral veins inside
        # the stone rather than a detached glow beneath it.
        if variant == 0:
            spans = ((2, 13), (2, 13), (3, 12), (3, 12), (4, 11), (4, 11),
                     (5, 10), (5, 10), (6, 9), (6, 9), (6, 8), (7, 8), (7, 7))
        elif variant == 1:
            spans = ((1, 12), (1, 12), (2, 11), (2, 11), (3, 10), (3, 10),
                     (3, 9), (4, 9), (4, 8), (4, 8), (5, 8), (5, 7), (5, 7),
                     (5, 6), (6, 6))
        else:
            spans = ((1, 14), (1, 14), (2, 13), (2, 13), (2, 6, 9, 13),
                     (3, 6, 9, 12), (3, 5, 10, 12), (3, 5, 10, 11),
                     (4, 5, 10, 11), (4, 4))

        for y, span in enumerate(spans):
            ranges = (span,) if len(span) == 2 else (span[:2], span[2:])
            for start, end in ranges:
                for x in range(start, end + 1):
                    color = 5
                    if x == start:
                        color = 7
                    elif x == end:
                        color = 4
                    elif (x * 3 + y * 5 + variant * 7) % 13 == 0:
                        color = 6
                    pixels[y][x] = color

        vein_x = (8, 7, 6)[variant]
        for y in range(2, min(len(spans), 9 + variant * 2), 2):
            if pixels[y][vein_x] != 1:
                pixels[y][vein_x] = 14
            vein_x += -1 if (y + variant) % 4 == 0 else 1
            vein_x = max(2, min(13, vein_x))
        tip_y = len(spans) - 1
        tip_x = next(x for x in range(16) if pixels[tip_y][x] != 1)
        pixels[tip_y][tip_x] = 12
        return pixels

    left = 0 if kind != "right" else 4
    right = 15 if kind != "left" else 11
    for y in range(16):
        inset = max(0, (y - (5 + variant)) // 4)
        for x in range(left + inset, right - inset + 1):
            if y < 2:
                color = 7
            elif y < 9:
                color = 5 if (x + y + variant) % 4 else 6
            else:
                color = 4 if (x * 3 + y + variant) % 5 else 5
            if y > 3 and (x * 5 + y * 7 + variant * 11) % 47 == 0:
                color = 14
            elif y > 6 and (x * 7 + y * 3 + variant * 13) % 59 == 0:
                color = 12
            pixels[y][x] = color
    return pixels


def trail_metatile(connections: set[str], blocked: str | None = None) -> list[list[int]]:
    pixels = rock_metatile(set(), variant=len(connections))
    path = [[False] * 16 for _ in range(16)]

    for y in range(5, 11):
        for x in range(5, 11):
            path[y][x] = True
    if "N" in connections:
        for y in range(0, 8):
            for x in range(5, 11):
                path[y][x] = True
    if "S" in connections:
        for y in range(8, 16):
            for x in range(5, 11):
                path[y][x] = True
    if "W" in connections:
        for y in range(5, 11):
            for x in range(0, 8):
                path[y][x] = True
    if "E" in connections:
        for y in range(5, 11):
            for x in range(8, 16):
                path[y][x] = True

    for y in range(16):
        for x in range(16):
            if not path[y][x]:
                continue
            edge = any(
                nx < 0 or nx >= 16 or ny < 0 or ny >= 16 or not path[ny][nx]
                for nx, ny in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1))
            )
            pixels[y][x] = 9 if edge else (11 if (x + y) % 5 == 0 else 10)

    if connections:
        pixels[7][7] = 12
        pixels[8][8] = 13

    if blocked == "H":
        for y in range(3, 13):
            for x in range(6, 10):
                pixels[y][x] = 14 if x in (6, 9) else 8
        pixels[7][7] = pixels[8][8] = 15
    elif blocked == "V":
        for y in range(6, 10):
            for x in range(3, 13):
                pixels[y][x] = 14 if y in (6, 9) else 8
        pixels[7][7] = pixels[8][8] = 15
    return pixels


class TileBuilder:
    def __init__(self) -> None:
        self.tiles = star_tiles(0)
        self.tiles.append(blank(8, 8))

    def add_tile(self, pixels: list[list[int]]) -> int:
        for index, existing in enumerate(self.tiles):
            if existing == pixels:
                return index
        self.tiles.append(pixels)
        return len(self.tiles) - 1

    def add_metatile(self, pixels: list[list[int]], palette: int) -> list[int]:
        entries = []
        for tile_y, tile_x in ((0, 0), (0, 1), (1, 0), (1, 1)):
            tile = [row[tile_x * 8:(tile_x + 1) * 8] for row in pixels[tile_y * 8:(tile_y + 1) * 8]]
            tile_index = self.add_tile(tile)
            entries.append((palette << 12) | (SECONDARY_TILE_BASE + tile_index))
        return entries + [0, 0, 0, 0]

    def add_layered_metatile(self, background: list[list[int]], overlay: list[list[int]], palette: int) -> list[int]:
        entries = []
        for pixels in (background, overlay):
            for tile_y, tile_x in ((0, 0), (0, 1), (1, 0), (1, 1)):
                tile = [row[tile_x * 8:(tile_x + 1) * 8] for row in pixels[tile_y * 8:(tile_y + 1) * 8]]
                tile_index = self.add_tile(tile)
                entries.append((palette << 12) | (SECONDARY_TILE_BASE + tile_index))
        return entries


def build_assets() -> tuple[list[list[list[int]]], list[list[int]]]:
    builder = TileBuilder()
    metatiles = [[0] * 8 for _ in range(METATILE_COUNT)]
    edge_width, edge_height, edge_source = read_indexed_png(ISLAND_EDGE_SOURCE)
    _, _, cave_source = read_indexed_png(CAVE_TILE_SOURCE)

    if (edge_width, edge_height) != (128, 128):
        raise ValueError(f"{ISLAND_EDGE_SOURCE} must contain the adapted 47-case lips and south-edge pairs")

    def put(name: str, pixels: list[list[int]], palette: int) -> None:
        metatiles[METATILES[name] - METATILE_BASE] = builder.add_metatile(pixels, palette)

    blank_tile = 4
    star_layouts = (
        (blank_tile, blank_tile, blank_tile, blank_tile),
        (0, blank_tile, blank_tile, 2),
        (blank_tile, 1, 3, blank_tile),
        (2, blank_tile, 0, blank_tile),
    )
    for index, layout in enumerate(star_layouts):
        metatiles[index] = [(PALETTE_VOID << 12) | (SECONDARY_TILE_BASE + tile) for tile in layout] + [0, 0, 0, 0]

    put("Island_Interior0", rock_metatile(set(), variant=0), PALETTE_ROCK)
    put("Island_Interior1", rock_metatile(set(), variant=1), PALETTE_ROCK)
    put("Island_Interior2", rock_metatile(set(), variant=2), PALETTE_ROCK)
    north_edge = edge_metatile(edge_source, "N")
    east_edge = edge_metatile(edge_source, "E")
    west_edge = edge_metatile(edge_source, "W")
    put("Island_Edge_North", north_edge, PALETTE_ROCK)
    put("Island_Edge_East", east_edge, PALETTE_ROCK)
    put("Island_Edge_South", south_edge_metatile(edge_source, 2), PALETTE_ROCK)
    put("Island_Edge_South1", south_edge_metatile(edge_source, 1, 1), PALETTE_ROCK)
    put("Island_Edge_South2", south_edge_metatile(edge_source, 3, 2), PALETTE_ROCK)
    put("Island_Edge_West", west_edge, PALETTE_ROCK)

    north_west_corner = edge_metatile(edge_source, "NW")
    north_east_corner = edge_metatile(edge_source, "NE")
    for y in range(16):
        north_west_corner[y][15] = north_edge[y][0]
        north_east_corner[y][0] = north_edge[y][15]
    for x in range(16):
        north_west_corner[15][x] = west_edge[0][x]
        north_east_corner[15][x] = east_edge[0][x]

    # Treat the adjoining edge profiles as a contract. If either straight
    # edge changes later, corner generation must still meet it pixel-for-pixel.
    assert [row[15] for row in north_west_corner[:15]] == [row[0] for row in north_edge[:15]]
    assert [row[0] for row in north_east_corner[:15]] == [row[15] for row in north_edge[:15]]
    assert north_west_corner[15] == west_edge[0]
    assert north_east_corner[15] == east_edge[0]
    put("Island_Corner_NorthWest", north_west_corner, PALETTE_ROCK)
    put("Island_Corner_NorthEast", north_east_corner, PALETTE_ROCK)

    for short, full in (("SE", "SouthEast"), ("SW", "SouthWest")):
        put(f"Island_Corner_{full}", edge_metatile(edge_source, short), PALETTE_ROCK)
    for short, full in (("NE", "NorthEast"), ("SE", "SouthEast"), ("SW", "SouthWest"), ("NW", "NorthWest")):
        put(f"Island_InnerCorner_{full}", edge_metatile(edge_source, f"INNER_{short}"), PALETTE_ROCK)

    satellite_left = overlay_pixels(north_west_corner, edge_source, 0, 96)
    satellite_middles = [
        overlay_pixels(north_edge, edge_source, column * 16, 96)
        for column in (1, 2, 3)
    ]
    satellite_right = overlay_pixels(north_east_corner, edge_source, 5 * 16, 96)
    satellite_seam = [row[0] for row in satellite_middles[0]]
    for y in range(16):
        satellite_left[y][15] = satellite_seam[y]
        satellite_right[y][0] = satellite_seam[y]
        for middle in satellite_middles:
            middle[y][0] = satellite_seam[y]
            middle[y][15] = satellite_seam[y]
    put("Satellite_SurfaceLeft", satellite_left, PALETTE_ROCK)
    for index, middle in enumerate(satellite_middles):
        put(f"Satellite_SurfaceMiddle{index}", middle, PALETTE_ROCK)
    put("Satellite_SurfaceRight", satellite_right, PALETTE_ROCK)

    debris_pair_left, debris_pair_right = paired_debris_metatiles(edge_source)
    for index, debris in enumerate(single_debris_metatiles(edge_source)):
        put(f"Debris_Single{index}", debris, PALETTE_ROCK)
    put("Debris_PairLeft", debris_pair_left, PALETTE_ROCK)
    put("Debris_PairRight", debris_pair_right, PALETTE_ROCK)

    put("Island_CliffFace", cliff_metatile("centre", 0), PALETTE_ROCK)
    put("Island_CliffFace_Left", cliff_metatile("left"), PALETTE_ROCK)
    put("Island_CliffFace_Right", cliff_metatile("right"), PALETTE_ROCK)
    put("Island_CliffGlow", cliff_metatile("underside", 0), PALETTE_ROCK)
    put("Island_CliffGlow1", cliff_metatile("underside", 1), PALETTE_ROCK)
    put("Island_CliffGlow2", cliff_metatile("underside", 2), PALETTE_ROCK)
    put("Island_CliffFace1", cliff_metatile("centre", 1), PALETTE_ROCK)
    put("Island_CliffFace2", cliff_metatile("centre", 2), PALETTE_ROCK)

    terrace_source_ids = (0x069, 0x06C, 0x079)
    for index, source_id in enumerate(terrace_source_ids):
        terrace = astral_recolor_rgb(render_general_hub_metatile(source_id))
        put(f"Terrace_Face{index}", terrace, PALETTE_ROCK)
        put(f"Terrace_FaceLeftSlant{index}", apply_wall_slant(terrace, edge_source, 0), PALETTE_ROCK)
        put(
            f"Terrace_FaceShadow{index}",
            wall_shadow_metatile(rock_metatile(set(), variant=index), terrace, edge_source),
            PALETTE_ROCK,
        )
        put(f"Terrace_FaceRightSlant{index}", apply_wall_slant(terrace, edge_source, 5), PALETTE_ROCK)

    put("Island_Underside_Left", underside_metatile(edge_source, 0), PALETTE_ROCK)
    put("Island_Underside_Middle0", underside_metatile(edge_source, 1), PALETTE_ROCK)
    put("Island_Underside_Middle1", underside_metatile(edge_source, 2), PALETTE_ROCK)
    put("Island_Underside_Middle2", underside_metatile(edge_source, 3), PALETTE_ROCK)
    put("Island_Underside_Right", underside_metatile(edge_source, 5), PALETTE_ROCK)
    for index in range(3):
        put(
            f"Island_Underside_Shadow{index}",
            wall_shadow_metatile(
                rock_metatile(set(), variant=index),
                underside_metatile(edge_source, index + 1),
                edge_source,
            ),
            PALETTE_ROCK,
        )

    trail_shapes = {
        "Trail_Centre": set(), "Trail_Horizontal": {"E", "W"}, "Trail_Vertical": {"N", "S"},
        "Trail_Corner_NorthEast": {"N", "E"}, "Trail_Corner_SouthEast": {"S", "E"},
        "Trail_Corner_SouthWest": {"S", "W"}, "Trail_Corner_NorthWest": {"N", "W"},
        "Trail_T_MissingNorth": {"E", "S", "W"}, "Trail_T_MissingEast": {"N", "S", "W"},
        "Trail_T_MissingSouth": {"N", "E", "W"}, "Trail_T_MissingWest": {"N", "E", "S"},
        "Trail_Cross": {"N", "E", "S", "W"}, "Trail_EndNorth": {"N"},
        "Trail_EndEast": {"E"}, "Trail_EndSouth": {"S"}, "Trail_EndWest": {"W"},
    }
    for name, connections in trail_shapes.items():
        put(name, trail_metatile(connections), PALETTE_PATH)
    # The Item Room exit occupies the island's southern lip. Keep a dedicated
    # metatile for its warp behavior, but draw it as the matching bottom edge.
    put("Trail_ExitSouth", south_edge_metatile(edge_source, 2), PALETTE_ROCK)
    put("Trail_BlockedHorizontal", trail_metatile({"E", "W"}, blocked="H"), PALETTE_PATH)
    put("Trail_BlockedVertical", trail_metatile({"N", "S"}, blocked="V"), PALETTE_PATH)

    put("Surface_Fractured0", fractured_metatile(0), PALETTE_ROCK)
    put("Surface_Fractured1", fractured_metatile(1), PALETTE_ROCK)
    put("Surface_FleckCyan", fleck_metatile(12), PALETTE_ROCK)
    put("Surface_FleckViolet", fleck_metatile(14), PALETTE_ROCK)
    put("Surface_Pebbles0", pebble_metatile(0), PALETTE_ROCK)
    put("Surface_Pebbles1", pebble_metatile(1), PALETTE_ROCK)

    put("Item_Pedestal", item_pedestal_metatile(), PALETTE_ROCK)

    original_rock = render_general_hub_metatile_overlay(0x0E0)
    surface_rock_names = (
        "Surface_Rock0",
        "Surface_Rock1",
        "Surface_MineralCyan",
        "Surface_MineralViolet",
    )
    for variant, name in enumerate(surface_rock_names):
        metatiles[METATILES[name] - METATILE_BASE] = builder.add_layered_metatile(
            rock_metatile(set(), variant=variant),
            adapt_surface_rock(original_rock, variant),
            PALETTE_ROCK,
        )

    crystal_clusters = (
        "Surface_CrystalClusterBlue0",
        "Surface_CrystalClusterBlue1",
    )
    for variant, name in enumerate(crystal_clusters):
        metatiles[METATILES[name] - METATILE_BASE] = builder.add_layered_metatile(
            rock_metatile(set(), variant=variant + 1),
            detailed_crystal_cluster(variant),
            PALETTE_ROCK,
        )

    formations = (
        ("FormationA", adapt_geology_formation(cave_source, 0, 144, False)),
        ("FormationB", adapt_geology_formation(cave_source, 0, 160, True)),
    )
    for formation_index, (name, formation) in enumerate(formations):
        for row, row_name in enumerate(("Top", "Middle", "Bottom")):
            for column, column_name in enumerate(("Left", "Right")):
                put(
                    f"{name}_{row_name}{column_name}",
                    formation_metatile(formation, column, row, formation_index),
                    PALETTE_ROCK,
                )

    formation_backgrounds = (
        rock_metatile(set(), variant=0),
        rock_metatile(set(), variant=1),
        rock_metatile(set(), variant=2),
        edge_metatile(edge_source, "N"),
        edge_metatile(edge_source, "E"),
        south_edge_metatile(edge_source, 2),
        edge_metatile(edge_source, "W"),
        edge_metatile(edge_source, "NE"),
        edge_metatile(edge_source, "SE"),
        edge_metatile(edge_source, "SW"),
        edge_metatile(edge_source, "NW"),
        edge_metatile(edge_source, "INNER_NE"),
        edge_metatile(edge_source, "INNER_SE"),
        edge_metatile(edge_source, "INNER_SW"),
        edge_metatile(edge_source, "INNER_NW"),
    )
    for formation_index, (_, formation) in enumerate(formations):
        base_metatile = FORMATION_OVERLAY_A_BASE if formation_index == 0 else FORMATION_OVERLAY_B_BASE
        for row in range(3):
            for column in range(2):
                part = row * 2 + column
                overlay = formation_overlay(formation, column, row)
                for background_index, background in enumerate(formation_backgrounds):
                    metatile_id = base_metatile + part * FORMATION_BACKGROUND_COUNT + background_index
                    metatiles[metatile_id - METATILE_BASE] = builder.add_layered_metatile(background, overlay, PALETTE_ROCK)
    return builder.tiles, metatiles


def write_tilesheet(tiles: list[list[list[int]]]) -> None:
    rows = math.ceil(len(tiles) / SHEET_WIDTH_TILES)
    pixels = blank(SHEET_WIDTH_TILES * 8, rows * 8, 0)
    for index, tile in enumerate(tiles):
        ox = (index % SHEET_WIDTH_TILES) * 8
        oy = (index // SHEET_WIDTH_TILES) * 8
        for y in range(8):
            pixels[oy + y][ox:ox + 8] = tile[y]
    write_indexed_png(TILESET / "tiles.png", len(pixels[0]), len(pixels), pixels, PALETTES[PALETTE_PATH])


def write_animation_frames() -> None:
    for frame in range(4):
        pixels = blank(32, 8, 0)
        for tile_index, tile in enumerate(star_tiles(frame)):
            for y in range(8):
                pixels[y][tile_index * 8:(tile_index + 1) * 8] = tile[y]
        write_indexed_png(TILESET / f"anim/stars/{frame}.png", 32, 8, pixels, PALETTES[PALETTE_VOID])


def get_template_metatile(x: int, y: int) -> int:
    value = SURFACE_TEMPLATE[y][x]
    if value == '.':
        return METATILES["Island_Interior0"] + ((x * 5 + y * 7) % 3)
    if value == 'f':
        return METATILES["Surface_Fractured0"] + ((x + y) & 1)
    if value == '*':
        if ((x * 7 + y * 11) % 11) == 0:
            return METATILES["Surface_CrystalClusterBlue0"] + ((x + y) & 1)
        return METATILES["Surface_FleckCyan"] + ((x * 3 + y * 5) & 1)
    if value == 'o':
        return METATILES["Surface_Rock0"] + ((x * 3 + y * 5) & 3)

    offsets = {
        'A': ("TopLeft", 2), 'B': ("TopRight", 2),
        'C': ("MiddleLeft", 1), 'D': ("MiddleRight", 1),
        'E': ("BottomLeft", 0), 'F': ("BottomRight", 0),
        'G': ("BottomLeft", 0), 'H': ("BottomRight", 0),
    }
    if value in offsets:
        part, bottom_offset = offsets[value]
        is_sparse = value in 'GH'
        if bottom_offset != 0 and y + bottom_offset < MAP_HEIGHT:
            is_sparse = SURFACE_TEMPLATE[y + bottom_offset][x] in 'GH'
        return METATILES[f"Formation{'B' if is_sparse else 'A'}_{part}"]
    raise ValueError(f"Unknown Adventure Paths surface-template value {value!r} at {x},{y}")


def write_layout_template() -> None:
    if len(SURFACE_TEMPLATE) != MAP_HEIGHT or any(len(row) != MAP_WIDTH for row in SURFACE_TEMPLATE):
        raise ValueError("Adventure Paths surface template must be exactly 44x44")
    entries = [
        get_template_metatile(x, y) | MAP_COLLISION | MAP_ELEVATION
        for y in range(MAP_HEIGHT)
        for x in range(MAP_WIDTH)
    ]
    (LAYOUT / "map.bin").write_bytes(struct.pack(f"<{len(entries)}H", *entries))
    (LAYOUT / "border.bin").write_bytes(struct.pack("<4H", *([METATILES["Void"]] * 4)))


def get_item_room_void_metatile(x: int, y: int) -> int:
    value = ((x + 0x9E37) * 0x45D9F3B) ^ ((y + 0x7F4A) * 0x119DE1F3)
    value ^= value >> 16
    variant = value % 10
    if variant < 3:
        return METATILES[f"Void_Stars{variant}"]
    return METATILES["Void"]


def write_item_room_layout() -> None:
    """Write the static Sableye-themed floating island used by Item Rooms."""
    surface_bounds = {
        2: (6, 10),
        3: (4, 12),
        **{y: (3, 13) for y in range(4, 13)},
        13: (5, 11),
        14: (7, 9),
    }
    surface = {
        (x, y)
        for y, (left, right) in surface_bounds.items()
        for x in range(left, right + 1)
    }
    trail = {(8, y) for y in range(5, 15)}
    solid_decorations = {(8, 4)}
    crystals = {
        (5, 5): "Surface_CrystalClusterBlue0",
        (11, 5): "Surface_CrystalClusterBlue1",
        (4, 9): "Surface_CrystalClusterBlue1",
        (12, 9): "Surface_CrystalClusterBlue0",
    }
    accents = {
        (6, 8): "Surface_MineralCyan",
        (10, 8): "Surface_MineralViolet",
        (5, 11): "Surface_FleckViolet",
        (11, 11): "Surface_FleckCyan",
    }
    # Preserve the hand-authored surface details from the Item Room map while
    # keeping this generator as the source of truth for regenerated layouts.
    authored_overrides = {
        (8, 3): ("Surface_Fractured1", False),
        (6, 4): ("Surface_Fractured1", False),
        (8, 4): ("Surface_Fractured1", True),
        (10, 4): ("Surface_Fractured1", False),
        (8, 5): ("Island_Interior2", True),
        (12, 5): ("Surface_Fractured1", False),
        (8, 6): ("Item_Pedestal", True),
        (12, 6): ("Surface_Fractured1", False),
        (6, 8): ("Surface_MineralCyan", True),
        (10, 8): ("Surface_MineralViolet", True),
        (4, 10): ("Surface_Fractured1", False),
        (7, 10): ("Island_Interior2", False),
        (6, 11): ("Surface_Fractured1", False),
        (12, 11): ("Surface_Fractured1", False),
        (4, 12): ("Island_Edge_South", True),
        (10, 12): ("Surface_Fractured1", False),
        (10, 13): ("Island_Edge_South", True),
    }

    def is_surface(x: int, y: int) -> bool:
        return (x, y) in surface

    def island_metatile(x: int, y: int) -> int:
        north = is_surface(x, y - 1)
        east = is_surface(x + 1, y)
        south = is_surface(x, y + 1)
        west = is_surface(x - 1, y)

        if not north and not east:
            return METATILES["Island_Corner_NorthEast"]
        if not east and not south:
            return METATILES["Island_Corner_SouthEast"]
        if not south and not west:
            return METATILES["Island_Corner_SouthWest"]
        if not west and not north:
            return METATILES["Island_Corner_NorthWest"]
        if not north:
            return METATILES["Island_Edge_North"]
        if not east:
            return METATILES["Island_Edge_East"]
        if not south:
            return METATILES["Island_Edge_South"] + ((x * 5 + y * 3) % 3)
        if not west:
            return METATILES["Island_Edge_West"]
        if not is_surface(x + 1, y - 1):
            return METATILES["Island_InnerCorner_NorthEast"]
        if not is_surface(x + 1, y + 1):
            return METATILES["Island_InnerCorner_SouthEast"]
        if not is_surface(x - 1, y + 1):
            return METATILES["Island_InnerCorner_SouthWest"]
        if not is_surface(x - 1, y - 1):
            return METATILES["Island_InnerCorner_NorthWest"]
        return METATILES["Island_Interior0"] + ((x * 5 + y * 7) % 3)

    def trail_metatile(x: int, y: int) -> int:
        north = (x, y - 1) in trail
        south = (x, y + 1) in trail
        if north and south:
            return METATILES["Trail_Vertical"]
        if north:
            return METATILES["Trail_ExitSouth"]
        if south:
            return METATILES["Trail_EndSouth"]
        return METATILES["Trail_Centre"]

    entries = []
    for y in range(ITEM_ROOM_HEIGHT):
        for x in range(ITEM_ROOM_WIDTH):
            collision = MAP_COLLISION
            if (x, y) in surface:
                metatile = island_metatile(x, y)
                if (x, y) in trail:
                    metatile = trail_metatile(x, y)
                    collision = 0
                elif (x, y) in crystals:
                    metatile = METATILES[crystals[(x, y)]]
                elif (x, y) in accents:
                    metatile = METATILES[accents[(x, y)]]
                    collision = 0
                elif (x, y) in solid_decorations:
                    pass
                elif all(
                    is_surface(nx, ny)
                    for nx, ny in ((x, y - 1), (x + 1, y), (x, y + 1), (x - 1, y))
                ):
                    collision = 0
            elif is_surface(x, y - 1):
                left = not is_surface(x - 1, y) and is_surface(x - 1, y - 1)
                right = not is_surface(x + 1, y) and is_surface(x + 1, y - 1)
                if not left and right:
                    metatile = METATILES["Island_Underside_Left"]
                elif left and not right:
                    metatile = METATILES["Island_Underside_Right"]
                else:
                    metatile = METATILES["Island_Underside_Middle0"] + ((x + y) % 3)
            else:
                metatile = get_item_room_void_metatile(x, y)

            if (x, y) in authored_overrides:
                metatile_name, is_solid = authored_overrides[(x, y)]
                metatile = METATILES[metatile_name]
                collision = MAP_COLLISION if is_solid else 0
            entries.append(metatile | collision | MAP_ELEVATION)

    ITEM_ROOM_LAYOUT.mkdir(parents=True, exist_ok=True)
    (ITEM_ROOM_LAYOUT / "map.bin").write_bytes(struct.pack(f"<{len(entries)}H", *entries))
    (ITEM_ROOM_LAYOUT / "border.bin").write_bytes(struct.pack("<4H", *([METATILES["Void"]] * 4)))


def main() -> None:
    tiles, metatiles = build_assets()
    if len(tiles) > 512:
        raise RuntimeError(f"Adventure Paths uses {len(tiles)} tiles; maximum is 512")

    write_tilesheet(tiles)
    write_animation_frames()
    for palette_index, colors in PALETTES.items():
        write_palette(TILESET / f"palettes/{palette_index:02}.pal", colors)

    flat_metatiles = [entry for metatile in metatiles for entry in metatile]
    (TILESET / "metatiles.bin").write_bytes(struct.pack(f"<{len(flat_metatiles)}H", *flat_metatiles))
    attributes = [0] * METATILE_COUNT
    covered_metatiles = (
        *range(METATILES["Surface_Rock0"], METATILES["Surface_MineralViolet"] + 1),
        *range(FORMATION_OVERLAY_A_BASE, FORMATION_OVERLAY_B_BASE + 6 * FORMATION_BACKGROUND_COUNT),
        METATILES["Surface_CrystalClusterBlue0"],
        METATILES["Surface_CrystalClusterBlue1"],
    )
    for metatile in covered_metatiles:
        attributes[metatile - METATILE_BASE] = METATILE_LAYER_COVERED
    attributes[METATILES["Trail_ExitSouth"] - METATILE_BASE] = METATILE_BEHAVIOR_SOUTH_ARROW_WARP
    (TILESET / "metatile_attributes.bin").write_bytes(struct.pack(f"<{len(attributes)}H", *attributes))

    write_layout_template()
    write_item_room_layout()
    print(f"Generated {len(tiles)} tiles and {len(METATILES)} named metatiles")


if __name__ == "__main__":
    main()
