#include "global.h"
#include "diploma.h"
#include "palette.h"
#include "main.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "task.h"
#include "main.h"
#include "malloc.h"
#include "decompress.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_screen_effect.h"
#include "bg.h"
#include "window.h"
#include "script.h"
#include "string_util.h"
#include "item_icon.h"
#include "text.h"
#include "text_window.h"
#include "overworld.h"
#include "menu.h"
#include "pokedex.h"
#include "sound.h"
#include "constants/event_objects.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_timeofday.h"
#include "rogue_worldmap.h"

#define TAG_ITEM_ICON_BASE 2110

#define TILE_RED_MARKER         0x09
#define TILE_GREEN_MARKER       0x0A
#define TILE_YELLOW_MARKER      0x0D
#define TILE_PATH               0x0B
#define TILE_PATH_GREEN_FADE    0x0C

struct WorldMapData
{
    s16 posX;
    s16 posY;
    s16 centreX;
    s16 centreY;
    s16 playerX;
    s16 playerY;
    u8 currentArea;
    u8 playerArea;
    u8 mode;
    u8 movingArea;
    bool8 areaMoved;
};

EWRAM_DATA static struct WorldMapData *sWorldMapData = NULL;
EWRAM_DATA static u8 *sWorldMapTilemapPtr = NULL;
static u8 sPendingWorldMapMode = 0;

enum WorldMapMode
{
    WORLD_MAP_MODE_VIEW,
    WORLD_MAP_MODE_MOVE_SELECT,
    WORLD_MAP_MODE_MOVE_PLACE,
};

static u8 const sText_BasicDesc[] = _("{STR_VAR_1}");
static u8 const sText_TeleportDesc[] = _("{STR_VAR_1}\n{A_BUTTON}{FONT_SHORT} Teleport");
static u8 const sText_MoveSelectDesc[] = _("{STR_VAR_1}\n{A_BUTTON}{FONT_SHORT} Move");
static u8 const sText_CannotMoveDesc[] = _("{STR_VAR_1}\n{COLOR RED}{FONT_SHORT}Can't move");
static u8 const sText_MovePlaceDesc[] = _("{STR_VAR_1}\n{A_BUTTON}{FONT_SHORT} Place {B_BUTTON}{FONT_SHORT} Back");
static u8 const sText_MoveCannotPlaceDesc[] = _("{STR_VAR_1}\n{COLOR RED}{FONT_SHORT}Can't place");

static void MainCB2(void);
static void InitData();
static void Task_WorldMapFadeIn(u8);
static void Task_WorldMapWaitForKeyPress(u8);
static void Task_WorldMapWaitForMoveKeyPress(u8);
static void Task_WorldMapFadeOut(u8);
static void Task_WorldMapWarpOut(u8);
static void DrawWorldMapTiles();
static void DisplayTitleText(void);
static void DisplayDescText(void);
static void DisplayIcons(void);
static bool8 IsCoordOnWorldMap(s16 x, s16 y);
static void InitWorldMapBg(void);
static void InitWorldMapWindow(void);
static void PrintWorldMapText(u8, u8 const*, u8, u8);
static void DrawBgWindowFrames(void);

enum PaletteIds
{
    BG_PAL_ID_WORLD_MAP,
    BG_PAL_ID_WINDOW_FRAMES,
    BG_PAL_ID_ICON,
    BG_PAL_ID_STD_MENU = 15,
};

enum WindowIds
{
    WIN_TITLE,
    WIN_DESC,
    WIN_COUNT
};

struct WindowBorder
{
    u8 bg;
    u8 tilemapLeft;
    u8 tilemapTop;
    u8 width;
    u8 height;
};

static const struct BgTemplate sWorldMapBgTemplates[2] =
{
    {
        .bg = 0,
        .charBaseIndex = 1,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 6,
        .screenSize = 1,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0,
    },
};

static const struct WindowTemplate sWorldMapWinTemplates[WIN_COUNT + 1] =
{
    [WIN_TITLE] = 
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 12,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_DESC] = 
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 3,
        .width = 12,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 1 + (12 * 2),
    },
    DUMMY_WIN_TEMPLATE,
};

static const struct WindowBorder sWorldMapWinBorders[] =
{
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 12,
        .height = 6,
    }
};

#define FREE_BLOCK_START (1 + (12 * 2) + (12 * 4))

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static const u32 sWorldMapTiles[] = INCBIN_U32("graphics/rogue_worldmap/map.4bpp.lz");
static const u16 sWorldMapPalette[] = INCBIN_U16("graphics/rogue_worldmap/map.gbapal");

static const u32 sWorldMapTilemap0[] = INCBIN_U32("graphics/rogue_worldmap/map.bin.lz");
static const u32 sWorldMapTilemap1[] = INCBIN_U32("graphics/rogue_worldmap/map1.bin.lz");
static const u32 sWorldMapTilemap2[] = INCBIN_U32("graphics/rogue_worldmap/map2.bin.lz");
static const u32 sWorldMapTilemap3[] = INCBIN_U32("graphics/rogue_worldmap/map3.bin.lz");
static const u32 sWorldMapTilemap4[] = INCBIN_U32("graphics/rogue_worldmap/map4.bin.lz");
static const u32 sWorldMapTilemap5[] = INCBIN_U32("graphics/rogue_worldmap/map5.bin.lz");
static const u32 sWorldMapTilemap6[] = INCBIN_U32("graphics/rogue_worldmap/map6.bin.lz");
static const u32 sWorldMapTilemap7[] = INCBIN_U32("graphics/rogue_worldmap/map7.bin.lz");

static const u32* const sWorldMapTilemaps[] = 
{
    sWorldMapTilemap0,
    sWorldMapTilemap1,
    sWorldMapTilemap2,
    sWorldMapTilemap3,
    sWorldMapTilemap4,
    sWorldMapTilemap5,
    sWorldMapTilemap6,
    sWorldMapTilemap7,
};

void Rogue_OpenWorldMap(MainCallback callback)
{
    sPendingWorldMapMode = WORLD_MAP_MODE_VIEW;
    gMain.savedCallback = callback;
    SetMainCallback2(CB2_ShowWorldMap);
    LockPlayerFieldControls();
}

void Rogue_OpenWorldMapMoveArea(MainCallback callback)
{
    sPendingWorldMapMode = WORLD_MAP_MODE_MOVE_SELECT;
    VarSet(VAR_RESULT, FALSE);
    gMain.savedCallback = callback;
    SetMainCallback2(CB2_ShowWorldMap);
    LockPlayerFieldControls();
}

void CB2_ShowWorldMap(void)
{
    RogueToD_SetTempDisableTimeVisuals(TRUE);

    SetVBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG3CNT, 0);
    SetGpuReg(REG_OFFSET_BG2CNT, 0);
    SetGpuReg(REG_OFFSET_BG1CNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    SetGpuReg(REG_OFFSET_BG3HOFS, 0);
    SetGpuReg(REG_OFFSET_BG3VOFS, 0);
    SetGpuReg(REG_OFFSET_BG2HOFS, 0);
    SetGpuReg(REG_OFFSET_BG2VOFS, 0);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
    SetGpuReg(REG_OFFSET_BG1VOFS, 0);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
    // why doesn't this one use the dma manager either?
    DmaFill16(3, 0, VRAM, VRAM_SIZE);
    DmaFill32(3, 0, OAM, OAM_SIZE);
    DmaFill16(3, 0, PLTT, PLTT_SIZE);
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    ResetPaletteFade();
    FreeAllSpritePalettes();
    LoadPalette(sWorldMapPalette, BG_PLTT_ID(BG_PAL_ID_WORLD_MAP), sizeof(sWorldMapPalette));
    sWorldMapTilemapPtr = Alloc(0x1000);
    InitData();
    InitWorldMapBg();
    InitWorldMapWindow();
    ResetTempTileDataBuffers();
    DecompressAndCopyTileDataToVram(1, &sWorldMapTiles, 0, 0, 0);
    while (FreeTempTileDataBuffersIfPossible())
        ;

    LZDecompressWram(sWorldMapTilemaps[RogueHub_GetHubVariantNumber() % ARRAY_COUNT(sWorldMapTilemaps)], sWorldMapTilemapPtr);
    LoadBgTiles(0, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, FREE_BLOCK_START);
    LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, BG_PLTT_ID(BG_PAL_ID_WINDOW_FRAMES), PLTT_SIZE_4BPP);
    CopyBgTilemapBufferToVram(1);
    DisplayTitleText();
    DisplayDescText();
    DisplayIcons();
    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    EnableInterrupts(1);
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB2);

    DrawWorldMapTiles();

    CreateTask(Task_WorldMapFadeIn, 0);
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void InitData()
{
    AGB_ASSERT(sWorldMapData == NULL);
    sWorldMapData = AllocZeroed(sizeof(struct WorldMapData));

    sWorldMapData->currentArea = HUB_AREA_ADVENTURE_ENTRANCE;

    sWorldMapData->playerArea = RogueHub_GetAreaFromCurrentMap();
    if(sWorldMapData->playerArea != HUB_AREA_NONE)
    {
        sWorldMapData->currentArea = sWorldMapData->playerArea;
        sWorldMapData->playerX = RogueHub_GetAreaCoords(sWorldMapData->playerArea).x;
        sWorldMapData->playerY = RogueHub_GetAreaCoords(sWorldMapData->playerArea).y;
    }

    sWorldMapData->posX = RogueHub_GetAreaCoords(sWorldMapData->currentArea).x;
    sWorldMapData->posY = RogueHub_GetAreaCoords(sWorldMapData->currentArea).y;

    sWorldMapData->centreX = RogueHub_GetAreaCoords(HUB_AREA_ADVENTURE_ENTRANCE).x;
    sWorldMapData->centreY = RogueHub_GetAreaCoords(HUB_AREA_ADVENTURE_ENTRANCE).y;
    sWorldMapData->mode = sPendingWorldMapMode;
    sWorldMapData->movingArea = HUB_AREA_NONE;
    sWorldMapData->areaMoved = FALSE;
    sPendingWorldMapMode = WORLD_MAP_MODE_VIEW;

}

static bool8 CanTeleport()
{
    if(sWorldMapData->mode != WORLD_MAP_MODE_VIEW)
        return FALSE;

    if(FlagGet(FLAG_ROGUE_UNLOCKED_MAP_TELEPORT))
    {
        if(!Rogue_IsRunActive())
        {
            if(sWorldMapData->currentArea != sWorldMapData->playerArea)
                return TRUE;
        }
    }

    return FALSE;
}

static void Task_WorldMapFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if(sWorldMapData->mode == WORLD_MAP_MODE_VIEW)
            gTasks[taskId].func = Task_WorldMapWaitForKeyPress;
        else
            gTasks[taskId].func = Task_WorldMapWaitForMoveKeyPress;
    }
}

static void Task_WorldMapWaitForKeyPress(u8 taskId)
{
    u8 desiredArea = sWorldMapData->currentArea;

    if(CanTeleport() && JOY_NEW(A_BUTTON))
    {
        //PlaySE(SE_WARP_OUT);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_WorldMapWarpOut;
        return;
    }

    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_WorldMapFadeOut;
        return;
    }

    if(JOY_NEW(DPAD_LEFT))
    {
        desiredArea = RogueHub_FindAreaAtCoord(sWorldMapData->posX - 1, sWorldMapData->posY);
    }
    if(JOY_NEW(DPAD_RIGHT))
    {
        desiredArea = RogueHub_FindAreaAtCoord(sWorldMapData->posX + 1, sWorldMapData->posY);
    }
    if(JOY_NEW(DPAD_UP))
    {
        desiredArea = RogueHub_FindAreaAtCoord(sWorldMapData->posX, sWorldMapData->posY + 1);
    }
    if(JOY_NEW(DPAD_DOWN))
    {
        desiredArea = RogueHub_FindAreaAtCoord(sWorldMapData->posX, sWorldMapData->posY - 1);
    }

    if(desiredArea != HUB_AREA_NONE && sWorldMapData->currentArea != desiredArea)
    {
        PlaySE(SE_SELECT);

        sWorldMapData->currentArea = desiredArea;
        sWorldMapData->posX = RogueHub_GetAreaCoords(desiredArea).x;
        sWorldMapData->posY = RogueHub_GetAreaCoords(desiredArea).y;

        DrawWorldMapTiles();
        DisplayDescText();
        DisplayIcons();
    }
}

static void Task_WorldMapWaitForMoveKeyPress(u8 taskId)
{
    u8 desiredArea = sWorldMapData->currentArea;
    bool8 redraw = FALSE;

    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);

        if(sWorldMapData->mode == WORLD_MAP_MODE_MOVE_PLACE)
        {
            struct Coords8 coords;
            sWorldMapData->mode = WORLD_MAP_MODE_MOVE_SELECT;
            sWorldMapData->currentArea = sWorldMapData->movingArea;
            coords = RogueHub_GetAreaCoords(sWorldMapData->currentArea);
            sWorldMapData->posX = coords.x;
            sWorldMapData->posY = coords.y;
            redraw = TRUE;
        }
        else
        {
            if(sWorldMapData->areaMoved)
                VarSet(VAR_RESULT, 2);

            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_WorldMapFadeOut;
            return;
        }
    }

    if(sWorldMapData->mode == WORLD_MAP_MODE_MOVE_SELECT)
    {
        if(JOY_NEW(A_BUTTON))
        {
            if(RogueHub_CanMoveArea(sWorldMapData->currentArea))
            {
                PlaySE(SE_SELECT);
                sWorldMapData->mode = WORLD_MAP_MODE_MOVE_PLACE;
                sWorldMapData->movingArea = sWorldMapData->currentArea;
                redraw = TRUE;
            }
            else
            {
                PlaySE(SE_FAILURE);
            }
        }

        if(JOY_NEW(DPAD_LEFT))
            desiredArea = RogueHub_FindAreaAtCoord(sWorldMapData->posX - 1, sWorldMapData->posY);
        if(JOY_NEW(DPAD_RIGHT))
            desiredArea = RogueHub_FindAreaAtCoord(sWorldMapData->posX + 1, sWorldMapData->posY);
        if(JOY_NEW(DPAD_UP))
            desiredArea = RogueHub_FindAreaAtCoord(sWorldMapData->posX, sWorldMapData->posY + 1);
        if(JOY_NEW(DPAD_DOWN))
            desiredArea = RogueHub_FindAreaAtCoord(sWorldMapData->posX, sWorldMapData->posY - 1);

        if(desiredArea != HUB_AREA_NONE && sWorldMapData->currentArea != desiredArea)
        {
            PlaySE(SE_SELECT);
            sWorldMapData->currentArea = desiredArea;
            sWorldMapData->posX = RogueHub_GetAreaCoords(desiredArea).x;
            sWorldMapData->posY = RogueHub_GetAreaCoords(desiredArea).y;
            redraw = TRUE;
        }
    }
    else if(sWorldMapData->mode == WORLD_MAP_MODE_MOVE_PLACE)
    {
        if(JOY_NEW(DPAD_LEFT))
        {
            if(IsCoordOnWorldMap(sWorldMapData->posX - 1, sWorldMapData->posY))
            {
                --sWorldMapData->posX;
                redraw = TRUE;
            }
        }
        if(JOY_NEW(DPAD_RIGHT))
        {
            if(IsCoordOnWorldMap(sWorldMapData->posX + 1, sWorldMapData->posY))
            {
                ++sWorldMapData->posX;
                redraw = TRUE;
            }
        }
        if(JOY_NEW(DPAD_UP))
        {
            if(IsCoordOnWorldMap(sWorldMapData->posX, sWorldMapData->posY + 1))
            {
                ++sWorldMapData->posY;
                redraw = TRUE;
            }
        }
        if(JOY_NEW(DPAD_DOWN))
        {
            if(IsCoordOnWorldMap(sWorldMapData->posX, sWorldMapData->posY - 1))
            {
                --sWorldMapData->posY;
                redraw = TRUE;
            }
        }

        if(JOY_NEW(A_BUTTON))
        {
            if(RogueHub_CanMoveAreaToCoord(sWorldMapData->movingArea, sWorldMapData->posX, sWorldMapData->posY))
            {
                PlaySE(SE_SELECT);
                RogueHub_MoveAreaToCoord(sWorldMapData->movingArea, sWorldMapData->posX, sWorldMapData->posY);
                sWorldMapData->areaMoved = TRUE;
                sWorldMapData->mode = WORLD_MAP_MODE_MOVE_SELECT;
                sWorldMapData->currentArea = sWorldMapData->movingArea;
                sWorldMapData->movingArea = HUB_AREA_NONE;
                redraw = TRUE;
            }
            else
            {
                PlaySE(SE_FAILURE);
            }
        }
    }

    if(redraw)
    {
        DrawWorldMapTiles();
        DisplayDescText();
        DisplayIcons();
    }
}

static void Task_WorldMapFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        Free(sWorldMapTilemapPtr);
        Free(sWorldMapData);
        sWorldMapTilemapPtr = NULL;
        sWorldMapData = NULL;
        FreeAllWindowBuffers();
        ResetSpriteData();
        DestroyTask(taskId);
        RogueToD_SetTempDisableTimeVisuals(FALSE);
        SetMainCallback2(gMain.savedCallback);
    }
}

static void Task_WorldMapWarpOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        struct WarpData warp;
        u8 currentArea = sWorldMapData->currentArea;

        Free(sWorldMapTilemapPtr);
        Free(sWorldMapData);
        sWorldMapTilemapPtr = NULL;
        sWorldMapData = NULL;
        FreeAllWindowBuffers();
        ResetSpriteData();
        DestroyTask(taskId);
        RogueToD_SetTempDisableTimeVisuals(FALSE);
        //SetMainCallback2(CB2_ReturnToFieldFadeFromBlack);

        // Fill warp
        warp.mapGroup = gRogueHubAreas[currentArea].primaryMapGroup;
        warp.mapNum = gRogueHubAreas[currentArea].primaryMapNum;
        warp.warpId = gRogueHubAreas[currentArea].connectionWarps[HUB_AREA_CONN_TELEPORT][0];
        warp.x = -1;
        warp.y = -1;

        SetWarpDestination(warp.mapGroup, warp.mapNum, warp.warpId, warp.x, warp.y);
        DoTeleportTileWarp();
        ResetInitialPlayerAvatarState();
    }
}

static u8 GetDisplayWindowOffsetX()
{
    if(sWorldMapData->posX < 0)// && sWorldMapData->posY < -1)
        return 128;
    else
        return 0;
}

static u8 GetDisplayWindowOffsetY()
{
    if(sWorldMapData->posY >= 0)// && sWorldMapData->posY < -1)
        return 96;
    else
        return 0;
}

static bool8 TryGetWorldMapTileCoords(s16 x, s16 y, u8 *tileX, u8 *tileY)
{
    s16 const xOffset = 15;
    s16 const yOffset = 8;
    s16 tx = xOffset + x * 2;
    s16 ty = yOffset - y * 2;

    if(tx < 0 || tx >= 32 || ty < 0 || ty >= 32)
        return FALSE;

    *tileX = tx;
    *tileY = ty;
    return TRUE;
}

static bool8 IsCoordOnWorldMap(s16 x, s16 y)
{
    u8 tileX, tileY;
    return RogueHub_IsCoordInTownGrid(x, y) && TryGetWorldMapTileCoords(x, y, &tileX, &tileY);
}

static void DrawMarkerAtWorldMapCoord(s16 x, s16 y, u8 tile)
{
    u8 tileX, tileY;

    if(TryGetWorldMapTileCoords(x, y, &tileX, &tileY))
        FillBgTilemapBufferRect_Palette0(1, tile, tileX, tileY, 1, 1);
}

static void DrawWorldMapTiles()
{
    u8 i;

    LZDecompressWram(sWorldMapTilemaps[RogueHub_GetHubVariantNumber() % ARRAY_COUNT(sWorldMapTilemaps)], sWorldMapTilemapPtr);

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(RogueHub_HasAreaBuilt(i))
        {
            struct Coords8 coords = RogueHub_GetAreaCoords(i);

            if(i == sWorldMapData->playerArea)
                DrawMarkerAtWorldMapCoord(coords.x, coords.y, TILE_YELLOW_MARKER);
            else
                DrawMarkerAtWorldMapCoord(coords.x, coords.y, TILE_RED_MARKER);

            if(RogueHub_FindAreaInDir(i, HUB_AREA_CONN_SOUTH) != HUB_AREA_NONE)
            {
                u8 tileX, tileY;
                if(TryGetWorldMapTileCoords(coords.x, coords.y, &tileX, &tileY) && tileY + 1 < 32)
                    FillBgTilemapBufferRect_Palette0(1, TILE_PATH, tileX, tileY + 1, 1, 1);
            }

            if(RogueHub_FindAreaInDir(i, HUB_AREA_CONN_EAST) != HUB_AREA_NONE)
            {
                u8 tileX, tileY;
                if(TryGetWorldMapTileCoords(coords.x, coords.y, &tileX, &tileY) && tileX + 1 < 32)
                    FillBgTilemapBufferRect_Palette0(1, TILE_PATH, tileX + 1, tileY, 1, 1);
            }

            if(i == HUB_AREA_ADVENTURE_ENTRANCE)
            {
                u8 tileX, tileY;
                // Place indicator that this is where the adventure entrance is above
                if(TryGetWorldMapTileCoords(coords.x, coords.y, &tileX, &tileY))
                {
                    if(tileY >= 1)
                        FillBgTilemapBufferRect_Palette0(1, TILE_PATH_GREEN_FADE, tileX, tileY - 1, 1, 1);
                    if(tileY >= 2)
                        FillBgTilemapBufferRect_Palette0(1, TILE_GREEN_MARKER, tileX, tileY - 2, 1, 1);
                }
            }
        }
    }

    if(sWorldMapData->mode == WORLD_MAP_MODE_MOVE_PLACE)
    {
        u8 marker = RogueHub_CanMoveAreaToCoord(sWorldMapData->movingArea, sWorldMapData->posX, sWorldMapData->posY)
            ? TILE_GREEN_MARKER
            : TILE_RED_MARKER;
        DrawMarkerAtWorldMapCoord(sWorldMapData->posX, sWorldMapData->posY, marker);
    }

    // Move to right
    SetGpuReg(REG_OFFSET_BG0HOFS, GetDisplayWindowOffsetX());
    SetGpuReg(REG_OFFSET_BG0VOFS, -GetDisplayWindowOffsetY());

    ScheduleBgCopyTilemapToVram(1);
}

static void DisplayTitleText(void)
{
    PrintWorldMapText(WIN_TITLE, RogueHub_GetHubName(), 1, 0);

    PutWindowTilemap(WIN_TITLE);
    CopyWindowToVram(WIN_TITLE, COPYWIN_FULL);
}

static void DisplayDescText(void)
{
    FillWindowPixelBuffer(WIN_DESC, PIXEL_FILL(TEXT_COLOR_WHITE));

    StringCopyN(gStringVar1, gRogueHubAreas[sWorldMapData->currentArea].areaName, ITEM_NAME_LENGTH);

    if(sWorldMapData->mode == WORLD_MAP_MODE_MOVE_SELECT)
    {
        if(RogueHub_CanMoveArea(sWorldMapData->currentArea))
            StringExpandPlaceholders(gStringVar4, sText_MoveSelectDesc);
        else
            StringExpandPlaceholders(gStringVar4, sText_CannotMoveDesc);
    }
    else if(sWorldMapData->mode == WORLD_MAP_MODE_MOVE_PLACE)
    {
        if(RogueHub_CanMoveAreaToCoord(sWorldMapData->movingArea, sWorldMapData->posX, sWorldMapData->posY))
            StringExpandPlaceholders(gStringVar4, sText_MovePlaceDesc);
        else
            StringExpandPlaceholders(gStringVar4, sText_MoveCannotPlaceDesc);
    }
    else if(CanTeleport())
    {
        StringExpandPlaceholders(gStringVar4, sText_TeleportDesc);
    }
    else
    {
        StringExpandPlaceholders(gStringVar4, sText_BasicDesc);
    }
        
    PrintWorldMapText(WIN_DESC, gStringVar4, 25, 0);

    PutWindowTilemap(WIN_DESC);
    CopyWindowToVram(WIN_DESC, COPYWIN_FULL);

    // Place icon
    //
    //BlitCustomItemIconToWindow(WIN_ICON, 0, 0, NULL, gRogueHubAreas[HUB_AREA_RIDE_TRAINING].iconImage, gRogueHubAreas[HUB_AREA_RIDE_TRAINING].iconPalette);
    //PutWindowTilemap(WIN_ICON);
    //CopyWindowToVram(WIN_ICON, COPYWIN_FULL);
}

static u8 PlaceSprite(u8 area, u16 iconSlot, u8 x, u8 y)
{
    const u32* image = gRogueHubAreas[area].iconImage;
    const u32* palette = gRogueHubAreas[area].iconPalette;

    if(image != NULL && palette != NULL)
    {
        u8 spriteId = AddIconSprite(iconSlot + TAG_ITEM_ICON_BASE, iconSlot + TAG_ITEM_ICON_BASE, image, palette);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].x = x;
            gSprites[spriteId].y = y;
            return spriteId;
        }
    }

    return MAX_SPRITES;
}

static void DisplayIcons()
{
    // Centre coord
    s16 const xOffset = 128;
    s16 const yOffset = 58;
    s16 const xDelta = 16;
    s16 const yDelta = -16;

    //u8 nextArea;
    u16 iconSlot = 0;
    s16 posX = sWorldMapData->posX - sWorldMapData->centreX;
    s16 posY = sWorldMapData->posY - sWorldMapData->centreY;
    s16 playerX = sWorldMapData->playerX - sWorldMapData->centreX;
    s16 playerY = sWorldMapData->playerY - sWorldMapData->centreY;


    ResetSpriteData();
    FreeAllSpritePalettes();

    // Place icon on UI
    PlaceSprite(sWorldMapData->currentArea, iconSlot++, GetDisplayWindowOffsetX() + 24, GetDisplayWindowOffsetY() + 40);

    // Place icon on map
    PlaceSprite(sWorldMapData->currentArea, iconSlot++, xOffset + (posX + 0) * xDelta, yOffset + (posY + 0) * yDelta + 12);

    // Place player icon
    if(sWorldMapData->playerArea != HUB_AREA_NONE)
    {
        CreateObjectGraphicsSprite(OBJ_EVENT_GFX_PLAYER_RIDING, SpriteCallbackDummy, xOffset + (playerX + 0) * xDelta - 4, yOffset + (playerY + 0) * yDelta + 1, 2);
    }

    //nextArea = RogueHub_FindAreaInDir(sWorldMapData->currentArea, HUB_AREA_CONN_NORTH);
    //if(nextArea != HUB_AREA_NONE)
    //    PlaceSprite(nextArea, iconSlot++, xOffset + (posX + 0) * xDelta, yOffset + (posY + 1) * yDelta);
//
    //nextArea = RogueHub_FindAreaInDir(sWorldMapData->currentArea, HUB_AREA_CONN_SOUTH);
    //if(nextArea != HUB_AREA_NONE)
    //    PlaceSprite(nextArea, iconSlot++, xOffset + (posX + 0) * xDelta, yOffset + (posY - 1) * yDelta + 25);
//
    //nextArea = RogueHub_FindAreaInDir(sWorldMapData->currentArea, HUB_AREA_CONN_EAST);
    //if(nextArea != HUB_AREA_NONE)
    //    PlaceSprite(nextArea, iconSlot++, xOffset + (posX + 1) * xDelta + 12, yOffset + (posY + 0) * yDelta + 12);
//
    //nextArea = RogueHub_FindAreaInDir(sWorldMapData->currentArea, HUB_AREA_CONN_WEST);
    //if(nextArea != HUB_AREA_NONE)
    //    PlaceSprite(nextArea, iconSlot++, xOffset + (posX - 1) * xDelta - 12, yOffset + (posY + 0) * yDelta  + 12);
}

static void InitWorldMapBg(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sWorldMapBgTemplates, ARRAY_COUNT(sWorldMapBgTemplates));
    SetBgTilemapBuffer(1, sWorldMapTilemapPtr);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
}

static void InitWorldMapWindow(void)
{
    InitWindows(sWorldMapWinTemplates);
    DeactivateAllTextPrinters();
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(BG_PAL_ID_STD_MENU), PLTT_SIZE_4BPP);
    FillWindowPixelBuffer(WIN_TITLE, PIXEL_FILL(TEXT_COLOR_WHITE));
    FillWindowPixelBuffer(WIN_DESC, PIXEL_FILL(TEXT_COLOR_WHITE));
    PutWindowTilemap(WIN_TITLE);
    PutWindowTilemap(WIN_DESC);
    DrawBgWindowFrames();

    //CopyWindowToVram(WIN_TITLE, 3);
    //CopyWindowToVram(WIN_DESC, 3);
}

static void PrintWorldMapText(u8 windowId, u8 const *text, u8 var1, u8 var2)
{
    u8 color[3] = {0, 2, 3};
    AddTextPrinterParameterized4(windowId, FONT_NORMAL, var1, var2, 0, 0, color, TEXT_SKIP_DRAW, text);
}

#define TILE_TOP_CORNER_L (FREE_BLOCK_START + 0x1A2 - 0x1A2)
#define TILE_TOP_EDGE     (FREE_BLOCK_START + 0x1A3 - 0x1A2)
#define TILE_TOP_CORNER_R (FREE_BLOCK_START + 0x1A4 - 0x1A2)
#define TILE_LEFT_EDGE    (FREE_BLOCK_START + 0x1A5 - 0x1A2)
#define TILE_RIGHT_EDGE   (FREE_BLOCK_START + 0x1A7 - 0x1A2)
#define TILE_BOT_CORNER_L (FREE_BLOCK_START + 0x1A8 - 0x1A2)
#define TILE_BOT_EDGE     (FREE_BLOCK_START + 0x1A9 - 0x1A2)
#define TILE_BOT_CORNER_R (FREE_BLOCK_START + 0x1AA - 0x1A2)

static void DrawBgWindowFrames(void)
{
    u8 i, left, right, top, bottom, bg;
    bool8 leftValid, rightValid, topValid, bottomValid;

    for(i = 0; i < ARRAY_COUNT(sWorldMapWinBorders); ++i)
    {
        bg = sWorldMapWinBorders[i].bg;

        left = sWorldMapWinBorders[i].tilemapLeft - 1;
        right = sWorldMapWinBorders[i].tilemapLeft + sWorldMapWinBorders[i].width;
        top = sWorldMapWinBorders[i].tilemapTop - 1;
        bottom = sWorldMapWinBorders[i].tilemapTop + sWorldMapWinBorders[i].height;

        leftValid = left <= 29 && left <= right;
        rightValid = right <= 29 && right >= left;
        topValid = top <= 19 && top <= bottom;
        bottomValid = bottom <= 19;// && bottom >= bottom;

        //                     bg, tile,              x, y, width, height, palNum
        // Draw title window frame
        if(topValid && leftValid)
            FillBgTilemapBufferRect(bg, TILE_TOP_CORNER_L, left, top, 1, 1, BG_PAL_ID_WINDOW_FRAMES);

        if(topValid && rightValid)
            FillBgTilemapBufferRect(bg, TILE_TOP_CORNER_R, right, top, 1, 1, BG_PAL_ID_WINDOW_FRAMES);
            
        if(bottomValid && leftValid)
            FillBgTilemapBufferRect(bg, TILE_BOT_CORNER_L, left, bottom, 1, 1, BG_PAL_ID_WINDOW_FRAMES);

        if(bottomValid && rightValid)
            FillBgTilemapBufferRect(bg, TILE_BOT_CORNER_R, right, bottom, 1, 1, BG_PAL_ID_WINDOW_FRAMES);


        if(topValid)
            FillBgTilemapBufferRect(bg, TILE_TOP_EDGE, left + 1, top, (right - left - 1), 1, BG_PAL_ID_WINDOW_FRAMES);

        if(bottomValid)
            FillBgTilemapBufferRect(bg, TILE_BOT_EDGE, left + 1, bottom, (right - left - 1), 1, BG_PAL_ID_WINDOW_FRAMES);

        if(leftValid)
            FillBgTilemapBufferRect(bg, TILE_LEFT_EDGE, left, top + 1, 1, (bottom - top - 1), BG_PAL_ID_WINDOW_FRAMES);

        if(rightValid)
            FillBgTilemapBufferRect(bg, TILE_RIGHT_EDGE, right, top + 1, 1, (bottom - top - 1), BG_PAL_ID_WINDOW_FRAMES);

        //CopyBgTilemapBufferToVram(bg);
    }
}
