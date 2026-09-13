#include "global.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "constants/rogue.h"
#include "constants/rogue_hub.h"
#include "constants/map_types.h"
#include "constants/metatile_behaviors.h"
#include "constants/songs.h"
#include "event_data.h"
#include "field_door.h"
#include "fieldmap.h"
#include "load_save.h"
#include "malloc.h"
#include "overworld.h"
#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_save.h"
#include "test/test.h"

TEST("Lab junction: all connections preserve doors, seal unused lanes and redirect extra warps")
{
    const struct MapHeader *header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_LABS), MAP_NUM(ROGUE_AREA_LABS));
    struct MapHeader oldHeader = gMapHeader;
    struct BackupMapLayout oldBackup = gBackupMapLayout;
    struct RogueHubMap *oldHub = Alloc(sizeof(*oldHub));
    u16 *grid = Alloc((28 + MAP_OFFSET_W) * (24 + MAP_OFFSET_H) * sizeof(u16));
    u8 mask, upgrade, x, y;
    static const u8 addedLaneMasks[] = { 8, 2, 1, 4 };
    static const u8 originalLaneWarps[] = { 6, 2, 0, 4 };
    EXPECT(oldHub != NULL && grid != NULL);
    if (oldHub == NULL || grid == NULL)
    {
        Free(oldHub); Free(grid); return;
    }
    *oldHub = gRogueSaveBlock->hubMap;
    gMapHeader = *header;
    gBackupMapLayout.width = 28 + MAP_OFFSET_W;
    gBackupMapLayout.height = 24 + MAP_OFFSET_H;
    gBackupMapLayout.map = grid;
    for (mask = 0; mask < 16; ++mask)
    {
        memset(gRogueSaveBlock->hubMap.areaBuiltFlags, 0, sizeof(gRogueSaveBlock->hubMap.areaBuiltFlags));
        RogueHub_BuildArea(HUB_AREA_LABS, 0, 0);
        if (mask & 1) RogueHub_BuildArea(HUB_AREA_TOWN_SQUARE, 0, 1);
        if (mask & 2) RogueHub_BuildArea(HUB_AREA_ADVENTURE_ENTRANCE, 1, 0);
        if (mask & 4) RogueHub_BuildArea(HUB_AREA_MARTS, 0, -1);
        if (mask & 8) RogueHub_BuildArea(HUB_AREA_DAY_CARE, -1, 0);
        for (upgrade = 0; upgrade < 2; ++upgrade)
        {
            RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_LAB, upgrade);
            for (y = 0; y < 24; ++y)
                for (x = 0; x < 28; ++x)
                    grid[(y + MAP_OFFSET) * gBackupMapLayout.width + x + MAP_OFFSET] = header->mapLayout->map[y * 28 + x];
            RogueHub_ApplyMapMetatiles();
            for (x = 13; x <= 15; ++x)
            {
                EXPECT_EQ(!!MapGridIsImpassableAt(x + MAP_OFFSET, 1 + MAP_OFFSET), !(mask & 1));
                EXPECT_EQ(MapGridGetMetatileBehaviorAt(x + MAP_OFFSET, 1 + MAP_OFFSET), mask & 1 ? MB_NORTH_ARROW_WARP : MB_NORMAL);
                EXPECT_EQ(!!MapGridIsImpassableAt(x + MAP_OFFSET, 20 + MAP_OFFSET), !(mask & 4));
                EXPECT_EQ(MapGridGetMetatileBehaviorAt(x + MAP_OFFSET, 20 + MAP_OFFSET), mask & 4 ? MB_SOUTH_ARROW_WARP : MB_NORMAL);
            }
            for (y = 8; y <= 10; ++y)
            {
                EXPECT_EQ(!!MapGridIsImpassableAt(2 + MAP_OFFSET, y + MAP_OFFSET), !(mask & 8));
                EXPECT_EQ(MapGridGetMetatileBehaviorAt(2 + MAP_OFFSET, y + MAP_OFFSET), mask & 8 ? MB_WEST_ARROW_WARP : MB_NORMAL);
                EXPECT_EQ(!!MapGridIsImpassableAt(26 + MAP_OFFSET, y + MAP_OFFSET), !(mask & 2));
                EXPECT_EQ(MapGridGetMetatileBehaviorAt(26 + MAP_OFFSET, y + MAP_OFFSET), mask & 2 ? MB_EAST_ARROW_WARP : MB_NORMAL);
            }
            for (x = 9; x <= 19; x += 10)
            {
                EXPECT(!MapGridIsImpassableAt(x + MAP_OFFSET, 4 + MAP_OFFSET));
                EXPECT_EQ(MapGridGetMetatileBehaviorAt(x + MAP_OFFSET, 4 + MAP_OFFSET), MB_ANIMATED_DOOR);
                EXPECT_EQ(GetDoorSoundEffect(x + MAP_OFFSET, 4 + MAP_OFFSET), SE_SLIDING_DOOR);
                EXPECT(MapGridIsImpassableAt(x - 1 + MAP_OFFSET, 4 + MAP_OFFSET));
            }
            EXPECT_EQ(!!MapGridIsImpassableAt(8 + MAP_OFFSET, 13 + MAP_OFFSET), !upgrade);
            EXPECT_EQ(MapGridGetMetatileBehaviorAt(8 + MAP_OFFSET, 13 + MAP_OFFSET), upgrade ? MB_ANIMATED_DOOR : MB_NORMAL);
#ifdef ROGUE_DEBUG
            EXPECT_EQ(MapGridGetMetatileBehaviorAt(20 + MAP_OFFSET, 13 + MAP_OFFSET), MB_ANIMATED_DOOR);
#else
            EXPECT(MapGridIsImpassableAt(20 + MAP_OFFSET, 13 + MAP_OFFSET));
            EXPECT_EQ(MapGridGetMetatileBehaviorAt(20 + MAP_OFFSET, 13 + MAP_OFFSET), MB_NORMAL);
#endif
            for (x = 0; x < ARRAY_COUNT(addedLaneMasks); ++x)
                if (mask & addedLaneMasks[x])
                {
                    struct WarpEvent added = header->events->warps[15 + x];
                    struct WarpEvent normal = header->events->warps[originalLaneWarps[x]];
                    RogueHub_ModifyMapWarpEvent(&gMapHeader, 15 + x, &added);
                    RogueHub_ModifyMapWarpEvent(&gMapHeader, originalLaneWarps[x], &normal);
                    EXPECT_EQ(added.mapGroup, normal.mapGroup);
                    EXPECT_EQ(added.mapNum, normal.mapNum);
                    EXPECT_EQ(added.warpId, normal.warpId);
                }
        }
    }
    EXPECT_EQ(header->mapType, MAP_TYPE_INDOOR);
    gRogueSaveBlock->hubMap = *oldHub;
    gMapHeader = oldHeader;
    gBackupMapLayout = oldBackup;
    Free(grid); Free(oldHub);
}

TEST("Lab junction: stale saves recover safely without replaying introduction progress")
{
    struct MapHeader header = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_LABS), MAP_NUM(ROGUE_AREA_LABS));
    struct ObjectEventTemplate objects[OBJECT_EVENT_TEMPLATES_COUNT];
    struct WarpData oldWarp = gSaveBlock1Ptr->continueGameWarp;
    s16 oldX = gSaveBlock1Ptr->pos.x, oldY = gSaveBlock1Ptr->pos.y;
    u16 oldIntro = VarGet(VAR_ROGUE_INTRO_STATE);
    bool8 oldStatus = UseContinueGameWarp(), oldRun = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 count, intro;
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gSaveBlock1Ptr->pos.x = 14;
    gSaveBlock1Ptr->pos.y = 11;
    for (intro = ROGUE_INTRO_STATE_SPAWN; intro <= ROGUE_INTRO_STATE_COMPLETE; ++intro)
    {
        count = header.events->objectEventCount;
        memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
        objects[0].x = 5; // The old outdoor workbench.
        VarSet(VAR_ROGUE_INTRO_STATE, intro);
        ClearContinueGameWarpStatus();
        Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
        EXPECT(UseContinueGameWarp());
        EXPECT_EQ(VarGet(VAR_ROGUE_INTRO_STATE), intro);
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.mapGroup, MAP_GROUP(ROGUE_AREA_LABS));
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.mapNum, MAP_NUM(ROGUE_AREA_LABS));
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.x, intro == ROGUE_INTRO_STATE_SPAWN || intro == ROGUE_INTRO_STATE_LEAVE_LAB ? 9 : 14);
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.y, intro == ROGUE_INTRO_STATE_SPAWN ? 7 : intro == ROGUE_INTRO_STATE_LEAVE_LAB ? 5 : 11);
    }
    count = header.events->objectEventCount;
    memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
    objects[1].x = 0; // Birch's completed scripted movement is not a stale layout.
    ClearContinueGameWarpStatus();
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(!UseContinueGameWarp());
    gSaveBlock1Ptr->pos.x = 0;
    gSaveBlock1Ptr->pos.y = 0; // A newly solid position requires safe recovery.
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(UseContinueGameWarp());
    gSaveBlock1Ptr->pos.x = oldX;
    gSaveBlock1Ptr->pos.y = oldY;
    VarSet(VAR_ROGUE_INTRO_STATE, oldIntro);
    gSaveBlock1Ptr->continueGameWarp = oldWarp;
    if (oldStatus) SetContinueGameWarpStatus(); else ClearContinueGameWarpStatus();
    if (oldRun) FlagSet(FLAG_ROGUE_RUN_ACTIVE); else FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}
