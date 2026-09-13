#include "global.h"
#include "constants/event_object_movement.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "constants/rogue.h"
#include "constants/rogue_hub.h"
#include "constants/map_types.h"
#include "constants/metatile_behaviors.h"
#include "constants/songs.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_door.h"
#include "fieldmap.h"
#include "load_save.h"
#include "malloc.h"
#include "overworld.h"
#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_save.h"
#include "test/test.h"

TEST("Main Hall: all connections, closed School, gallery stairs and hallway arrivals")
{
    const struct MapHeader *header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_TOWN_SQUARE), MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
    struct MapHeader oldHeader = gMapHeader;
    struct BackupMapLayout oldBackup = gBackupMapLayout;
    struct RogueHubMap *oldHub = Alloc(sizeof(*oldHub));
    u16 *grid = Alloc((36 + MAP_OFFSET_W) * (30 + MAP_OFFSET_H) * sizeof(u16));
    u8 mask, school, tutors, x, y, i;
    static const u8 bits[] = {1, 2, 4, 8};
    EXPECT(oldHub != NULL && grid != NULL);
    if (oldHub == NULL || grid == NULL) { Free(oldHub); Free(grid); return; }
    *oldHub = gRogueSaveBlock->hubMap;
    gMapHeader = *header;
    gBackupMapLayout.width = 36 + MAP_OFFSET_W;
    gBackupMapLayout.height = 30 + MAP_OFFSET_H;
    gBackupMapLayout.map = grid;
    for (mask = 0; mask < 16; ++mask)
    {
        memset(gRogueSaveBlock->hubMap.areaBuiltFlags, 0, sizeof(gRogueSaveBlock->hubMap.areaBuiltFlags));
        RogueHub_BuildArea(HUB_AREA_TOWN_SQUARE, 0, 0);
        if (mask & 1) RogueHub_BuildArea(HUB_AREA_LABS, 0, 1);
        if (mask & 2) RogueHub_BuildArea(HUB_AREA_ADVENTURE_ENTRANCE, 1, 0);
        if (mask & 4) RogueHub_BuildArea(HUB_AREA_MARTS, 0, -1);
        if (mask & 8) RogueHub_BuildArea(HUB_AREA_DAY_CARE, -1, 0);
        for (school = 0; school < 2; ++school)
        for (tutors = 0; tutors < 2; ++tutors)
        {
            RogueHub_SetUpgrade(HUB_UPGRADE_TOWN_SQUARE_SCHOOL, school);
            RogueHub_SetUpgrade(HUB_UPGRADE_TOWN_SQUARE_TUTORS, tutors);
            for (y = 0; y < 30; ++y)
                for (x = 0; x < 36; ++x)
                    grid[(y + MAP_OFFSET) * gBackupMapLayout.width + x + MAP_OFFSET] = header->mapLayout->map[y * 36 + x];
            RogueHub_ApplyMapMetatiles();
            for (i = 0; i < header->events->warpCount; ++i)
            {
                const struct WarpEvent *warp = &header->events->warps[i];
                s16 cx = -1, cy = -1;
                u8 dir, behavior;
                if (i >= 8 && i <= 10)
                {
                    EXPECT(!RogueHub_GetWarpArrivalPosition(header, i, &cx, &cy));
                    EXPECT_EQ(cx, -1); EXPECT_EQ(cy, -1);
                    continue;
                }
                dir = i < 8 ? i / 2 : i - 11;
                behavior = (u8[]){MB_NORTH_ARROW_WARP, MB_EAST_ARROW_WARP, MB_SOUTH_ARROW_WARP, MB_WEST_ARROW_WARP}[dir];
                EXPECT(RogueHub_GetWarpArrivalPosition(header, i, &cx, &cy));
                EXPECT_EQ(cx, ((u8[]){18,35,18,1}[dir]));
                EXPECT_EQ(cy, ((u8[]){1,17,28,17}[dir]));
                EXPECT_EQ(!!MapGridIsImpassableAt(warp->x + MAP_OFFSET, warp->y + MAP_OFFSET), !(mask & bits[dir]));
                EXPECT_EQ(MapGridGetMetatileBehaviorAt(warp->x + MAP_OFFSET, warp->y + MAP_OFFSET), mask & bits[dir] ? behavior : MB_NORMAL);
                if (mask & bits[dir])
                {
                    struct WarpEvent added = *warp, normal = header->events->warps[dir * 2];
                    RogueHub_ModifyMapWarpEvent(&gMapHeader, i, &added);
                    RogueHub_ModifyMapWarpEvent(&gMapHeader, dir * 2, &normal);
                    EXPECT_EQ(added.mapGroup, normal.mapGroup); EXPECT_EQ(added.mapNum, normal.mapNum); EXPECT_EQ(added.warpId, normal.warpId);
                }
            }
            EXPECT_EQ(!!MapGridIsImpassableAt(9 + MAP_OFFSET, 4 + MAP_OFFSET), !school);
            EXPECT_EQ(MapGridGetMetatileBehaviorAt(9 + MAP_OFFSET, 4 + MAP_OFFSET), school ? MB_ANIMATED_DOOR : MB_NORMAL);
            EXPECT_EQ(MapGridGetElevationAt(9 + MAP_OFFSET, 4 + MAP_OFFSET), 5);
            EXPECT_EQ(MapGridGetMetatileBehaviorAt(27 + MAP_OFFSET, 4 + MAP_OFFSET), MB_ANIMATED_DOOR);
            EXPECT_EQ(GetDoorSoundEffect(27 + MAP_OFFSET, 4 + MAP_OFFSET), SE_SLIDING_DOOR);
            for (x = 4; x <= 32; ++x)
                for (y = 8; y <= 9; ++y)
                {
                    bool8 stair = (x >= 11 && x <= 13) || (x >= 23 && x <= 25);
                    EXPECT_EQ(!!MapGridIsImpassableAt(x + MAP_OFFSET, y + MAP_OFFSET), !stair);
                    if (stair)
                    {
                        EXPECT_EQ(MapGridGetElevationAt(x + MAP_OFFSET, y + MAP_OFFSET), 0);
                        EXPECT_EQ(MapGridGetElevationAt(x + MAP_OFFSET, 7 + MAP_OFFSET), 5);
                        EXPECT_EQ(MapGridGetElevationAt(x + MAP_OFFSET, 10 + MAP_OFFSET), 3);
                    }
                }
            // Tutor unlocks never replace furniture or remove a circulation lane.
            for (x = 4; x <= 32; ++x)
                EXPECT(!MapGridIsImpassableAt(x + MAP_OFFSET, 17 + MAP_OFFSET));
        }
    }
    EXPECT_EQ(header->mapType, MAP_TYPE_INDOOR);
    gRogueSaveBlock->hubMap = *oldHub; gMapHeader = oldHeader; gBackupMapLayout = oldBackup;
    Free(grid); Free(oldHub);
}

TEST("Main Hall: recover obsolete saves without repeating gifts, retain both floor levels")
{
    struct MapHeader header = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_TOWN_SQUARE), MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
    struct ObjectEventTemplate objects[OBJECT_EVENT_TEMPLATES_COUNT];
    struct WarpData oldWarp = gSaveBlock1Ptr->continueGameWarp;
    s16 oldX = gSaveBlock1Ptr->pos.x, oldY = gSaveBlock1Ptr->pos.y;
    u16 oldIntro = VarGet(VAR_ROGUE_INTRO_STATE);
    bool8 oldStatus = UseContinueGameWarp(), oldRun = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 count, intro, y;
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gSaveBlock1Ptr->pos.x = 18; gSaveBlock1Ptr->pos.y = 19;
    for (intro = ROGUE_INTRO_STATE_SPAWN; intro <= ROGUE_INTRO_STATE_COMPLETE; ++intro)
    {
        count = header.events->objectEventCount;
        memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
        objects[0].x = 24; objects[0].y = 8; // Original outdoor workbench.
        VarSet(VAR_ROGUE_INTRO_STATE, intro);
        ClearContinueGameWarpStatus();
        Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
        EXPECT(UseContinueGameWarp());
        EXPECT_EQ(VarGet(VAR_ROGUE_INTRO_STATE), intro);
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.mapNum, MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.x, intro == ROGUE_INTRO_STATE_LEARN_TO_BUILD ? header.events->objectEvents[0].x : 18);
        EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.y, intro == ROGUE_INTRO_STATE_LEARN_TO_BUILD ? header.events->objectEvents[0].y - 2 : 19);
        EXPECT_EQ(objects[2].elevation, intro < ROGUE_INTRO_STATE_REPORT_TO_PROF ? 5 : 3);
        EXPECT_EQ(objects[2].x, intro < ROGUE_INTRO_STATE_REPORT_TO_PROF ? 27 : header.events->objectEvents[0].x + (intro > ROGUE_INTRO_STATE_LEARN_TO_BUILD));
        EXPECT_EQ(objects[2].y, intro < ROGUE_INTRO_STATE_REPORT_TO_PROF ? 5 : header.events->objectEvents[0].y - 1);
        EXPECT_EQ(objects[2].movementType, intro < ROGUE_INTRO_STATE_REPORT_TO_PROF ? MOVEMENT_TYPE_FACE_UP : intro <= ROGUE_INTRO_STATE_LEARN_TO_BUILD ? MOVEMENT_TYPE_FACE_DOWN : MOVEMENT_TYPE_FACE_LEFT);
    }
    VarSet(VAR_ROGUE_INTRO_STATE, ROGUE_INTRO_STATE_COMPLETE);
    count = header.events->objectEventCount;
    memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
    Rogue_ModifyObjectEvents(&header, FALSE, objects, &count, ARRAY_COUNT(objects));
    for (y = 6; y <= 19; y += 13)
    {
        gSaveBlock1Ptr->pos.x = 18; gSaveBlock1Ptr->pos.y = y;
        ClearContinueGameWarpStatus();
        Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
        EXPECT(!UseContinueGameWarp());
        EXPECT_EQ(gSaveBlock1Ptr->pos.y, y);
    }
    gSaveBlock1Ptr->pos.x = 18; gSaveBlock1Ptr->pos.y = 9; // Solid gallery face.
    Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
    EXPECT(UseContinueGameWarp());
    gSaveBlock1Ptr->pos.x = oldX; gSaveBlock1Ptr->pos.y = oldY;
    VarSet(VAR_ROGUE_INTRO_STATE, oldIntro); gSaveBlock1Ptr->continueGameWarp = oldWarp;
    if (oldStatus) SetContinueGameWarpStatus(); else ClearContinueGameWarpStatus();
    if (oldRun) FlagSet(FLAG_ROGUE_RUN_ACTIVE); else FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}
