#include "global.h"

#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/metatile_labels.h"
#include "constants/pokemon.h"
#include "constants/rogue_route_events.h"
#include "constants/rogue_route_scenes.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "battle_main.h"
#include "characters.h"
#include "fieldmap.h"
#include "item.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokedex.h"
#include "random.h"
#include "strings.h"
#include "string_util.h"

#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_adventurepaths.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_event_transactions.h"
#include "rogue_gifts.h"
#include "rogue_pokedex.h"
#include "rogue_popup.h"
#include "rogue_route_events.h"
#include "rogue_route_scene_internal.h"
#include "rogue_route_scenes.h"
#include "rogue_trainers.h"

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
extern const struct Tileset gTileset_General;
extern const struct Tileset gTileset_GeneralHub;

static u32 GetActiveTeamClassFlag(void)
{
    switch(gRogueRun.teamEncounterNum)
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

    return CLASS_FLAG_TEAM_ROCKET;
}

static u16 SelectEvilTeamTrainer(struct RogueRouteSceneRng *rng)
{
    u32 teamClassFlag = GetActiveTeamClassFlag();
    u16 eligibleCount = 0;
    u16 selectedIdx;
    u16 trainerNum;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        const struct RogueTrainer *trainer = &gRogueTrainers[trainerNum];

        if((trainer->trainerFlags & TRAINER_FLAG_CLASS_TEAM) != 0
            && (trainer->trainerFlags & TRAINER_FLAG_CLASS_TEAM_BOSS) == 0
            && (trainer->classFlags & teamClassFlag) != 0)
            ++eligibleCount;
    }

    AGB_ASSERT(eligibleCount != 0);
    if(eligibleCount == 0)
        return TRAINER_NONE;

    selectedIdx = RogueRouteSceneRng_Next(rng) % eligibleCount;
    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        const struct RogueTrainer *trainer = &gRogueTrainers[trainerNum];

        if((trainer->trainerFlags & TRAINER_FLAG_CLASS_TEAM) != 0
            && (trainer->trainerFlags & TRAINER_FLAG_CLASS_TEAM_BOSS) == 0
            && (trainer->classFlags & teamClassFlag) != 0
            && selectedIdx-- == 0)
            return trainerNum;
    }

    return TRAINER_NONE;
}

static bool8 CanShowStolenTradeCaseOffer(u8 roomId)
{
    if(FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED))
        return FALSE;

    if(!RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE))
        return Rogue_GetCurrentDifficulty() < ROGUE_CHAMP_START_DIFFICULTY;

    // Preserve an accepted offer across a quickload until its source route is
    // actually left. Later quest nodes still outrank this source scene.
    return RogueAdventureQuests_IsDefinitionSourceRoom(
        ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE,
        roomId);
}

static bool8 CanShowHexedShrine(u8 roomId)
{
    if(Rogue_SelectDarkDealCurseItem(0) == ITEM_NONE)
        return FALSE;

    if(gRogueRun.temporaryDarkDealCurseItem == ITEM_NONE)
        return TRUE;

    // Rebuild the accepted scene on a same-route quickload. Once the route is
    // left, the active temporary Curse suppresses future shrine offers.
    return gRogueRun.routeSceneRoomId == roomId
        && (VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) & ROUTE_SCENE_HEXED_SHRINE_ACCEPTED) != 0;
}

static const u16 sAnomalousFossilItems[] =
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

static u16 CountEligibleAnomalousFossils(void)
{
    u16 count = 0;
    u16 i;

    for(i = 0; i < ARRAY_COUNT(sAnomalousFossilItems); ++i)
    {
        u16 species = RogueAdventureQuests_GetFossilSpecies(sAnomalousFossilItems[i]);

        if(species != SPECIES_NONE && RoguePokedex_IsSpeciesEnabled(species))
            ++count;
    }

    return count;
}

static bool8 CanShowAnomalousFossilOffer(u8 roomId)
{
    if(RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL))
    {
        return RogueAdventureQuests_IsDefinitionSourceRoom(
            ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL,
            roomId);
    }

    return Rogue_GetCurrentDifficulty() < ROGUE_CHAMP_START_DIFFICULTY
        && CountEligibleAnomalousFossils() != 0;
}

static bool8 CanShowForbiddenStoneOffer(u8 roomId)
{
    if(RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE))
    {
        return RogueAdventureQuests_IsDefinitionSourceRoom(
            ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE,
            roomId);
    }

    return RoguePokedex_IsSpeciesEnabled(SPECIES_SPIRITOMB);
}

static bool8 CanShowApricornGrove(u8 roomId)
{
    if(RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING))
    {
        return RogueAdventureQuests_IsDefinitionSourceRoom(
            ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING,
            roomId);
    }

    return TRUE;
}

static const u16 sApricornItems[] =
{
    ITEM_RED_APRICORN,
    ITEM_BLUE_APRICORN,
    ITEM_YELLOW_APRICORN,
    ITEM_GREEN_APRICORN,
    ITEM_PINK_APRICORN,
    ITEM_WHITE_APRICORN,
    ITEM_BLACK_APRICORN,
};

static const u16 sApricornBalls[] =
{
    ITEM_LEVEL_BALL,
    ITEM_LURE_BALL,
    ITEM_MOON_BALL,
    ITEM_FRIEND_BALL,
    ITEM_LOVE_BALL,
    ITEM_FAST_BALL,
    ITEM_HEAVY_BALL,
};

static u16 GetApricornBall(u16 apricorn)
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sApricornItems); ++i)
    {
        if(sApricornItems[i] == apricorn)
            return sApricornBalls[i];
    }

    return ITEM_NONE;
}

static u8 FindAdventureQuestId(u8 definitionId)
{
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(i);

        if(quest != NULL && quest->definitionId == definitionId)
            return i;
    }

    return ROGUE_ADVENTURE_QUEST_INVALID_ID;
}

static bool8 SelectStolenTradeCaseOfferPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    (void)request;
    *payload = SelectEvilTeamTrainer(rng);
    return *payload != TRAINER_NONE;
}

static void ExpandStolenTradeCaseOfferPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    request->primaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    request->requestedItem = ITEM_TRADE_CASE;
    request->rewardItem = ITEM_BIG_POKEBLOCK_BUNDLE;
    request->trainerNum = payload;
}

#define HEXED_SHRINE_PAYLOAD_ITEM_MASK 0xFFF
#define HEXED_SHRINE_PAYLOAD_REWARD_SHIFT 12

static bool8 SelectHexedShrinePayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u16 curseItem;
    u16 rewardAmount;

    (void)request;
    if(gRogueRun.temporaryDarkDealCurseItem != ITEM_NONE
        && (VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) & ROUTE_SCENE_HEXED_SHRINE_ACCEPTED) != 0)
        curseItem = gRogueRun.temporaryDarkDealCurseItem;
    else
        curseItem = Rogue_SelectDarkDealCurseItem(RogueRouteSceneRng_Next(rng));

    rewardAmount = min(
        ROGUE_HEXED_SHRINE_REWARD_MAX,
        ROGUE_HEXED_SHRINE_REWARD_BASE + ROGUE_HEXED_SHRINE_REWARD_PER_DIFFICULTY * Rogue_GetCurrentDifficulty());
    if(curseItem == ITEM_NONE
        || curseItem > HEXED_SHRINE_PAYLOAD_ITEM_MASK
        || rewardAmount % 1000 != 0)
        return FALSE;

    *payload = curseItem | ((u32)(rewardAmount / 1000) << HEXED_SHRINE_PAYLOAD_REWARD_SHIFT);
    return TRUE;
}

static void ExpandHexedShrinePayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    request->primaryGraphicsId = OBJ_EVENT_GFX_DEVIL_MAN;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_DEVIL_MAN;
    request->requestedItem = payload & HEXED_SHRINE_PAYLOAD_ITEM_MASK;
    request->rewardAmount = (payload >> HEXED_SHRINE_PAYLOAD_REWARD_SHIFT) * 1000;
}

#define FOSSIL_PAYLOAD_INDEX_MASK 0xF
#define FOSSIL_PAYLOAD_SEED_SHIFT 4

static bool8 SelectAnomalousFossilOfferPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u16 eligibleCount = CountEligibleAnomalousFossils();
    u16 selected = eligibleCount == 0 ? 0 : RogueRouteSceneRng_Next(rng) % eligibleCount;
    u16 rewardSeed = RogueRouteSceneRng_Next(rng);
    u16 i;

    (void)request;
    if(eligibleCount == 0)
        return FALSE;

    for(i = 0; i < ARRAY_COUNT(sAnomalousFossilItems); ++i)
    {
        u16 species = RogueAdventureQuests_GetFossilSpecies(sAnomalousFossilItems[i]);

        if(species != SPECIES_NONE
            && RoguePokedex_IsSpeciesEnabled(species)
            && selected-- == 0)
        {
            AGB_ASSERT(i <= FOSSIL_PAYLOAD_INDEX_MASK);
            *payload = i | ((u32)rewardSeed << FOSSIL_PAYLOAD_SEED_SHIFT);
            return i <= FOSSIL_PAYLOAD_INDEX_MASK;
        }
    }

    return FALSE;
}

static void ExpandAnomalousFossilOfferPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    u8 fossilIndex = payload & FOSSIL_PAYLOAD_INDEX_MASK;

    if(fossilIndex >= ARRAY_COUNT(sAnomalousFossilItems))
        return;

    request->primaryGraphicsId = OBJ_EVENT_GFX_SCIENTIST_1;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_SCIENTIST_2;
    request->requestedItem = sAnomalousFossilItems[fossilIndex];
    request->rewardItem = RogueAdventureQuests_GetFossilSpecies(request->requestedItem);
    request->rewardAmount = payload >> FOSSIL_PAYLOAD_SEED_SHIFT;
}

static bool8 SelectForbiddenStoneOfferPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    (void)request;
    *payload = RogueRouteSceneRng_Next(rng);
    return TRUE;
}

static void ExpandForbiddenStoneOfferPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    request->primaryGraphicsId = OBJ_EVENT_GFX_MISC_CHANNELER;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MISC_CHANNELER;
    request->requestedItem = ITEM_ODD_KEYSTONE;
    request->rewardItem = ITEM_ABILITY_PATCH;
    request->rewardAmount = payload;
}

#define APRICORN_PAYLOAD_CHOICE_BITS 3
#define APRICORN_PAYLOAD_CHOICE_MASK 0x7

static bool8 SelectApricornGrovePayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u8 choices[ARRAY_COUNT(sApricornItems)];
    u8 i;

    (void)request;
    for(i = 0; i < ARRAY_COUNT(choices); ++i)
        choices[i] = i;
    for(i = 0; i < ROGUE_APRICORN_CHOICE_COUNT; ++i)
    {
        u8 selected = i + RogueRouteSceneRng_Next(rng) % (ARRAY_COUNT(choices) - i);
        u8 temp = choices[i];

        choices[i] = choices[selected];
        choices[selected] = temp;
    }

    *payload = choices[0]
        | ((u32)choices[1] << APRICORN_PAYLOAD_CHOICE_BITS)
        | ((u32)choices[2] << (APRICORN_PAYLOAD_CHOICE_BITS * 2));
    return TRUE;
}

static void ExpandApricornGrovePayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    if(request->recipeId == ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN
        && request->lotRole == 1)
    {
        u8 questId = FindAdventureQuestId(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);
        const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);

        request->primaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN;
        request->secondaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN;
        request->rewardAmount = ROGUE_APRICORN_BALL_REWARD_COUNT;
        if(quest != NULL)
        {
            request->requestedItem = quest->payload[0];
            request->rewardItem = quest->payload[1];
        }
        return;
    }

    request->primaryGraphicsId = OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN;
    request->requestedItem = sApricornItems[payload & APRICORN_PAYLOAD_CHOICE_MASK];
    request->rewardItem = sApricornItems[(payload >> APRICORN_PAYLOAD_CHOICE_BITS) & APRICORN_PAYLOAD_CHOICE_MASK];
    request->trainerNum = sApricornItems[(payload >> (APRICORN_PAYLOAD_CHOICE_BITS * 2)) & APRICORN_PAYLOAD_CHOICE_MASK];
}

enum
{
    ROUTE_FALLBACK_FAMILY_STOLEN_TRADE_CASE,
    ROUTE_FALLBACK_FAMILY_HEXED_SHRINE,
    ROUTE_FALLBACK_FAMILY_ANOMALOUS_FOSSIL,
    ROUTE_FALLBACK_FAMILY_FORBIDDEN_STONE,
    ROUTE_FALLBACK_FAMILY_APRICORN_CRAFTING,
};

#include "data/rogue_route_scene_recipes.h"

static const struct RogueRouteFallbackDefinition sRouteFallbacks[] =
{
    {ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER, 50, ROUTE_FALLBACK_FAMILY_STOLEN_TRADE_CASE, CanShowStolenTradeCaseOffer},
    {ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, 50, ROUTE_FALLBACK_FAMILY_HEXED_SHRINE, CanShowHexedShrine},
    {ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER, 50, ROUTE_FALLBACK_FAMILY_ANOMALOUS_FOSSIL, CanShowAnomalousFossilOffer},
    {ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER, 50, ROUTE_FALLBACK_FAMILY_FORBIDDEN_STONE, CanShowForbiddenStoneOffer},
    {ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE, 25, ROUTE_FALLBACK_FAMILY_APRICORN_CRAFTING, CanShowApricornGrove},
    {ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN, 25, ROUTE_FALLBACK_FAMILY_APRICORN_CRAFTING, CanShowApricornGrove},
};

const struct RogueRouteRecipeDefinition *RogueRouteEvents_GetRecipeDefinition(u8 recipeId)
{
    if(recipeId == ROGUE_ROUTE_SCENE_RECIPE_NONE
        || recipeId >= ROGUE_ROUTE_SCENE_RECIPE_COUNT
        || sRouteRecipes[recipeId].lotCount == 0)
        return NULL;

    return &sRouteRecipes[recipeId];
}

const struct RogueRouteFallbackDefinition *RogueRouteEvents_GetFallbackDefinition(u8 fallbackId)
{
    if(fallbackId >= ARRAY_COUNT(sRouteFallbacks))
        return NULL;

    return &sRouteFallbacks[fallbackId];
}

u8 RogueRouteEvents_GetFallbackCount(void)
{
    return ARRAY_COUNT(sRouteFallbacks);
}

void RogueRouteEvents_OnEnterScene(const struct RogueRouteSceneRequest *scene)
{
    const struct RogueAdventureQuest *quest;

    if(scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE)
        return;

    quest = RogueAdventureQuests_Get(scene->ownerQuestId);
    if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        && quest != NULL
        && quest->progress != 0
        && RogueRouteScenes_GetState(scene->sceneSlot) != ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        RogueRouteScenes_SetState(scene->sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
    else if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS
        && quest != NULL
        && (quest->progress & ((1 << ROGUE_FORBIDDEN_STONE_SOUL_COUNT) - 1)) == (1 << ROGUE_FORBIDDEN_STONE_SOUL_COUNT) - 1)
        RogueRouteScenes_SetState(scene->sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    else if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF
        && quest != NULL
        && quest->progress != 0
        && RogueRouteScenes_GetState(scene->sceneSlot) != ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        RogueRouteScenes_SetState(scene->sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
}

void RogueRouteEvents_PrepareSceneTrainers(const struct RogueRouteSceneRequest *scene)
{
    u8 trainerIdx;

    if(scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP)
        return;

    for(trainerIdx = 0; trainerIdx < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++trainerIdx)
    {
        if(Rogue_GetDynamicTrainer(trainerIdx) == scene->trainerNum)
            Rogue_SetDynamicTrainer(trainerIdx, TRAINER_NONE);
    }
}

u8 RogueRouteEvents_OnExitScene(const struct RogueRouteSceneRequest *scene)
{
    if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN
        && scene->lotRole == 1
        && RogueRouteScenes_GetState(scene->sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        return FindAdventureQuestId(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);

    return ROGUE_ADVENTURE_QUEST_INVALID_ID;
}

void RogueRouteEvents_GetInteractionData(void)
{
    struct RogueRouteSceneRequest scene;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_STATE_NOT_STARTED;
    gSpecialVar_0x8004 = ITEM_NONE;
    gSpecialVar_0x8005 = ITEM_NONE;
    gSpecialVar_0x8006 = TRAINER_NONE;
    gSpecialVar_0x8007 = 0;
    gSpecialVar_0x8008 = ROGUE_ROUTE_EVENT_STATE_NOT_STARTED;
    gSpecialVar_0x8009 = 0;

    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;

    gSpecialVar_Result = RogueRouteScenes_GetState(scene.sceneSlot);
    gSpecialVar_0x8004 = scene.rewardItem;
    gSpecialVar_0x8005 = scene.requestedItem;
    gSpecialVar_0x8006 = scene.trainerNum;
    gSpecialVar_0x8007 = scene.rewardAmount;
    gSpecialVar_0x8008 = gSpecialVar_Result;
    gSpecialVar_0x8009 = scene.lotRole;
}

void RogueRouteEvents_TryAcceptStolenTradeCaseQuest(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED)
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE)
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED)
        return;

    params.payload[1] = scene.trainerNum;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
        return;

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_BeginStolenTradeCaseBattle(void)
{
    struct RogueRouteSceneRequest scene;
    u16 state;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;
    state = RogueRouteScenes_GetState(scene.sceneSlot);
    if(scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED && state != ROGUE_ROUTE_EVENT_STATE_ACTIVE))
        return;

    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, scene.trainerNum);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_FinishStolenTradeCaseBattle(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};
    bool8 alreadyHasCase;
    u16 state;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;
    state = RogueRouteScenes_GetState(scene.sceneSlot);
    if(scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_ACTIVE && state != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING))
        return;

    if(state == ROGUE_ROUTE_EVENT_STATE_ACTIVE
        && !RogueAdventureQuests_EmitSignalForQuest(
            scene.ownerQuestId,
            ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
            1))
        return;

    alreadyHasCase = CheckBagHasItem(scene.requestedItem, 1);
    transaction.rewards[0].itemId = scene.requestedItem;
    transaction.rewards[0].count = 1;
    transaction.rewardCount = 1;
    if(!alreadyHasCase)
    {
        gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
        if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        {
            RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
            return;
        }
    }

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteScenes_HideProp(scene.sceneSlot, 1);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryClaimStolenTradeCaseReward(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};
    u16 state;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;
    state = RogueRouteScenes_GetState(scene.sceneSlot);
    if(scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
            && state != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING))
        return;

    transaction.costs[0].itemId = scene.requestedItem;
    transaction.costs[0].count = 1;
    transaction.rewards[0].itemId = scene.rewardItem;
    transaction.rewards[0].count = 1;
    transaction.moneyReward = ROGUE_STOLEN_TRADE_CASE_REWARD_MONEY;
    transaction.costCount = 1;
    transaction.rewardCount = 1;
    transaction.flags = ROGUE_EVENT_TRANSACTION_FLAG_ALLOW_COST_SLOTS_FOR_REWARDS;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        return;
    }

    FlagSet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteScenes_HideProp(scene.sceneSlot, 1);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryAcceptHexedShrine(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || !Rogue_CanActivateTemporaryDarkDealCurse(scene.requestedItem))
        return;

    transaction.rewards[0].itemId = scene.requestedItem;
    transaction.rewards[0].count = 1;
    transaction.moneyReward = scene.rewardAmount;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    if(!Rogue_TryActivateTemporaryDarkDealCurse(scene.requestedItem))
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
        return;
    }

    Rogue_PushPopup_AddMoney(scene.rewardAmount);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    VarSet(
        VAR_ROGUE_ROUTE_EVENT_STATE,
        VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) | ROUTE_SCENE_HEXED_SHRINE_ACCEPTED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryAcceptAnomalousFossilQuest(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    struct RogueEventTransaction transaction = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL)
        || RogueAdventureQuests_GetFossilSpecies(scene.requestedItem) != scene.rewardItem)
        return;

    transaction.rewards[0].itemId = scene.requestedItem;
    transaction.rewards[0].count = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    params.payload[0] = scene.requestedItem;
    params.payload[1] = scene.rewardAmount;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
        return;
    }

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static u32 GenerateFossilCustomMonId(const struct RogueRouteSceneRequest *scene, u8 restoration)
{
    RAND_TYPE originalRng = gRngValue;
    u32 customMonId;

    SeedRng(scene->rewardAmount ^ (restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0x51A7 : 0xB4E3));
    customMonId = RogueGift_CreateDynamicMonIdRawWithTypingChance(
        UNIQUE_RARITY_RARE,
        scene->rewardItem,
        restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0 : 100);
    gRngValue = originalRng;
    return customMonId;
}

static void BufferFossilCustomTyping(u8 *dest, u16 species, u32 customMonId)
{
    static const u8 sText_TypeSeparator[] = _("/");
    u8 type1 = RogueGift_GetCustomMonType(customMonId, 0);
    u8 type2 = RogueGift_GetCustomMonType(customMonId, 1);

    if(!IS_STANDARD_TYPE(type1))
        type1 = RoguePokedex_GetSpeciesType(species, 0);
    if(!IS_STANDARD_TYPE(type2))
        type2 = RoguePokedex_GetSpeciesType(species, 1);

    StringCopy(dest, gTypeNames[type1]);
    if(type2 != type1)
    {
        StringAppend(dest, sText_TypeSeparator);
        StringAppend(dest, gTypeNames[type2]);
    }
}

void RogueRouteEvents_BufferFossilRestorationData(void)
{
    struct RogueRouteSceneRequest scene;

    gStringVar1[0] = EOS;
    gStringVar2[0] = EOS;
    gStringVar3[0] = EOS;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE)
        return;

    BufferFossilCustomTyping(
        gStringVar1,
        scene.rewardItem,
        GenerateFossilCustomMonId(&scene, ROGUE_FOSSIL_RESTORATION_STABLE));
    BufferFossilCustomTyping(
        gStringVar2,
        scene.rewardItem,
        GenerateFossilCustomMonId(&scene, ROGUE_FOSSIL_RESTORATION_ADAPTIVE));
    StringCopy(gStringVar3, RoguePokedex_GetSpeciesName(scene.rewardItem));
}

void RogueRouteEvents_TryRestoreAnomalousFossil(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};
    struct Pokemon mon;
    RAND_TYPE originalRng;
    u32 customMonId;
    u8 giveResult;
    u8 restoration = gSpecialVar_0x8004;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED
        || restoration > ROGUE_FOSSIL_RESTORATION_ADAPTIVE)
        return;

    originalRng = gRngValue;
    SeedRng(scene.rewardAmount ^ (restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0x51A7 : 0xB4E3));
    customMonId = RogueGift_CreateDynamicMonIdRawWithTypingChance(
        UNIQUE_RARITY_RARE,
        scene.rewardItem,
        restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0 : 100);
    RogueGift_CreateMon(customMonId, &mon, scene.rewardItem, 1, USE_RANDOM_IVS);
    gRngValue = originalRng;

    transaction.costs[0].itemId = scene.requestedItem;
    transaction.costs[0].count = 1;
    transaction.costCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    giveResult = GiveTradedMonToPlayer(&mon);
    if(giveResult == MON_CANT_GIVE)
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;
        return;
    }

    GetSetPokedexSpeciesFlag(scene.rewardItem, FLAG_SET_CAUGHT);
    Rogue_PushPopup_AddPokemon(scene.rewardItem, TRUE, FALSE);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static u8 CountForbiddenStoneSouls(u8 progress)
{
    u8 count = 0;
    u8 i;

    for(i = 0; i < ROGUE_FORBIDDEN_STONE_SOUL_COUNT; ++i)
    {
        if((progress & (1 << i)) != 0)
            ++count;
    }

    return count;
}

void RogueRouteEvents_TryAcceptForbiddenStoneQuest(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    struct RogueEventTransaction transaction = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE))
        return;

    transaction.rewards[0].itemId = ITEM_ODD_KEYSTONE;
    transaction.rewards[0].count = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    params.payload[0] = scene.rewardAmount;
    params.target = ROGUE_FORBIDDEN_STONE_SOUL_COUNT;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
        return;
    }

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_CollectForbiddenStoneSoul(void)
{
    struct RogueRouteSceneRequest scene;
    const struct RogueAdventureQuest *quest;
    u8 progress;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    gSpecialVar_0x8007 = 0;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || scene.lotRole >= ROGUE_FORBIDDEN_STONE_SOUL_COUNT
        || !CheckBagHasItem(ITEM_ODD_KEYSTONE, 1))
        return;

    quest = RogueAdventureQuests_Get(scene.ownerQuestId);
    if(quest == NULL
        || quest->definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE
        || quest->nodeId != 0)
        return;

    if(!RogueAdventureQuests_EmitSignalForQuest(
        scene.ownerQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        1 << scene.lotRole))
        return;

    quest = RogueAdventureQuests_Get(scene.ownerQuestId);
    if(quest == NULL)
        return;
    progress = quest->progress;
    gSpecialVar_0x8007 = CountForbiddenStoneSouls(progress);
    if(gSpecialVar_0x8007 == ROGUE_FORBIDDEN_STONE_SOUL_COUNT)
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    else
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_PrepareForbiddenStoneBattle(void)
{
    struct RogueRouteSceneRequest scene;
    const struct RogueAdventureQuest *quest;
    struct RoguePokemonCompetitiveSetRules rules = {0};
    RAND_TYPE originalRng;
    u16 presetCount;
    u32 temp;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED
        || !CheckBagHasItem(ITEM_ODD_KEYSTONE, 1))
        return;

    quest = RogueAdventureQuests_Get(scene.ownerQuestId);
    if(quest == NULL
        || quest->definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE
        || quest->nodeId != 1
        || quest->progress != 0)
        return;

    originalRng = gRngValue;
    SeedRng(scene.trainerNum ^ 0x108);
    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], SPECIES_SPIRITOMB, Rogue_CalculateBossMonLvl(), USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    presetCount = gRoguePokemonProfiles[SPECIES_SPIRITOMB].competitiveSetCount;
    if(presetCount != 0)
    {
        const struct RoguePokemonCompetitiveSet *preset = &gRoguePokemonProfiles[SPECIES_SPIRITOMB].competitiveSets[Random() % presetCount];

        Rogue_ApplyMonCompetitiveSet(&gEnemyParty[0], Rogue_CalculateBossMonLvl(), preset, &rules);
    }
    temp = FALSE;
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &temp);
    CalculateMonStats(&gEnemyParty[0]);
    temp = GetMonData(&gEnemyParty[0], MON_DATA_MAX_HP);
    SetMonData(&gEnemyParty[0], MON_DATA_HP, &temp);
    gRngValue = originalRng;

    Rogue_ActivateUncatchableWildBattle();
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_FinishForbiddenStoneBattle(void)
{
    struct RogueRouteSceneRequest scene;
    const struct RogueAdventureQuest *quest;
    struct RogueEventTransaction transaction = {0};

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        return;

    quest = RogueAdventureQuests_Get(scene.ownerQuestId);
    if(quest == NULL
        || quest->definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE
        || quest->nodeId != 1)
        return;

    // Calling this after the battle records the win before attempting the
    // atomic payoff. A full Bag or wallet therefore never repeats the boss.
    if(quest->progress == 0
        && !RogueAdventureQuests_EmitSignalForQuest(
            scene.ownerQuestId,
            ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
            1))
        return;

    transaction.costs[0].itemId = ITEM_ODD_KEYSTONE;
    transaction.costs[0].count = 1;
    transaction.rewards[0].itemId = ITEM_ABILITY_PATCH;
    transaction.rewards[0].count = 1;
    transaction.moneyReward = ROGUE_FORBIDDEN_STONE_REWARD_MONEY;
    transaction.costCount = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        return;
    }

    Rogue_PushPopup_AddItem(ITEM_ABILITY_PATCH, 1);
    Rogue_PushPopup_AddMoney(ROGUE_FORBIDDEN_STONE_REWARD_MONEY);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static u16 GetApricornChoice(const struct RogueRouteSceneRequest *scene, u8 choice)
{
    switch(choice)
    {
    case 0:
        return scene->requestedItem;
    case 1:
        return scene->rewardItem;
    case 2:
        return scene->trainerNum;
    default:
        return ITEM_NONE;
    }
}

void RogueRouteEvents_BufferApricornTreeData(void)
{
    struct RogueRouteSceneRequest scene;
    u16 apricorn = ITEM_NONE;
    u16 ball = ITEM_NONE;

    gStringVar1[0] = EOS;
    gStringVar2[0] = EOS;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || (scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE
            && scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN)
        || scene.lotRole != 0)
        return;

    {
        u8 questId = FindAdventureQuestId(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);
        const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);

        if(quest != NULL)
        {
            apricorn = quest->payload[0];
            ball = quest->payload[1];
        }
        else if(gSelectedObjectEvent < OBJECT_EVENTS_COUNT)
        {
            u8 choice = RogueRouteScenes_GetSelectedPropId();

            apricorn = GetApricornChoice(&scene, choice);
            ball = GetApricornBall(apricorn);
        }
    }

    if(apricorn != ITEM_NONE)
        CopyItemName(apricorn, gStringVar1);
    if(ball != ITEM_NONE)
        CopyItemName(ball, gStringVar2);
}

void RogueRouteEvents_TryChooseApricorn(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    struct RogueEventTransaction transaction = {0};
    u16 apricorn;
    u16 ball;
    u8 choice;
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || (scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE
            && scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN)
        || scene.lotRole != 0
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING)
        || gSelectedObjectEvent >= OBJECT_EVENTS_COUNT)
        return;

    choice = RogueRouteScenes_GetSelectedPropId();
    apricorn = GetApricornChoice(&scene, choice);
    ball = GetApricornBall(apricorn);
    if(apricorn == ITEM_NONE || ball == ITEM_NONE)
        return;

    transaction.rewards[0].itemId = apricorn;
    transaction.rewards[0].count = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    params.payload[0] = apricorn;
    params.payload[1] = ball;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
        return;
    }

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryCraftApricornBalls(void)
{
    struct RogueRouteSceneRequest scene;
    const struct RogueAdventureQuest *quest;
    struct RogueEventTransaction transaction = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || (scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN
            && (scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN || scene.lotRole != 1))
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        return;

    questId = scene.source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        ? scene.ownerQuestId
        : FindAdventureQuestId(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);
    quest = RogueAdventureQuests_Get(questId);
    if(quest == NULL
        || quest->definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING
        || GetApricornBall(quest->payload[0]) != quest->payload[1])
        return;

    transaction.costs[0].itemId = quest->payload[0];
    transaction.costs[0].count = 1;
    transaction.rewards[0].itemId = quest->payload[1];
    transaction.rewards[0].count = ROGUE_APRICORN_BALL_REWARD_COUNT;
    transaction.costCount = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    Rogue_PushPopup_AddItem(quest->payload[1], ROGUE_APRICORN_BALL_REWARD_COUNT);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}
