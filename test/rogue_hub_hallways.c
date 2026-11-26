#include "global.h"
#include "constants/layouts.h"
#include "constants/metatile_behaviors.h"
#include "constants/rogue_hub.h"
#include "overworld.h"
#include "rogue_hub.h"
#include "test/test.h"
#include "event_data.h"
#include "load_save.h"
#include "fieldmap.h"
#include "field_control_avatar.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "palette.h"
#include "script.h"
#include "task.h"
#include "malloc.h"
#include "rogue_save.h"
#include "rogue_controller.h"
#include "constants/flags.h"
#include "constants/berry.h"
#include "constants/vars.h"
#include "constants/event_objects.h"
#include "pokemon.h"

TEST("Hub hallways: Berry Lab refreshes legacy objects and invalid positions")
{
    struct MapHeader header = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_FARMING_FIELD), MAP_NUM(ROGUE_AREA_FARMING_FIELD));
    struct ObjectEventTemplate objects[OBJECT_EVENT_TEMPLATES_COUNT];
    struct WarpData oldWarp = gSaveBlock1Ptr->continueGameWarp;
    s16 oldX = gSaveBlock1Ptr->pos.x, oldY = gSaveBlock1Ptr->pos.y;
    bool8 oldStatus = UseContinueGameWarp(), oldRun = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    bool8 oldUpgrade = RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD1);
    struct BerryTree oldTree = gSaveBlock1Ptr->berryTrees[BERRY_TREE_HUB_11];
    u8 count = header.events->objectEventCount;

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    RogueHub_SetUpgrade(HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD1, TRUE);
    gSaveBlock1Ptr->berryTrees[BERRY_TREE_HUB_11].berry = 1;
    gSaveBlock1Ptr->berryTrees[BERRY_TREE_HUB_11].stage = BERRY_STAGE_BERRIES;
    gSaveBlock1Ptr->berryTrees[BERRY_TREE_HUB_11].berryYield = 7;
    gSaveBlock1Ptr->pos.x = 14;
    gSaveBlock1Ptr->pos.y = 18;
    memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
    objects[21].x = 11; // Old merchant location, same object count.
    objects[21].y = 2;
    ClearContinueGameWarpStatus();
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(UseContinueGameWarp());
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.mapNum, MAP_NUM(ROGUE_AREA_FARMING_FIELD));
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.x, 14);
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.y, 18);
    EXPECT_EQ(gSaveBlock1Ptr->berryTrees[BERRY_TREE_HUB_11].berry, 1);
    EXPECT_EQ((u8)gSaveBlock1Ptr->berryTrees[BERRY_TREE_HUB_11].stage, BERRY_STAGE_BERRIES);
    EXPECT_EQ(gSaveBlock1Ptr->berryTrees[BERRY_TREE_HUB_11].berryYield, 7);
    EXPECT(RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD1));

    count = header.events->objectEventCount;
    memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
    memset(&objects[count], 0, sizeof(*objects));
    objects[count++].localId = 50; // Followers must not force a relocation.
    ClearContinueGameWarpStatus();
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(!UseContinueGameWarp());
    gSaveBlock1Ptr->pos.x = 37; // Within the old outdoor map, beyond the new room.
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(UseContinueGameWarp());
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.x, 14);
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.y, 18);
    ClearContinueGameWarpStatus();
    gSaveBlock1Ptr->pos.x = 7;
    gSaveBlock1Ptr->pos.y = 6; // New glass back panel.
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(UseContinueGameWarp());

    gSaveBlock1Ptr->continueGameWarp = oldWarp;
    gSaveBlock1Ptr->pos.x = oldX;
    gSaveBlock1Ptr->pos.y = oldY;
    gSaveBlock1Ptr->berryTrees[BERRY_TREE_HUB_11] = oldTree;
    RogueHub_SetUpgrade(HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD1, oldUpgrade);
    if (oldStatus) SetContinueGameWarpStatus(); else ClearContinueGameWarpStatus();
    if (oldRun) FlagSet(FLAG_ROGUE_RUN_ACTIVE); else FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}

TEST("Hub hallways: Supply Depot recovers legacy staff without changing upgrades or money")
{
    struct MapHeader header = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_MARTS), MAP_NUM(ROGUE_AREA_MARTS));
    struct ObjectEventTemplate objects[OBJECT_EVENT_TEMPLATES_COUNT];
    struct WarpData oldWarp = gSaveBlock1Ptr->continueGameWarp;
    s16 oldX = gSaveBlock1Ptr->pos.x, oldY = gSaveBlock1Ptr->pos.y;
    bool8 oldStatus = UseContinueGameWarp(), oldRun = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    bool8 oldUpgrade = RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_TMS);
    u32 oldMoney = gSaveBlock1Ptr->money;
    u8 count = header.events->objectEventCount;
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    RogueHub_SetUpgrade(HUB_UPGRADE_MARTS_TMS, TRUE);
    gSaveBlock1Ptr->pos.x = 18;
    gSaveBlock1Ptr->pos.y = 23;
    memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
    objects[0].x = 15;
    objects[0].y = 11;
    ClearContinueGameWarpStatus();
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(UseContinueGameWarp());
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.mapNum, MAP_NUM(ROGUE_AREA_MARTS));
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.x, 18);
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.y, 23);
    EXPECT(RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_TMS));
    EXPECT_EQ(gSaveBlock1Ptr->money, oldMoney);
    count = header.events->objectEventCount;
    memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
    ClearContinueGameWarpStatus();
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(!UseContinueGameWarp());
    gSaveBlock1Ptr->pos.x = 4;
    gSaveBlock1Ptr->pos.y = 3;
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(UseContinueGameWarp());
    gSaveBlock1Ptr->continueGameWarp = oldWarp;
    gSaveBlock1Ptr->pos.x = oldX;
    gSaveBlock1Ptr->pos.y = oldY;
    RogueHub_SetUpgrade(HUB_UPGRADE_MARTS_TMS, oldUpgrade);
    if (oldStatus) SetContinueGameWarpStatus(); else ClearContinueGameWarpStatus();
    if (oldRun) FlagSet(FLAG_ROGUE_RUN_ACTIVE); else FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}

TEST("Hub hallways: every lane arrives centered at the same shaded threshold")
{
    static const struct {u8 mapNum, warp, x, y, behavior;} lanes[] = {
        {MAP_NUM(ROGUE_AREA_DAY_CARE),0,18,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),1,18,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),9,18,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),2,34,14,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),3,34,14,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),10,34,14,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),4,18,27,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),5,18,27,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),11,18,27,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),6,2,14,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),7,2,14,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_DAY_CARE),12,2,14,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),0,18,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),1,18,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),9,18,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),2,34,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),3,34,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),10,34,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),4,18,28,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),5,18,28,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),11,18,28,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),6,2,15,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),7,2,15,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),12,2,15,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),0,0,11,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),1,0,11,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),7,0,11,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),4,18,11,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),5,18,11,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),8,18,11,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),2,9,15,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),3,9,15,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),9,9,15,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),0,14,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),1,14,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),17,14,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),2,26,9,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),3,26,9,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),16,26,9,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),4,14,20,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),5,14,20,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),18,14,20,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),6,2,9,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),7,2,9,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_LABS),15,2,9,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),0,4,15,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),1,4,15,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),8,4,15,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),4,32,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),5,32,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),9,32,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),2,18,27,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),3,18,27,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),10,18,27,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),0,4,15,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),1,4,15,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),6,4,15,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),4,32,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),5,32,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),7,32,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),2,18,27,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),3,18,27,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),8,18,27,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),0,14,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),1,14,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),9,14,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),2,26,11,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),3,26,11,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),10,26,11,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),4,14,26,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),5,14,26,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),11,14,26,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),6,2,11,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),7,2,11,MB_WEST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),12,2,11,MB_WEST_ARROW_WARP},
    };
    u32 i;
    for (i = 0; i < ARRAY_COUNT(lanes); ++i)
    {
        const struct MapHeader *header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_LABS), lanes[i].mapNum);
        const struct WarpEvent *warp = &header->events->warps[lanes[i].warp];
        u16 tile = header->mapLayout->map[warp->y * header->mapLayout->width + warp->x] & MAPGRID_METATILE_ID_MASK;
        s16 x = -1, y = -1;
        EXPECT(RogueHub_GetWarpArrivalPosition(header, lanes[i].warp, &x, &y));
        EXPECT_EQ(x, lanes[i].x); EXPECT_EQ(y, lanes[i].y);
        EXPECT_EQ(header->mapLayout->primaryTileset->metatileAttributes[tile] & 0xFF, lanes[i].behavior);
        // One inward step must leave the warp strip on a traversable tile.
        if (lanes[i].behavior == MB_NORTH_ARROW_WARP) ++y;
        else if (lanes[i].behavior == MB_SOUTH_ARROW_WARP) --y;
        else if (lanes[i].behavior == MB_WEST_ARROW_WARP) ++x;
        else --x;
        tile = header->mapLayout->map[y * header->mapLayout->width + x];
        EXPECT(!(tile & MAPGRID_COLLISION_MASK));
        EXPECT_EQ(header->mapLayout->primaryTileset->metatileAttributes[tile & MAPGRID_METATILE_ID_MASK] & 0xFF, MB_NORMAL);
    }
}

TEST("Hub hallways: keep special arrivals and outdoor districts unchanged, recover old corridor saves")
{
    static const struct {u8 mapNum, warp;} special[] = {
        {MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE),6},
        {MAP_NUM(ROGUE_AREA_LABS),8},{MAP_NUM(ROGUE_AREA_LABS),9},{MAP_NUM(ROGUE_AREA_LABS),10},
        {MAP_NUM(ROGUE_AREA_LABS),11},{MAP_NUM(ROGUE_AREA_LABS),12},{MAP_NUM(ROGUE_AREA_LABS),13},{MAP_NUM(ROGUE_AREA_LABS),14},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE),6},{MAP_NUM(ROGUE_AREA_SAFARI_ZONE),7},
        {MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL),9},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),8},
        {MAP_NUM(ROGUE_AREA_TOWN_SQUARE),15},{MAP_NUM(ROGUE_AREA_MARTS),8},{MAP_NUM(ROGUE_AREA_DAY_CARE),8},
    };
    const struct MapHeader *safari = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_SAFARI_ZONE), MAP_NUM(ROGUE_AREA_SAFARI_ZONE));
    struct RogueHubMap *oldHub = Alloc(sizeof(*oldHub));
    u8 i;
    s16 x, y;
    EXPECT(oldHub != NULL);
    *oldHub = gRogueSaveBlock->hubMap;
    memset(gRogueSaveBlock->hubMap.areaBuiltFlags, 0, sizeof(gRogueSaveBlock->hubMap.areaBuiltFlags));
    RogueHub_BuildArea(HUB_AREA_SAFARI_ZONE, 0, 0);
    RogueHub_BuildArea(HUB_AREA_LABS, 1, 0);
    RogueHub_BuildArea(HUB_AREA_TOWN_SQUARE, -1, 0);
    RogueHub_BuildArea(HUB_AREA_ADVENTURE_ENTRANCE, 0, -1);
    for (i = 0; i < ARRAY_COUNT(special); ++i)
    {
        const struct MapHeader *header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_LABS), special[i].mapNum);
        x = 100; y = 100;
        EXPECT(!RogueHub_GetWarpArrivalPosition(header, special[i].warp, &x, &y));
        EXPECT_EQ(x, 100); EXPECT_EQ(y, 100);
    }
    x = 36; y = 16;
    EXPECT(RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, 31); EXPECT_EQ(y, 15);
    x = 1; y = 14;
    EXPECT(RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, 5); EXPECT_EQ(y, 15);
    x = 17; y = 30;
    EXPECT(RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, 18); EXPECT_EQ(y, 26);
    x = 18; y = 18;
    EXPECT(!RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, 18); EXPECT_EQ(y, 18);

    memset(gRogueSaveBlock->hubMap.areaBuiltFlags, 0, sizeof(gRogueSaveBlock->hubMap.areaBuiltFlags));
    RogueHub_BuildArea(HUB_AREA_SAFARI_ZONE, 0, 0);
    x = 36; y = 16;
    EXPECT(RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, safari->events->warps[7].x);
    EXPECT_EQ(y, safari->events->warps[7].y);
    x = 32; y = 15;
    EXPECT(RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, safari->events->warps[7].x);
    EXPECT_EQ(y, safari->events->warps[7].y);
    gRogueSaveBlock->hubMap = *oldHub;
    Free(oldHub);
}

static void CheckNorthHallwayTurn(void)
{
    struct FieldInput input = {.heldDirection = TRUE, .dpadDirection = DIR_NORTH};
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct ObjectEvent oldPlayer = *player;
    struct PaletteFadeControl oldFade = gPaletteFade;
    void (*oldFieldCallback)(void) = gFieldCallback;
    bool8 wasLocked = ArePlayerFieldControlsLocked();
    u8 task;

    player->currentCoords.x = 18 + MAP_OFFSET;
    player->currentCoords.y = 1 + MAP_OFFSET;
    player->previousElevation = MapGridGetElevationAt(player->currentCoords.x, player->currentCoords.y);
    player->facingDirection = DIR_EAST;
    EXPECT(ProcessPlayerFieldInput(&input));
    EXPECT_EQ(GetDestinationWarpMapHeader()->mapLayoutId, LAYOUT_ROGUE_AREA_LABS);
    task = FindTaskIdByFunc(Task_WarpAndLoadMap);
    EXPECT_NE(task, TASK_NONE);
    DestroyTask(task);

    *player = oldPlayer;
    gFieldCallback = oldFieldCallback;
    gPaletteFade = oldFade;
    if (!wasLocked)
        UnlockPlayerFieldControls();
}

TEST("Hub hallways: visible facades cannot load and stranded saves return inside")
{
    struct MapHeader oldHeader = gMapHeader;
    struct BackupMapLayout oldLayout = gBackupMapLayout;
    struct RogueHubMap *oldHub = Alloc(sizeof(*oldHub));
    struct WarpData returnWarp;
    u16 layout;
    s16 x, y;
    bool8 oldRun = FlagGet(FLAG_ROGUE_RUN_ACTIVE);

    EXPECT(oldHub != NULL);
    *oldHub = gRogueSaveBlock->hubMap;
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    memset(gRogueSaveBlock->hubMap.areaBuiltFlags, 0, sizeof(gRogueSaveBlock->hubMap.areaBuiltFlags));
    RogueHub_BuildArea(HUB_AREA_TOWN_SQUARE, 0, 0);
    RogueHub_BuildArea(HUB_AREA_LABS, 0, 1);
    RogueHub_BuildArea(HUB_AREA_ADVENTURE_ENTRANCE, 1, 0);

    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_TOWN_SQUARE), MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
    gMapHeader.mapScripts = NULL;
    InitMap();
    // The north and east facades are present in the map grid for rendering.
    EXPECT_NE(MapGridGetMetatileIdAt(18 + MAP_OFFSET, MAP_OFFSET - 1), MAPGRID_UNDEFINED);
    EXPECT_NE(MapGridGetMetatileIdAt(36 + MAP_OFFSET, 17 + MAP_OFFSET), MAPGRID_UNDEFINED);
    // They cannot be traversed as seamless camera connections.
    EXPECT_EQ(GetMapBorderIdAt(18 + MAP_OFFSET, MAP_OFFSET - 1), CONNECTION_INVALID);
    EXPECT_EQ(GetMapBorderIdAt(36 + MAP_OFFSET, 17 + MAP_OFFSET), CONNECTION_INVALID);
    for (x = 17; x <= 19; ++x)
    {
        EXPECT(RogueHub_IsHallwayExit(&gMapHeader, x, 1, DIR_NORTH));
        EXPECT(!RogueHub_IsHallwayExit(&gMapHeader, x, 1, DIR_SOUTH));
    }
    for (y = 16; y <= 18; ++y)
    {
        EXPECT(RogueHub_IsHallwayExit(&gMapHeader, 35, y, DIR_EAST));
        EXPECT(!RogueHub_IsHallwayExit(&gMapHeader, 35, y, DIR_WEST));
    }
    EXPECT(!RogueHub_IsHallwayExit(&gMapHeader, 23, 4, DIR_NORTH)); // Dressing room door
    CheckNorthHallwayTurn();
    for (layout = LAYOUT_ROGUE_PORTAL_CORRIDOR_HORIZONTAL; layout <= LAYOUT_ROGUE_TRIAL_CORRIDOR_WEST; ++layout)
    {
        const struct MapHeader *destination;
        EXPECT(RogueHub_GetHallwayFacadeReturn(layout, &returnWarp));
        destination = Overworld_GetMapHeaderByGroupAndId(returnWarp.mapGroup, returnWarp.mapNum);
        EXPECT(!RogueHub_IsHallwayFacade(destination->mapLayoutId));
        EXPECT(!(destination->mapLayout->map[returnWarp.y * destination->mapLayout->width + returnWarp.x] & MAPGRID_COLLISION_MASK));
    }
    EXPECT(RogueHub_GetHallwayFacadeReturn(LAYOUT_ROGUE_MAIN_HALL_CORRIDOR_NORTH, &returnWarp));
    EXPECT_EQ(returnWarp.mapNum, MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
    EXPECT_EQ(returnWarp.x, 18);
    EXPECT_EQ(returnWarp.y, 19);
    EXPECT(RogueHub_GetHallwayFacadeReturn(LAYOUT_ROGUE_MAIN_HALL_CORRIDOR_HORIZONTAL, &returnWarp));
    EXPECT_EQ(returnWarp.mapNum, MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
    EXPECT(!RogueHub_GetHallwayFacadeReturn(LAYOUT_ROGUE_AREA_TOWN_SQUARE, &returnWarp));

    {
        struct MapHeader facade = *Overworld_GetMapHeaderByGroupAndId(
            MAP_GROUP(ROGUE_MAIN_HALL_CORRIDOR_NORTH), MAP_NUM(ROGUE_MAIN_HALL_CORRIDOR_NORTH));
        struct WarpData oldContinue = gSaveBlock1Ptr->continueGameWarp;
        bool8 oldContinueStatus = UseContinueGameWarp();
        struct ObjectEventTemplate objects[OBJECT_EVENT_TEMPLATES_COUNT];
        u8 count = 0;

        ClearContinueGameWarpStatus();
        Rogue_ModifyObjectEvents(&facade, TRUE, objects, &count, ARRAY_COUNT(objects));
        EXPECT(UseContinueGameWarp());
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.mapNum, MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.x, 18);
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.y, 19);
        gSaveBlock1Ptr->continueGameWarp = oldContinue;
        if (!oldContinueStatus)
            ClearContinueGameWarpStatus();
    }

    gMapHeader = oldHeader;
    gBackupMapLayout = oldLayout;
    gRogueSaveBlock->hubMap = *oldHub;
    if (oldRun)
        FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Free(oldHub);
}

TEST("Hub hallways: dressing room uses the Main Hall door in both directions")
{
    const struct MapHeader *hall = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_TOWN_SQUARE), MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
    const struct MapHeader *dressing = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_INTERIOR_DRESSING_ROOM), MAP_NUM(ROGUE_INTERIOR_DRESSING_ROOM));
    const struct WarpEvent *entrance = &hall->events->warps[15];
    const struct WarpEvent *exit = &dressing->events->warps[0];
    s16 x, y;

    EXPECT_EQ(entrance->mapGroup, MAP_GROUP(ROGUE_INTERIOR_DRESSING_ROOM));
    EXPECT_EQ(entrance->mapNum, MAP_NUM(ROGUE_INTERIOR_DRESSING_ROOM));
    EXPECT_EQ(entrance->warpId, 0);
    EXPECT_EQ(exit->mapGroup, MAP_GROUP(ROGUE_AREA_TOWN_SQUARE));
    EXPECT_EQ(exit->mapNum, MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
    EXPECT_EQ(exit->warpId, 15);
    EXPECT(!RogueHub_GetWarpArrivalPosition(hall, 15, &x, &y));
    EXPECT(!RogueHub_GetWarpArrivalPosition(dressing, 0, &x, &y));
    EXPECT_EQ(dressing->events->objectEventCount, 2);
}

TEST("Hub hallways: Park rock scenery remains visible independently of links")
{
    struct MapHeader park = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_RIDE_TRAINING), MAP_NUM(ROGUE_AREA_RIDE_TRAINING));
    u32 i;
    EXPECT_EQ(park.connections->count, 4);
    // Facade visibility must not consult hub/save state. The map metatile
    // updater separately replaces disconnected entrances with solid rock.
    for (i = 0; i < park.connections->count; ++i)
        EXPECT(RogueHub_AcceptMapConnection(&park, &park.connections->connections[i]));
}

TEST("Hub hallways: Nursery recovers old saves and accepts dynamic graphics")
{
    struct MapHeader header = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_DAY_CARE), MAP_NUM(ROGUE_AREA_DAY_CARE));
    struct ObjectEventTemplate objects[OBJECT_EVENT_TEMPLATES_COUNT];
    struct WarpData oldWarp = gSaveBlock1Ptr->continueGameWarp;
    struct BoxPokemon oldMon = *Rogue_GetDaycareBoxMon(0);
    s16 oldX = gSaveBlock1Ptr->pos.x, oldY = gSaveBlock1Ptr->pos.y;
    bool8 oldStatus = UseContinueGameWarp(), oldRun = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    bool8 oldUpgrade = RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_TEA_SHOP);
    bool8 oldCapacity0 = RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_CAPACITY0);
    bool8 oldCapacity1 = RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_CAPACITY1);
    u16 oldEgg = VarGet(VAR_ROGUE_DAYCARE_EGG_SPECIES), oldCycles = VarGet(VAR_ROGUE_DAYCARE_EGG_CYCLES);
    u16 species = SPECIES_BULBASAUR;
    u8 count = header.events->objectEventCount;
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    RogueHub_SetUpgrade(HUB_UPGRADE_DAY_CARE_TEA_SHOP, FALSE);
    SetBoxMonData(Rogue_GetDaycareBoxMon(0), MON_DATA_SPECIES, &species);
    VarSet(VAR_ROGUE_DAYCARE_EGG_SPECIES, SPECIES_PIKACHU);
    VarSet(VAR_ROGUE_DAYCARE_EGG_CYCLES, 7);
    gSaveBlock1Ptr->pos.x = 18;
    gSaveBlock1Ptr->pos.y = 21;
    memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
    objects[2].x = 6; objects[2].y = 11; objects[2].elevation = 0;
    ClearContinueGameWarpStatus();
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(UseContinueGameWarp());
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.mapNum, MAP_NUM(ROGUE_AREA_DAY_CARE));
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.x, 18);
    EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.y, 21);
    EXPECT_EQ(GetBoxMonData(Rogue_GetDaycareBoxMon(0), MON_DATA_SPECIES), species);
    EXPECT_EQ(VarGet(VAR_ROGUE_DAYCARE_EGG_SPECIES), SPECIES_PIKACHU);
    EXPECT_EQ(VarGet(VAR_ROGUE_DAYCARE_EGG_CYCLES), 7);
    EXPECT(!RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_TEA_SHOP));
    count = header.events->objectEventCount;
    memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
    objects[1].graphicsId = OBJ_EVENT_GFX_MISC_YOUNG_COUPLE_F;
    objects[4].graphicsId = OBJ_EVENT_GFX_OLD_WOMAN;
    objects[5].graphicsId = OBJ_EVENT_GFX_OLD_WOMAN;
    objects[6].graphicsId = OBJ_EVENT_GFX_OLD_WOMAN;
    ClearContinueGameWarpStatus();
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(!UseContinueGameWarp());
    // Former inactive-bay floor is now a permanent coffee-corner aisle.
    RogueHub_SetUpgrade(HUB_UPGRADE_DAY_CARE_CAPACITY0, FALSE);
    RogueHub_SetUpgrade(HUB_UPGRADE_DAY_CARE_CAPACITY1, FALSE);
    gSaveBlock1Ptr->pos.x = 11; gSaveBlock1Ptr->pos.y = 8;
    count = header.events->objectEventCount;
    memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
    ClearContinueGameWarpStatus();
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(!UseContinueGameWarp());
    gSaveBlock1Ptr->pos.x = 9; gSaveBlock1Ptr->pos.y = 20;
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(UseContinueGameWarp());
    gSaveBlock1Ptr->continueGameWarp = oldWarp;
    gSaveBlock1Ptr->pos.x = oldX; gSaveBlock1Ptr->pos.y = oldY;
    *Rogue_GetDaycareBoxMon(0) = oldMon;
    VarSet(VAR_ROGUE_DAYCARE_EGG_SPECIES, oldEgg); VarSet(VAR_ROGUE_DAYCARE_EGG_CYCLES, oldCycles);
    RogueHub_SetUpgrade(HUB_UPGRADE_DAY_CARE_TEA_SHOP, oldUpgrade);
    RogueHub_SetUpgrade(HUB_UPGRADE_DAY_CARE_CAPACITY0, oldCapacity0);
    RogueHub_SetUpgrade(HUB_UPGRADE_DAY_CARE_CAPACITY1, oldCapacity1);
    if (oldStatus) SetContinueGameWarpStatus(); else ClearContinueGameWarpStatus();
    if (oldRun) FlagSet(FLAG_ROGUE_RUN_ACTIVE); else FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}
