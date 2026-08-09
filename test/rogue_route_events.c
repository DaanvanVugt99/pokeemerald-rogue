#include "global.h"

#include "battle.h"
#include "constants/battle.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/metatile_labels.h"
#include "constants/metatile_behaviors.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/pokemon.h"
#include "constants/rogue_adventure_quests.h"
#include "constants/rogue_route_events.h"
#include "constants/rogue_route_scenes.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "fieldmap.h"
#include "item.h"
#include "metatile_behavior.h"
#include "money.h"
#include "move_relearner.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_adventurepaths.h"
#include "rogue_baked.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_event_transactions.h"
#include "rogue_gifts.h"
#include "rogue_pokedex.h"
#include "rogue_route_events.h"
#include "rogue_route_scene_internal.h"
#include "rogue_route_scenes.h"
#include "rogue_trainers.h"
#include "shop.h"
#include "strings.h"
#include "string_util.h"
#include "test/test.h"

extern const u8 Rogue_RouteEvent_Interact[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseOffer[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseCamp[];
extern const u8 Rogue_RouteEvent_StolenTradeCasePayoff[];
extern const u8 Rogue_RouteEvent_Prop[];
extern const u8 Rogue_RouteEvent_HexedShrine[];
extern const u8 Rogue_RouteEvent_AnomalousFossilOffer[];
extern const u8 Rogue_RouteEvent_AnomalousFossilRestoration[];
extern const u8 Rogue_RouteEvent_AnomalousFossilProp[];
extern const u8 Rogue_RouteEvent_FossilWorkbench[];
extern const u8 Rogue_RouteEvent_ForbiddenStoneOffer[];
extern const u8 Rogue_RouteEvent_ForbiddenStoneSoul[];
extern const u8 Rogue_RouteEvent_ForbiddenStonePayoff[];
extern const u8 Rogue_RouteEvent_ApricornTree[];
extern const u8 Rogue_RouteEvent_ApricornArtisan[];
extern const u8 Rogue_RouteEvent_ApricornProp[];
extern const u8 Rogue_RouteEvent_UnboundTutor[];
extern const u8 Rogue_RouteEvent_UnboundTutorProp[];
extern const u8 Rogue_RouteEvent_TravelingMerchant[];
extern const u8 Rogue_RouteEvent_BreedersExchange[];
extern const u8 Rogue_RouteEvent_BreedersExchangePokemon[];
extern const u8 Rogue_RouteEvent_BuriedCacheArchaeologist[];
extern const u8 Rogue_RouteEvent_BuriedCacheSupplies[];
extern const u8 Rogue_RouteEvent_BuriedCacheSite[];
extern const u8 Rogue_RouteEvent_TideSalvage[];

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

static u8 GetRouteTestMetatileBehavior(const struct MapLayout *mapLayout, u16 block)
{
    u16 metatileId = block & MAPGRID_METATILE_ID_MASK;
    const u16 *attributes;

    if(metatileId < NUM_METATILES_IN_PRIMARY)
    {
        attributes = mapLayout->primaryTileset->metatileAttributes;
        return attributes[metatileId] & METATILE_ATTR_BEHAVIOR_MASK;
    }

    attributes = Rogue_ModifyOverworldTileset(mapLayout->secondaryTileset)->metatileAttributes;
    return attributes[metatileId - NUM_METATILES_IN_PRIMARY] & METATILE_ATTR_BEHAVIOR_MASK;
}

static bool8 IsTerrainForbiddenForNormalRouteLot(u8 behavior)
{
    return MetatileBehavior_IsSurfableWaterOrUnderwater(behavior)
        || MetatileBehavior_IsForcedMovementTile(behavior)
        || MetatileBehavior_IsIce(behavior)
        || MetatileBehavior_IsWaterfall(behavior)
        || behavior == MB_WATER_DOOR
        || behavior == MB_WATER_SOUTH_ARROW_WARP
        || MetatileBehavior_IsNorthwardCurrent(behavior)
        || MetatileBehavior_IsSouthwardCurrent(behavior)
        || MetatileBehavior_IsWestwardCurrent(behavior)
        || MetatileBehavior_IsEastwardCurrent(behavior);
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

static void ClearAdventureQuestSlotsForRouteTest(void)
{
    u8 i;

    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        gRogueRun.adventureQuests[i].definitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_NONE;
        gRogueRun.adventureQuests[i].sceneRoomId = ROGUE_ADVENTURE_QUEST_INVALID_ROOM;
    }
}

static void ExpectRouteScenePlansEqual(const struct RogueRouteScenePlan *a, const struct RogueRouteScenePlan *b)
{
    EXPECT_EQ(a->placements[0].packed, b->placements[0].packed);
    EXPECT_EQ(a->placements[1].packed, b->placements[1].packed);
    EXPECT_EQ(a->placements[2].packed, b->placements[2].packed);
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

static bool8 RouteHasDecorSpotForLot(const struct MapEvents *events, u8 groupId, const struct RogueRouteSceneLotDefinition *lot)
{
    u8 objectIdx;

    if(lot->decorSpotType == ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT)
        return TRUE;

    for(objectIdx = 0; objectIdx < events->objectEventCount; ++objectIdx)
    {
        const struct ObjectEventTemplate *object = &events->objectEvents[objectIdx];

        if(RogueRouteScenes_IsLotTemplate(object)
            && object->trainerRange_berryTreeId == groupId
            && object->movementRangeX == lot->decorSpotType
            && (lot->terrainMask & ROGUE_ROUTE_SCENE_TERRAIN_MASK(object->movementRangeY)) != 0)
            return TRUE;
    }

    return FALSE;
}

static u8 CountEligibleRouteGroupsForLot(u8 routeIdx, const struct RogueRouteSceneLotDefinition *lot)
{
    const struct RogueRouteEncounter *route = &gRogueRouteTable.routes[routeIdx];
    const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(route->map.group, route->map.num);
    const struct MapEvents *events = mapHeader->events;
    u16 countedGroups = 0;
    u8 count = 0;
    u8 objectIdx;

    for(objectIdx = 0; objectIdx < events->objectEventCount; ++objectIdx)
    {
        const struct ObjectEventTemplate *object = &events->objectEvents[objectIdx];
        u8 groupId = object->trainerRange_berryTreeId;

        if(RogueRouteScenes_IsLotTemplate(object)
            && groupId < ROGUE_ROUTE_SCENE_MAX_SPOT_GROUPS
            && (countedGroups & (1 << groupId)) == 0
            && object->movementRangeX == lot->spotType
            && (lot->terrainMask & ROGUE_ROUTE_SCENE_TERRAIN_MASK(object->movementRangeY)) != 0
            && RouteHasDecorSpotForLot(events, groupId, lot))
        {
            countedGroups |= 1 << groupId;
            ++count;
        }
    }

    return count;
}

static bool8 RouteCanHostRecipe(u8 routeIdx, u8 recipeId)
{
    const struct RogueRouteRecipeDefinition *recipe = RogueRouteEvents_GetRecipeDefinition(recipeId);
    u16 usedGroups = 0;
    u8 role;

    for(role = 0; role < recipe->lotCount; ++role)
    {
        const struct RogueRouteSceneLotDefinition *lot = &recipe->lots[role];
        const struct RogueRouteEncounter *route = &gRogueRouteTable.routes[routeIdx];
        const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(route->map.group, route->map.num);
        const struct MapEvents *events = mapHeader->events;
        bool8 found = FALSE;
        u8 objectIdx;

        for(objectIdx = 0; objectIdx < events->objectEventCount; ++objectIdx)
        {
            const struct ObjectEventTemplate *object = &events->objectEvents[objectIdx];
            u8 groupId = object->trainerRange_berryTreeId;

            if(RogueRouteScenes_IsLotTemplate(object)
                && groupId < ROGUE_ROUTE_SCENE_MAX_SPOT_GROUPS
                && (usedGroups & (1 << groupId)) == 0
                && object->movementRangeX == lot->spotType
                && (lot->terrainMask & ROGUE_ROUTE_SCENE_TERRAIN_MASK(object->movementRangeY)) != 0
                && RouteHasDecorSpotForLot(events, groupId, lot))
            {
                usedGroups |= 1 << groupId;
                found = TRUE;
                break;
            }
        }

        if(!found)
            return FALSE;
    }

    return TRUE;
}

static u8 FindRouteForRecipe(u8 recipeId)
{
    u8 routeIdx;

    for(routeIdx = 0; routeIdx < gRogueRouteTable.routeCount; ++routeIdx)
    {
        if(RouteCanHostRecipe(routeIdx, recipeId))
            return routeIdx;
    }

    EXPECT(FALSE);
    return 0;
}

static u8 FindRouteWithRepeatedRecipeLot(u8 recipeId, u8 minCount)
{
    const struct RogueRouteRecipeDefinition *recipe = RogueRouteEvents_GetRecipeDefinition(recipeId);
    u8 routeIdx;

    for(routeIdx = 0; routeIdx < gRogueRouteTable.routeCount; ++routeIdx)
    {
        if(CountEligibleRouteGroupsForLot(routeIdx, &recipe->lots[0]) >= minCount)
            return routeIdx;
    }

    EXPECT(FALSE);
    return 0;
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
        ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT,
        ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE,
        ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE,
        ROGUE_ROUTE_SCENE_RECIPE_TIDE_SALVAGE,
    };
    struct RogueAdvPath originalPath;
    struct RogueRouteSceneRequest selected;
    struct RogueRouteSceneRequest restored;
    struct RogueRouteScenePlan selectedPlan;
    struct RogueWildEncounters originalWildEncounters = gRogueRun.wildEncounters;
    u16 originalDifficulty = Rogue_GetCurrentDifficulty();
    u16 originalTeamNum = gRogueRun.teamEncounterNum;
    u16 originalTempCurse = gRogueRun.temporaryDarkDealCurseItem;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalComplete = FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    bool8 originalRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 i;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE);
    memset(&gRogueRun.wildEncounters, 0, sizeof(gRogueRun.wildEncounters));
    gRogueRun.wildEncounters.species[0] = SPECIES_MIGHTYENA;
    gRogueRun.teamEncounterNum = TEAM_NUM_KANTO_ROCKET;
    gRogueRun.temporaryDarkDealCurseItem = ITEM_NONE;
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
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

        ExpectRouteScenePlansEqual(&gRogueAdvPath.rooms[0].routeScenePlan, &selectedPlan);
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
    RestoreFlag(FLAG_ROGUE_RUN_ACTIVE, originalRunActive);
    gRogueRun.wildEncounters = originalWildEncounters;
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Route event fallback registry is deterministic and RNG neutral")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    static struct RogueRunData originalRun;
    static struct RogueRunData baseRun;
    struct Pokemon originalParty[PARTY_SIZE];
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE originalStandardRng = gRngValue;
    u16 originalTeamNum = gRogueRun.teamEncounterNum;
    u16 originalTempCurse = gRogueRun.temporaryDarkDealCurseItem;
    u16 originalDifficulty = Rogue_GetCurrentDifficulty();
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 originalHistory = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY);
    u16 originalHistory2 = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalComplete = FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    bool8 originalRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 originalPartyCount = gPlayerPartyCount;
    u16 seed;
    u8 i;
    RAND_TYPE rogueRngBefore;
    RAND_TYPE standardRngBefore;

    originalRun = gRogueRun;
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalParty, gPlayerParty, sizeof(originalParty));
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    CreateMon(&gPlayerParty[0], SPECIES_MAGIKARP, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
    gPlayerPartyCount = 1;
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(&gRogueRun, 0, sizeof(gRogueRun));
    gRogueRun.adventureRoomId = 0;
    ClearAdventureQuestSlotsForRouteTest();
    gRogueRun.teamEncounterNum = TEAM_NUM_KANTO_ROCKET;
    gRogueRun.temporaryDarkDealCurseItem = ITEM_NONE;
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE);
    memset(&gRogueRun.wildEncounters, 0, sizeof(gRogueRun.wildEncounters));
    gRogueRun.wildEncounters.species[0] = SPECIES_MIGHTYENA;
    gRogueRun.routeSceneRoomId = 1;
    Rogue_SetCurrentDifficulty(2);
    baseRun = gRogueRun;

    for(seed = 1; seed <= 96; ++seed)
    {
        gRogueRun = baseRun;
        ClearAdventureQuestSlotsForRouteTest();
        FlagSet(FLAG_ROGUE_RUN_ACTIVE);
        FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
        VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);
        VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, 0);
        gRogueAdvPath.rooms[0].rngSeed = seed;
        VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
        SeedRng(0x2468);
        SeedRogueRng(0x1357);
        standardRngBefore = gRngValue;
        rogueRngBefore = gRngRogueValue;
        RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
        RogueRouteScenes_OnEnterRoute();
        EXPECT_EQ(memcmp(&gRngValue, &standardRngBefore, sizeof(standardRngBefore)), 0);
        EXPECT_EQ(memcmp(&gRngRogueValue, &rogueRngBefore, sizeof(rogueRngBefore)), 0);
        EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY), 0);
        EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2), 0);

        EXPECT_GE(RogueRouteScenes_GetPlacementCount(), 1);
        EXPECT_LE(RogueRouteScenes_GetPlacementCount(), ROGUE_ROUTE_SCENE_MAX_PLACEMENTS);

        for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
        {
            struct RogueRouteSceneRequest request;

            EXPECT(RogueRouteScenes_GetPlacementRequest(i, &request));
            if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER)
            {
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
                EXPECT_EQ(request.requestedItem, ITEM_TRADE_CASE);
                EXPECT((gRogueTrainers[request.trainerNum].classFlags & GetActiveTeamClassFlag(gRogueRun.teamEncounterNum)) != 0);
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE)
            {
                u8 curseIdx;

                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
                EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_DEVIL_MAN);
                for(curseIdx = 0; curseIdx < Rogue_GetDarkDealCurseCount(); ++curseIdx)
                    if(request.requestedItem == Rogue_SelectDarkDealCurseItem(curseIdx))
                        break;
                EXPECT_NE(curseIdx, Rogue_GetDarkDealCurseCount());
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER)
            {
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
                EXPECT_EQ(RogueAdventureQuests_GetFossilSpecies(request.requestedItem), request.rewardItem);
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER)
            {
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR);
                EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_MISC_CHANNELER);
                EXPECT_EQ(request.requestedItem, ITEM_ODD_KEYSTONE);
                EXPECT_EQ(request.rewardItem, ITEM_ABILITY_PATCH);
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR)
            {
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
                EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_MISC_NPC_TUTOR);
                EXPECT_NE(request.requestedItem, MOVE_NONE);
                EXPECT_NE(request.rewardItem, MOVE_NONE);
                EXPECT_NE(request.trainerNum, MOVE_NONE);
                EXPECT_NE(request.requestedItem, request.rewardItem);
                EXPECT_NE(request.requestedItem, request.trainerNum);
                EXPECT_NE(request.rewardItem, request.trainerNum);
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT)
            {
                u16 category = ROGUE_SHOP_GET_CATEGORY(request.rewardAmount);

                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
                EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_MART_EMPLOYEE);
                EXPECT(ROGUE_SHOP_IS_TRAVELING_MERCHANT(request.rewardAmount));
                EXPECT_GE(category, ROGUE_SHOP_GENERAL);
                EXPECT_LE(category, ROGUE_SHOP_RARE_HELD_ITEMS);
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE)
            {
                u16 requestedBst = RoguePokedex_GetSpeciesBST(request.requestedItem);
                u16 offeredBst = RoguePokedex_GetSpeciesBST(request.rewardItem);

                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
                EXPECT_EQ(request.requestedItem, SPECIES_MIGHTYENA);
                EXPECT_NE(request.rewardItem, SPECIES_NONE);
                EXPECT(!RoguePokedex_IsSpeciesLegendary(request.rewardItem));
                EXPECT_EQ(Rogue_GetActiveEvolutionCount(request.rewardItem), Rogue_GetActiveEvolutionCount(request.requestedItem));
                EXPECT(offeredBst + 80 >= requestedBst);
                EXPECT(requestedBst + 80 >= offeredBst);
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE)
            {
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
                if(request.lotRole == 0)
                    EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_MISC_RUIN_MANIAC);
                else
                {
                    EXPECT_GE(request.primaryGraphicsId, ROUTE_SCENE_GFX_SEMANTIC_LANDMARK_2);
                    EXPECT_LE(request.primaryGraphicsId, ROUTE_SCENE_GFX_SEMANTIC_LANDMARK_0);
                }
                EXPECT_NE(request.rewardItem, ITEM_NONE);
                EXPECT_NE(request.trainerNum, SPECIES_NONE);
            }
            else if(request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_TIDE_SALVAGE)
            {
                EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
                EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_SWIMMER_M);
                EXPECT_NE(request.rewardItem, ITEM_NONE);
                EXPECT_NE(request.rewardAmount, 0);
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
                }
                else
                {
                    EXPECT_EQ(request.recipeId, ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN);
                    EXPECT_EQ(request.primaryGraphicsId, OBJ_EVENT_GFX_OLD_MAN);
                }
            }
        }
    }

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.teamEncounterNum = originalTeamNum;
    gRogueRun.temporaryDarkDealCurseItem = originalTempCurse;
    Rogue_SetCurrentDifficulty(originalDifficulty);
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, originalHistory);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, originalHistory2);
    RestoreFlag(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED, originalComplete);
    RestoreFlag(FLAG_ROGUE_RUN_ACTIVE, originalRunActive);
    gRogueRun = originalRun;
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    gRngRogueValue = originalRogueRng;
    gRngValue = originalStandardRng;
    memcpy(gPlayerParty, originalParty, sizeof(originalParty));
    gPlayerPartyCount = originalPartyCount;
}

TEST("Route event family history packs encountered and completed state without touching RNG")
{
    struct RogueRouteSceneRequest scene =
    {
        .recipeId = ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION,
    };
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE originalStandardRng = gRngValue;
    u16 originalHistory = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY);
    u16 originalHistory2 = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2);

    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, 0);
    EXPECT(!RogueRouteEvents_HasEncounteredFamily(ROGUE_ROUTE_FAMILY_ANOMALOUS_FOSSIL));
    EXPECT(!RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_ANOMALOUS_FOSSIL));

    RogueRouteEvents_MarkFamilyEncountered(ROGUE_ROUTE_FAMILY_ANOMALOUS_FOSSIL);
    EXPECT(RogueRouteEvents_HasEncounteredFamily(ROGUE_ROUTE_FAMILY_ANOMALOUS_FOSSIL));
    EXPECT(!RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_ANOMALOUS_FOSSIL));
    EXPECT(!RogueRouteEvents_HasEncounteredFamily(ROGUE_ROUTE_FAMILY_STOLEN_TRADE_CASE));

    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
    EXPECT(RogueRouteEvents_HasEncounteredFamily(ROGUE_ROUTE_FAMILY_ANOMALOUS_FOSSIL));
    EXPECT(RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_ANOMALOUS_FOSSIL));
    EXPECT_EQ(memcmp(&gRngRogueValue, &originalRogueRng, sizeof(originalRogueRng)), 0);
    EXPECT_EQ(memcmp(&gRngValue, &originalStandardRng, sizeof(originalStandardRng)), 0);

    scene.recipeId = ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE;
    RogueRouteEvents_MarkFamilyEncountered(ROGUE_ROUTE_FAMILY_BURIED_CACHE);
    EXPECT(RogueRouteEvents_HasEncounteredFamily(ROGUE_ROUTE_FAMILY_BURIED_CACHE));
    EXPECT(!RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_BURIED_CACHE));
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
    EXPECT(RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_BURIED_CACHE));
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY),
        (1 << ROGUE_ROUTE_FAMILY_ANOMALOUS_FOSSIL)
            | (1 << (ROGUE_ROUTE_FAMILY_ANOMALOUS_FOSSIL + ROGUE_ROUTE_FAMILY_HISTORY_COMPLETED_SHIFT)));

    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, originalHistory);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, originalHistory2);
}

TEST("Buried cache composes three lots and resolves a recoverable wrong dig")
{
    struct RogueAdvPath originalPath;
    struct RogueRouteSceneRequest archaeologist;
    struct RogueRouteSceneRequest siteA;
    struct RogueRouteSceneRequest siteB;
    u8 observationA[256];
    u8 observationB[256];
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 originalHistory2 = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2);
    u32 originalMoney = GetMoney(&gSaveBlock1Ptr->money);
    bool8 originalFlagA = FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    bool8 originalFlagB = FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
    u8 originalRoomId;
    u16 seed;
    u16 rewardItem;
    u16 secondaryRewardItem;
    u8 cacheType;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE);
    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, 0);
    for(seed = 1; seed < 256; ++seed)
    {
        gRogueAdvPath.rooms[0].rngSeed = seed;
        RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
        RogueRouteScenes_DebugSetPlacement(0, ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE, 0, 0, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
        RogueRouteScenes_DebugSetPlacement(1, ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE, 1, 1, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
        RogueRouteScenes_DebugSetPlacement(2, ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE, 2, 2, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
        EXPECT_EQ(RogueRouteScenes_GetPlacementCount(), 3);
        EXPECT(RogueRouteScenes_GetPlacementRequest(0, &archaeologist));
        EXPECT(RogueRouteScenes_GetPlacementRequest(1, &siteA));
        EXPECT(RogueRouteScenes_GetPlacementRequest(2, &siteB));
        EXPECT_EQ(archaeologist.sceneSlot, siteA.sceneSlot);
        EXPECT_EQ(archaeologist.sceneSlot, siteB.sceneSlot);
        EXPECT_EQ(archaeologist.lotRole, 0);
        EXPECT_EQ(siteA.lotRole, 1);
        EXPECT_EQ(siteB.lotRole, 2);

        {
            struct ObjectEventTemplate objects[8] =
            {
                {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 10, .y = 10, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
                {.localId = 44, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 11, .y = 9, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_RELIC_DECOR, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
                {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 20, .y = 20, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
                {.localId = 43, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 30, .y = 30, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 2, .script = Rogue_RouteEvent_Interact},
            };
            u8 count = 4;
            u8 archaeologistCount = 0;
            u8 suppliesCount = 0;
            u8 siteCount = 0;
            u8 i;
            u8 j;

            RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
            EXPECT_EQ(count, 4);
            for(i = 0; i < count; ++i)
            {
                EXPECT_NE(objects[i].graphicsId, OBJ_EVENT_GFX_BATTLE_STATUE);
                EXPECT_NE(objects[i].graphicsId, OBJ_EVENT_GFX_BREAKABLE_ROCK);
                archaeologistCount += objects[i].script == Rogue_RouteEvent_BuriedCacheArchaeologist;
                suppliesCount += objects[i].script == Rogue_RouteEvent_BuriedCacheSupplies;
                siteCount += objects[i].script == Rogue_RouteEvent_BuriedCacheSite;
                for(j = i + 1; j < count; ++j)
                    EXPECT_NE(objects[i].localId, objects[j].localId);
            }
            EXPECT_EQ(archaeologistCount, 1);
            EXPECT_EQ(suppliesCount, 1);
            EXPECT_EQ(siteCount, 2);
            for(i = 0; i < count; ++i)
            {
                if(objects[i].script == Rogue_RouteEvent_BuriedCacheArchaeologist)
                {
                    EXPECT_EQ(objects[i].localId, 41);
                    EXPECT_EQ(objects[i].x, 10);
                    EXPECT_EQ(objects[i].y, 10);
                }
                else if(objects[i].script == Rogue_RouteEvent_BuriedCacheSupplies)
                {
                    EXPECT_EQ(objects[i].localId, 44);
                    EXPECT_EQ(objects[i].x, 11);
                    EXPECT_EQ(objects[i].y, 9);
                }
            }
        }

        SelectPlacement(&siteA);
        RogueRouteEvents_BufferBuriedCacheData();
        StringCopy(observationA, gStringVar1);
        SelectPlacement(&siteB);
        RogueRouteEvents_BufferBuriedCacheData();
        StringCopy(observationB, gStringVar1);
        EXPECT_NE(StringCompare(observationA, observationB), 0);

        SelectPlacement(&archaeologist);
        RogueRouteEvents_BufferBuriedCacheData();
        rewardItem = gSpecialVar_0x8004;
        secondaryRewardItem = gSpecialVar_0x8005;
        cacheType = gSpecialVar_0x8007;
        RogueRouteEvents_TryAcceptBuriedCache();
        EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
        EXPECT(CheckBagHasItem(ITEM_FIELD_SHOVEL, 1));
        EXPECT_EQ(RogueRouteScenes_GetState(0), ROGUE_ROUTE_EVENT_STATE_ACTIVE);

        SelectPlacement(&siteA);
        RogueRouteEvents_TryDigBuriedCache();
        if(gSpecialVar_Result == ROGUE_ROUTE_EVENT_RESULT_WRONG_SITE)
            break;

        ClearBag();
        FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
        VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, 0);
        VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, 0);
    }

    EXPECT_LT(seed, 256);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), siteA.trainerNum);
    EXPECT(FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN));
    EXPECT(!FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN));
    EXPECT(CheckBagHasItem(ITEM_FIELD_SHOVEL, 1));
    EXPECT_EQ(RogueRouteScenes_GetState(0), ROGUE_ROUTE_EVENT_STATE_ACTIVE);

    SelectPlacement(&siteA);
    RogueRouteEvents_TryDigBuriedCache();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_ALREADY_DUG);

    SelectPlacement(&siteB);
    RogueRouteEvents_TryDigBuriedCache();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(!CheckBagHasItem(ITEM_FIELD_SHOVEL, 1));
    EXPECT(CheckBagHasItem(rewardItem, 1));
    if(secondaryRewardItem != ITEM_NONE)
        EXPECT(CheckBagHasItem(secondaryRewardItem, 1));
    if(cacheType == ROGUE_BURIED_CACHE_ANCIENT)
        EXPECT_GE(GetMoney(&gSaveBlock1Ptr->money), ROGUE_BURIED_CACHE_MONEY_BASE);
    EXPECT_EQ(RogueRouteScenes_GetState(0), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT(RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_BURIED_CACHE));

    // An unfinished borrowed tool never survives the route boundary.
    ClearBag();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, 0);
    SelectPlacement(&archaeologist);
    RogueRouteEvents_TryAcceptBuriedCache();
    EXPECT(CheckBagHasItem(ITEM_FIELD_SHOVEL, 1));
    RogueRouteEvents_OnExitScene(&archaeologist);
    EXPECT(!CheckBagHasItem(ITEM_FIELD_SHOVEL, 1));

    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, originalMoney);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, originalHistory2);
    RestoreFlag(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN, originalFlagA);
    RestoreFlag(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN, originalFlagB);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Route events provide typed exact spots on every classified active route")
{
    bool8 hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT] = {FALSE};
    bool8 hasEligibleStall = FALSE;
    bool8 hasEligibleCamp = FALSE;
    bool8 hasEligibleWorkbench = FALSE;
    bool8 hasEligibleRelic = FALSE;
    bool8 hasEligibleCreature = FALSE;
    bool8 hasEligibleBuriedCache = FALSE;
    bool8 hasEligibleApricornGrove = FALSE;
    bool8 hasEligibleTideSalvage = FALSE;
    u8 routeIdx;

    for(routeIdx = 0; routeIdx < gRogueRouteTable.routeCount; ++routeIdx)
    {
        const struct RogueRouteEncounter *route = &gRogueRouteTable.routes[routeIdx];
        const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(route->map.group, route->map.num);
        const struct MapLayout *mapLayout = mapHeader->mapLayout;
        const struct MapEvents *events = mapHeader->events;
        bool8 hasPairedSpot[ROGUE_ROUTE_SCENE_MAX_SPOT_GROUPS][ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT] = {FALSE};
        u8 spotTypeCount[ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT] = {0};
        u16 groupMask = 0;
        u8 spotCount = 0;
        u8 baseObjectCount = 0;
        u8 objectIdx;

        EXPECT_LT(route->environment, ROGUE_ROUTE_ENVIRONMENT_COUNT);
        EXPECT_LE(events->objectEventCount, OBJECT_EVENT_TEMPLATES_COUNT);

        for(objectIdx = 0; objectIdx < events->objectEventCount; ++objectIdx)
        {
            const struct ObjectEventTemplate *object = &events->objectEvents[objectIdx];

            if(RogueRouteScenes_IsLotTemplate(object))
            {
                u16 groupId = object->trainerRange_berryTreeId;
                u16 block;

                EXPECT_LT(groupId, ROGUE_ROUTE_SCENE_MAX_SPOT_GROUPS);
                EXPECT_LT((u8)object->movementRangeX, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
                EXPECT_LT((u8)object->movementRangeY, ROGUE_ROUTE_SCENE_TERRAIN_COUNT);
                EXPECT_GE(object->x, 0);
                EXPECT_GE(object->y, 0);
                EXPECT_LT(object->x, mapLayout->width);
                EXPECT_LT(object->y, mapLayout->height);
                EXPECT_NE(object->localId, 0);

                block = mapLayout->map[object->y * mapLayout->width + object->x];
                EXPECT_EQ((block & MAPGRID_ELEVATION_MASK) >> MAPGRID_ELEVATION_SHIFT, object->elevation);
                if(object->movementRangeY == ROGUE_ROUTE_SCENE_TERRAIN_LAND
                    || object->movementRangeY == ROGUE_ROUTE_SCENE_TERRAIN_CAVE)
                    EXPECT(!IsTerrainForbiddenForNormalRouteLot(GetRouteTestMetatileBehavior(mapLayout, block)));

                groupMask |= 1 << groupId;
                ++spotCount;
                ++spotTypeCount[object->movementRangeX];
                hasGlobalSpotType[object->movementRangeX] = TRUE;
                hasPairedSpot[groupId][object->movementRangeX] = TRUE;
            }
            else
                ++baseObjectCount;
        }

        EXPECT_GE(spotCount, 4);
        EXPECT_LE(spotCount, ROGUE_ROUTE_SCENE_MAX_SPOTS);
        EXPECT_LE(baseObjectCount + ROGUE_ROUTE_SCENE_MAX_PLACEMENTS * 4, OBJECT_EVENT_TEMPLATES_COUNT);

        for(objectIdx = 0; objectIdx < events->objectEventCount; ++objectIdx)
        {
            const struct ObjectEventTemplate *spot = &events->objectEvents[objectIdx];
            u8 otherIdx;
            u8 warpIdx;

            if(!RogueRouteScenes_IsLotTemplate(spot))
                continue;

            switch(spot->movementRangeX)
            {
            case ROGUE_ROUTE_SCENE_SPOT_STALL_NPC:
                EXPECT(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR]);
                hasEligibleStall = TRUE;
                break;
            case ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR:
                EXPECT(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_STALL_NPC]);
                break;
            case ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC:
                EXPECT(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR]);
                hasEligibleCamp = TRUE;
                break;
            case ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR:
                EXPECT(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC]);
                break;
            case ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC:
                EXPECT(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR]);
                hasEligibleWorkbench = TRUE;
                break;
            case ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR:
                EXPECT(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC]);
                break;
            case ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC:
                hasEligibleRelic = TRUE;
                if(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_RELIC_DECOR])
                    hasEligibleBuriedCache = hasEligibleBuriedCache || spotTypeCount[ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE] >= 2;
                break;
            case ROGUE_ROUTE_SCENE_SPOT_RELIC_DECOR:
                EXPECT(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC]);
                break;
            case ROGUE_ROUTE_SCENE_SPOT_CREATURE_NPC:
                EXPECT(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_CREATURE_DECOR]);
                hasEligibleCreature = TRUE;
                break;
            case ROGUE_ROUTE_SCENE_SPOT_CREATURE_DECOR:
                EXPECT(hasPairedSpot[spot->trainerRange_berryTreeId][ROGUE_ROUTE_SCENE_SPOT_CREATURE_NPC]);
                break;
            }

            for(otherIdx = 0; otherIdx < events->objectEventCount; ++otherIdx)
            {
                const struct ObjectEventTemplate *other = &events->objectEvents[otherIdx];
                if(other != spot)
                    EXPECT(other->x != spot->x || other->y != spot->y);
            }

            for(warpIdx = 0; warpIdx < events->warpCount; ++warpIdx)
            {
                const struct WarpEvent *warp = &events->warps[warpIdx];
                EXPECT(warp->x != spot->x || warp->y != spot->y);
            }
        }

        EXPECT_NE(groupMask, 0);
        if(spotTypeCount[ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH] != 0)
            hasEligibleApricornGrove = TRUE;
        if(spotTypeCount[ROGUE_ROUTE_SCENE_SPOT_WATER_NPC] != 0)
            hasEligibleTideSalvage = TRUE;
    }

    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_STALL_NPC]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_RELIC_DECOR]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_CREATURE_NPC]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_CREATURE_DECOR]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_SOLO_NPC]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH]);
    EXPECT(hasGlobalSpotType[ROGUE_ROUTE_SCENE_SPOT_WATER_NPC]);
    EXPECT(hasEligibleStall);
    EXPECT(hasEligibleCamp);
    EXPECT(hasEligibleWorkbench);
    EXPECT(hasEligibleRelic);
    EXPECT(hasEligibleCreature);
    EXPECT(hasEligibleBuriedCache);
    EXPECT(hasEligibleApricornGrove);
    EXPECT(hasEligibleTideSalvage);
}

TEST("Route scene recipes compose bounded unique route objects")
{
    static const u8 sRecipes[] =
    {
        ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER,
        ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE,
        ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER,
        ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR,
        ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT,
    };
    struct RogueAdvPath originalPath;
    struct Pokemon originalParty[PARTY_SIZE];
    u8 originalPartyCount = gPlayerPartyCount;
    u8 originalRoomId;
    u8 recipeIdx;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalParty, gPlayerParty, sizeof(originalParty));
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    CreateMon(&gPlayerParty[0], SPECIES_MAGIKARP, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
    gPlayerPartyCount = 1;

    for(recipeIdx = 0; recipeIdx < ARRAY_COUNT(sRecipes); ++recipeIdx)
    {
        u8 recipeId = sRecipes[recipeIdx];
        u8 primarySpotType = recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE ? ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER ? ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR ? ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC
            : ROGUE_ROUTE_SCENE_SPOT_STALL_NPC;
        u8 decorSpotType = recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE ? ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER ? ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR ? ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR
            : ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR;
        struct RogueRouteSceneRequest request;
        const struct ObjectEventTemplate baseObjects[] =
        {
            {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 12, .y = 34, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_STALL_NPC, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
            {.localId = 44, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 13, .y = 33, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
            {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 56, .y = 78, .elevation = 3, .movementRangeX = primarySpotType, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
            {.localId = 43, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 57, .y = 77, .elevation = 3, .movementRangeX = decorSpotType, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
        };
        struct ObjectEventTemplate objects[8] =
        {
            {.localId = 1, .graphicsId = OBJ_EVENT_GFX_BOY_1, .x = 2, .y = 2},
            baseObjects[0],
            baseObjects[1],
            baseObjects[2],
            baseObjects[3],
        };
        const u8 *expectedScript = recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER ? Rogue_RouteEvent_StolenTradeCaseOffer
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP ? Rogue_RouteEvent_StolenTradeCaseCamp
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF ? Rogue_RouteEvent_StolenTradeCasePayoff
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE ? Rogue_RouteEvent_HexedShrine
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER ? Rogue_RouteEvent_AnomalousFossilOffer
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR ? Rogue_RouteEvent_UnboundTutor
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT ? Rogue_RouteEvent_TravelingMerchant
            : Rogue_RouteEvent_AnomalousFossilRestoration;
        const u8 *expectedPropScript = recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE ? NULL
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR ? Rogue_RouteEvent_UnboundTutorProp
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER ? Rogue_RouteEvent_AnomalousFossilProp
            : recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION ? Rogue_RouteEvent_FossilWorkbench
            : Rogue_RouteEvent_Prop;
        u8 count = 5;
        u8 expectedCount = recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE ? 2 : 3;
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
            else if(expectedPropScript != NULL && objects[i].script == expectedPropScript)
            {
                ++propCount;
                EXPECT_EQ(objects[i].localId, 43);
                EXPECT_EQ(objects[i].x, 57);
                EXPECT_EQ(objects[i].y, 77);
                EXPECT_NE(objects[i].graphicsId, OBJ_EVENT_GFX_BATTLE_STATUE);
                EXPECT_NE(objects[i].graphicsId, OBJ_EVENT_GFX_BREAKABLE_ROCK);
                EXPECT_NE(objects[i].graphicsId, OBJ_EVENT_GFX_CUTTABLE_TREE);
                EXPECT_NE(objects[i].graphicsId, OBJ_EVENT_GFX_PUSHABLE_BOULDER);
                EXPECT_NE(objects[i].graphicsId, OBJ_EVENT_GFX_MOVING_BOX);
                EXPECT(objects[i].graphicsId < OBJ_EVENT_GFX_ROUTE_BUG
                    || objects[i].graphicsId > OBJ_EVENT_GFX_ROUTE_WATER);
            }

        }

        EXPECT_EQ(npcCount, 1);
        EXPECT_EQ(propCount, expectedPropScript == NULL ? 0 : 1);

        // Save loading refreshes authored scripts by local ID. This used to
        // turn the NPC back into an inert anchor and could give props unrelated
        // scripts, including when a prop reused the other anchor's ID.
        for(i = 0; i < count; ++i)
        {
            if(objects[i].script == expectedScript || (expectedPropScript != NULL && objects[i].script == expectedPropScript))
                objects[i].script = Rogue_RouteEvent_Interact;
        }

        RogueRouteScenes_RestoreObjectEvents(objects, count, baseObjects, ARRAY_COUNT(baseObjects));
        npcCount = 0;
        propCount = 0;
        for(i = 0; i < count; ++i)
        {
            if(objects[i].script == expectedScript)
                ++npcCount;
            else if(expectedPropScript != NULL && objects[i].script == expectedPropScript)
                ++propCount;
        }
        EXPECT_EQ(npcCount, 1);
        EXPECT_EQ(propCount, expectedPropScript == NULL ? 0 : 1);
    }

    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    memcpy(gPlayerParty, originalParty, sizeof(originalParty));
    gPlayerPartyCount = originalPartyCount;
}

TEST("Semantic route props adapt supplies and camps to every environment")
{
    static const u16 sExpectedSupplies[ROGUE_ROUTE_ENVIRONMENT_COUNT] =
    {
        OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
        OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
        OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
        OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
        OBJ_EVENT_GFX_ROUTE_PROP_DRIFTWOOD,
        OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
    };
    static const u16 sExpectedCamps[ROGUE_ROUTE_ENVIRONMENT_COUNT] =
    {
        OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
        OBJ_EVENT_GFX_ROUTE_PROP_STUMP,
        OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
        OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
        OBJ_EVENT_GFX_ROUTE_PROP_DRIFTWOOD,
        OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
    };
    struct RogueAdvPath originalPath;
    struct Pokemon originalParty[PARTY_SIZE];
    u8 originalPartyCount = gPlayerPartyCount;
    u8 originalRoomId;
    u8 environment;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalParty, gPlayerParty, sizeof(originalParty));
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    CreateMon(&gPlayerParty[0], SPECIES_MAGIKARP, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
    gPlayerPartyCount = 1;

    for(environment = 0; environment < ROGUE_ROUTE_ENVIRONMENT_COUNT; ++environment)
    {
        static const u8 sRecipes[] =
        {
            ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER,
            ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR,
        };
        u8 routeIdx;
        u8 recipeIdx;

        for(routeIdx = 0; routeIdx < gRogueRouteTable.routeCount; ++routeIdx)
        {
            if(gRogueRouteTable.routes[routeIdx].environment == environment)
                break;
        }
        EXPECT_LT(routeIdx, gRogueRouteTable.routeCount);
        gRogueAdvPath.rooms[0].roomParams.roomIdx = routeIdx;

        for(recipeIdx = 0; recipeIdx < ARRAY_COUNT(sRecipes); ++recipeIdx)
        {
            struct ObjectEventTemplate objects[4] =
            {
                {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 12, .y = 34, .elevation = 3, .movementRangeX = recipeIdx == 0 ? ROGUE_ROUTE_SCENE_SPOT_STALL_NPC : ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
                {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 13, .y = 33, .elevation = 3, .movementRangeX = recipeIdx == 0 ? ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR : ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
            };
            const u8 *propScript = recipeIdx == 0 ? Rogue_RouteEvent_Prop : Rogue_RouteEvent_UnboundTutorProp;
            u16 expectedGraphics = recipeIdx == 0 ? sExpectedSupplies[environment] : sExpectedCamps[environment];
            u8 count = 2;
            u8 i;
            bool8 foundProp = FALSE;

            SetDebugPlacement(sRecipes[recipeIdx], 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
            RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
            EXPECT_EQ(count, 2);
            for(i = 0; i < count; ++i)
            {
                if(objects[i].script == propScript)
                {
                    foundProp = TRUE;
                    EXPECT_EQ(objects[i].graphicsId, expectedGraphics);
                    EXPECT_EQ(objects[i].localId, 42);
                    EXPECT_EQ(objects[i].x, 13);
                    EXPECT_EQ(objects[i].y, 33);
                }
            }
            EXPECT(foundProp);
        }
    }

    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    memcpy(gPlayerParty, originalParty, sizeof(originalParty));
    gPlayerPartyCount = originalPartyCount;
}

TEST("Route scene spot recipes stay sparse and typed")
{
    u8 recipeId;

    for(recipeId = 1; recipeId < ROGUE_ROUTE_SCENE_RECIPE_COUNT; ++recipeId)
    {
        const struct RogueRouteRecipeDefinition *recipe = RogueRouteEvents_GetRecipeDefinition(recipeId);
        u8 role;

        EXPECT(recipe != NULL);
        for(role = 0; role < recipe->lotCount; ++role)
        {
            const struct RogueRouteSceneLotDefinition *spot = &recipe->lots[role];
            bool8 hasPrimary = FALSE;
            u8 propCount = 0;
            u8 objectIdx;

            EXPECT_LE(spot->objectCount, 3);
            EXPECT_LT(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
            EXPECT_NE(spot->terrainMask, 0);
            EXPECT_EQ(spot->terrainMask & ~ROGUE_ROUTE_SCENE_TERRAIN_MASK_ALL, 0);
            for(objectIdx = 0; objectIdx < spot->objectCount; ++objectIdx)
            {
                const struct RogueRouteSceneObjectDefinition *object = &spot->objects[objectIdx];

                EXPECT_NE(object->graphicsId, OBJ_EVENT_GFX_BATTLE_STATUE);
                EXPECT_NE(object->graphicsId, OBJ_EVENT_GFX_BREAKABLE_ROCK);
                if(object->propId == 0)
                    hasPrimary = TRUE;
                else
                    ++propCount;
            }

            EXPECT(hasPrimary);
            if(spot->spotType == ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH)
            {
                EXPECT_EQ(propCount, 2);
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
            }
            else if(propCount != 0)
            {
                EXPECT_LE(propCount, 1);
                EXPECT_LT(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
            }
            else
            {
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
            }

            switch(recipeId)
            {
            case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER:
            case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF:
            case ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT:
                EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_STALL_NPC);
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR);
                break;
            case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP:
            case ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR:
                EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC);
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR);
                break;
            case ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE:
            case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER:
            case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF:
                EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC);
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
                break;
            case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER:
            case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION:
            case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN:
                EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC);
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR);
                break;
            case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS:
                EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE);
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
                break;
            case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE:
                EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH);
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
                break;
            case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN:
                if(role == 0)
                {
                    EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH);
                    EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
                }
                else
                {
                    EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC);
                    EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR);
                }
                break;
            case ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE:
                EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_CREATURE_NPC);
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_CREATURE_DECOR);
                break;
            case ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE:
                if(role == 0)
                {
                    EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC);
                    EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_RELIC_DECOR);
                }
                else
                {
                    EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE);
                    EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
                }
                break;
            case ROGUE_ROUTE_SCENE_RECIPE_TIDE_SALVAGE:
                EXPECT_EQ(spot->spotType, ROGUE_ROUTE_SCENE_SPOT_WATER_NPC);
                EXPECT_EQ(spot->decorSpotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT);
                EXPECT_EQ(spot->terrainMask, ROGUE_ROUTE_SCENE_TERRAIN_MASK_WATER);
                break;
            }
        }
    }
}

TEST("Declarative route scene visibility drives insertion and restoration")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    const struct ObjectEventTemplate baseObjects[] =
    {
        {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 12, .y = 34, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
        {.localId = 44, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 13, .y = 33, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
        {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 56, .y = 78, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
        {.localId = 43, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 57, .y = 77, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
    };
    struct ObjectEventTemplate objects[8] =
    {
        {.localId = 1, .graphicsId = OBJ_EVENT_GFX_BOY_1, .x = 2, .y = 2},
        baseObjects[0],
        baseObjects[1],
        baseObjects[2],
        baseObjects[3],
    };
    struct ObjectEventTemplate originalSavedTemplates[OBJECT_EVENT_TEMPLATES_COUNT];
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    u8 originalSavedCount = gSaveBlock1Ptr->objectEventTemplatesCount;
    bool8 originalHidden = FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    u8 count = 5;
    u8 i;
    bool8 foundConditionalProp;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalSavedTemplates, gSaveBlock1Ptr->objectEventTemplates, sizeof(originalSavedTemplates));
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.adventureQuests[0].definitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE;
    gRogueRun.adventureQuests[0].nodeId = 0;
    gRogueRun.adventureQuests[0].payload[1] = 1;
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP, 1, 0);

    RogueRouteScenes_SetState(0, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
    EXPECT_EQ(count, 3);
    foundConditionalProp = FALSE;
    for(i = 0; i < count; ++i)
    {
        if(objects[i].x == 57 && objects[i].y == 77)
        {
            foundConditionalProp = TRUE;
            EXPECT_EQ(objects[i].localId, 43);
            EXPECT_EQ(objects[i].graphicsId, OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE);
            EXPECT_EQ(objects[i].flagId, 0);
        }
    }
    EXPECT(foundConditionalProp);

    memcpy(gSaveBlock1Ptr->objectEventTemplates, objects, count * sizeof(objects[0]));
    gSaveBlock1Ptr->objectEventTemplatesCount = count;
    RogueRouteScenes_HideProp(0, 1);
    foundConditionalProp = FALSE;
    for(i = 0; i < gSaveBlock1Ptr->objectEventTemplatesCount; ++i)
    {
        if(gSaveBlock1Ptr->objectEventTemplates[i].localId == 43)
        {
            foundConditionalProp = TRUE;
            EXPECT_EQ(gSaveBlock1Ptr->objectEventTemplates[i].flagId, FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        }
    }
    EXPECT(foundConditionalProp);
    EXPECT(FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN));

    // Restoration consumes the same visibility definition and hides the
    // conditional object without changing the saved template's identity.
    RogueRouteScenes_SetState(0, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteScenes_RestoreObjectEvents(objects, count, baseObjects, ARRAY_COUNT(baseObjects));
    foundConditionalProp = FALSE;
    for(i = 0; i < count; ++i)
    {
        if(objects[i].x == 57 && objects[i].y == 77)
        {
            foundConditionalProp = TRUE;
            EXPECT_EQ(objects[i].localId, 43);
            EXPECT_EQ(objects[i].flagId, FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        }
    }
    EXPECT(foundConditionalProp);
    EXPECT(FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN));

    // A fresh composition in the completed state omits that same object.
    objects[0] = (struct ObjectEventTemplate){.localId = 1, .graphicsId = OBJ_EVENT_GFX_BOY_1, .x = 2, .y = 2};
    objects[1] = baseObjects[0];
    objects[2] = baseObjects[1];
    objects[3] = baseObjects[2];
    objects[4] = baseObjects[3];
    count = 5;
    RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
    EXPECT_EQ(count, 2);
    for(i = 0; i < count; ++i)
        EXPECT(objects[i].x != 57 || objects[i].y != 77);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    memcpy(gSaveBlock1Ptr->objectEventTemplates, originalSavedTemplates, sizeof(originalSavedTemplates));
    gSaveBlock1Ptr->objectEventTemplatesCount = originalSavedCount;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    RestoreFlag(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN, originalHidden);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Route scene spots do not paint broad metatile accents")
{
    struct RogueAdvPath originalPath;
    struct BackupMapLayout originalBackup = gBackupMapLayout;
    struct MapHeader originalHeader = gMapHeader;
    const struct RogueRouteMap *routeMap = &gRogueRouteTable.routes[0].map;
    const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(routeMap->group, routeMap->num);
    u8 originalRoomId;
    u32 cellCount;
    u32 i;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    gMapHeader = *mapHeader;
    gBackupMapLayout.map = sBackupMapData;
    gBackupMapLayout.width = mapHeader->mapLayout->width + MAP_OFFSET_W;
    gBackupMapLayout.height = mapHeader->mapLayout->height + MAP_OFFSET_H;
    cellCount = gBackupMapLayout.width * gBackupMapLayout.height;
    for(i = 0; i < cellCount; ++i)
        sBackupMapData[i] = METATILE_General_Grass | (3 << MAPGRID_ELEVATION_SHIFT);

    RogueRouteScenes_ApplyMetatiles();
    for(i = 0; i < cellCount; ++i)
        EXPECT_EQ(sBackupMapData[i], METATILE_General_Grass | (3 << MAPGRID_ELEVATION_SHIFT));

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
    u16 originalHistory = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY);
    u16 originalHistory2 = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
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
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, 0);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetCurrentDifficulty(3);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE);
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
    gRogueAdvPath.rooms[1].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE);
    gRogueAdvPath.rooms[1].rngSeed = 701;
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(!GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, &shrine));

    Rogue_ClearTemporaryDarkDealCurse();
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    Rogue_SetCurrentDifficulty(13);
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
    RogueRouteScenes_OnEnterRoute();
    EXPECT(!GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, &shrine));

    // A new Adventure clears encounter history and permits the family again.
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, 0);
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    for(seed = 1; seed != 0; ++seed)
    {
        gRogueAdvPath.rooms[1].rngSeed = seed;
        RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[1]);
        RogueRouteScenes_OnEnterRoute();
        if(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, &shrine))
            break;
    }
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, &shrine));
    EXPECT_EQ(shrine.rewardAmount, ROGUE_HEXED_SHRINE_REWARD_MAX);

    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    gRogueRun.temporaryDarkDealCurseItem = originalTempCurse;
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    Rogue_SetCurrentDifficulty(originalDifficulty);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, originalHistory);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, originalHistory2);
    RestoreFlag(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED, originalComplete);
    RestoreFlag(FLAG_ROGUE_RUN_ACTIVE, originalRunActive);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    SetMoney(&gSaveBlock1Ptr->money, originalMoney);
    ClearBag();
}

TEST("Unbound Tutor offers three universal moves and completes after one lesson")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct Pokemon originalParty[PARTY_SIZE];
    struct RogueRouteSceneRequest tutor;
    u16 offeredMoves[3];
    u16 originalDifficulty = Rogue_GetCurrentDifficulty();
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u32 originalMoney = GetMoney(&gSaveBlock1Ptr->money);
    u8 originalPartyCount = gPlayerPartyCount;
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    u8 moveIdx;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memcpy(originalParty, gPlayerParty, sizeof(originalParty));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    CreateMon(&gPlayerParty[0], SPECIES_MAGIKARP, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
    for(moveIdx = 0; moveIdx < MAX_MON_MOVES; ++moveIdx)
        SetMonMoveSlot(&gPlayerParty[0], MOVE_NONE, moveIdx);
    gPlayerPartyCount = 1;
    Rogue_SetCurrentDifficulty(5);
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    EXPECT(GetFirstPlacement(&tutor));
    SelectPlacement(&tutor);

    offeredMoves[0] = tutor.requestedItem;
    offeredMoves[1] = tutor.rewardItem;
    offeredMoves[2] = tutor.trainerNum;
    EXPECT_EQ((u8)tutor.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);
    EXPECT_NE(offeredMoves[0], MOVE_NONE);
    EXPECT_NE(offeredMoves[0], offeredMoves[1]);
    EXPECT_NE(offeredMoves[0], offeredMoves[2]);
    EXPECT_NE(offeredMoves[1], offeredMoves[2]);

    RogueRouteEvents_PrepareUnboundTutor();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(gSpecialVar_0x8007, offeredMoves[0]);
    EXPECT_EQ(gSpecialVar_0x8008, offeredMoves[1]);
    EXPECT_EQ(gSpecialVar_0x8009, offeredMoves[2]);
    // The special context deliberately ignores species compatibility.
    EXPECT_EQ(GetNumberOfRelearnableMovesForContext(&gPlayerParty[0]), 3);

    gSpecialVar_0x8006 = FALSE;
    RogueRouteEvents_FinishUnboundTutor();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_FAILED);
    EXPECT_EQ(RogueRouteScenes_GetState(tutor.sceneSlot), ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);

    EXPECT_NE(GiveMoveToMon(&gPlayerParty[0], offeredMoves[0]), MON_HAS_MAX_MOVES);
    gSpecialVar_0x8006 = TRUE;
    RogueRouteEvents_FinishUnboundTutor();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(RogueRouteScenes_GetState(tutor.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), originalMoney);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 0);

    RogueRouteEvents_PrepareUnboundTutor();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_FAILED);

    Rogue_SetCurrentDifficulty(originalDifficulty);
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    memcpy(gPlayerParty, originalParty, sizeof(originalParty));
    gPlayerPartyCount = originalPartyCount;
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Traveling Merchant offers one seeded half-price shop with normal selling")
{
    static const u8 sExpectedCategories[] =
    {
        ROGUE_SHOP_GENERAL,
        ROGUE_SHOP_BALLS,
        ROGUE_SHOP_TMS,
        ROGUE_SHOP_BATTLE_ENHANCERS,
        ROGUE_SHOP_HELD_ITEMS,
        ROGUE_SHOP_RARE_HELD_ITEMS,
    };
    struct RogueAdvPath originalPath;
    struct RogueRouteSceneRequest merchant;
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE originalStandardRng = gRngValue;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 originalHistory = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY);
    bool8 seenCategories[ARRAY_COUNT(sExpectedCategories)] = {FALSE};
    u8 originalRoomId;
    u16 seed;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);

    for(seed = 0; seed < 256; ++seed)
    {
        u16 category;
        u8 i;

        gRogueAdvPath.rooms[0].rngSeed = seed;
        SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
        EXPECT(GetFirstPlacement(&merchant));
        EXPECT_EQ((u8)merchant.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
        EXPECT_EQ(merchant.primaryGraphicsId, OBJ_EVENT_GFX_MART_EMPLOYEE);
        EXPECT(ROGUE_SHOP_IS_TRAVELING_MERCHANT(merchant.rewardAmount));
        category = ROGUE_SHOP_GET_CATEGORY(merchant.rewardAmount);

        for(i = 0; i < ARRAY_COUNT(sExpectedCategories); ++i)
        {
            if(category == sExpectedCategories[i])
            {
                seenCategories[i] = TRUE;
                break;
            }
        }
        EXPECT_LT(i, ARRAY_COUNT(sExpectedCategories));
    }
    for(seed = 0; seed < ARRAY_COUNT(seenCategories); ++seed)
        EXPECT(seenCategories[seed]);

    EXPECT_EQ(Shop_ApplyDynamicPriceModifier(ROGUE_SHOP_GENERAL, 1000), 1000);
    EXPECT_EQ(
        Shop_ApplyDynamicPriceModifier(ROGUE_SHOP_FLAG_TRAVELING_MERCHANT | ROGUE_SHOP_GENERAL, 1000),
        500);
    EXPECT_EQ(
        Shop_ApplyDynamicPriceModifier(ROGUE_SHOP_FLAG_TRAVELING_MERCHANT | ROGUE_SHOP_GENERAL, 5),
        2);
    EXPECT_EQ(
        Shop_ApplyDynamicPriceModifier(ROGUE_SHOP_FLAG_TRAVELING_MERCHANT | ROGUE_SHOP_GENERAL, 0),
        0);

    SelectPlacement(&merchant);
    gSpecialVar_Result = FALSE;
    RogueRouteEvents_FinishTravelingMerchant();
    EXPECT_EQ(RogueRouteScenes_GetState(merchant.sceneSlot), ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    EXPECT(!RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_TRAVELING_MERCHANT));

    gSpecialVar_Result = TRUE;
    RogueRouteEvents_FinishTravelingMerchant();
    EXPECT_EQ(RogueRouteScenes_GetState(merchant.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT(RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_TRAVELING_MERCHANT));
    EXPECT_EQ(memcmp(&gRngRogueValue, &originalRogueRng, sizeof(originalRogueRng)), 0);
    EXPECT_EQ(memcmp(&gRngValue, &originalStandardRng, sizeof(originalStandardRng)), 0);

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, originalHistory);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Breeder's Exchange trades one local catch for a deterministic trained Pokemon atomically")
{
    struct RogueAdvPath originalPath;
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueWildEncounters originalWildEncounters = gRogueRun.wildEncounters;
    struct Pokemon originalParty[PARTY_SIZE];
    struct RogueRouteSceneRequest exchange;
    struct Pokemon offeredMon;
    struct RogueAdventureQuestCreateParams questParams = {0};
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE originalStandardRng = gRngValue;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 originalHistory = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY);
    u8 originalPartyCount = gPlayerPartyCount;
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 perfectIvCount = 0;
    u8 moveCount = 0;
    u8 stat;
    u8 moveIdx;
    u16 heldItem;
    u32 uniqueOtId = OTID_FLAG_CUSTOM_MON | OTID_FLAG_DYNAMIC_CUSTOM_MON;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalParty, gPlayerParty, sizeof(originalParty));
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    memset(&gRogueRun.wildEncounters, 0, sizeof(gRogueRun.wildEncounters));
    gRogueRun.wildEncounters.species[0] = SPECIES_MIGHTYENA;
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE);
    gRogueAdvPath.rooms[0].rngSeed = 0xBEEF;
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);

    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    EXPECT(GetFirstPlacement(&exchange));
    EXPECT_EQ(exchange.requestedItem, SPECIES_MIGHTYENA);
    EXPECT_NE(exchange.rewardItem, SPECIES_NONE);
    EXPECT(!RoguePokedex_IsSpeciesLegendary(exchange.rewardItem));
    EXPECT_EQ(Rogue_GetActiveEvolutionCount(exchange.rewardItem), Rogue_GetActiveEvolutionCount(exchange.requestedItem));
    EXPECT_EQ(gRoguePokemonProfiles[exchange.rewardItem].competitiveSetCount != 0, TRUE);

    SelectPlacement(&exchange);
    EXPECT(RogueRouteEvents_CreateBreedersExchangeMon(&offeredMon));
    EXPECT_EQ(GetMonData(&offeredMon, MON_DATA_SPECIES), exchange.rewardItem);
    EXPECT_EQ(GetMonData(&offeredMon, MON_DATA_LEVEL), Rogue_CalculatePlayerMonLvl());
    EXPECT_EQ(GetNature(&offeredMon), gRoguePokemonProfiles[exchange.rewardItem].competitiveSets[exchange.trainerNum].nature);
    EXPECT_EQ(RogueGift_GetCustomMonId(&offeredMon), CUSTOM_MON_NONE);
    for(stat = 0; stat < NUM_STATS; ++stat)
    {
        if(GetMonData(&offeredMon, MON_DATA_HP_IV + stat) == 31)
            ++perfectIvCount;
    }
    for(moveIdx = 0; moveIdx < MAX_MON_MOVES; ++moveIdx)
    {
        if(GetMonData(&offeredMon, MON_DATA_MOVE1 + moveIdx) != MOVE_NONE)
            ++moveCount;
    }
    EXPECT_EQ(perfectIvCount, 3);
    EXPECT_NE(moveCount, 0);
    EXPECT_EQ(memcmp(&gRngRogueValue, &originalRogueRng, sizeof(originalRogueRng)), 0);
    EXPECT_EQ(memcmp(&gRngValue, &originalStandardRng, sizeof(originalStandardRng)), 0);

    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    CreateMon(&gPlayerParty[0], exchange.requestedItem, Rogue_CalculatePlayerMonLvl(), 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
    gPlayerPartyCount = 1;
    heldItem = ITEM_ODD_KEYSTONE;
    SetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM, &heldItem);
    EXPECT_NE(
        RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE, &questParams),
        ROGUE_ADVENTURE_QUEST_INVALID_ID);
    EXPECT(RogueAdventureQuests_IsItemProtected(ITEM_ODD_KEYSTONE));
    gSpecialVar_0x8006 = 0;
    RogueRouteEvents_ValidateBreedersExchangeSelection();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_PROTECTED_MON);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), exchange.requestedItem);

    RogueAdventureQuests_Clear();
    CreateMon(&gPlayerParty[0], exchange.requestedItem, Rogue_CalculatePlayerMonLvl(), 0, FALSE, 0, OT_ID_CUSTOM_MON, uniqueOtId);
    RogueRouteEvents_ValidateBreedersExchangeSelection();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_PROTECTED_MON);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), exchange.requestedItem);

    CreateMon(&gPlayerParty[0], exchange.requestedItem, Rogue_CalculatePlayerMonLvl(), 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
    ClearBag();
    heldItem = ITEM_POTION;
    SetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM, &heldItem);
    RogueRouteEvents_ValidateBreedersExchangeSelection();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    RogueRouteEvents_TryCompleteBreedersExchange();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), exchange.rewardItem);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT(CheckBagHasItem(ITEM_POTION, 1));
    EXPECT_EQ(RogueRouteScenes_GetState(exchange.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT(RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_BREEDERS_EXCHANGE));

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, originalHistory);
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    gRogueRun.wildEncounters = originalWildEncounters;
    memcpy(gRogueRun.adventureQuests, originalQuests, sizeof(originalQuests));
    memcpy(gPlayerParty, originalParty, sizeof(originalParty));
    gPlayerPartyCount = originalPartyCount;
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    gRngRogueValue = originalRogueRng;
    gRngValue = originalStandardRng;
    RestoreFlag(FLAG_ROGUE_RUN_ACTIVE, originalRunActive);
}

TEST("Breeder's Exchange composes a visible offer and removes it after trading")
{
    struct RogueAdvPath originalPath;
    struct RogueWildEncounters originalWildEncounters = gRogueRun.wildEncounters;
    struct RogueRouteSceneRequest exchange;
    struct ObjectEventTemplate objects[5] =
    {
        {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 12, .y = 34, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_CREATURE_NPC, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
        {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 13, .y = 33, .elevation = 3, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_CREATURE_DECOR, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
    };
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    bool8 originalRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 count = 2;
    bool8 foundBreeder = FALSE;
    bool8 foundPokemon = FALSE;
    u8 i;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    memset(&gRogueRun.wildEncounters, 0, sizeof(gRogueRun.wildEncounters));
    gRogueRun.wildEncounters.species[0] = SPECIES_MIGHTYENA;
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE);
    gRogueAdvPath.rooms[0].rngSeed = 0xCAFE;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    EXPECT(GetFirstPlacement(&exchange));

    RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
    EXPECT_EQ(count, 2);
    for(i = 0; i < count; ++i)
    {
        if(objects[i].script == Rogue_RouteEvent_BreedersExchange)
        {
            foundBreeder = TRUE;
            EXPECT_EQ(objects[i].localId, 41);
            EXPECT_EQ(objects[i].x, 12);
            EXPECT_EQ(objects[i].y, 34);
        }
        else if(objects[i].script == Rogue_RouteEvent_BreedersExchangePokemon)
        {
            foundPokemon = TRUE;
            EXPECT_EQ(objects[i].graphicsId, OBJ_EVENT_GFX_FOLLOW_MON_1);
            EXPECT_EQ(objects[i].x, 13);
            EXPECT_EQ(objects[i].y, 33);
        }
    }
    EXPECT(foundBreeder);
    EXPECT(foundPokemon);
    EXPECT_EQ(VarGet(VAR_FOLLOW_MON_1), exchange.rewardItem);
    EXPECT(RogueRouteScenes_IsFollowMonSlotReserved(1));

    RogueRouteScenes_SetState(exchange.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT(!RogueRouteScenes_IsFollowMonSlotReserved(1));
    count = 2;
    objects[0] = (struct ObjectEventTemplate)
    {
        .localId = 41,
        .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE,
        .x = 12,
        .y = 34,
        .elevation = 3,
        .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_CREATURE_NPC,
        .trainerType = TRAINER_TYPE_NONE,
        .trainerRange_berryTreeId = 0,
        .script = Rogue_RouteEvent_Interact,
    };
    objects[1] = (struct ObjectEventTemplate)
    {
        .localId = 42,
        .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE,
        .x = 13,
        .y = 33,
        .elevation = 3,
        .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_CREATURE_DECOR,
        .trainerType = TRAINER_TYPE_NONE,
        .trainerRange_berryTreeId = 0,
        .script = Rogue_RouteEvent_Interact,
    };
    RogueRouteScenes_ModifyObjectEvents(objects, &count, ARRAY_COUNT(objects));
    EXPECT_EQ(count, 1);
    for(i = 0; i < count; ++i)
        EXPECT(objects[i].script != Rogue_RouteEvent_BreedersExchangePokemon);

    gRogueRun.wildEncounters = originalWildEncounters;
    gRogueRun.routeSceneRoomId = originalSceneRoomId;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    RestoreFlag(FLAG_ROGUE_RUN_ACTIVE, originalRunActive);
}

TEST("Anomalous Fossil restores deterministic stable and adaptive Rare Unique Pokemon")
{
    static const u16 sQuestFossils[] =
    {
        ITEM_HELIX_FOSSIL,
        ITEM_DOME_FOSSIL,
        ITEM_OLD_AMBER,
        ITEM_ROOT_FOSSIL,
        ITEM_CLAW_FOSSIL,
#ifdef ROGUE_EXPANSION
        ITEM_ARMOR_FOSSIL,
        ITEM_SKULL_FOSSIL,
        ITEM_COVER_FOSSIL,
        ITEM_PLUME_FOSSIL,
        ITEM_JAW_FOSSIL,
        ITEM_SAIL_FOSSIL,
#endif
    };
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
    u16 originalHistory = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY);
    u8 originalPartyCount = gPlayerPartyCount;
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    bool8 originalComplete = FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    bool8 originalPropA = FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    u8 questId;
    u32 customMonId;
    u8 customType0;
    u8 customType1;
    u8 i;

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
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER);
    for(i = 0; i < ARRAY_COUNT(sQuestFossils); ++i)
    {
        EXPECT_EQ(ItemId_GetPocket(sQuestFossils[i]), POCKET_KEY_ITEMS);
        EXPECT_EQ(ItemId_GetPrice(sQuestFossils[i]), 0);
        EXPECT(ItemId_GetImportance(sQuestFossils[i]));
    }
    {
        u16 seed;

        for(seed = 1; seed != 0; ++seed)
        {
            VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);
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
    EXPECT_EQ(ItemId_GetPocket(offer.requestedItem), POCKET_KEY_ITEMS);
    EXPECT_EQ(ItemId_GetPrice(offer.requestedItem), 0);
    EXPECT(ItemId_GetImportance(offer.requestedItem));

    RogueRouteEvents_TryAcceptAnomalousFossilQuest();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(CheckBagHasItem(offer.requestedItem, 1));
    EXPECT(FlagGet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN));
    EXPECT(ItemId_GetImportance(offer.requestedItem));
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);
    questId = RogueAdventureQuests_GetQuestIdAt(0);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->payload[0], offer.requestedItem);
    EXPECT_EQ(RogueAdventureQuests_Get(questId)->payload[1], offer.rewardAmount);

    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    // The packed plan remains stable, but an accepted generator is filtered
    // from object restoration while its quest is active.
    {
        struct RogueRouteSceneRequest staleOffer;
        EXPECT(!GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER, &staleOffer));
    }

    RogueRouteScenes_OnExitRoute();
    gRogueAdvPath.roomCount = 2;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION);
    gRogueAdvPath.rooms[1].rngSeed = 301;
    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION, 0, questId);
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

    // A full party requires an explicit replacement. Cancelling at this point
    // leaves both the fossil and quest untouched.
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    for(customType0 = 0; customType0 < PARTY_SIZE; ++customType0)
        CreateMon(&gPlayerParty[customType0], SPECIES_MAGIKARP, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, customType0 + 1);
    gPlayerPartyCount = PARTY_SIZE;
    rngBefore = gRngValue;
    gSpecialVar_0x8004 = ROGUE_FOSSIL_RESTORATION_STABLE;
    gSpecialVar_0x8006 = PARTY_NOTHING_CHOSEN;
    RogueRouteEvents_TryRestoreAnomalousFossil();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_PARTY_FULL);
    EXPECT(CheckBagHasItem(restoration.requestedItem, 1));
    EXPECT_EQ(RogueRouteScenes_GetState(restoration.sceneSlot), ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), 1);

    gSpecialVar_0x8006 = 2;
    RogueRouteEvents_TryRestoreAnomalousFossil();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(&gRngValue, &rngBefore, sizeof(rngBefore)), 0);
    EXPECT(!CheckBagHasItem(restoration.requestedItem, 1));
    EXPECT_EQ(RogueRouteScenes_GetState(restoration.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT_EQ(gPlayerPartyCount, PARTY_SIZE);
    EXPECT_EQ(GetMonData(&gPlayerParty[2], MON_DATA_SPECIES), restoration.rewardItem);
    EXPECT_EQ(GetMonData(&gPlayerParty[2], MON_DATA_LEVEL), 1);
    customMonId = RogueGift_GetCustomMonId(&gPlayerParty[2]);
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
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, originalHistory);
    RestoreFlag(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED, originalComplete);
    RestoreFlag(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN, originalPropA);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    gRngValue = originalStandardRng;
    ClearBag();
}

TEST("Forbidden Stone binds three souls before its Spiritomb payoff")
{
    const struct RogueRouteRecipeDefinition *offerRecipe = RogueRouteEvents_GetRecipeDefinition(ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER);
    const struct RogueRouteRecipeDefinition *soulRecipe = RogueRouteEvents_GetRecipeDefinition(ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS);
    const struct RogueRouteRecipeDefinition *payoffRecipe = RogueRouteEvents_GetRecipeDefinition(ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF);
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

    // Spirit Stone objects identify the three collectible souls only. The
    // quest giver and final battle must not reuse an indistinguishable prop.
    EXPECT_EQ(offerRecipe->lots[0].objectCount, 1);
    EXPECT_EQ(payoffRecipe->lots[0].objectCount, 1);
    EXPECT_EQ(soulRecipe->lotCount, ROGUE_FORBIDDEN_STONE_SOUL_COUNT);
    for(i = 0; i < soulRecipe->lotCount; ++i)
    {
        EXPECT_EQ(soulRecipe->lots[i].objectCount, 1);
        EXPECT_EQ(soulRecipe->lots[i].objects[0].graphicsId, ROUTE_SCENE_GFX_SEMANTIC_SPIRIT_STONE);
        EXPECT_EQ(soulRecipe->lots[i].objects[0].script, Rogue_RouteEvent_ForbiddenStoneSoul);
    }

    memcpy(originalEnemyParty, gEnemyParty, sizeof(originalEnemyParty));
    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, 5000);
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.routeSceneRoomId = 0;
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 1;
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
    gRogueAdvPath.rooms[0].roomParams.roomIdx = 1;
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
            {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 10, .y = 10, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = souls[0].lotId, .script = Rogue_RouteEvent_Interact},
            {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 20, .y = 20, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = souls[1].lotId, .script = Rogue_RouteEvent_Interact},
            {.localId = 43, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 30, .y = 30, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = souls[2].lotId, .script = Rogue_RouteEvent_Interact},
        };
        u8 objectCount = ARRAY_COUNT(objects);

        RogueRouteScenes_ModifyObjectEvents(objects, &objectCount, ARRAY_COUNT(objects));
        EXPECT_EQ(objectCount, 2);
        for(i = 0; i < objectCount; ++i)
        {
            EXPECT_EQ(objects[i].script, Rogue_RouteEvent_ForbiddenStoneSoul);
            EXPECT_EQ(objects[i].graphicsId, OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE);
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
        {.localId = 40, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 10, .y = 10, .movementType = MOVEMENT_TYPE_FACE_DOWN, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 0, .script = Rogue_RouteEvent_Interact},
        {.localId = 41, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 20, .y = 20, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
        {.localId = 42, .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE, .x = 21, .y = 19, .movementRangeX = ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR, .trainerType = TRAINER_TYPE_NONE, .trainerRange_berryTreeId = 1, .script = Rogue_RouteEvent_Interact},
    };
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u8 originalRoomId;
    u8 originalSceneRoomId = gRogueRun.routeSceneRoomId;
    u8 objectCount = 3;
    u8 questId;
    u16 apricorn;
    u16 ball;
    u16 itemId;
    u8 treeCount = 0;
    u8 artisanCount = 0;
    u8 propCount = 0;
    u8 apricornChoiceMask = 0;
    u8 i;

    ClearBag();
    SetupCurrentEvent(&originalPath, &originalRoomId);
    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    gRogueRun.routeSceneRoomId = 0;
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN);
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
    EXPECT_EQ(objectCount, 5);
    for(i = 0; i < objectCount; ++i)
    {
        u8 j;

        for(j = i + 1; j < objectCount; ++j)
            EXPECT_NE(objects[i].localId, objects[j].localId);
        if(objects[i].script == Rogue_RouteEvent_ApricornTree)
        {
            u8 choice = (objects[i].trainerRange_berryTreeId >> 4) & 0xF;

            ++treeCount;
            apricornChoiceMask |= 1 << choice;
            EXPECT_EQ(objects[i].y, 11);
            EXPECT_GE(objects[i].x, 9);
            EXPECT_LE(objects[i].x, 11);
            EXPECT_EQ(objects[i].movementType, MOVEMENT_TYPE_FACE_DOWN);
        }
        else if(objects[i].script == Rogue_RouteEvent_ApricornArtisan)
            ++artisanCount;
        else if(objects[i].script == Rogue_RouteEvent_ApricornProp)
            ++propCount;
    }
    EXPECT_EQ(treeCount, 3);
    EXPECT_EQ(apricornChoiceMask, 0x7);
    EXPECT_EQ(artisanCount, 1);
    EXPECT_EQ(propCount, 1);

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
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE);
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
    gRogueAdvPath.rooms[1].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN);
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
    u16 originalHistory = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY);
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
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);
    gRogueRun.teamEncounterNum = TEAM_NUM_KANTO_ROCKET;
    // This test covers the quest chain specifically; an active temporary
    // Curse makes the shrine ineligible and deterministically exercises the
    // remaining fallback entry.
    gRogueRun.temporaryDarkDealCurseItem = Rogue_SelectDarkDealCurseItem(0);
    Rogue_SetCurrentDifficulty(0);
    FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER);
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

    // Quicksaving on the source route must not restore a generator whose
    // quest is already active.
    RogueRouteScenes_GenerateRoom(&gRogueAdvPath.rooms[0]);
    RogueRouteScenes_OnEnterRoute();
    {
        struct RogueRouteSceneRequest staleOffer;
        EXPECT(!GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER, &staleOffer));
    }

    Rogue_SetCurrentDifficulty(ROGUE_CHAMP_START_DIFFICULTY);
    RogueRouteScenes_OnExitRoute();
    gRogueAdvPath.roomCount = 3;
    gRogueRun.adventureRoomId = 1;
    gRogueAdvPath.rooms[1].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP);
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
    gRogueAdvPath.rooms[3].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP);
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
    gRogueAdvPath.rooms[4].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF);
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
    gRogueAdvPath.rooms[5].roomParams.roomIdx = FindRouteForRecipe(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF);
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
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, originalHistory);
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

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        gRogueRun.adventureRoomId = i;
        EXPECT_EQ(RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params), i);
    }
    EXPECT_EQ(RogueAdventureQuests_GetCount(), ROGUE_ADVENTURE_QUEST_CAPACITY);

    EXPECT(RogueAdventureQuests_TryCollectSceneRequest(30, &request, &priority));
    EXPECT_EQ(request.ownerQuestId, 0);
    EXPECT_EQ(request.recipeId, ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP);
    EXPECT_EQ((u8)request.source, ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE);
    RogueAdventureQuests_LeaveRoute(30);
    EXPECT_EQ(RogueAdventureQuests_GetState(0), ROGUE_ADVENTURE_QUEST_STATE_ACTIVE);

    EXPECT(RogueAdventureQuests_TryCollectSceneRequest(31, &request, &priority));
    EXPECT_EQ(request.ownerQuestId, 1);
    EXPECT_EQ(RogueAdventureQuests_GetState(1), ROGUE_ADVENTURE_QUEST_STATE_READY);
    EXPECT_EQ(RogueAdventureQuests_GetCount(), ROGUE_ADVENTURE_QUEST_CAPACITY);

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
    gRogueAdvPath.rooms[0].roomParams.roomIdx = FindRouteWithRepeatedRecipeLot(ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP, 3);
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
    ExpectRouteScenePlansEqual(&firstPlan, &gRogueAdvPath.rooms[0].routeScenePlan);
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

TEST("Adventure quest runtime packs independent quest records into a compact run buffer")
{
    struct RogueAdventureQuest originalQuests[ROGUE_ADVENTURE_QUEST_CAPACITY];
    struct RogueAdventureQuestCreateParams params = {0};
    u8 i;

    memcpy(originalQuests, gRogueRun.adventureQuests, sizeof(originalQuests));
    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    EXPECT_EQ((u32)sizeof(gRogueRun.adventureQuests), ROGUE_ADVENTURE_QUEST_CAPACITY * 8);

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

TEST("Tide Salvage claims one seeded water reward atomically")
{
    struct RogueAdvPath originalPath;
    struct RogueRouteSceneRequest scene;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 originalHistory2 = VarGet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2);
    u8 originalRoomId;

    SetupCurrentEvent(&originalPath, &originalRoomId);
    ClearBag();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, 0);
    gRogueAdvPath.rooms[0].rngSeed = 0xA717;

    SetDebugPlacement(ROGUE_ROUTE_SCENE_RECIPE_TIDE_SALVAGE, 0, ROGUE_ADVENTURE_QUEST_INVALID_ID);
    EXPECT(GetPlacementByRecipe(ROGUE_ROUTE_SCENE_RECIPE_TIDE_SALVAGE, &scene));
    EXPECT_EQ(scene.source, ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF);
    EXPECT_EQ(scene.primaryGraphicsId, OBJ_EVENT_GFX_SWIMMER_M);
    EXPECT_NE(scene.rewardItem, ITEM_NONE);
    EXPECT_NE(scene.rewardAmount, 0);
    EXPECT(!CheckBagHasItem(scene.rewardItem, scene.rewardAmount));

    SelectPlacement(&scene);
    RogueRouteEvents_TryClaimTideSalvage();
    EXPECT_EQ(gSpecialVar_Result, ROGUE_ROUTE_EVENT_RESULT_SUCCESS);
    EXPECT(CheckBagHasItem(scene.rewardItem, scene.rewardAmount));
    EXPECT_EQ(RogueRouteScenes_GetState(scene.sceneSlot), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT(RogueRouteEvents_HasCompletedFamily(ROGUE_ROUTE_FAMILY_TIDE_SALVAGE));

    ClearBag();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, originalHistory2);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
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
        ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR,
        ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT,
        ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE,
        ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE,
        ROGUE_ROUTE_SCENE_RECIPE_TIDE_SALVAGE,
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
        EXPECT_EQ(fallback->weight, i == 4 || i == 5 ? 25 : 50);
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
