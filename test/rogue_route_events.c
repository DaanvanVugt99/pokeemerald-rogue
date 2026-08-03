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
extern const u8 Rogue_RouteEvent_Delivery[];
extern const u8 Rogue_RouteEvent_DeliveryPayoff[];
extern const u8 Rogue_RouteEvent_SupplyRequest[];
extern const u8 Rogue_RouteEvent_TrainerChallenge[];
extern const u8 Rogue_RouteEvent_Prop[];

static bool8 IsDeliveryReward(u16 itemId)
{
    return itemId == ITEM_NUGGET || itemId == ITEM_RARE_CANDY || itemId == ITEM_PP_UP;
}

static bool8 IsSupplyRequest(u16 itemId)
{
    return itemId == ITEM_POTION
        || itemId == ITEM_ANTIDOTE
        || itemId == ITEM_PARALYZE_HEAL
        || itemId == ITEM_REPEL
        || itemId == ITEM_ORAN_BERRY;
}

static bool8 IsSupplyReward(u16 itemId)
{
    return itemId == ITEM_SUPER_POTION || itemId == ITEM_ETHER || itemId == ITEM_REVIVE;
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

TEST("Route events generate deterministic weighted RNG-neutral descriptors")
{
    static const u8 expectedWeights[ROGUE_ROUTE_ENVIRONMENT_COUNT][ROGUE_ROUTE_SCENE_RECIPE_GENERATED_COUNT - 1] =
    {
        {35, 40, 25},
        {30, 40, 30},
        {30, 25, 45},
        {30, 25, 45},
        {35, 40, 25},
        {40, 35, 25},
    };
    struct RogueAdvPathRoom roomA = {0};
    struct RogueAdvPathRoom roomB = {0};
    RAND_TYPE originalRng = gRngRogueValue;
    RAND_TYPE rngBefore;
    bool8 foundRecipes[ROGUE_ROUTE_SCENE_RECIPE_COUNT] = {FALSE};
    u16 seed;
    u8 environment;
    u8 recipeId;

    for(environment = 0; environment < ROGUE_ROUTE_ENVIRONMENT_COUNT; ++environment)
    {
        for(recipeId = ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER; recipeId < ROGUE_ROUTE_SCENE_RECIPE_GENERATED_COUNT; ++recipeId)
            EXPECT_EQ(RogueRouteScenes_Test_GetOneOffWeight(environment, recipeId), expectedWeights[environment][recipeId - 1]);
    }

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
    EXPECT_EQ((u8)roomA.routeScene.environment, gRogueRouteTable.routes[0].environment);
    EXPECT_LT((u8)roomA.routeScene.anchor, ROGUE_ROUTE_EVENT_ANCHOR_COUNT);

    for(seed = 1; seed < 1000; ++seed)
    {
        roomA.rngSeed = seed;
        RogueRouteScenes_GenerateRoom(&roomA);
        foundRecipes[roomA.routeScene.recipeId] = TRUE;

        if(roomA.routeScene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER)
        {
            EXPECT_EQ((u8)roomA.routeScene.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
            EXPECT(IsDeliveryReward(roomA.routeScene.rewardItem));
            EXPECT_NE(roomA.routeScene.primaryGraphicsId, roomA.routeScene.secondaryGraphicsId);
        }
        else if(roomA.routeScene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST)
        {
            EXPECT_EQ((u8)roomA.routeScene.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
            EXPECT(IsSupplyRequest(roomA.routeScene.requestedItem));
            EXPECT(IsSupplyReward(roomA.routeScene.rewardItem));
        }
        else if(roomA.routeScene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE)
        {
            EXPECT_EQ((u8)roomA.routeScene.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
            EXPECT(IsDeliveryReward(roomA.routeScene.rewardItem));
            EXPECT_NE(roomA.routeScene.trainerNum, TRAINER_NONE);
            EXPECT_EQ(roomA.routeScene.primaryGraphicsId, Rogue_GetTrainerObjectEventGfx(roomA.routeScene.trainerNum));
        }
    }

    EXPECT(foundRecipes[ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER]);
    EXPECT(foundRecipes[ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST]);
    EXPECT(foundRecipes[ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE]);
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

TEST("Route event scene composition removes markers and inserts bounded unique objects")
{
    struct RogueAdvPath originalPath;
    u8 originalRoomId;
    u8 recipeId;

    SetupCurrentEvent(&originalPath, &originalRoomId);

    for(recipeId = ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER; recipeId < ROGUE_ROUTE_SCENE_RECIPE_COUNT; ++recipeId)
    {
        struct ObjectEventTemplate objects[8] =
        {
            {.localId = 1, .graphicsId = OBJ_EVENT_GFX_BOY_1, .x = 2, .y = 2},
            {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 12, .y = 34, .elevation = 3, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
            {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 56, .y = 78, .elevation = 3, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
        };
        const u8 *expectedScript = recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER ? Rogue_RouteEvent_Delivery
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST ? Rogue_RouteEvent_SupplyRequest
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE ? Rogue_RouteEvent_TrainerChallenge
            : Rogue_RouteEvent_DeliveryPayoff;
        u8 count = 3;
        u8 expectedCount = (recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER || recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_PAYOFF) ? 3 : 4;
        u8 i;
        u8 j;
        u8 npcCount = 0;

        memset(&gRogueAdvPath.rooms[0].routeScene, 0, sizeof(gRogueAdvPath.rooms[0].routeScene));
        gRogueAdvPath.rooms[0].routeScene.recipeId = recipeId;
        gRogueAdvPath.rooms[0].routeScene.environment = ROGUE_ROUTE_ENVIRONMENT_MOUNTAIN;
        gRogueAdvPath.rooms[0].routeScene.anchor = ROGUE_ROUTE_EVENT_ANCHOR_RECIPIENT;
        gRogueAdvPath.rooms[0].routeScene.variant = 1;
        gRogueAdvPath.rooms[0].routeScene.primaryGraphicsId = OBJ_EVENT_GFX_GENTLEMAN;
        gRogueAdvPath.rooms[0].routeScene.secondaryGraphicsId = OBJ_EVENT_GFX_PICNICKER;

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
                EXPECT(objects[i].localId == 41 || objects[i].localId == 42);
                EXPECT((objects[i].x == 12 && objects[i].y == 34) || (objects[i].x == 56 && objects[i].y == 78));
                if(recipeId != ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER)
                    EXPECT_EQ(objects[i].localId, 42);
            }
            else if(objects[i].script == Rogue_RouteEvent_Prop)
            {
                if(recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER || recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_PAYOFF)
                {
                    EXPECT_EQ(objects[i].x, 55);
                    EXPECT_EQ(objects[i].y, 79);
                }
                else
                {
                    EXPECT(objects[i].x == 55 || objects[i].x == 57);
                    EXPECT_EQ(objects[i].y, recipeId == ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST ? 79 : 77);
                }
            }
        }

        EXPECT_EQ(npcCount, 1);
    }

    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Route event metatile accents are bounded compatible and idempotent")
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
    gRogueAdvPath.rooms[0].routeScene.recipeId = ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST;
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
    EXPECT_EQ(MapGridGetMetatileIdAt(x, y), METATILE_General_Grass);
    EXPECT_EQ(MapGridGetElevationAt(x - 1, y), 3);

    RogueRouteScenes_ApplyMetatiles();
    EXPECT_EQ(MapGridGetMetatileIdAt(x - 1, y), METATILE_General_Grass_Stone);

    MapGridSetMetatileIdAt(x + 1, y, METATILE_General_TallGrass);
    RogueRouteScenes_ApplyMetatiles();
    EXPECT_EQ(MapGridGetMetatileIdAt(x + 1, y), METATILE_General_TallGrass);

    gBackupMapLayout = originalBackup;
    gMapHeader = originalHeader;
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Route delivery quests persist across routes and dynamically insert their payoff")
{
    struct RogueAdvPath originalPath;
    struct RogueRouteSceneRequest generatedScene;
    struct RogueAdventureQuestCreateParams params = {0};
    u8 originalRoomId;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    u8 questId;
    u16 itemId;
    u16 seed;

    ClearBag();
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    for(seed = 0; seed < 1000; ++seed)
    {
        gRogueAdvPath.rooms[0].rngSeed = seed;
        RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
        if(gRogueAdvPath.rooms[0].routeScene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER)
            break;
    }
    EXPECT_LT(seed, 1000);
    generatedScene = gRogueAdvPath.rooms[0].routeScene;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    RogueRouteScenes_OnEnterRoute();

    RogueRouteEvents_TryAcceptDelivery();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    EXPECT(CheckBagHasItem(ITEM_PARCEL, 1));
    EXPECT(FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN));
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    questId = RogueAdventureQuests_GetQuestIdAt(0);
    EXPECT_EQ(RogueAdventureQuests_GetState(questId), ROGUE_ADVENTURE_QUEST_STATE_ACTIVE);

    // A quickload regenerates the descriptor but must preserve the source scene and quest.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    EXPECT_EQ(memcmp(&gRogueAdvPath.rooms[0].routeScene, &generatedScene, sizeof(generatedScene)), 0);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    EXPECT(CheckBagHasItem(ITEM_PARCEL, 1));
    EXPECT_EQ(gRogueAdvPath.rooms[0].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER);
    EXPECT_EQ((u8)gRogueAdvPath.rooms[0].routeScene.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);

    // Leaving without completing keeps the cargo and makes the next route reactive.
    RogueRouteScenes_OnExitRoute();
    EXPECT(CheckBagHasItem(ITEM_PARCEL, 1));
    gRogueAdvPath.roomCount = 2;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1] = gRogueAdvPath.rooms[0];
    gRogueAdvPath.rooms[1].rngSeed = seed + 1;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.recipeId, ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_PAYOFF);
    EXPECT_EQ((u8)gRogueAdvPath.rooms[1].routeScene.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE);
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.ownerQuestId, questId);
    EXPECT_EQ(gRogueAdvPath.rooms[1].routeScene.rewardItem, generatedScene.rewardItem);
    EXPECT_EQ(RogueAdventureQuests_GetState(questId), ROGUE_ADVENTURE_QUEST_STATE_READY);

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && GetBagUnreservedFreeSlots() != 0; ++itemId)
    {
        u8 pocket = ItemId_GetPocket(itemId);
        if(pocket != POCKET_NONE && pocket != POCKET_KEY_ITEMS && itemId != generatedScene.rewardItem)
            AddBagItem(itemId, 1);
    }
    RogueRouteEvents_TryCompleteDelivery();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_NO_SPACE);
    EXPECT_NE(RogueAdventureQuests_Get(questId), NULL);
    EXPECT(CheckBagHasItem(ITEM_PARCEL, 1));

    ClearBag();
    EXPECT(AddBagItem(ITEM_PARCEL, 1));
    RogueRouteEvents_TryCompleteDelivery();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT(!CheckBagHasItem(ITEM_PARCEL, 1));
    EXPECT(CheckBagHasItem(generatedScene.rewardItem, 1));
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);

    // Unclaimed payoff nodes move forward until cashed in.
    ClearBag();
    gRogueRun.adventureRoomId = 0;
    gRogueAdvPath.rooms[0].routeScene = generatedScene;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    EXPECT(AddBagItem(ITEM_PARCEL, 1));
    params.payload[0] = generatedScene.rewardItem;
    params.payload[1] = generatedScene.secondaryGraphicsId;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_PARCEL_DELIVERY, &params);
    EXPECT_NE(questId, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].routeScene = generatedScene;
    RogueRouteScenes_OnEnterRoute();
    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ(RogueAdventureQuests_GetState(questId), ROGUE_ADVENTURE_QUEST_STATE_ACTIVE);
    EXPECT(CheckBagHasItem(ITEM_PARCEL, 1));

    RogueAdventureQuests_Clear();
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);
    EXPECT(!CheckBagHasItem(ITEM_PARCEL, 1));

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    ClearBag();
}

TEST("Route event supply requests handle missing items full bags and atomic exchange")
{
    struct RogueAdvPath originalPath;
    u8 originalRoomId;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 itemId;

    ClearBag();
    SetupCurrentEvent(&originalPath, &originalRoomId);
    gRogueAdvPath.rooms[0].routeScene.recipeId = ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST;
    gRogueAdvPath.rooms[0].routeScene.source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF;
    gRogueAdvPath.rooms[0].routeScene.requestedItem = ITEM_POTION;
    gRogueAdvPath.rooms[0].routeScene.rewardItem = ITEM_REVIVE;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);

    RogueRouteEvents_TryCompleteSupplyRequest();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM);

    EXPECT(AddBagItem(ITEM_POTION, 2));
    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && GetBagUnreservedFreeSlots() != 0; ++itemId)
    {
        u8 pocket = ItemId_GetPocket(itemId);
        if(pocket != POCKET_NONE && pocket != POCKET_KEY_ITEMS && itemId != ITEM_POTION && itemId != ITEM_REVIVE)
            AddBagItem(itemId, 1);
    }
    EXPECT(!CheckBagHasSpace(ITEM_REVIVE, 1));
    RogueRouteEvents_TryCompleteSupplyRequest();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_NO_SPACE);
    EXPECT(CheckBagHasItem(ITEM_POTION, 2));

    ClearBag();
    EXPECT(AddBagItem(ITEM_POTION, 1));
    RogueRouteEvents_TryCompleteSupplyRequest();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(!CheckBagHasItem(ITEM_POTION, 1));
    EXPECT(CheckBagHasItem(ITEM_REVIVE, 1));
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    ClearBag();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Adventure quest registry queues multiple route objectives without replacing them")
{
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueAdventureQuestCreateParams params =
    {
        .payload = {ITEM_NUGGET, OBJ_EVENT_GFX_PICNICKER},
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
        EXPECT_EQ(RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_PARCEL_DELIVERY, &params), i);
    }
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 20);

    EXPECT(RogueAdventureQuests_TryCollectSceneRequest(30, &request, &priority));
    EXPECT_EQ(request.ownerQuestId, 0);
    EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE);
    EXPECT_EQ(RogueAdventureQuests_GetState(0), ROGUE_ADVENTURE_QUEST_STATE_READY);
    EXPECT_EQ(RogueAdventureQuests_GetState(1), ROGUE_ADVENTURE_QUEST_STATE_ACTIVE);
    RogueAdventureQuests_LeaveRoute(30);
    EXPECT_EQ(RogueAdventureQuests_GetState(0), ROGUE_ADVENTURE_QUEST_STATE_ACTIVE);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 20);
    EXPECT(RogueAdventureQuests_TryCollectSceneRequest(31, &request, &priority));
    EXPECT_EQ(request.ownerQuestId, 1);
    EXPECT_EQ(RogueAdventureQuests_GetState(0), ROGUE_ADVENTURE_QUEST_STATE_ACTIVE);
    EXPECT_EQ(RogueAdventureQuests_GetState(1), ROGUE_ADVENTURE_QUEST_STATE_READY);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Adventure quest graph nodes advance from passive gameplay signals")
{
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueAdventureQuestCreateParams params =
    {
        .target = 3,
    };
    const struct RogueAdventureQuest *quest;
    u8 questId;

    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));

    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_TRAINER_HUNT, &params);
    EXPECT_NE(questId, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    quest = RogueAdventureQuests_Get(questId);
    EXPECT_EQ(quest->definitionId, ROGUE_ADVENTURE_QUEST_DEFINITION_TRAINER_HUNT);
    EXPECT_EQ(RogueAdventureQuests_GetState(questId), ROGUE_ADVENTURE_QUEST_STATE_ACTIVE);

    RogueAdventureQuests_EmitSignal(ROGUE_ADVENTURE_QUEST_SIGNAL_TRAINER_DEFEATED, 2);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->progress, 2);
    EXPECT_EQ(RogueAdventureQuests_GetState(questId), ROGUE_ADVENTURE_QUEST_STATE_ACTIVE);

    RogueAdventureQuests_EmitSignal(ROGUE_ADVENTURE_QUEST_SIGNAL_TRAINER_DEFEATED, 1);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->nodeId, 1);
    EXPECT_EQ(RogueAdventureQuests_GetState(questId), ROGUE_ADVENTURE_QUEST_STATE_READY);
    EXPECT(RogueAdventureQuests_Advance(questId));
    EXPECT_EQ(RogueAdventureQuests_Get(questId), NULL);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
}

TEST("Route event trainer challenges exclude duplicates and retain pending rewards")
{
    struct RogueAdvPath originalPath;
    u8 originalRoomId;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 originalTrainers[ROGUE_MAX_ACTIVE_TRAINER_COUNT];
    u16 trainerNum;
    u16 itemId;
    u8 i;

    ClearBag();
    SetupCurrentEvent(&originalPath, &originalRoomId);
    trainerNum = 1;
    gRogueAdvPath.rooms[0].routeScene.recipeId = ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE;
    gRogueAdvPath.rooms[0].routeScene.source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF;
    gRogueAdvPath.rooms[0].routeScene.trainerNum = trainerNum;
    gRogueAdvPath.rooms[0].routeScene.rewardItem = ITEM_NUGGET;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);

    for(i = 0; i < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++i)
    {
        originalTrainers[i] = Rogue_GetDynamicTrainer(i);
        Rogue_SetDynamicTrainer(i, i < 2 ? trainerNum : TRAINER_NONE);
    }
    RogueRouteScenes_PrepareRouteTrainers();
    EXPECT_EQ(Rogue_GetDynamicTrainer(0), TRAINER_NONE);
    EXPECT_EQ(Rogue_GetDynamicTrainer(1), TRAINER_NONE);

    RogueRouteEvents_BeginTrainerChallenge();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA), trainerNum);

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && GetBagUnreservedFreeSlots() != 0; ++itemId)
    {
        u8 pocket = ItemId_GetPocket(itemId);
        if(pocket != POCKET_NONE && pocket != POCKET_KEY_ITEMS && itemId != ITEM_NUGGET)
            AddBagItem(itemId, 1);
    }
    RogueRouteEvents_FinishTrainerChallenge();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_NO_SPACE);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);

    ClearBag();
    RogueRouteEvents_TryClaimTrainerReward();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT(CheckBagHasItem(ITEM_NUGGET, 1));

    for(i = 0; i < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++i)
        Rogue_SetDynamicTrainer(i, originalTrainers[i]);
    ClearBag();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}
