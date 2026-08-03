#include "global.h"

#include "battle.h"
#include "constants/battle.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/metatile_labels.h"
#include "constants/pokemon.h"
#include "constants/rogue_adventure_quests.h"
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
#include "rogue_event_transactions.h"
#include "rogue_gifts.h"
#include "rogue_pokedex.h"
#include "rogue_route_events.h"
#include "rogue_route_scene_internal.h"
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
extern const u8 Rogue_RouteEvent_ForbiddenStoneOffer[];
extern const u8 Rogue_RouteEvent_ForbiddenStoneSoul[];
extern const u8 Rogue_RouteEvent_ForbiddenStonePayoff[];
extern const u8 Rogue_RouteEvent_ForbiddenStoneProp[];
extern const u8 Rogue_RouteEvent_ApricornTree[];
extern const u8 Rogue_RouteEvent_ApricornArtisan[];
extern const u8 Rogue_RouteEvent_ApricornProp[];

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

TEST("Event transactions exchange and roll back items and money atomically")
{
    struct RogueEventTransaction transaction = {0};
    u32 originalMoney = GetMoney(&gSaveBlock1Ptr->money);
    u16 itemId;

    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, 12000);
    EXPECT(AddBagItem(ITEM_TRADE_CASE, 1));
    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && CheckBagHasSpace(ITEM_BIG_POKEBLOCK_BUNDLE, 1); ++itemId)
    {
        if(ItemId_GetPocket(itemId) == POCKET_KEY_ITEMS
            && itemId != ITEM_TRADE_CASE
            && itemId != ITEM_BIG_POKEBLOCK_BUNDLE)
            AddBagItem(itemId, 1);
    }
    EXPECT(!CheckBagHasSpace(ITEM_BIG_POKEBLOCK_BUNDLE, 1));

    transaction.costs[0].itemId = ITEM_TRADE_CASE;
    transaction.costs[0].count = 1;
    transaction.rewards[0].itemId = ITEM_BIG_POKEBLOCK_BUNDLE;
    transaction.rewards[0].count = 1;
    transaction.moneyReward = 5000;
    transaction.costCount = 1;
    transaction.rewardCount = 1;
    transaction.flags = ROGUE_EVENT_TRANSACTION_FLAG_ALLOW_COST_SLOTS_FOR_REWARDS;

    // Consuming the cost frees the slot required by the reward.
    EXPECT_EQ(RogueEventTransaction_Execute(&transaction), ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(!CheckBagHasItem(ITEM_TRADE_CASE, 1));
    EXPECT(CheckBagHasItem(ITEM_BIG_POKEBLOCK_BUNDLE, 1));
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 17000);

    RogueEventTransaction_Rollback(&transaction);
    EXPECT(CheckBagHasItem(ITEM_TRADE_CASE, 1));
    EXPECT(!CheckBagHasItem(ITEM_BIG_POKEBLOCK_BUNDLE, 1));
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 12000);

    SetMoney(&gSaveBlock1Ptr->money, MAX_MONEY - transaction.moneyReward + 1);
    EXPECT_EQ(RogueEventTransaction_Execute(&transaction), ROGUE_ROUTE_EVENT_RESULT_MONEY_FULL);
    EXPECT(CheckBagHasItem(ITEM_TRADE_CASE, 1));
    EXPECT(!CheckBagHasItem(ITEM_BIG_POKEBLOCK_BUNDLE, 1));
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), MAX_MONEY - transaction.moneyReward + 1);

    RemoveBagItem(ITEM_TRADE_CASE, 1);
    EXPECT_EQ(RogueEventTransaction_Execute(&transaction), ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM);
    EXPECT(!CheckBagHasItem(ITEM_BIG_POKEBLOCK_BUNDLE, 1));

    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, originalMoney);
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

static bool8 GetPlacementByRecipe(u8 recipeId, struct RogueRouteSceneRequest *request)
{
    u8 i;

    for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
    {
        if(RogueRouteScenes_GetPlacementRequest(i, request)
            && request->recipeId == recipeId)
            return TRUE;
    }

    memset(request, 0, sizeof(*request));
    return FALSE;
}

static bool8 GetFirstPlacement(struct RogueRouteSceneRequest *request)
{
    return RogueRouteScenes_GetPlacementRequest(0, request);
}

static void SelectPlacement(const struct RogueRouteSceneRequest *request)
{
    gSelectedObjectEvent = 0;
    gObjectEvents[0].trainerRange_berryTreeId = request->sceneSlot | (request->lotRole << 2);
}

static void SelectPlacementProp(const struct RogueRouteSceneRequest *request, u8 propId)
{
    SelectPlacement(request);
    gObjectEvents[0].trainerRange_berryTreeId |= propId << 4;
}

static void SetDebugPlacement(u8 recipeId, u8 lotId, u8 ownerQuestId)
{
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[gRogueRun.adventureRoomId]);
    RogueRouteScenes_DebugSetPlacement(0, recipeId, lotId, 0, 0, ownerQuestId);
}

static u16 GetExpectedApricornBall(u16 apricorn)
{
    switch(apricorn)
    {
    case ITEM_RED_APRICORN:
        return ITEM_LEVEL_BALL;
    case ITEM_BLUE_APRICORN:
        return ITEM_LURE_BALL;
    case ITEM_YELLOW_APRICORN:
        return ITEM_MOON_BALL;
    case ITEM_GREEN_APRICORN:
        return ITEM_FRIEND_BALL;
    case ITEM_PINK_APRICORN:
        return ITEM_LOVE_BALL;
    case ITEM_WHITE_APRICORN:
        return ITEM_FAST_BALL;
    case ITEM_BLACK_APRICORN:
        return ITEM_HEAVY_BALL;
    default:
        return ITEM_NONE;
    }
}

TEST("Route scene local RNG matches Rogue RNG without mutating it")
{
    static const u16 sSeeds[] = {0, 1, 0x5EED, 0xA7E1, 0xFFFF};
    struct RogueRouteSceneRng localRng;
    RAND_TYPE originalRng = gRngRogueValue;
    u16 expected[8];
    u8 seedIdx;
    u8 drawIdx;

    for(seedIdx = 0; seedIdx < ARRAY_COUNT(sSeeds); ++seedIdx)
    {
        SeedRogueRng(sSeeds[seedIdx]);
        for(drawIdx = 0; drawIdx < ARRAY_COUNT(expected); ++drawIdx)
            expected[drawIdx] = RogueRandom();

        gRngRogueValue = originalRng;
        RogueRouteSceneRng_Seed(&localRng, sSeeds[seedIdx]);
        for(drawIdx = 0; drawIdx < ARRAY_COUNT(expected); ++drawIdx)
            EXPECT_EQ(RogueRouteSceneRng_Next(&localRng), expected[drawIdx]);
        EXPECT_EQ(memcmp(&gRngRogueValue, &originalRng, sizeof(originalRng)), 0);
    }

    gRngRogueValue = originalRng;
}

TEST("Selected standalone route scene payloads remain immutable")
{
    static const u8 sRecipes[] =
    {
        ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER,
        ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE,
        ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER,
        ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER,
        ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE,
    };
    struct RogueAdvPath originalPath;
    struct RogueRouteSceneRequest selected;
    struct RogueRouteSceneRequest restored;
    struct RogueRouteScenePlan selectedPlan;
    u16 originalDifficulty = Rogue_GetCurrentDifficulty();
    u16 originalTeamNum = gRogueRun.teamEncounterNum;
    u16 originalTempCurse = gRogueRun.temporaryDarkDealCurseItem;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalComplete = FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    u8 i;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    gRogueRun.teamEncounterNum = TEAM_NUM_KANTO_ROCKET;
    gRogueRun.temporaryDarkDealCurseItem = ITEM_NONE;
    Rogue_SetCurrentDifficulty(2);
    FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);

    for(i = 0; i < ARRAY_COUNT(sRecipes); ++i)
    {
        gRogueAdvPath.rooms[0].rngSeed = 0x6100 + i;
        SetDebugPlacement(sRecipes[i], 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
        EXPECT(RogueRouteScenes_GetPlacementRequest(0, &selected));
        selectedPlan = gRogueAdvPath.rooms[0].routeScenePlan;

        gRogueRun.teamEncounterNum = TEAM_NUM_AQUA;
        gRogueRun.temporaryDarkDealCurseItem = Rogue_SelectDarkDealCurseItem(1);
        Rogue_SetCurrentDifficulty(10);
        FlagSet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
        RogueRouteScenes_OnEnterRoute();

        EXPECT_EQ(memcmp(&gRogueAdvPath.rooms[0].routeScenePlan, &selectedPlan, sizeof(selectedPlan)), 0);
        EXPECT(RogueRouteScenes_GetPlacementRequest(0, &restored));
        EXPECT_EQ(memcmp(&restored, &selected, sizeof(selected)), 0);

        gRogueRun.teamEncounterNum = TEAM_NUM_KANTO_ROCKET;
        gRogueRun.temporaryDarkDealCurseItem = ITEM_NONE;
        Rogue_SetCurrentDifficulty(2);
        FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    }

    Rogue_SetCurrentDifficulty(originalDifficulty);
    gRogueRun.teamEncounterNum = originalTeamNum;
    gRogueRun.temporaryDarkDealCurseItem = originalTempCurse;
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    RestoreFlag(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED, originalComplete);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Route event fallback registry is deterministic weighted and RNG neutral")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueRouteScenePlan firstPlan;
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
    u16 forbiddenStoneCount = 0;
    u16 apricornCount = 0;
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

        firstPlan = gRogueAdvPath.rooms[0].routeScenePlan;
        SeedRng(0xEF01);
        SeedRogueRng(0xDCBA);
        RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
        RogueRouteScenes_OnEnterRoute();
        EXPECT_EQ(memcmp(&firstPlan, &gRogueAdvPath.rooms[0].routeScenePlan, sizeof(firstPlan)), 0);
        EXPECT_GE(RogueRouteScenes_GetPlacementCount(), 1);
        EXPECT_LE(RogueRouteScenes_GetPlacementCount(), ROGUE_ROUTE_SCENE_MAX_PLACEMENTS);

        for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
        {
            struct RogueRouteSceneRequest request;

            EXPECT(RogueRouteScenes_GetPlacementRequest(i, &request));
            if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER)
            {
                ++merchantCount;
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
                EXPECT_EQ(request.requestedItem, ITEM_TRADE_CASE);
                EXPECT((gRogueTrainers[request.trainerNum].classFlags & GetActiveTeamClassFlag(gRogueRun.teamEncounterNum)) != 0);
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE)
            {
                u8 curseIdx;

                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
                EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_DEVIL_MAN);
                ++shrineCount;
                for(curseIdx = 0; curseIdx < curseCount; ++curseIdx)
                {
                    if(request.requestedItem == Rogue_SelectDarkDealCurseItem(curseIdx))
                        seenCurse[curseIdx] = TRUE;
                }
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER)
            {
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
                EXPECT_EQ(RogueAdventureQuests_GetFossilSpecies(request.requestedItem), request.rewardItem);
                ++fossilCount;
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER)
            {
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
                EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_MISC_CHANNELER);
                EXPECT_EQ(request.requestedItem, ITEM_ODD_KEYSTONE);
                EXPECT_EQ(request.rewardItem, ITEM_ABILITY_PATCH);
                ++forbiddenStoneCount;
            }
            else
            {
                EXPECT(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE
                    || request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN);
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
                if(request.lotRole == 0)
                {
                    EXPECT_GE(request.requestedItem, ITEM_RED_APRICORN);
                    EXPECT_LE(request.requestedItem, ITEM_BLACK_APRICORN);
                    EXPECT_GE(request.rewardItem, ITEM_RED_APRICORN);
                    EXPECT_LE(request.rewardItem, ITEM_BLACK_APRICORN);
                    EXPECT_GE(request.trainerNum, ITEM_RED_APRICORN);
                    EXPECT_LE(request.trainerNum, ITEM_BLACK_APRICORN);
                    EXPECT_NE(request.requestedItem, request.rewardItem);
                    EXPECT_NE(request.requestedItem, request.trainerNum);
                    EXPECT_NE(request.rewardItem, request.trainerNum);
                    ++apricornCount;
                }
                else
                {
                    EXPECT_EQ(request.recipeId, ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN);
                    EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_OLD_MAN);
                }
            }
        }
    }
    EXPECT_GE(merchantCount, 300);
    EXPECT_LE(merchantCount, 700);
    EXPECT_GE(shrineCount, 300);
    EXPECT_LE(shrineCount, 700);
    EXPECT_GE(fossilCount, 300);
    EXPECT_LE(fossilCount, 700);
    EXPECT_GE(forbiddenStoneCount, 300);
    EXPECT_LE(forbiddenStoneCount, 700);
    EXPECT_GE(apricornCount, 300);
    EXPECT_LE(apricornCount, 700);
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

TEST("Route events provide clear typed lots on every classified active route")
{
    u8 routeIdx;

    for(routeIdx = 0; routeIdx < gRogueRouteTable.routeCount; ++routeIdx)
    {
        const struct RogueRouteEncounter *route = &gRogueRouteTable.routes[routeIdx];
        const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(route->map.group, route->map.num);
        const struct MapLayout *mapLayout = mapHeader->mapLayout;
        const struct MapEvents *events = mapHeader->events;
        const struct ObjectEventTemplate *lots[ROGUE_ROUTE_SCENE_MAX_LOTS] = {0};
        u8 lotCount = 0;
        u8 baseObjectCount = 0;
        u8 objectIdx;

        EXPECT_LT(route->environment, ROGUE_ROUTE_ENVIRONMENT_COUNT);
        EXPECT_LE(events->objectEventCount, OBJECT_EVENT_TEMPLATES_COUNT);

        for(objectIdx = 0; objectIdx < events->objectEventCount; ++objectIdx)
        {
            const struct ObjectEventTemplate *object = &events->objectEvents[objectIdx];

            if(RogueRouteScenes_IsLotTemplate(object))
            {
                u16 lotId = object->trainerRange_berryTreeId;
                EXPECT_LT(lotId, ROGUE_ROUTE_SCENE_MAX_LOTS);
                EXPECT_EQ(lots[lotId], NULL);
                lots[lotId] = object;
                ++lotCount;
            }
            else
                ++baseObjectCount;
        }

        EXPECT_GE(lotCount, 5);
        EXPECT_LE(lotCount, 10);
        EXPECT_LE(baseObjectCount + ROGUE_ROUTE_SCENE_MAX_PLACEMENTS * 4, OBJECT_EVENT_TEMPLATES_COUNT);

        for(objectIdx = 0; objectIdx < ROGUE_ROUTE_SCENE_MAX_LOTS; ++objectIdx)
        {
            const struct ObjectEventTemplate *lot = lots[objectIdx];
            s16 firstOffset;
            s16 lastOffset;
            u8 size;
            s16 x;
            s16 y;
            u8 otherIdx;
            u8 warpIdx;

            if(lot == NULL)
                continue;

            size = lot->movementRangeX + 2;
            firstOffset = -(size / 2);
            lastOffset = firstOffset + size - 1;
            EXPECT_GE(lot->x + firstOffset, 0);
            EXPECT_GE(lot->y + firstOffset, 0);
            EXPECT_LT(lot->x + lastOffset, mapLayout->width);
            EXPECT_LT(lot->y + lastOffset, mapLayout->height);

            for(y = lot->y + firstOffset; y <= lot->y + lastOffset; ++y)
            {
                for(x = lot->x + firstOffset; x <= lot->x + lastOffset; ++x)
                {
                    u16 block = mapLayout->map[y * mapLayout->width + x];
                    EXPECT_EQ(block & MAPGRID_COLLISION_MASK, 0);
                    EXPECT_EQ((block & MAPGRID_ELEVATION_MASK) >> MAPGRID_ELEVATION_SHIFT, lot->elevation);
                }
            }

            // Recipes keep the tile directly south of their centre open so
            // the player can interact with the main object from below.
            y = lot->y + 1;
            EXPECT_LT(y, mapLayout->height);
            EXPECT_EQ(mapLayout->map[y * mapLayout->width + lot->x] & MAPGRID_COLLISION_MASK, 0);

            for(otherIdx = 0; otherIdx < events->objectEventCount; ++otherIdx)
            {
                const struct ObjectEventTemplate *other = &events->objectEvents[otherIdx];
                if(other != lot && !RogueRouteScenes_IsLotTemplate(other))
                    EXPECT(other->x < lot->x + firstOffset || other->x > lot->x + lastOffset
                        || other->y < lot->y + firstOffset || other->y > lot->y + lastOffset);
                else if(other != lot)
                {
                    u8 otherSize = other->movementRangeX + 2;
                    s16 otherFirst = -(otherSize / 2);
                    s16 otherLast = otherFirst + otherSize - 1;

                    EXPECT(lot->x + lastOffset < other->x + otherFirst
                        || other->x + otherLast < lot->x + firstOffset
                        || lot->y + lastOffset < other->y + otherFirst
                        || other->y + otherLast < lot->y + firstOffset);
                }
            }

            for(warpIdx = 0; warpIdx < events->warpCount; ++warpIdx)
            {
                const struct WarpEvent *warp = &events->warps[warpIdx];
                EXPECT(warp->x < lot->x + firstOffset || warp->x > lot->x + lastOffset
                    || warp->y < lot->y + firstOffset || warp->y > lot->y + lastOffset);
            }
        }
    }
}

TEST("Route scene recipes compose bounded unique route objects")
{
    static const u8 sRecipes[] =
    {
        ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER,
        ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE,
        ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER,
    };
    struct RogueAdvPath originalPath;
    u8 originalRoomId;
    u8 recipeIdx;

    SetupCurrentEvent(&originalPath, &originalRoomId);

    for(recipeIdx = 0; recipeIdx < ARRAY_COUNT(sRecipes); ++recipeIdx)
    {
        u8 recipeId = sRecipes[recipeIdx];
        struct RogueRouteSceneRequest request;
        const struct ObjectEventTemplate baseObjects[] =
        {
            {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 12, .y = 34, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_LOT_MEDIUM, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
            {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 56, .y = 78, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_LOT_MEDIUM, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
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

        SetDebugPlacement(recipeId, 1, ROGUE_ADVENTURE_QUEST_INVALID_ID);
        EXPECT(GetFirstPlacement(&request));

        RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
        EXPECT_EQ(count, expectedCount);

        for(i = 0; i < count; ++i)
        {
            EXPECT(!RogueRouteScenes_IsLotTemplate(&objects[i]));
            for(j = i + 1; j < count; ++j)
                EXPECT_NE(objects[i].localId, objects[j].localId);

            if(objects[i].script == expectedScript)
            {
                ++npcCount;
                EXPECT_EQ(objects[i].localId, 42);
                EXPECT_EQ(objects[i].x, 56);
                EXPECT_EQ(objects[i].y, 78);
                EXPECT_EQ(objects[i].graphicsId, request.primaryGraphicsId);
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

TEST("Declarative route scene visibility drives insertion and restoration")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    const struct ObjectEventTemplate baseObjects[] =
    {
        {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 12, .y = 34, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_LOT_MEDIUM, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
        {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 56, .y = 78, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_LOT_MEDIUM, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
    };
    struct ObjectEventTemplate objects[8] =
    {
        {.localId = 1, .graphicsId = OBJ_EVENT_GFX_BOY_1, .x = 2, .y = 2},
        baseObjects[0],
        baseObjects[1],
    };
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    bool8 originalHidden = FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    u8 count = 3;
    u8 i;
    bool8 foundConditionalProp;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.adventureQuests[0].definitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE;
    gRogueRun.adventureQuests[0].nodeId = 0;
    gRogueRun.adventureQuests[0].payload[1] = 1;
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP, 1, 0);

    RogueRouteScenes_SetState(0, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
    EXPECT_EQ(count, 5);
    foundConditionalProp = FALSE;
    for(i = 0; i < count; ++i)
    {
        if(objects[i].x == 56 && objects[i].y == 79)
        {
            foundConditionalProp = TRUE;
            EXPECT_EQ(objects[i].graphicsId, OBJ_EVENT_GFX_MOVING_BOX);
            EXPECT_EQ(objects[i].flagId, 0);
        }
    }
    EXPECT(foundConditionalProp);

    // Restoration consumes the same visibility definition and hides the
    // conditional object without changing the saved template's identity.
    RogueRouteScenes_SetState(0, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteScenes_RestoreObjectEvents(objects, count, baseObjects, ARRAY_COUNT(baseObjects));
    foundConditionalProp = FALSE;
    for(i = 0; i < count; ++i)
    {
        if(objects[i].x == 56 && objects[i].y == 79)
        {
            foundConditionalProp = TRUE;
            EXPECT_EQ(objects[i].flagId, FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        }
    }
    EXPECT(foundConditionalProp);
    EXPECT(FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN));

    // A fresh composition in the completed state omits that same object.
    objects[0] = (struct ObjectEventTemplate){.localId = 1, .graphicsId = OBJ_EVENT_GFX_BOY_1, .x = 2, .y = 2};
    objects[1] = baseObjects[0];
    objects[2] = baseObjects[1];
    count = 3;
    RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
    EXPECT_EQ(count, 4);
    for(i = 0; i < count; ++i)
        EXPECT(objects[i].x != 56 || objects[i].y != 79);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    RestoreFlag(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN, originalHidden);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Route scene metatiles stay bounded compatible and idempotent")
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
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    gMapHeader = *mapHeader;
    gBackupMapLayout.map = sBackupMapData;
    gBackupMapLayout.width = mapHeader->mapLayout->width + MAP_OFFSET_W;
    gBackupMapLayout.height = mapHeader->mapLayout->height + MAP_OFFSET_H;
    cellCount = gBackupMapLayout.width * gBackupMapLayout.height;
    for(i = 0; i < cellCount; ++i)
        sBackupMapData[i] = METATILE_General_Grass | (3 << MAPGRID_ELEVATION_SHIFT);

    for(i = 0; i < mapHeader->events->objectEventCount; ++i)
    {
        if(RogueRouteScenes_IsLotTemplate(&mapHeader->events->objectEvents[i])
            && mapHeader->events->objectEvents[i].trainerRange_berryTreeId == 0)
            anchor = &mapHeader->events->objectEvents[i];
    }
    EXPECT_NE(anchor, NULL);
    x = anchor->x + MAP_OFFSET;
    y = anchor->y + MAP_OFFSET;

    RogueRouteScenes_ApplyMetatiles();
    EXPECT_EQ(MapGridGetMetatileIdAt(x - 1, y), METATILE_General_Grass_Stone);
    EXPECT_EQ(MapGridGetMetatileIdAt(x + 1, y), METATILE_General_Grass_Stone);
    EXPECT_EQ(MapGridGetMetatileIdAt(x, y - 1), METATILE_General_Grass_Stone);
    EXPECT_EQ(MapGridGetMetatileIdAt(x, y), METATILE_General_Grass);
    EXPECT_EQ(MapGridGetElevationAt(x, y - 1), 3);

    RogueRouteScenes_ApplyMetatiles();
    EXPECT_EQ(MapGridGetMetatileIdAt(x, y - 1), METATILE_General_Grass_Stone);

    MapGridSetMetatileIdAt(x + 1, y, METATILE_General_TallGrass);
    RogueRouteScenes_ApplyMetatiles();
    EXPECT_EQ(MapGridGetMetatileIdAt(x + 1, y), METATILE_General_TallGrass);

    for(i = 0; i < cellCount; ++i)
        sBackupMapData[i] = METATILE_General_Grass | (3 << MAPGRID_ELEVATION_SHIFT);
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
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
        if(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, &shrine))
            break;
    }
    SelectPlacement(&shrine);

    EXPECT_EQ(shrine.recipeId, ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE);
    EXPECT_EQ((u8)shrine.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
    EXPECT_EQ(shrine.rewardAmount, 8000);
    EXPECT_EQ(RogueRouteScenes_GetState(shrine.sceneSlot), ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
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
    EXPECT_EQ(RogueRouteScenes_GetState(shrine.sceneSlot), ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);

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
    EXPECT_EQ(RogueRouteScenes_GetState(shrine.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);

    // A same-route quickload reconstructs the accepted shrine even though its
    // active Curse would normally make new shrines ineligible.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    {
        struct RogueRouteSceneRequest restoredShrine;
        EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, &restoredShrine));
        EXPECT_EQ(shrine.recipeId, restoredShrine.recipeId);
        EXPECT_EQ(shrine.lotId, restoredShrine.lotId);
        EXPECT_EQ(shrine.lotRole, restoredShrine.lotRole);
        EXPECT_EQ(shrine.sceneSlot, restoredShrine.sceneSlot);
        EXPECT_EQ(shrine.requestedItem, restoredShrine.requestedItem);
        EXPECT_EQ(shrine.rewardAmount, restoredShrine.rewardAmount);
    }
    EXPECT_EQ(RogueRouteScenes_GetState(shrine.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    Rogue_SetCurrentDifficulty(ROGUE_CHAMP_START_DIFFICULTY);
    RogueRouteScenes_OnExitRoute();
    gRogueAdvPath.roomCount = 2;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = 1;
    gRogueAdvPath.rooms[1].rngSeed = 701;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(!GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, &shrine));

    Rogue_ClearTemporaryDarkDealCurse();
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    Rogue_SetCurrentDifficulty(13);
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, &shrine));
    EXPECT_EQ(shrine.rewardAmount, ROGUE_HEXED_SHRINE_REWARD_MAX);

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
    {
        u16 seed;

        for(seed = 1; seed != 0; ++seed)
        {
            gRogueAdvPath.rooms[0].rngSeed = seed;
            RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
            RogueRouteScenes_OnEnterRoute();
            if(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER, &offer))
                break;
        }
        EXPECT_NE(seed, 0);
    }
    SelectPlacement(&offer);

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
    {
        struct RogueRouteSceneRequest restoredOffer;
        EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER, &restoredOffer));
        EXPECT_EQ(offer.sceneSlot, restoredOffer.sceneSlot);
        EXPECT_EQ(offer.lotId, restoredOffer.lotId);
        EXPECT_EQ(offer.lotRole, restoredOffer.lotRole);
        EXPECT_EQ(offer.requestedItem, restoredOffer.requestedItem);
        EXPECT_EQ(offer.rewardItem, restoredOffer.rewardItem);
        EXPECT_EQ(offer.rewardAmount, restoredOffer.rewardAmount);
        EXPECT_EQ(memcmp(&offer, &restoredOffer, sizeof(offer)), 0);
    }

    RogueRouteScenes_OnExitRoute();
    gRogueAdvPath.roomCount = 2;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = 1;
    gRogueAdvPath.rooms[1].rngSeed = 301;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION, &restoration));
    SelectPlacement(&restoration);
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
    EXPECT_EQ(RogueRouteScenes_GetState(restoration.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
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
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION, 0, questId);
    EXPECT(GetFirstPlacement(&restoration));
    SelectPlacement(&restoration);
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
        EXPECT_NE(customType0, RoguePokedex_GetSpeciesType(restoration.rewardItem, 0));
        EXPECT_NE(customType0, RoguePokedex_GetSpeciesType(restoration.rewardItem, 1));
    }
    if(IS_STANDARD_TYPE(customType1))
    {
        EXPECT_NE(customType1, RoguePokedex_GetSpeciesType(restoration.rewardItem, 0));
        EXPECT_NE(customType1, RoguePokedex_GetSpeciesType(restoration.rewardItem, 1));
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

TEST("Forbidden Stone binds three souls before its Spiritomb payoff")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct Pokemon originalEnemyParty[PARTY_SIZE];
    struct RogueRouteSceneRequest offer;
    struct RogueRouteSceneRequest souls[ROGUE_FORBIDDEN_STONE_SOUL_COUNT];
    struct RogueRouteSceneRequest payoff;
    RAND_TYPE originalStandardRng = gRngValue;
    RAND_TYPE rngBefore;
    u32 originalMoney = GetMoney(&gSaveBlock1Ptr->money);
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    u8 originalLevelOffset = gRogueRun.currentLevelOffset;
    u8 originalBattleOutcome = gBattleOutcome;
    u8 questId;
    u16 itemId;
    u8 i;

    memcpy(originalEnemyParty, gEnemyParty, sizeof(originalEnemyParty));
    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, 5000);
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.routeSceneRoomId = 0;
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    gRogueAdvPath.rooms[0].rngSeed = 0x108;

    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    EXPECT(GetFirstPlacement(&offer));
    SelectPlacement(&offer);
    EXPECT_EQ(offer.requestedItem, ITEM_ODD_KEYSTONE);
    EXPECT_EQ(offer.rewardItem, ITEM_ABILITY_PATCH);
    EXPECT_EQ(offer.primaryGraphicsId, OBJ_EVENT_GFX_MISC_CHANNELER);
    EXPECT_EQ(ItemId_GetPocket(ITEM_ODD_KEYSTONE), POCKET_KEY_ITEMS);
    EXPECT_EQ(ItemId_GetPrice(ITEM_ODD_KEYSTONE), 0);

    RogueRouteEvents_TryAcceptForbiddenStoneQuest();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(CheckBagHasItem(ITEM_ODD_KEYSTONE, 1));
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    questId = RogueAdventureQuests_GetQuestIdAt(0);
    EXPECT_EQ((u8)RogueAdventureQuests_Get(questId)->definitionId, ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->target, ROGUE_FORBIDDEN_STONE_SOUL_COUNT);
    EXPECT(RogueAdventureQuests_IsItemProtected(ITEM_ODD_KEYSTONE));

    // Bind the first graph node as it would be on the next generated route.
    gRogueRun.adventureQuests[questId].routesUntilScene = 0;
    gRogueRun.adventureQuests[questId].sceneRoomId = 0;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(RogueRouteScenes_GetPlacementCount(), ROGUE_FORBIDDEN_STONE_SOUL_COUNT);
    memset(souls, 0, sizeof(souls));
    for(i = 0; i < ROGUE_FORBIDDEN_STONE_SOUL_COUNT; ++i)
    {
        struct RogueRouteSceneRequest request;

        EXPECT(RogueRouteScenes_GetPlacementRequest(i, &request));
        EXPECT_EQ(request.recipeId, ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS);
        EXPECT_EQ(request.ownerQuestId, questId);
        EXPECT_LT(request.lotRole, ROGUE_FORBIDDEN_STONE_SOUL_COUNT);
        souls[request.lotRole] = request;
    }
    EXPECT_NE(souls[0].lotId, souls[1].lotId);
    EXPECT_NE(souls[0].lotId, souls[2].lotId);
    EXPECT_NE(souls[1].lotId, souls[2].lotId);
    EXPECT_EQ(souls[0].sceneSlot, souls[1].sceneSlot);
    EXPECT_EQ(souls[0].sceneSlot, souls[2].sceneSlot);

    SelectPlacement(&souls[0]);
    RogueRouteEvents_CollectForbiddenStoneSoul();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(gSpecialVar_0x8007, 1);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->progress, 1);
    EXPECT_EQ(RogueRouteScenes_GetState(souls[0].sceneSlot), ROGUE_ROUTE_EVENT_STATE_ACTIVE);

    // Same-route reconstruction keeps the bitmask and omits the soul which
    // was already sealed, while retaining the other two scene objects.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    {
        struct ObjectEventTemplate objects[3] =
        {
            {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 10, .y = 10, .movementRangeX = ROGUE_ROUTE_SCENE_LOT_SMALL, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = souls[0].lotId, .script = Rogue_RouteEvent_Interact},
            {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 20, .y = 20, .movementRangeX = ROGUE_ROUTE_SCENE_LOT_SMALL, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = souls[1].lotId, .script = Rogue_RouteEvent_Interact},
            {.localId = 43, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 30, .y = 30, .movementRangeX = ROGUE_ROUTE_SCENE_LOT_SMALL, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = souls[2].lotId, .script = Rogue_RouteEvent_Interact},
        };
        u8 objectCount = ARRAY_COUNT(objects);

        RogueRouteScenes_ModifyObjectEvents(objects, &objectCount, ARRAY_COUNT(objects));
        EXPECT_EQ(objectCount, 2);
        for(i = 0; i < objectCount; ++i)
        {
            EXPECT_EQ(objects[i].script, Rogue_RouteEvent_ForbiddenStoneSoul);
            EXPECT_EQ(objects[i].graphicsId, OBJ_EVENT_GFX_ROUTE_GHOST);
            EXPECT_NE(objects[i].localId, 41);
        }
    }

    for(i = 1; i < ROGUE_FORBIDDEN_STONE_SOUL_COUNT; ++i)
    {
        SelectPlacement(&souls[i]);
        RogueRouteEvents_CollectForbiddenStoneSoul();
        EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
        EXPECT_EQ(gSpecialVar_0x8007, i + 1);
    }
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->progress, 7);
    EXPECT_EQ(RogueRouteScenes_GetState(souls[0].sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    // All three placements share one graph node, so route exit advances it
    // exactly once to the final encounter.
    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ((u8)RogueAdventureQuests_Get(questId)->nodeId, 1);
    gRogueAdvPath.roomCount = 2;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = 1;
    gRogueAdvPath.rooms[1].rngSeed = 0x109;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF, &payoff));
    EXPECT_EQ(payoff.ownerQuestId, questId);
    EXPECT_EQ(payoff.requestedItem, ITEM_ODD_KEYSTONE);
    EXPECT_EQ(payoff.rewardItem, ITEM_ABILITY_PATCH);
    EXPECT_EQ(payoff.rewardAmount, ROGUE_FORBIDDEN_STONE_REWARD_MONEY);
    SelectPlacement(&payoff);

    SeedRng(0x4242);
    rngBefore = gRngValue;
    RogueRouteEvents_PrepareForbiddenStoneBattle();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(&gRngValue, &rngBefore, sizeof(rngBefore)), 0);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_SPIRITOMB);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_LEVEL), Rogue_CalculateBossMonLvl());
    EXPECT_EQ(RogueGift_GetCustomMonId(&gEnemyParty[0]), 0);
    EXPECT(Rogue_IsShrineChallengeActive());
    gBattleOutcome = B_OUTCOME_WON;
    Rogue_Battle_EndWildBattle();
    EXPECT(!Rogue_IsShrineChallengeActive());

    // The win is retained when the wallet rejects the atomic payoff, and the
    // boss is not repeated when the player returns with room for both rewards.
    SetMoney(&gSaveBlock1Ptr->money, MAX_MONEY);
    RogueRouteEvents_FinishForbiddenStoneBattle();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_MONEY_FULL);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->progress, 1);
    EXPECT_EQ(RogueRouteScenes_GetState(payoff.sceneSlot), ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
    EXPECT(CheckBagHasItem(ITEM_ODD_KEYSTONE, 1));
    EXPECT(!CheckBagHasItem(ITEM_ABILITY_PATCH, 1));

    SetMoney(&gSaveBlock1Ptr->money, 5000);
    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && CheckBagHasSpace(ITEM_ABILITY_PATCH, 1); ++itemId)
    {
        if(ItemId_GetPocket(itemId) == ItemId_GetPocket(ITEM_ABILITY_PATCH) && itemId != ITEM_ABILITY_PATCH)
            AddBagItem(itemId, 1);
    }
    EXPECT(!CheckBagHasSpace(ITEM_ABILITY_PATCH, 1));
    RogueRouteEvents_FinishForbiddenStoneBattle();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_NO_SPACE);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 5000);
    EXPECT(CheckBagHasItem(ITEM_ODD_KEYSTONE, 1));
    EXPECT(!CheckBagHasItem(ITEM_ABILITY_PATCH, 1));

    ClearBag();
    EXPECT(AddBagItem(ITEM_ODD_KEYSTONE, 1));
    SetMoney(&gSaveBlock1Ptr->money, 5000);
    RogueRouteEvents_FinishForbiddenStoneBattle();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 5000 + ROGUE_FORBIDDEN_STONE_REWARD_MONEY);
    EXPECT(!CheckBagHasItem(ITEM_ODD_KEYSTONE, 1));
    EXPECT(CheckBagHasItem(ITEM_ABILITY_PATCH, 1));
    EXPECT_EQ(RogueRouteScenes_GetState(payoff.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF, &payoff));
    EXPECT_EQ(RogueRouteScenes_GetState(payoff.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    memcpy(gEnemyParty, originalEnemyParty, sizeof(originalEnemyParty));
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    gRogueRun.currentLevelOffset = originalLevelOffset;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    gBattleOutcome = originalBattleOutcome;
    gRngValue = originalStandardRng;
    SetMoney(&gSaveBlock1Ptr->money, originalMoney);
    ClearBag();
}

TEST("Apricorn Crafting can finish locally or follow the player to a later route")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueRouteSceneRequest grove;
    struct RogueRouteSceneRequest artisan;
    struct ObjectEventTemplate objects[6] =
    {
        {.localId = 40, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 10, .y = 10, .movementRangeX = ROGUE_ROUTE_SCENE_LOT_MEDIUM, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
        {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 20, .y = 20, .movementRangeX = ROGUE_ROUTE_SCENE_LOT_MEDIUM, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
    };
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    u8 objectCount = 2;
    u8 questId;
    u16 apricorn;
    u16 ball;
    u16 itemId;
    u8 treeCount = 0;
    u8 artisanCount = 0;
    u8 propCount = 0;
    u8 i;

    ClearBag();
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.routeSceneRoomId = 0;
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    gRogueAdvPath.rooms[0].rngSeed = 0xA91C;

    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_DebugSetPlacement(0, ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN, 0, 0, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    RogueRouteScenes_DebugSetPlacement(1, ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN, 1, 1, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    EXPECT(RogueRouteScenes_GetPlacementRequest(0, &grove));
    EXPECT(RogueRouteScenes_GetPlacementRequest(1, &artisan));
    EXPECT_EQ(grove.recipeId, ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN);
    EXPECT_EQ(grove.lotRole, 0);
    EXPECT_EQ(artisan.lotRole, 1);
    EXPECT_EQ(grove.sceneSlot, artisan.sceneSlot);
    EXPECT_NE(grove.requestedItem, grove.rewardItem);
    EXPECT_NE(grove.requestedItem, grove.trainerNum);
    EXPECT_NE(grove.rewardItem, grove.trainerNum);

    RogueRouteScenes_ModifyObjectEvents(objects, &objectCount, ARRAY_COUNT(objects));
    EXPECT_EQ(objectCount, 6);
    for(i = 0; i < objectCount; ++i)
    {
        u8 j;

        for(j = i + 1; j < objectCount; ++j)
            EXPECT_NE(objects[i].localId, objects[j].localId);
        if(objects[i].script == Rogue_RouteEvent_ApricornTree)
            ++treeCount;
        else if(objects[i].script == Rogue_RouteEvent_ApricornArtisan)
            ++artisanCount;
        else if(objects[i].script == Rogue_RouteEvent_ApricornProp)
            ++propCount;
    }
    EXPECT_EQ(treeCount, ROGUE_APRICORN_CHOICE_COUNT);
    EXPECT_EQ(artisanCount, 1);
    EXPECT_EQ(propCount, 2);

    SelectPlacementProp(&grove, 2);
    apricorn = grove.trainerNum;
    ball = GetExpectedApricornBall(apricorn);
    EXPECT_NE(ball, ITEM_NONE);
    RogueRouteEvents_TryChooseApricorn();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(CheckBagHasItem(apricorn, 1));
    EXPECT(RogueAdventureQuests_IsItemProtected(apricorn));
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    questId = RogueAdventureQuests_GetQuestIdAt(0);
    EXPECT_EQ((u8)RogueAdventureQuests_Get(questId)->definitionId, ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->payload[0], apricorn);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->payload[1], ball);

    EXPECT(RogueRouteScenes_GetPlacementRequest(1, &artisan));
    EXPECT_EQ(artisan.requestedItem, apricorn);
    EXPECT_EQ(artisan.rewardItem, ball);
    SelectPlacement(&artisan);
    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && CheckBagHasSpace(ball, ROGUE_APRICORN_BALL_REWARD_COUNT); ++itemId)
    {
        if(itemId != ball
            && itemId != apricorn
            && ItemId_GetPocket(itemId) != POCKET_NONE
            && !ItemPocketUsesReservedSlots(ItemId_GetPocket(itemId)))
            AddBagItem(itemId, 1);
    }
    EXPECT(!CheckBagHasSpace(ball, ROGUE_APRICORN_BALL_REWARD_COUNT));
    RogueRouteEvents_TryCraftApricornBalls();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_NO_SPACE);
    EXPECT(CheckBagHasItem(apricorn, 1));
    EXPECT(!CheckBagHasItem(ball, ROGUE_APRICORN_BALL_REWARD_COUNT));

    ClearBag();
    EXPECT(AddBagItem(apricorn, 1));
    RogueRouteEvents_TryCraftApricornBalls();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(!CheckBagHasItem(apricorn, 1));
    EXPECT(CheckBagHasItem(ball, ROGUE_APRICORN_BALL_REWARD_COUNT));
    EXPECT_EQ(RogueRouteScenes_GetState(artisan.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);

    // The grove-only variant uses the same quest. Leaving with its Apricorn
    // schedules the ordinary artisan consumer on the next eligible route.
    ClearBag();
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueAdvPath.roomCount = 1;
    gRogueRun.adventureRoomId = 0;
    gRogueRun.routeSceneRoomId = 0;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, 0);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    gRogueAdvPath.rooms[0].rngSeed = 0xB72D;
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    EXPECT(GetFirstPlacement(&grove));
    SelectPlacementProp(&grove, 1);
    apricorn = grove.rewardItem;
    ball = GetExpectedApricornBall(apricorn);
    RogueRouteEvents_TryChooseApricorn();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    questId = RogueAdventureQuests_GetQuestIdAt(0);
    EXPECT_EQ((u8)RogueAdventureQuests_Get(questId)->routesUntilScene, 1);
    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);

    gRogueAdvPath.roomCount = 2;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = 1;
    gRogueAdvPath.rooms[1].rngSeed = 0xB72E;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN, &artisan));
    EXPECT_EQ(artisan.ownerQuestId, questId);
    EXPECT_EQ(artisan.requestedItem, apricorn);
    EXPECT_EQ(artisan.rewardItem, ball);
    SelectPlacement(&artisan);
    RogueRouteEvents_TryCraftApricornBalls();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(CheckBagHasItem(ball, ROGUE_APRICORN_BALL_REWARD_COUNT));
    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    ClearBag();
}

TEST("Stolen Trade Case completes its three route-node handoffs")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueRouteSceneRequest offer;
    struct RogueRouteSceneRequest scene;
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
        if(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER, &offer))
            break;
    }
    SelectPlacement(&offer);

    EXPECT_EQ(offer.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER);
    RogueRouteEvents_TryAcceptStolenTradeCaseQuest();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(RogueRouteScenes_GetState(offer.sceneSlot), ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    questId = RogueAdventureQuests_GetQuestIdAt(0);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->payload[1], offer.trainerNum);
    EXPECT(!CheckBagHasItem(ITEM_TRADE_CASE, 1));

    // Quicksaving on the source route retains the accepted merchant scene.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER, &offer));
    EXPECT_EQ(RogueRouteScenes_GetState(offer.sceneSlot), ROGUE_ROUTE_EVENT_STATE_ACTIVE);

    Rogue_SetCurrentDifficulty(ROGUE_CHAMP_START_DIFFICULTY);
    RogueRouteScenes_OnExitRoute();
    gRogueAdvPath.roomCount = 3;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = 1;
    gRogueAdvPath.rooms[1].rngSeed = 101;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP, &scene));
    EXPECT_EQ((u8)scene.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE);
    EXPECT_EQ(scene.ownerQuestId, questId);
    EXPECT_EQ(scene.trainerNum, offer.trainerNum);
    EXPECT_EQ(scene.primaryGraphicsId, Rogue_GetTrainerObjectEventGfx(offer.trainerNum));
    SelectPlacement(&scene);

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
    EXPECT_EQ(RogueRouteScenes_GetState(scene.sceneSlot), ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
    EXPECT_EQ((u8)RogueAdventureQuests_Get(questId)->nodeId, 0);
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
    EXPECT(!GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP, &scene));
    RogueRouteScenes_OnExitRoute();

    gRogueRun.adventureRoomId = 3;
    gRogueAdvPath.rooms[3].roomParams.roomIdx = 3;
    gRogueAdvPath.rooms[3].rngSeed = 103;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[3]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP, &scene));
    EXPECT_EQ(RogueRouteScenes_GetState(scene.sceneSlot), ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
    SelectPlacement(&scene);

    ClearBag();
    RogueRouteEvents_FinishStolenTradeCaseBattle();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ((u8)RogueAdventureQuests_Get(questId)->nodeId, 0);
    EXPECT(CheckBagHasItem(ITEM_TRADE_CASE, 1));
    EXPECT_EQ(RogueRouteScenes_GetState(scene.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    // A same-route quickload restores the completed camp rather than binding
    // the next quest node to this room.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[3]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP, &scene));
    EXPECT_EQ(RogueRouteScenes_GetState(scene.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ((u8)RogueAdventureQuests_Get(questId)->nodeId, 1);
    gRogueRun.adventureRoomId = 4;
    gRogueAdvPath.rooms[4].roomParams.roomIdx = 4;
    gRogueAdvPath.rooms[4].rngSeed = 104;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[4]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF, &scene));
    EXPECT_EQ(scene.requestedItem, ITEM_TRADE_CASE);
    EXPECT_EQ(scene.rewardItem, ITEM_BIG_POKEBLOCK_BUNDLE);
    EXPECT_EQ(scene.primaryGraphicsId, OBJ_EVENT_GFX_MART_EMPLOYEE);
    SelectPlacement(&scene);
    EXPECT(RemoveBagItem(ITEM_TRADE_CASE, 1));
    RogueRouteEvents_TryClaimStolenTradeCaseReward();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 12345);
    EXPECT(!CheckBagHasItem(ITEM_BIG_POKEBLOCK_BUNDLE, 1));
    EXPECT(AddBagItem(ITEM_TRADE_CASE, 1));

    SetMoney(&gSaveBlock1Ptr->money, MAX_MONEY - ROGUE_STOLEN_TRADE_CASE_REWARD_MONEY + 1);
    RogueRouteEvents_TryClaimStolenTradeCaseReward();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_MONEY_FULL);
    EXPECT(CheckBagHasItem(ITEM_TRADE_CASE, 1));
    EXPECT(!CheckBagHasItem(ITEM_BIG_POKEBLOCK_BUNDLE, 1));
    EXPECT_EQ(
        GetMoney(&gSaveBlock1Ptr->money),
        MAX_MONEY - ROGUE_STOLEN_TRADE_CASE_REWARD_MONEY + 1);
    SetMoney(&gSaveBlock1Ptr->money, 12345);

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
    EXPECT_EQ((u8)RogueAdventureQuests_Get(questId)->nodeId, 1);
    EXPECT(FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED));
    EXPECT_EQ(RogueRouteScenes_GetState(scene.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    // The completed payoff also survives a same-route quickload so its
    // acknowledgement remains available until the player leaves.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[4]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF, &scene));
    EXPECT_EQ(RogueRouteScenes_GetState(scene.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);

    RogueRouteScenes_OnExitRoute();
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);
    gRogueRun.adventureRoomId = 5;
    gRogueAdvPath.rooms[5].roomParams.roomIdx = 5;
    gRogueAdvPath.rooms[5].rngSeed = 105;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[5]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(!GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF, &scene));

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

TEST("Route director composes three pending quest consumers and preserves them on reload")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueAdventureQuestCreateParams params = {0};
    struct RogueRouteScenePlan firstPlan;
    bool8 seenLots[ROGUE_ROUTE_SCENE_MAX_LOTS] = {FALSE};
    bool8 seenSlots[ROGUE_ROUTE_SCENE_MAX_PLACEMENTS] = {FALSE};
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    u8 i;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 0;
    gRogueAdvPath.rooms[0].rngSeed = 0x5151;
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;

    for(i = 0; i < 4; ++i)
    {
        params.payload[1] = i + 1;
        EXPECT_EQ(RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params), i);
        gRogueRun.adventureQuests[i].routesUntilScene = 0;
        gRogueRun.adventureQuests[i].sceneRoomId = ROGUE_ADVENTURE_QUEST_INVALID_ROOM;
    }

    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(RogueRouteScenes_GetPlacementCount(), 3);
    for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
    {
        struct RogueRouteSceneRequest request;

        EXPECT(RogueRouteScenes_GetPlacementRequest(i, &request));
        EXPECT_EQ(request.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP);
        EXPECT_EQ(request.ownerQuestId, i);
        EXPECT_LT(request.lotId, ROGUE_ROUTE_SCENE_MAX_LOTS);
        EXPECT_LT(request.sceneSlot, ROGUE_ROUTE_SCENE_MAX_PLACEMENTS);
        EXPECT(!seenLots[request.lotId]);
        EXPECT(!seenSlots[request.sceneSlot]);
        seenLots[request.lotId] = TRUE;
        seenSlots[request.sceneSlot] = TRUE;
    }
    EXPECT_EQ((u8)gRogueRun.adventureQuests[3].sceneRoomId, ROGUE_ADVENTURE_QUEST_INVALID_ROOM);

    firstPlan = gRogueAdvPath.rooms[0].routeScenePlan;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT_EQ(memcmp(&firstPlan, &gRogueAdvPath.rooms[0].routeScenePlan, sizeof(firstPlan)), 0);
    EXPECT_EQ((u8)gRogueRun.adventureQuests[3].sceneRoomId, ROGUE_ADVENTURE_QUEST_INVALID_ROOM);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Adventure quest node signals update progress and complete only their targeted node")
{
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueAdventureQuestCreateParams params = {0};
    const struct RogueAdventureQuest *quest;
    u8 stolenQuestId;
    u8 otherQuestId;
    u8 forbiddenQuestId;

    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));

    stolenQuestId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params);
    otherQuestId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params);
    EXPECT_NE(stolenQuestId, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    EXPECT_NE(otherQuestId, ROGUE_ADVENTURE_QUEST_INVALID_ID);

    EXPECT(RogueAdventureQuests_EmitSignalForQuest(
        stolenQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        1));
    EXPECT_EQ(RogueAdventureQuests_Get(stolenQuestId)->progress, 1);
    EXPECT_EQ(RogueAdventureQuests_Get(otherQuestId)->progress, 0);

    RogueAdventureQuests_EmitSignal(ROGUE_ADVENTURE_QUEST_SIGNAL_TRAINER_DEFEATED, 1);
    EXPECT_EQ((u8)RogueAdventureQuests_Get(stolenQuestId)->nodeId, 0);
    EXPECT_EQ((u8)RogueAdventureQuests_Get(otherQuestId)->nodeId, 0);

    EXPECT(RogueAdventureQuests_EmitSignalForQuest(
        stolenQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
        1));
    quest = RogueAdventureQuests_Get(stolenQuestId);
    EXPECT(quest != NULL);
    EXPECT_EQ((u8)quest->nodeId, 1);
    EXPECT_EQ(quest->progress, 0);
    EXPECT_EQ((u8)RogueAdventureQuests_Get(otherQuestId)->nodeId, 0);

    EXPECT(RogueAdventureQuests_EmitSignalForQuest(
        stolenQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
        1));
    EXPECT(RogueAdventureQuests_Get(stolenQuestId) == NULL);

    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    params.target = ROGUE_FORBIDDEN_STONE_SOUL_COUNT;
    forbiddenQuestId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE, &params);
    EXPECT_NE(forbiddenQuestId, ROGUE_ADVENTURE_QUEST_INVALID_ID);

    EXPECT(RogueAdventureQuests_EmitSignalForQuest(
        forbiddenQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        1 << 0));
    EXPECT(RogueAdventureQuests_EmitSignalForQuest(
        forbiddenQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        1 << 2));
    EXPECT(RogueAdventureQuests_EmitSignalForQuest(
        forbiddenQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        1 << 0));
    quest = RogueAdventureQuests_Get(forbiddenQuestId);
    EXPECT_EQ(quest->progress, (1 << 0) | (1 << 2));
    EXPECT_EQ((u8)quest->nodeId, 0);

    EXPECT(RogueAdventureQuests_EmitSignalForQuest(
        forbiddenQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        1 << 1));
    quest = RogueAdventureQuests_Get(forbiddenQuestId);
    EXPECT_EQ(quest->progress, (1 << ROGUE_FORBIDDEN_STONE_SOUL_COUNT) - 1);
    EXPECT_EQ((u8)quest->nodeId, 0);

    EXPECT(RogueAdventureQuests_EmitSignalForQuest(
        forbiddenQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
        1));
    quest = RogueAdventureQuests_Get(forbiddenQuestId);
    EXPECT_EQ((u8)quest->nodeId, 1);
    EXPECT_EQ(quest->progress, 0);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
}

TEST("Adventure quest runtime packs 64 independent quest records into 512 bytes")
{
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueAdventureQuestCreateParams params = {0};
    u8 i;

    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    EXPECT_EQ((u32)sizeof(gRogueRun.adventureQuests), 512);

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        params.payload[0] = i;
        params.payload[1] = 0xA000 | i;
        EXPECT_EQ(
            RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params),
            i);
        gRogueRun.adventureQuests[i].routesUntilScene = i % (ROGUE_ADVENTURE_QUEST_MAX_ROUTE_DELAY + 1);
        gRogueRun.adventureQuests[i].sceneRoomId = i % ADVPATH_ROOM_COUNT;
    }

    EXPECT_EQ(
        RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params),
        ROGUE_ADVENTURE_QUEST_INVALID_ID);
    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(i);

        EXPECT(quest != NULL);
        EXPECT_EQ(quest->payload[0], i);
        EXPECT_EQ(quest->payload[1], 0xA000 | i);
        EXPECT_EQ((u8)quest->definitionId, ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE);
        EXPECT_EQ((u8)quest->nodeId, 0);
        EXPECT_EQ((u8)quest->routesUntilScene, i % (ROGUE_ADVENTURE_QUEST_MAX_ROUTE_DELAY + 1));
        EXPECT_EQ((u8)quest->sceneRoomId, i % ADVPATH_ROOM_COUNT);
    }

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
}

TEST("All existing route events are registered through declarative tables")
{
    static const u8 sExpectedQuestDefinitions[ROGUE_ROUTE_SCENE_RECIPE_COUNT] =
    {
        [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER] = ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE,
        [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP] = ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE,
        [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF] = ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE,
        [ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER] = ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL,
        [ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION] = ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL,
        [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER] = ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE,
        [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS] = ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE,
        [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF] = ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE,
        [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE] = ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING,
        [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN] = ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING,
        [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN] = ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING,
    };
    static const u8 sExpectedFallbackRecipes[] =
    {
        ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER,
        ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE,
        ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER,
        ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER,
        ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE,
        ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN,
    };
    u8 i;

    for(i = 1; i < ROGUE_ROUTE_SCENE_RECIPE_COUNT; ++i)
    {
        const struct RogueRouteRecipeDefinition *recipe = RogueRouteEvents_GetRecipeDefinition(i);

        EXPECT(recipe != NULL);
        EXPECT(recipe->lots != NULL);
        EXPECT(recipe->lotCount != 0);
        EXPECT_EQ(recipe->linkedQuestDefinitionId, sExpectedQuestDefinitions[i]);
    }

    EXPECT_EQ(RogueRouteEvents_GetFallbackCount(), ARRAY_COUNT(sExpectedFallbackRecipes));
    for(i = 0; i < ARRAY_COUNT(sExpectedFallbackRecipes); ++i)
    {
        const struct RogueRouteFallbackDefinition *fallback = RogueRouteEvents_GetFallbackDefinition(i);

        EXPECT(fallback != NULL);
        EXPECT_EQ(fallback->recipeId, sExpectedFallbackRecipes[i]);
        EXPECT(fallback->isEligible != NULL);
        EXPECT_EQ(fallback->weight, i < 4 ? 50 : 25);
    }
    EXPECT_EQ(
        RogueRouteEvents_GetFallbackDefinition(4)->familyId,
        RogueRouteEvents_GetFallbackDefinition(5)->familyId);

    EXPECT((RogueRouteEvents_GetRecipeDefinition(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP)->flags
        & ROUTE_SCENE_RECIPE_FLAG_EXCLUDE_DYNAMIC_TRAINER) != 0);
    EXPECT_EQ(
        RogueRouteEvents_GetRecipeDefinition(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF)->resumeBehavior,
        ROUTE_SCENE_RESUME_REWARD_PENDING_IF_PROGRESS);
    EXPECT_EQ(
        RogueRouteEvents_GetRecipeDefinition(ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS)->resumeBehavior,
        ROUTE_SCENE_RESUME_COMPLETED_IF_TARGET_MET);
    EXPECT((RogueRouteEvents_GetRecipeDefinition(ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN)->flags
        & ROUTE_SCENE_RECIPE_FLAG_COMPLETE_LINKED_QUEST_ON_EXIT) != 0);
}
