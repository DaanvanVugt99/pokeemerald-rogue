#include "global.h"

#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/metatile_labels.h"
#include "constants/pokemon.h"
#include "constants/rogue_route_events.h"
#include "constants/rogue_route_scenes.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "fieldmap.h"
#include "item.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_adventurepaths.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_pokedex.h"
#include "rogue_route_events.h"
#include "rogue_route_scenes.h"
#include "rogue_trainers.h"
#include "test/test.h"

extern const u8 Rogue_RouteEvent_Interact[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseOffer[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseCamp[];
extern const u8 Rogue_RouteEvent_StolenTradeCasePayoff[];
extern const u8 Rogue_RouteEvent_Prop[];
extern const u8 Rogue_RouteEvent_HexedShrine[];
extern const u8 Rogue_RouteEvent_HexedShrineProp[];
extern const u8 Rogue_RouteEvent_AnomalousFossilOffer[];
extern const u8 Rogue_RouteEvent_AnomalousFossilRestoration[];
extern const u8 Rogue_RouteEvent_FossilProp[];

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

TEST("Route event fallback registry is deterministic weighted and RNG neutral")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueRouteSceneRequest firstRequest;
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE originalStandardRng = gRngValue;
    u16 originalTeamNum = gRogueRun.teamEncounterNum;
    u16 originalTempCurse = gRogueRun.temporaryDarkDealCurseItem;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalComplete = FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    bool8 seenCurse[32] = {FALSE};
    u16 merchantCount = 0;
    u16 shrineCount = 0;
    u16 fossilCount = 0;
    u8 curseCount = Rogue_GetDarkDealCurseCount();
    u16 seed;
    u8 i;
    RAND_TYPE rogueRngBefore;
    RAND_TYPE standardRngBefore;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.teamEncounterNum = TEAM_NUM_KANTO_ROCKET;
    gRogueRun.temporaryDarkDealCurseItem = ITEM_NONE;
    FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    EXPECT_EQ(curseCount, 17);

    for(seed = 1; seed <= 1000; ++seed)
    {
        gRogueAdvPath.rooms[0].rngSeed = seed;
        gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
        VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
        SeedRng(0x2468);
        SeedRogueRng(0x1357);
        standardRngBefore = gRngValue;
        rogueRngBefore = gRngRogueValue;
        RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
        RogueRouteScenes_OnEnterRoute();
        EXPECT_EQ(memcmp(&gRngValue, &standardRngBefore, sizeof(standardRngBefore)), 0);
        EXPECT_EQ(memcmp(&gRngRogueValue, &rogueRngBefore, sizeof(rogueRngBefore)), 0);

        firstRequest = gRogueAdvPath.rooms[0].routeScene;
        SeedRng(0xEF01);
        SeedRogueRng(0xDCBA);
        RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
        RogueRouteScenes_OnEnterRoute();
        EXPECT_EQ(memcmp(&firstRequest, &gRogueAdvPath.rooms[0].routeScene, sizeof(firstRequest)), 0);

        if(firstRequest.recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER)
        {
            ++merchantCount;
            EXPECT_EQ((u8)firstRequest.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
            EXPECT_EQ(firstRequest.requestedItem, ITEM_TRADE_CASE);
            EXPECT((gRogueTrainers[firstRequest.trainerNum].classFlags & GetActiveTeamClassFlag(gRogueRun.teamEncounterNum)) != 0);
        }
        else if(firstRequest.recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE)
        {
            EXPECT_EQ((u8)firstRequest.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
            EXPECT_EQ(firstRequest.primaryGraphicsId, OBJ_EVENT_GFX_DEVIL_MAN);
            ++shrineCount;
            for(i = 0; i < curseCount; ++i)
            {
                if(firstRequest.requestedItem == Rogue_SelectDarkDealCurseItem(i))
                    seenCurse[i] = TRUE;
            }
        }
        else
        {
            EXPECT_EQ(firstRequest.recipeId, ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER);
            EXPECT_EQ((u8)firstRequest.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
            EXPECT_EQ(RogueAdventureQuests_GetFossilSpecies(firstRequest.requestedItem), firstRequest.rewardItem);
            ++fossilCount;
        }
    }
    EXPECT_GE(merchantCount, 250);
    EXPECT_LE(merchantCount, 420);
    EXPECT_GE(shrineCount, 250);
    EXPECT_LE(shrineCount, 420);
    EXPECT_GE(fossilCount, 250);
    EXPECT_LE(fossilCount, 420);
    EXPECT_EQ(merchantCount + shrineCount + fossilCount, 1000);
    for(i = 0; i < curseCount; ++i)
        EXPECT(seenCurse[i]);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.teamEncounterNum = originalTeamNum;
    gRogueRun.temporaryDarkDealCurseItem = originalTempCurse;
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    RestoreFlag(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED, originalComplete);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    gRngRogueValue = originalRogueRng;
    gRngValue = originalStandardRng;
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

TEST("Route scene recipes compose bounded unique route objects")
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
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF ? Rogue_RouteEvent_StolenTradeCasePayoff
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE ? Rogue_RouteEvent_HexedShrine
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER ? Rogue_RouteEvent_AnomalousFossilOffer
            : Rogue_RouteEvent_AnomalousFossilRestoration;
        const u8 *expectedPropScript = recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
            ? Rogue_RouteEvent_HexedShrineProp
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
                || recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION ? Rogue_RouteEvent_FossilProp
            : Rogue_RouteEvent_Prop;
        u8 count = 3;
        u8 expectedCount = recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
            || recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
            || recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
            || recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION ? 5 : 4;
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
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE ? OBJ_EVENT_GFX_DEVIL_MAN
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER ? OBJ_EVENT_GFX_SCIENTIST_1
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION ? OBJ_EVENT_GFX_SCIENTIST_2
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
            else if(objects[i].script == expectedPropScript)
            {
                ++propCount;
                EXPECT_GE(objects[i].x, 55);
                EXPECT_LE(objects[i].x, 57);
                EXPECT_GE(objects[i].y, 77);
                EXPECT_LE(objects[i].y, 79);
            }

            if(recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
                || recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
                || recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION)
                EXPECT(objects[i].x != 56 || objects[i].y != 79);
        }

        EXPECT_EQ(npcCount, 1);
        EXPECT_EQ(propCount, recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
            || recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
            || recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
            || recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION ? 3 : 2);

        // Save loading refreshes authored scripts by local ID. This used to
        // turn the NPC back into an inert anchor and could give props unrelated
        // scripts, including when a prop reused the other anchor's ID.
        for(i = 0; i < count; ++i)
        {
            if(objects[i].script == expectedScript || objects[i].script == expectedPropScript)
                objects[i].script = Rogue_RouteEvent_Interact;
        }

        RogueRouteScenes_RestoreObjectEvents(objects, count, baseObjects, ARRAY_COUNT(baseObjects));
        npcCount = 0;
        propCount = 0;
        for(i = 0; i < count; ++i)
        {
            if(objects[i].script == expectedScript)
                ++npcCount;
            else if(objects[i].script == expectedPropScript)
                ++propCount;
        }
        EXPECT_EQ(npcCount, 1);
        EXPECT_EQ(propCount, recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
            || recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
            || recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
            || recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION ? 3 : 2);
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

    for(i = 0; i < cellCount; ++i)
        sBackupMapData[i] = METATILE_General_Grass | (3 << MAPGRID_ELEVATION_SHIFT);
    gRogueAdvPath.rooms[0].routeScene.recipeId = ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE;
    RogueRouteScenes_ApplyMetatiles();
    EXPECT_EQ(MapGridGetMetatileIdAt(x - 1, y), METATILE_General_Grass_Stone);
    EXPECT_EQ(MapGridGetMetatileIdAt(x + 1, y), METATILE_General_Grass_Stone);
    EXPECT_EQ(MapGridGetMetatileIdAt(x, y - 1), METATILE_General_Grass_Stone);
    EXPECT_EQ(MapGridGetMetatileIdAt(x, y + 1), METATILE_General_Grass);

    gBackupMapLayout = originalBackup;
    gMapHeader = originalHeader;
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Hexed Shrine bargain is atomic persistent and route local")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueRouteSceneRequest shrine;
    u32 originalMoney = GetMoney(&gSaveBlock1Ptr->money);
    u16 originalDifficulty = Rogue_GetCurrentDifficulty();
    u16 originalTempCurse = gRogueRun.temporaryDarkDealCurseItem;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalComplete = FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    u16 itemId;
    u16 seed;

    ClearBag();
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    FlagSet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    gRogueRun.temporaryDarkDealCurseItem = ITEM_NONE;
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    Rogue_SetCurrentDifficulty(3);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    for(seed = 1; seed != 0; ++seed)
    {
        gRogueAdvPath.rooms[0].rngSeed = seed;
        RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
        RogueRouteScenes_OnEnterRoute();
        if(gRogueAdvPath.rooms[0].routeScene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE)
            break;
    }
    shrine = gRogueAdvPath.rooms[0].routeScene;

    EXPECT_EQ(shrine.recipeId, ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE);
    EXPECT_EQ((u8)shrine.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
    EXPECT_EQ(shrine.rewardAmount, 8000);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);

    RogueRouteEvents_GetInteractionData();
    EXPECT_EQ(gSpecialVar_0x8005, shrine.requestedItem);
    EXPECT_EQ(gSpecialVar_0x8007, shrine.rewardAmount);

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && CheckBagHasSpace(shrine.requestedItem, 1); ++itemId)
    {
        if(ItemId_GetPocket(itemId) == ItemId_GetPocket(shrine.requestedItem) && itemId != shrine.requestedItem)
            AddBagItem(itemId, 1);
    }
    EXPECT(!CheckBagHasSpace(shrine.requestedItem, 1));
    SetMoney(&gSaveBlock1Ptr->money, 1000);
    RogueRouteEvents_TryAcceptHexedShrine();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_NO_SPACE);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 1000);
    EXPECT_EQ(gRogueRun.temporaryDarkDealCurseItem, ITEM_NONE);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);

    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, MAX_MONEY - shrine.rewardAmount + 1);
    RogueRouteEvents_TryAcceptHexedShrine();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_MONEY_FULL);
    EXPECT(!CheckBagHasItem(shrine.requestedItem, 1));
    EXPECT_EQ(gRogueRun.temporaryDarkDealCurseItem, ITEM_NONE);

    SetMoney(&gSaveBlock1Ptr->money, 1000);
    RogueRouteEvents_TryAcceptHexedShrine();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 1000 + shrine.rewardAmount);
    EXPECT(CheckBagHasItem(shrine.requestedItem, 1));
    EXPECT_EQ(gRogueRun.temporaryDarkDealCurseItem, shrine.requestedItem);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);

    // A same-route quickload reconstructs the accepted shrine even though its
    // active Curse would normally make new shrines ineligible.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(memcmp(&shrine, &gRogueAdvPath.rooms[0].routeScene, sizeof(shrine)), 0);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    Rogue_SetCurrentDifficulty(ROGUE_CHAMP_START_DIFFICULTY);
    RogueRouteScenes_OnExitRoute();
    gRogueAdvPath.roomCount = 2;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = 1;
    gRogueAdvPath.rooms[1].rngSeed = 701;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_NONE);

    Rogue_ClearTemporaryDarkDealCurse();
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    Rogue_SetCurrentDifficulty(13);
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE);
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.rewardAmount, ROGUE_HEXED_SHRINE_REWARD_MAX);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.temporaryDarkDealCurseItem = originalTempCurse;
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    Rogue_SetCurrentDifficulty(originalDifficulty);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    RestoreFlag(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED, originalComplete);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    SetMoney(&gSaveBlock1Ptr->money, originalMoney);
    ClearBag();
}

TEST("Anomalous Fossil restores deterministic stable and adaptive Rare Unique Pokemon")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct Pokemon originalParty[PARTY_SIZE];
    struct RogueRouteSceneRequest offer;
    struct RogueRouteSceneRequest restoration;
    struct RogueAdventureQuestCreateParams params = {0};
    RAND_TYPE originalStandardRng = gRngValue;
    RAND_TYPE rngBefore;
    u16 originalDifficulty = Rogue_GetCurrentDifficulty();
    u16 originalTempCurse = gRogueRun.temporaryDarkDealCurseItem;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalPartyCount = gPlayerPartyCount;
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalComplete = FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    u8 questId;
    u32 customMonId;
    u8 customType0;
    u8 customType1;

    memcpy(originalParty, gPlayerParty, sizeof(originalParty));
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
    ClearBag();
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    FlagSet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    gRogueRun.temporaryDarkDealCurseItem = Rogue_SelectDarkDealCurseItem(0);
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    Rogue_SetCurrentDifficulty(0);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    gRogueAdvPath.rooms[0].rngSeed = 300;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    offer = gRogueAdvPath.rooms[0].routeScene;

    EXPECT_EQ(offer.recipeId, ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER);
    EXPECT_EQ((u8)offer.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
    EXPECT_EQ(RogueAdventureQuests_GetFossilSpecies(offer.requestedItem), offer.rewardItem);
    EXPECT(!ItemId_GetImportance(offer.requestedItem));

    RogueRouteEvents_TryAcceptAnomalousFossilQuest();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(CheckBagHasItem(offer.requestedItem, 1));
    EXPECT(ItemId_GetImportance(offer.requestedItem));
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    questId = RogueAdventureQuests_GetQuestIdAt(0);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->payload[0], offer.requestedItem);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->payload[1], offer.rewardAmount);

    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(memcmp(&offer, &gRogueAdvPath.rooms[0].routeScene, sizeof(offer)), 0);

    RogueRouteScenes_OnExitRoute();
    gRogueAdvPath.roomCount = 2;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = 1;
    gRogueAdvPath.rooms[1].rngSeed = 301;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    restoration = gRogueAdvPath.rooms[1].routeScene;
    EXPECT_EQ(restoration.recipeId, ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION);
    EXPECT_EQ((u8)restoration.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE);
    EXPECT_EQ(restoration.ownerQuestId, questId);
    EXPECT_EQ(restoration.requestedItem, offer.requestedItem);
    EXPECT_EQ(restoration.rewardItem, offer.rewardItem);
    EXPECT_EQ(restoration.rewardAmount, offer.rewardAmount);

    SeedRng(0x2468);
    rngBefore = gRngValue;
    RogueRouteEvents_BufferFossilRestorationData();
    EXPECT_EQ(memcmp(&gRngValue, &rngBefore, sizeof(rngBefore)), 0);

    gSpecialVar_0x8004 = ROGUE_FOSSIL_RESTORATION_STABLE;
    RogueRouteEvents_TryRestoreAnomalousFossil();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(&gRngValue, &rngBefore, sizeof(rngBefore)), 0);
    EXPECT(!CheckBagHasItem(restoration.requestedItem, 1));
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), restoration.rewardItem);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_LEVEL), 1);
    customMonId = RogueGift_GetCustomMonId(&gPlayerParty[0]);
    EXPECT_NE(customMonId, 0);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_RARE);
    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 0), TYPE_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 1), TYPE_NONE);

    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);

    // Rebuild the same consumer with another compact seed and select the
    // adaptive branch. Exactly one generated type slot must differ from the
    // fossil species' native typing.
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
    params.payload[0] = offer.requestedItem;
    params.payload[1] = offer.rewardAmount + 1;
    EXPECT(AddBagItem(params.payload[0], 1));
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL, &params);
    EXPECT_NE(questId, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    memset(&gRogueAdvPath.rooms[1].routeScene, 0, sizeof(gRogueAdvPath.rooms[1].routeScene));
    gRogueAdvPath.rooms[1].routeScene.recipeId = ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION;
    gRogueAdvPath.rooms[1].routeScene.source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE;
    gRogueAdvPath.rooms[1].routeScene.ownerQuestId = questId;
    gRogueAdvPath.rooms[1].routeScene.requestedItem = params.payload[0];
    gRogueAdvPath.rooms[1].routeScene.rewardItem = RogueAdventureQuests_GetFossilSpecies(params.payload[0]);
    gRogueAdvPath.rooms[1].routeScene.rewardAmount = params.payload[1];
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    SeedRng(0x1357);
    rngBefore = gRngValue;
    gSpecialVar_0x8004 = ROGUE_FOSSIL_RESTORATION_ADAPTIVE;
    RogueRouteEvents_TryRestoreAnomalousFossil();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(&gRngValue, &rngBefore, sizeof(rngBefore)), 0);
    customMonId = RogueGift_GetCustomMonId(&gPlayerParty[0]);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_RARE);
    customType0 = RogueGift_GetCustomMonType(customMonId, 0);
    customType1 = RogueGift_GetCustomMonType(customMonId, 1);
    EXPECT(IS_STANDARD_TYPE(customType0) || IS_STANDARD_TYPE(customType1));
    if(IS_STANDARD_TYPE(customType0))
    {
        EXPECT_NE(customType0, RoguePokedex_GetSpeciesType(gRogueAdvPath.rooms[1].routeScene.rewardItem, 0));
        EXPECT_NE(customType0, RoguePokedex_GetSpeciesType(gRogueAdvPath.rooms[1].routeScene.rewardItem, 1));
    }
    if(IS_STANDARD_TYPE(customType1))
    {
        EXPECT_NE(customType1, RoguePokedex_GetSpeciesType(gRogueAdvPath.rooms[1].routeScene.rewardItem, 0));
        EXPECT_NE(customType1, RoguePokedex_GetSpeciesType(gRogueAdvPath.rooms[1].routeScene.rewardItem, 1));
    }

    RogueAdventureQuests_Clear();
    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    memcpy(gPlayerParty, originalParty, sizeof(originalParty));
    gPlayerPartyCount = originalPartyCount;
    gRogueRun.temporaryDarkDealCurseItem = originalTempCurse;
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    Rogue_SetCurrentDifficulty(originalDifficulty);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    RestoreFlag(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED, originalComplete);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    gRngValue = originalStandardRng;
    ClearBag();
}

TEST("Stolen Trade Case completes its three route-node handoffs")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueRouteSceneRequest offer;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 originalTeamNum = gRogueRun.teamEncounterNum;
    u16 originalDifficulty = Rogue_GetCurrentDifficulty();
    u16 originalTempCurse = gRogueRun.temporaryDarkDealCurseItem;
    u32 originalMoney = GetMoney(&gSaveBlock1Ptr->money);
    u16 originalTrainers[ROGUE_MAX_ACTIVE_TRAINER_COUNT];
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalComplete = FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    bool8 originalPropA = FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    bool8 originalPropB = FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
    u8 questId;
    u16 itemId;
    u16 seed;
    u8 i;

    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, 12345);
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    gRogueRun.teamEncounterNum = TEAM_NUM_KANTO_ROCKET;
    // This test covers the quest chain specifically; an active temporary
    // Curse makes the shrine ineligible and deterministically exercises the
    // remaining fallback entry.
    gRogueRun.temporaryDarkDealCurseItem = Rogue_SelectDarkDealCurseItem(0);
    Rogue_SetCurrentDifficulty(0);
    FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    for(seed = 1; seed != 0; ++seed)
    {
        gRogueAdvPath.rooms[0].rngSeed = seed;
        RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
        RogueRouteScenes_OnEnterRoute();
        if(gRogueAdvPath.rooms[0].routeScene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER)
            break;
    }
    offer = gRogueAdvPath.rooms[0].routeScene;

    EXPECT_EQ(offer.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER);
    RogueRouteEvents_TryAcceptStolenTradeCaseQuest();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    questId = RogueAdventureQuests_GetQuestIdAt(0);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->payload[1], offer.trainerNum);
    EXPECT(!CheckBagHasItem(ITEM_TRADE_CASE, 1));
    Rogue_SetCurrentDifficulty(ROGUE_CHAMP_START_DIFFICULTY);

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
    gRogueRun.temporaryDarkDealCurseItem = originalTempCurse;
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
