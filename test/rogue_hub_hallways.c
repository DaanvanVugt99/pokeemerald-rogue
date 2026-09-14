#include "global.h"
#include "constants/layouts.h"
#include "constants/metatile_behaviors.h"
#include "constants/rogue_hub.h"
#include "overworld.h"
#include "rogue_hub.h"
#include "test/test.h"
#include "event_data.h"
#include "load_save.h"
#include "rogue_controller.h"
#include "constants/flags.h"
#include "constants/berry.h"

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
        {MAP_NUM(ROGUE_AREA_MARTS),0,18,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),1,18,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),9,18,1,MB_NORTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),2,34,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),3,34,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),10,34,15,MB_EAST_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),4,18,29,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),5,18,29,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_MARTS),11,18,29,MB_SOUTH_ARROW_WARP},
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
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),4,14,23,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),5,14,23,MB_SOUTH_ARROW_WARP},
        {MAP_NUM(ROGUE_AREA_FARMING_FIELD),11,14,23,MB_SOUTH_ARROW_WARP},
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
        {MAP_NUM(ROGUE_AREA_HOME),0},{MAP_NUM(ROGUE_AREA_MARTS),8},
    };
    const struct MapHeader *safari = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_SAFARI_ZONE), MAP_NUM(ROGUE_AREA_SAFARI_ZONE));
    u8 i;
    s16 x, y;
    for (i = 0; i < ARRAY_COUNT(special); ++i)
    {
        const struct MapHeader *header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_LABS), special[i].mapNum);
        x = 100; y = 100;
        EXPECT(!RogueHub_GetWarpArrivalPosition(header, special[i].warp, &x, &y));
        EXPECT_EQ(x, 100); EXPECT_EQ(y, 100);
    }
    x = 36; y = 16;
    EXPECT(RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, 32); EXPECT_EQ(y, 15);
    x = 1; y = 14;
    EXPECT(RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, 4); EXPECT_EQ(y, 15);
    x = 17; y = 30;
    EXPECT(RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, 18); EXPECT_EQ(y, 27);
    x = 18; y = 18;
    EXPECT(!RogueHub_RecoverHallwayPosition(safari, &x, &y));
    EXPECT_EQ(x, 18); EXPECT_EQ(y, 18);
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
