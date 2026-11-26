#include "global.h"
#include "constants/event_object_movement.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "constants/rogue.h"
#include "constants/rogue_hub.h"
#include "constants/rogue_quests.h"
#include "constants/map_types.h"
#include "constants/main_hall_tiles.h"
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
#include "rogue_quest.h"
#include "rogue_save.h"
#include "test/test.h"

TEST("Hub music: primary maps play their assigned themes and preserve the Safari intro cue")
{
    struct
    {
        struct WarpData warp;
        u16 song;
    } hubMaps[] =
    {
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_TOWN_SQUARE), .mapNum = MAP_NUM(ROGUE_AREA_TOWN_SQUARE) }, MUS_DP_ROUTE228_DAY },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_LABS), .mapNum = MAP_NUM(ROGUE_AREA_LABS) }, MUS_DP_ROUTE228_DAY },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_ADVENTURE_ENTRANCE), .mapNum = MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE) }, MUS_DP_ROUTE228_DAY },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_SAFARI_ZONE), .mapNum = MAP_NUM(ROGUE_AREA_SAFARI_ZONE) }, MUS_DP_ROUTE228_DAY },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_SAFARI_ZONE_TUTORIAL), .mapNum = MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL) }, MUS_DP_ROUTE228_DAY },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_TRIAL_FRONTIER), .mapNum = MAP_NUM(ROGUE_AREA_TRIAL_FRONTIER) }, MUS_DP_FIGHT_AREA_DAY },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_MARKET), .mapNum = MAP_NUM(ROGUE_AREA_MARKET) }, MUS_SLATEPORT },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_MARTS), .mapNum = MAP_NUM(ROGUE_AREA_MARTS) }, MUS_SLATEPORT },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_RIDE_TRAINING), .mapNum = MAP_NUM(ROGUE_AREA_RIDE_TRAINING) }, MUS_DP_AMITY_SQUARE },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_FARMING_FIELD), .mapNum = MAP_NUM(ROGUE_AREA_FARMING_FIELD) }, MUS_DP_AMITY_SQUARE },
        { { .mapGroup = MAP_GROUP(ROGUE_AREA_DAY_CARE), .mapNum = MAP_NUM(ROGUE_AREA_DAY_CARE) }, MUS_DP_AMITY_SQUARE },
    };
    u16 previousIntroState = VarGet(VAR_ROGUE_INTRO_STATE);
    bool8 previousRunState = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 i;

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    for (i = 0; i < ARRAY_COUNT(hubMaps); i++)
    {
        VarSet(VAR_ROGUE_INTRO_STATE, ROGUE_INTRO_STATE_COMPLETE);
        EXPECT_EQ(GetLocationMusic(&hubMaps[i].warp), hubMaps[i].song);
        if (hubMaps[i].song == MUS_DP_ROUTE228_DAY)
        {
            VarSet(VAR_ROGUE_INTRO_STATE, ROGUE_INTRO_STATE_LEAVE_LAB);
            EXPECT_EQ(GetLocationMusic(&hubMaps[i].warp), MUS_HELP);
            VarSet(VAR_ROGUE_INTRO_STATE, ROGUE_INTRO_STATE_CATCH_MON);
            EXPECT_EQ(GetLocationMusic(&hubMaps[i].warp), MUS_HELP);
        }
    }

    VarSet(VAR_ROGUE_INTRO_STATE, previousIntroState);
    if (previousRunState)
        FlagSet(FLAG_ROGUE_RUN_ACTIVE);
}

TEST("Main Hall: all connections, closed School, gallery stairs and hallway arrivals")
{
    const struct MapHeader *header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_TOWN_SQUARE), MAP_NUM(ROGUE_AREA_TOWN_SQUARE));
    struct MapHeader oldHeader = gMapHeader;
    struct BackupMapLayout oldBackup = gBackupMapLayout;
    struct RogueHubMap *oldHub = Alloc(sizeof(*oldHub));
    u16 *grid = Alloc((36 + MAP_OFFSET_W) * (30 + MAP_OFFSET_H) * sizeof(u16));
    u8 mask, school, tutors, x, y, i, intro;
    u16 oldIntro = VarGet(VAR_ROGUE_INTRO_STATE);
    bool8 oldAdventureComplete = RogueQuest_GetStateFlag(QUEST_ID_TO_ADVENTUREEMARK, QUEST_STATE_HAS_COMPLETE);
    bool8 oldAdventurePending = RogueQuest_GetStateFlag(QUEST_ID_TO_ADVENTUREEMARK, QUEST_STATE_PENDING_REWARDS);
    static const u8 bits[] = {1, 2, 4, 8};
    EXPECT(oldHub != NULL && grid != NULL);
    if (oldHub == NULL || grid == NULL) { Free(oldHub); Free(grid); return; }
    *oldHub = gRogueSaveBlock->hubMap;
    gMapHeader = *header;
    gBackupMapLayout.width = 36 + MAP_OFFSET_W;
    gBackupMapLayout.height = 30 + MAP_OFFSET_H;
    gBackupMapLayout.map = grid;
    VarSet(VAR_ROGUE_INTRO_STATE, ROGUE_INTRO_STATE_REPORT_TO_PROF);
    RogueQuest_SetStateFlag(QUEST_ID_TO_ADVENTUREEMARK, QUEST_STATE_HAS_COMPLETE, FALSE);
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
            for (x = 16; x <= 20; x += 4)
            {
                u16 expected = (mask & 4)
                    ? header->mapLayout->map[24 * 36 + x] & MAPGRID_METATILE_ID_MASK
                    : METATILE_MainHall_Floor;
                EXPECT_EQ(MapGridGetMetatileIdAt(x + MAP_OFFSET, 24 + MAP_OFFSET), expected);
                EXPECT(!MapGridIsImpassableAt(x + MAP_OFFSET, 24 + MAP_OFFSET));
            }
            for (y = 17; y <= 18; ++y)
            {
                EXPECT_EQ(MapGridGetMetatileIdAt(4 + MAP_OFFSET, y + MAP_OFFSET),
                    mask & 8 ? METATILE_MainHall_Floor : METATILE_MainHall_FloorShadow);
                EXPECT(!MapGridIsImpassableAt(4 + MAP_OFFSET, y + MAP_OFFSET));
            }
            for (i = 0; i < header->events->warpCount; ++i)
            {
                const struct WarpEvent *warp = &header->events->warps[i];
                s16 cx = -1, cy = -1;
                u8 dir, behavior;
                if (warp->mapGroup == MAP_GROUP(ROGUE_INTERIOR_SCHOOL)
                    && (warp->mapNum == MAP_NUM(ROGUE_INTERIOR_SCHOOL)
                     || warp->mapNum == MAP_NUM(ROGUE_INTERIOR_POKE_CONNECT)
                     || warp->mapNum == MAP_NUM(ROGUE_INTERIOR_DRESSING_ROOM)))
                {
                    bool8 closed = warp->mapNum != MAP_NUM(ROGUE_INTERIOR_SCHOOL) || !school;
                    EXPECT_EQ(RogueHub_GetWarpArrivalPosition(header, i, &cx, &cy), closed);
                    EXPECT_EQ(cx, closed ? warp->x : -1);
                    EXPECT_EQ(cy, closed ? warp->y + 1 : -1);
                    EXPECT(!MapGridIsImpassableAt(warp->x + MAP_OFFSET, warp->y + 1 + MAP_OFFSET));
                    EXPECT_EQ(MapGridGetMetatileBehaviorAt(warp->x + MAP_OFFSET, warp->y + 1 + MAP_OFFSET), MB_NORMAL);
                    continue;
                }
                if (i == 10
                    || warp->mapGroup != MAP_GROUP(ROGUE_AREA_TOWN_SQUARE)
                    || warp->mapNum != MAP_NUM(ROGUE_AREA_TOWN_SQUARE))
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
            EXPECT_EQ(MapGridGetMetatileIdAt(9 + MAP_OFFSET, 4 + MAP_OFFSET),
                school ? METATILE_MainHall_Door : METATILE_MainHall_Shutter);
            EXPECT_EQ(MapGridGetMetatileIdAt(8 + MAP_OFFSET, 3 + MAP_OFFSET),
                school ? METATILE_MainHall_FrameTopLeft : METATILE_MainHall_ShutterTopLeft);
            if (!school)
                for (y = 3; y <= 4; ++y)
                    for (x = 8; x <= 10; ++x)
                        EXPECT_EQ(MapGridGetMetatileLayerTypeAt(x + MAP_OFFSET, y + MAP_OFFSET),
                            METATILE_LAYER_TYPE_COVERED);
            EXPECT_EQ(FieldAnimateDoorOpen(9 + MAP_OFFSET, 4 + MAP_OFFSET), -1);
            EXPECT_EQ(MapGridGetMetatileBehaviorAt(9 + MAP_OFFSET, 4 + MAP_OFFSET), school ? MB_NON_ANIMATED_DOOR : MB_NORMAL);
            EXPECT_EQ(MapGridGetElevationAt(9 + MAP_OFFSET, 4 + MAP_OFFSET), 5);
            EXPECT(MapGridIsImpassableAt(23 + MAP_OFFSET, 4 + MAP_OFFSET));
            EXPECT_EQ(MapGridGetMetatileIdAt(23 + MAP_OFFSET, 4 + MAP_OFFSET), METATILE_MainHall_Shutter);
            EXPECT_EQ(MapGridGetMetatileBehaviorAt(23 + MAP_OFFSET, 4 + MAP_OFFSET), MB_NORMAL);
            EXPECT_EQ(FieldAnimateDoorOpen(23 + MAP_OFFSET, 4 + MAP_OFFSET), -1);
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
    for (intro = ROGUE_INTRO_STATE_SPAWN; intro <= ROGUE_INTRO_STATE_COMPLETE; ++intro)
    {
        VarSet(VAR_ROGUE_INTRO_STATE, intro);
        RogueHub_ApplyMapMetatiles();
        EXPECT_EQ(MapGridGetMetatileIdAt(27 + MAP_OFFSET, 4 + MAP_OFFSET), METATILE_MainHall_Shutter);
        EXPECT(MapGridIsImpassableAt(27 + MAP_OFFSET, 4 + MAP_OFFSET));
        EXPECT_EQ(MapGridGetMetatileBehaviorAt(27 + MAP_OFFSET, 4 + MAP_OFFSET), MB_NORMAL);
        EXPECT_EQ(MapGridGetMetatileIdAt(28 + MAP_OFFSET, 4 + MAP_OFFSET), METATILE_MainHall_ShutterRight);
        EXPECT_EQ(FieldAnimateDoorOpen(27 + MAP_OFFSET, 4 + MAP_OFFSET), -1);
    }
    RogueQuest_SetStateFlag(QUEST_ID_TO_ADVENTUREEMARK, QUEST_STATE_HAS_COMPLETE, TRUE);
    RogueQuest_SetStateFlag(QUEST_ID_TO_ADVENTUREEMARK, QUEST_STATE_PENDING_REWARDS, TRUE);
    RogueHub_ApplyMapMetatiles();
    EXPECT(!MapGridIsImpassableAt(23 + MAP_OFFSET, 4 + MAP_OFFSET));
    EXPECT_EQ(MapGridGetMetatileIdAt(23 + MAP_OFFSET, 4 + MAP_OFFSET), METATILE_MainHall_Door);
    EXPECT_EQ(MapGridGetMetatileBehaviorAt(23 + MAP_OFFSET, 4 + MAP_OFFSET), MB_NON_ANIMATED_DOOR);
    EXPECT_EQ(MapGridGetMetatileIdAt(22 + MAP_OFFSET, 3 + MAP_OFFSET), METATILE_MainHall_FrameTopLeft);
    RogueQuest_SetStateFlag(QUEST_ID_TO_ADVENTUREEMARK, QUEST_STATE_HAS_COMPLETE, oldAdventureComplete);
    RogueQuest_SetStateFlag(QUEST_ID_TO_ADVENTUREEMARK, QUEST_STATE_PENDING_REWARDS, oldAdventurePending);
    VarSet(VAR_ROGUE_INTRO_STATE, oldIntro);
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
