#include "global.h"

#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/metatile_labels.h"
#include "constants/rogue_route_events.h"
#include "constants/rogue_route_scenes.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "fieldmap.h"
#include "item.h"
#include "money.h"
#include "overworld.h"
#include "random.h"
#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_route_events.h"
#include "rogue_route_scenes.h"
#include "rogue_trainers.h"
#include "test/test.h"

extern const u8 Rogue_RouteEvent_Interact[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseOffer[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseCamp[];
extern const u8 Rogue_RouteEvent_StolenTradeCasePayoff[];
extern const u8 Rogue_RouteEvent_Prop[];

static u32 GetActiveTeamClassFlag(u16 teamNum)
{
    switch(teamNum)
    {
    case TEAM_NUM_KANTO_ROCKET:
    case TEAM_NUM_JOHTO_ROCKET:
        return CLASS_FLAG_TEAM_ROCKET;
    case TEAM_NUM_AQUA:
        return CLASS_FLAG_TEAM_AQUA;
    case TEAM_NUM_MAGMA:
        return CLASS_FLAG_TEAM_MAGMA;
    case TEAM_NUM_GALACTIC:
        return CLASS_FLAG_TEAM_GALACTIC;
    case TEAM_NUM_PLASMA:
        return CLASS_FLAG_TEAM_PLASMA;
    case TEAM_NUM_NEOPLASMA:
        return CLASS_FLAG_TEAM_NEOPLASMA;
    case TEAM_NUM_FLARE:
        return CLASS_FLAG_TEAM_FLARE;
    }

    return CLASS_FLAG_NONE;
}

static void RestoreFlag(u16 flagId, bool8 value)
{
    if(value)
        FlagSet(flagId);
    else
        FlagClear(flagId);
}

static void SetupCurrentEvent(struct RogueAdvPath *originalPath, u8 *originalRoomId)
{
    *originalPath = gRogueAdvPath;
    *originalRoomId = gRogueRun.adventureRoomId;
    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
    gRogueAdvPath.roomCount = 1;
    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_ROUTE;
    gRogueRun.adventureRoomId = 0;
}

TEST("Stolen Trade Case scenes generate deterministic RNG-neutral offers")
{
    struct RogueAdvPathRoom roomA = {0};
    struct RogueAdvPathRoom roomB = {0};
    RAND_TYPE originalRng = gRngRogueValue;
    RAND_TYPE rngBefore;
    u16 originalTeamNum = gRogueRun.teamEncounterNum;
    u16 firstTrainer = TRAINER_NONE;
    bool8 foundDifferentTrainer = FALSE;
    u16 seed;

    gRogueRun.teamEncounterNum = TEAM_NUM_KANTO_ROCKET;
    roomA.roomParams.roomIdx = 0;
    roomA.rngSeed = 24680;
    roomB = roomA;

    SeedRogueRng(13579);
    rngBefore = gRngRogueValue;
    RogueRouteScenes_GenerateRoom(&roomA);
    EXPECT_EQ(memcmp(&gRngRogueValue, &rngBefore, sizeof(rngBefore)), 0);

    SeedRogueRng(31995);
    RogueRouteScenes_GenerateRoom(&roomB);
    EXPECT_EQ(memcmp(&roomA.routeScene, &roomB.routeScene, sizeof(roomA.routeScene)), 0);
    EXPECT_EQ(roomA.routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER);
    EXPECT_EQ((u8)roomA.routeScene.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
    EXPECT_EQ(roomA.routeScene.primaryGraphicsId, OBJ_EVENT_GFX_MART_EMPLOYEE);
    EXPECT_EQ(roomA.routeScene.secondaryGraphicsId, OBJ_EVENT_GFX_MART_EMPLOYEE);
    EXPECT_EQ(roomA.routeScene.requestedItem, ITEM_TRADE_CASE);
    EXPECT_EQ(ItemId_GetPocket(roomA.routeScene.requestedItem), POCKET_KEY_ITEMS);
    EXPECT_EQ(roomA.routeScene.rewardItem, ITEM_BIG_POKEBLOCK_BUNDLE);
    EXPECT_NE(roomA.routeScene.trainerNum, TRAINER_NONE);
    EXPECT((gRogueTrainers[roomA.routeScene.trainerNum].trainerFlags & TRAINER_FLAG_CLASS_TEAM) != 0);
    EXPECT((gRogueTrainers[roomA.routeScene.trainerNum].classFlags & GetActiveTeamClassFlag(gRogueRun.teamEncounterNum)) != 0);
    firstTrainer = roomA.routeScene.trainerNum;

    for(seed = 1; seed < 200 && !foundDifferentTrainer; ++seed)
    {
        roomA.rngSeed = seed;
        RogueRouteScenes_GenerateRoom(&roomA);
        EXPECT_EQ(roomA.routeScene.primaryGraphicsId, OBJ_EVENT_GFX_MART_EMPLOYEE);
        EXPECT_EQ(roomA.routeScene.requestedItem, ITEM_TRADE_CASE);
        EXPECT_EQ(roomA.routeScene.rewardItem, ITEM_BIG_POKEBLOCK_BUNDLE);
        if(roomA.routeScene.trainerNum != firstTrainer)
            foundDifferentTrainer = TRUE;
    }
    EXPECT(foundDifferentTrainer);

    gRogueRun.teamEncounterNum = originalTeamNum;
    gRngRogueValue = originalRng;
}

TEST("Route events reserve two clear anchors on every classified active route")
{
    u8 routeIdx;

    for(routeIdx = 0; routeIdx < gRogueRouteTable.routeCount; ++routeIdx)
    {
        const struct RogueRouteEncounter *route = &gRogueRouteTable.routes[routeIdx];
        const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(route->map.group, route->map.num);
        const struct MapLayout *mapLayout = mapHeader->mapLayout;
        const struct MapEvents *events = mapHeader->events;
        const struct ObjectEventTemplate *markers[ROGUE_ROUTE_EVENT_ANCHOR_COUNT] = {0};
        u8 markerCount = 0;
        u8 objectIdx;

        EXPECT_LT(route->environment, ROGUE_ROUTE_ENVIRONMENT_COUNT);
        EXPECT_LE(events->objectEventCount + 2, OBJECT_EVENT_TEMPLATES_COUNT);

        for(objectIdx = 0; objectIdx < events->objectEventCount; ++objectIdx)
        {
            const struct ObjectEventTemplate *object = &events->objectEvents[objectIdx];

            if(RogueRouteScenes_IsAnchorTemplate(object))
            {
                u16 role = object->trainerRange_berryTreeId;
                EXPECT_LT(role, ROGUE_ROUTE_EVENT_ANCHOR_COUNT);
                EXPECT_EQ(markers[role], NULL);
                markers[role] = object;
                ++markerCount;
            }
        }

        EXPECT_EQ(markerCount, ROGUE_ROUTE_EVENT_ANCHOR_COUNT);
        EXPECT_NE(markers[0]->localId, markers[1]->localId);

        for(objectIdx = 0; objectIdx < ROGUE_ROUTE_EVENT_ANCHOR_COUNT; ++objectIdx)
        {
            const struct ObjectEventTemplate *marker = markers[objectIdx];
            s16 x;
            s16 y;
            u8 otherIdx;
            u8 warpIdx;

            EXPECT_GT(marker->x, 0);
            EXPECT_GT(marker->y, 0);
            EXPECT_LT(marker->x, mapLayout->width - 1);
            EXPECT_LT(marker->y, mapLayout->height - 1);

            for(y = marker->y - 1; y <= marker->y + 1; ++y)
            {
                for(x = marker->x - 1; x <= marker->x + 1; ++x)
                {
                    u16 block = mapLayout->map[y * mapLayout->width + x];
                    EXPECT_EQ(block & MAPGRID_COLLISION_MASK, 0);
                    EXPECT_EQ((block & MAPGRID_ELEVATION_MASK) >> MAPGRID_ELEVATION_SHIFT, marker->elevation);
                }
            }

            for(otherIdx = 0; otherIdx < events->objectEventCount; ++otherIdx)
            {
                const struct ObjectEventTemplate *other = &events->objectEvents[otherIdx];
                if(other != marker)
                    EXPECT(abs(other->x - marker->x) > 1 || abs(other->y - marker->y) > 1);
            }

            for(warpIdx = 0; warpIdx < events->warpCount; ++warpIdx)
            {
                const struct WarpEvent *warp = &events->warps[warpIdx];
                EXPECT(abs(warp->x - marker->x) > 1 || abs(warp->y - marker->y) > 1);
            }
        }
    }
}

TEST("Stolen Trade Case scene recipes compose bounded unique route objects")
{
    struct RogueAdvPath originalPath;
    u8 originalRoomId;
    u8 recipeId;

    SetupCurrentEvent(&originalPath, &originalRoomId);

    for(recipeId = ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER; recipeId < ROGUE_ROUTE_SCENE_RECIPE_COUNT; ++recipeId)
    {
        const struct ObjectEventTemplate baseObjects[] =
        {
            {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 12, .y = 34, .elevation = 3, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
            {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 56, .y = 78, .elevation = 3, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
        };
        struct ObjectEventTemplate objects[8] =
        {
            {.localId = 1, .graphicsId = OBJ_EVENT_GFX_BOY_1, .x = 2, .y = 2},
            baseObjects[0],
            baseObjects[1],
        };
        const u8 *expectedScript = recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER ? Rogue_RouteEvent_StolenTradeCaseOffer
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP ? Rogue_RouteEvent_StolenTradeCaseCamp
            : Rogue_RouteEvent_StolenTradeCasePayoff;
        u8 count = 3;
        u8 expectedCount = recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP ? 5 : 4;
        u8 i;
        u8 j;
        u8 npcCount = 0;
        u8 propCount = 0;

        memset(&gRogueAdvPath.rooms[0].routeScene, 0, sizeof(gRogueAdvPath.rooms[0].routeScene));
        gRogueAdvPath.rooms[0].routeScene.recipeId = recipeId;
        gRogueAdvPath.rooms[0].routeScene.environment = ROGUE_ROUTE_ENVIRONMENT_MOUNTAIN;
        gRogueAdvPath.rooms[0].routeScene.anchor = ROGUE_ROUTE_EVENT_ANCHOR_RECIPIENT;
        gRogueAdvPath.rooms[0].routeScene.primaryGraphicsId = recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
            ? OBJ_EVENT_GFX_ROCKET_M
            : OBJ_EVENT_GFX_MART_EMPLOYEE;

        RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
        EXPECT_EQ(count, expectedCount);

        for(i = 0; i < count; ++i)
        {
            EXPECT(!RogueRouteScenes_IsAnchorTemplate(&objects[i]));
            for(j = i + 1; j < count; ++j)
                EXPECT_NE(objects[i].localId, objects[j].localId);

            if(objects[i].script == expectedScript)
            {
                ++npcCount;
                EXPECT_EQ(objects[i].localId, 42);
                EXPECT_EQ(objects[i].x, 56);
                EXPECT_EQ(objects[i].y, 78);
                EXPECT_EQ(objects[i].graphicsId, gRogueAdvPath.rooms[0].routeScene.primaryGraphicsId);
            }
            else if(objects[i].script == Rogue_RouteEvent_Prop)
            {
                ++propCount;
                EXPECT_GE(objects[i].x, 55);
                EXPECT_LE(objects[i].x, 57);
                EXPECT_GE(objects[i].y, 78);
                EXPECT_LE(objects[i].y, 79);
            }
        }

        EXPECT_EQ(npcCount, 1);
        EXPECT_EQ(propCount, recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP ? 3 : 2);

        // Save loading refreshes authored scripts by local ID. This used to
        // turn the NPC back into an inert anchor and could give props unrelated
        // scripts, including when a prop reused the other anchor's ID.
        for(i = 0; i < count; ++i)
        {
            if(objects[i].script == expectedScript || objects[i].script == Rogue_RouteEvent_Prop)
                objects[i].script = Rogue_RouteEvent_Interact;
        }

        RogueRouteScenes_RestoreObjectEvents(objects, count, baseObjects, ARRAY_COUNT(baseObjects));
        npcCount = 0;
        propCount = 0;
        for(i = 0; i < count; ++i)
        {
            if(objects[i].script == expectedScript)
                ++npcCount;
            else if(objects[i].script == Rogue_RouteEvent_Prop)
                ++propCount;
        }
        EXPECT_EQ(npcCount, 1);
        EXPECT_EQ(propCount, recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP ? 3 : 2);
    }

    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Stolen Trade Case camp metatiles stay bounded compatible and idempotent")
{
    struct RogueAdvPath originalPath;
    struct BackupMapLayout originalBackup = gBackupMapLayout;
    struct MapHeader originalHeader = gMapHeader;
    const struct RogueRouteMap *routeMap = &gRogueRouteTable.routes[0].map;
    const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(routeMap->group, routeMap->num);
    const struct ObjectEventTemplate *anchor = NULL;
    u8 originalRoomId;
    u32 cellCount;
    u32 i;
    u16 x;
    u16 y;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    gRogueAdvPath.rooms[0].routeScene.recipeId = ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP;
    gRogueAdvPath.rooms[0].routeScene.anchor = 0;
    gMapHeader = *mapHeader;
    gBackupMapLayout.map = sBackupMapData;
    gBackupMapLayout.width = mapHeader->mapLayout->width + MAP_OFFSET_W;
    gBackupMapLayout.height = mapHeader->mapLayout->height + MAP_OFFSET_H;
    cellCount = gBackupMapLayout.width * gBackupMapLayout.height;
    for(i = 0; i < cellCount; ++i)
        sBackupMapData[i] = METATILE_General_Grass | (3 << MAPGRID_ELEVATION_SHIFT);

    for(i = 0; i < mapHeader->events->objectEventCount; ++i)
    {
        if(RogueRouteScenes_IsAnchorTemplate(&mapHeader->events->objectEvents[i])
            && mapHeader->events->objectEvents[i].trainerRange_berryTreeId == 0)
            anchor = &mapHeader->events->objectEvents[i];
    }
    EXPECT_NE(anchor, NULL);
    x = anchor->x + MAP_OFFSET;
    y = anchor->y + MAP_OFFSET;

    RogueRouteScenes_ApplyMetatiles();
    EXPECT_EQ(MapGridGetMetatileIdAt(x - 1, y), METATILE_General_Grass_Stone);
    EXPECT_EQ(MapGridGetMetatileIdAt(x + 1, y), METATILE_General_Grass_Stone);
    EXPECT_EQ(MapGridGetMetatileIdAt(x, y + 1), METATILE_General_Grass_Stone);
    EXPECT_EQ(MapGridGetMetatileIdAt(x, y), METATILE_General_Grass);
    EXPECT_EQ(MapGridGetElevationAt(x, y + 1), 3);

    RogueRouteScenes_ApplyMetatiles();
    EXPECT_EQ(MapGridGetMetatileIdAt(x, y + 1), METATILE_General_Grass_Stone);

    MapGridSetMetatileIdAt(x + 1, y, METATILE_General_TallGrass);
    RogueRouteScenes_ApplyMetatiles();
    EXPECT_EQ(MapGridGetMetatileIdAt(x + 1, y), METATILE_General_TallGrass);

    gBackupMapLayout = originalBackup;
    gMapHeader = originalHeader;
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Stolen Trade Case completes its three route-node handoffs")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueRouteSceneRequest offer;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 originalTeamNum = gRogueRun.teamEncounterNum;
    u16 originalDifficulty = Rogue_GetCurrentDifficulty();
    u32 originalMoney = GetMoney(&gSaveBlock1Ptr->money);
    u16 originalTrainers[ROGUE_MAX_ACTIVE_TRAINER_COUNT];
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalComplete = FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    bool8 originalPropA = FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    bool8 originalPropB = FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
    u8 questId;
    u16 itemId;
    u8 i;

    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, 12345);
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    gRogueRun.teamEncounterNum = TEAM_NUM_KANTO_ROCKET;
    Rogue_SetCurrentDifficulty(0);
    FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    gRogueAdvPath.rooms[0].rngSeed = 100;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    offer = gRogueAdvPath.rooms[0].routeScene;
    RogueRouteScenes_OnEnterRoute();

    EXPECT_EQ(offer.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER);
    RogueRouteEvents_TryAcceptStolenTradeCaseQuest();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    questId = RogueAdventureQuests_GetQuestIdAt(0);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->payload[1], offer.trainerNum);
    EXPECT(!CheckBagHasItem(ITEM_TRADE_CASE, 1));

    // Quicksaving on the source route retains the accepted merchant scene.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[0].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_ACTIVE);

    RogueRouteScenes_OnExitRoute();
    gRogueAdvPath.roomCount = 3;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = 1;
    gRogueAdvPath.rooms[1].rngSeed = 101;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP);
    EXPECT_EQ((u8)gRogueAdvPath.rooms[1].routeScene.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE);
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.ownerQuestId, questId);
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.trainerNum, offer.trainerNum);
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.primaryGraphicsId, Rogue_GetTrainerObjectEventGfx(offer.trainerNum));

    for(i = 0; i < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++i)
    {
        originalTrainers[i] = Rogue_GetDynamicTrainer(i);
        Rogue_SetDynamicTrainer(i, i < 2 ? offer.trainerNum : TRAINER_NONE);
    }
    RogueRouteScenes_PrepareRouteTrainers();
    EXPECT_EQ(Rogue_GetDynamicTrainer(0), TRAINER_NONE);
    EXPECT_EQ(Rogue_GetDynamicTrainer(1), TRAINER_NONE);

    RogueRouteEvents_BeginStolenTradeCaseBattle();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA), offer.trainerNum);

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && CheckBagHasSpace(ITEM_TRADE_CASE, 1); ++itemId)
    {
        if(ItemId_GetPocket(itemId) == POCKET_KEY_ITEMS && itemId != ITEM_TRADE_CASE)
            AddBagItem(itemId, 1);
    }
    EXPECT(!CheckBagHasSpace(ITEM_TRADE_CASE, 1));
    RogueRouteEvents_FinishStolenTradeCaseBattle();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_NO_SPACE);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->nodeId, 0);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->progress, 1);

    // Leaving while collection is pending reschedules the camp without
    // forgetting that its battle has already been won.
    RogueRouteScenes_OnExitRoute();
    gRogueAdvPath.roomCount = 6;
    gRogueRun.adventureRoomId = 2;
    gRogueAdvPath.rooms[2].roomParams.roomIdx = 2;
    gRogueAdvPath.rooms[2].rngSeed = 102;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[2]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[2].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_NONE);
    RogueRouteScenes_OnExitRoute();

    gRogueRun.adventureRoomId = 3;
    gRogueAdvPath.rooms[3].roomParams.roomIdx = 3;
    gRogueAdvPath.rooms[3].rngSeed = 103;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[3]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[3].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);

    ClearBag();
    RogueRouteEvents_FinishStolenTradeCaseBattle();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->nodeId, 0);
    EXPECT(CheckBagHasItem(ITEM_TRADE_CASE, 1));
    EXPECT(FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN));

    // A same-route quickload restores the completed camp rather than binding
    // the next quest node to this room.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[3]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[3].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->nodeId, 1);
    gRogueRun.adventureRoomId = 4;
    gRogueAdvPath.rooms[4].roomParams.roomIdx = 4;
    gRogueAdvPath.rooms[4].rngSeed = 104;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[4]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[4].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF);
    EXPECT_EQ(gRogueAdvPath.rooms[4].routeScene.requestedItem, ITEM_TRADE_CASE);
    EXPECT_EQ(gRogueAdvPath.rooms[4].routeScene.rewardItem, ITEM_BIG_POKEBLOCK_BUNDLE);
    EXPECT_EQ(gRogueAdvPath.rooms[4].routeScene.primaryGraphicsId, OBJ_EVENT_GFX_MART_EMPLOYEE);
    EXPECT(RemoveBagItem(ITEM_TRADE_CASE, 1));
    RogueRouteEvents_TryClaimStolenTradeCaseReward();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 12345);
    EXPECT(!CheckBagHasItem(ITEM_BIG_POKEBLOCK_BUNDLE, 1));
    EXPECT(AddBagItem(ITEM_TRADE_CASE, 1));

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && CheckBagHasSpace(ITEM_BIG_POKEBLOCK_BUNDLE, 1); ++itemId)
    {
        u8 pocket = ItemId_GetPocket(itemId);
        if(pocket == POCKET_KEY_ITEMS && itemId != ITEM_TRADE_CASE && itemId != ITEM_BIG_POKEBLOCK_BUNDLE)
            AddBagItem(itemId, 1);
    }
    EXPECT(!CheckBagHasSpace(ITEM_BIG_POKEBLOCK_BUNDLE, 1));
    RogueRouteEvents_TryClaimStolenTradeCaseReward();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(!CheckBagHasItem(ITEM_TRADE_CASE, 1));
    EXPECT(CheckBagHasItem(ITEM_BIG_POKEBLOCK_BUNDLE, 1));
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 12345 + ROGUE_STOLEN_TRADE_CASE_REWARD_MONEY);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->nodeId, 1);
    EXPECT(FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED));
    EXPECT(FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN));

    // The completed payoff also survives a same-route quickload so its
    // acknowledgement remains available until the player leaves.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[4]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[4].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);
    gRogueRun.adventureRoomId = 5;
    gRogueAdvPath.rooms[5].roomParams.roomIdx = 5;
    gRogueAdvPath.rooms[5].rngSeed = 105;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[5]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[5].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_NONE);

    RogueAdventureQuests_Clear();
    EXPECT(!FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED));

    {
        struct RogueAdventureQuestCreateParams params = {0};

        ClearBag();
        EXPECT_NE(RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params), ROGUE_ADVENTURE_QUEST_INVALID_ID);
        EXPECT(AddBagItem(ITEM_TRADE_CASE, 1));
        RogueAdventureQuests_Clear();
        EXPECT(!CheckBagHasItem(ITEM_TRADE_CASE, 1));
    }

    for(i = 0; i < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++i)
        Rogue_SetDynamicTrainer(i, originalTrainers[i]);
    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.teamEncounterNum = originalTeamNum;
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    Rogue_SetCurrentDifficulty(originalDifficulty);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    RestoreFlag(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED, originalComplete);
    RestoreFlag(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN, originalPropA);
    RestoreFlag(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN, originalPropB);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    SetMoney(&gSaveBlock1Ptr->money, originalMoney);
    ClearBag();
}

TEST("Adventure quest registry reschedules skipped Stolen Trade Case nodes fairly")
{
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueAdventureQuestCreateParams params =
    {
        .payload = {0, 1},
    };
    struct RogueRouteSceneRequest request = {0};
    u16 priority;
    u8 originalRoomId = gRogueRun.adventureRoomId;
    u8 i;

    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));

    for(i = 0; i < 20; ++i)
    {
        gRogueRun.adventureRoomId = i;
        EXPECT_EQ(RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params), i);
    }
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 20);

    EXPECT(RogueAdventureQuests_TryCollectSceneRequest(30, &request, &priority));
    EXPECT_EQ(request.ownerQuestId, 0);
    EXPECT_EQ(request.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP);
    EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE);
    RogueAdventureQuests_LeaveRoute(30);
    EXPECT_EQ(RogueAdventureQuests_GetState(0), ROGUE_ADVENTURE_QUEST_STATE_ACTIVE);

    EXPECT(RogueAdventureQuests_TryCollectSceneRequest(31, &request, &priority));
    EXPECT_EQ(request.ownerQuestId, 1);
    EXPECT_EQ(RogueAdventureQuests_GetState(1), ROGUE_ADVENTURE_QUEST_STATE_READY);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 20);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.adventureRoomId = originalRoomId;
}
