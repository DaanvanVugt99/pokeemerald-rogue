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
#include "rogue_gifts.h"
#include "rogue_pokedex.h"
#include "rogue_popup.h"
#include "rogue_route_events.h"
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

#define ROUTE_SCENE_RECIPE_SHIFT 0
#define ROUTE_SCENE_RECIPE_MASK  0x3F
#define ROUTE_SCENE_LOT_SHIFT    6
#define ROUTE_SCENE_LOT_MASK     0x0F
#define ROUTE_SCENE_ROLE_SHIFT   10
#define ROUTE_SCENE_ROLE_MASK    0x03
#define ROUTE_SCENE_SLOT_SHIFT   12
#define ROUTE_SCENE_SLOT_MASK    0x03
#define ROUTE_SCENE_OWNER_SHIFT  14
#define ROUTE_SCENE_OWNER_MASK   0x3F

#define ROUTE_SCENE_OBJECT_SLOT_MASK 0x03
#define ROUTE_SCENE_OBJECT_ROLE_SHIFT 2
#define ROUTE_SCENE_OBJECT_ROLE_MASK 0x03
#define ROUTE_SCENE_HEXED_SHRINE_ACCEPTED (1 << 15)
#define ROUTE_SCENE_OBJECT_PROP_SHIFT 4
#define ROUTE_SCENE_OBJECT_PROP_MASK 0x0F

struct RogueRouteLot
{
    const struct ObjectEventTemplate *objectEvent;
    u8 id;
    u8 size;
};

static struct RogueRouteScenePlan *GetCurrentScenePlan(void)
{
    if(gRogueRun.adventureRoomId >= gRogueAdvPath.roomCount)
        return NULL;

    return &gRogueAdvPath.rooms[gRogueRun.adventureRoomId].routeScenePlan;
}

static u8 GetPlacementRecipe(const struct RogueRouteScenePlacement *placement)
{
    return (placement->packed >> ROUTE_SCENE_RECIPE_SHIFT) & ROUTE_SCENE_RECIPE_MASK;
}

static u8 GetPlacementLot(const struct RogueRouteScenePlacement *placement)
{
    return (placement->packed >> ROUTE_SCENE_LOT_SHIFT) & ROUTE_SCENE_LOT_MASK;
}

static u8 GetPlacementRole(const struct RogueRouteScenePlacement *placement)
{
    return (placement->packed >> ROUTE_SCENE_ROLE_SHIFT) & ROUTE_SCENE_ROLE_MASK;
}

static u8 GetPlacementSceneSlot(const struct RogueRouteScenePlacement *placement)
{
    return (placement->packed >> ROUTE_SCENE_SLOT_SHIFT) & ROUTE_SCENE_SLOT_MASK;
}

static u8 GetPlacementOwner(const struct RogueRouteScenePlacement *placement)
{
    return (placement->packed >> ROUTE_SCENE_OWNER_SHIFT) & ROUTE_SCENE_OWNER_MASK;
}

static struct RogueRouteScenePlacement PackPlacement(u8 recipeId, u8 lotId, u8 lotRole, u8 sceneSlot, u8 ownerQuestId)
{
    struct RogueRouteScenePlacement placement =
    {
        .packed = ((u32)recipeId << ROUTE_SCENE_RECIPE_SHIFT)
            | ((u32)lotId << ROUTE_SCENE_LOT_SHIFT)
            | ((u32)lotRole << ROUTE_SCENE_ROLE_SHIFT)
            | ((u32)sceneSlot << ROUTE_SCENE_SLOT_SHIFT)
            | ((u32)(ownerQuestId & ROUTE_SCENE_OWNER_MASK) << ROUTE_SCENE_OWNER_SHIFT),
    };

    return placement;
}

static u16 PackSceneObjectData(u8 sceneSlot, u8 lotRole, u8 propId)
{
    return (sceneSlot & ROUTE_SCENE_OBJECT_SLOT_MASK)
        | ((lotRole & ROUTE_SCENE_OBJECT_ROLE_MASK) << ROUTE_SCENE_OBJECT_ROLE_SHIFT)
        | ((propId & ROUTE_SCENE_OBJECT_PROP_MASK) << ROUTE_SCENE_OBJECT_PROP_SHIFT);
}

static u8 GetSceneObjectSlot(u16 objectData)
{
    return objectData & ROUTE_SCENE_OBJECT_SLOT_MASK;
}

static u8 GetSceneObjectRole(u16 objectData)
{
    return (objectData >> ROUTE_SCENE_OBJECT_ROLE_SHIFT) & ROUTE_SCENE_OBJECT_ROLE_MASK;
}

static u8 GetSceneObjectProp(u16 objectData)
{
    return (objectData >> ROUTE_SCENE_OBJECT_PROP_SHIFT) & ROUTE_SCENE_OBJECT_PROP_MASK;
}

u8 RogueRouteScenes_GetState(u8 sceneSlot)
{
    if(sceneSlot >= ROGUE_ROUTE_SCENE_MAX_PLACEMENTS)
        return ROGUE_ROUTE_EVENT_STATE_NOT_STARTED;

    return (VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) >> (sceneSlot * 2)) & 3;
}

void RogueRouteScenes_SetState(u8 sceneSlot, u8 state)
{
    u16 shift;
    u16 value;

    if(sceneSlot >= ROGUE_ROUTE_SCENE_MAX_PLACEMENTS)
        return;

    shift = sceneSlot * 2;
    value = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    value &= ~(3 << shift);
    value |= (state & 3) << shift;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, value);
}

#ifdef ROGUE_DEBUG
void RogueRouteScenes_DebugSetPlacement(u8 placementIndex, u8 recipeId, u8 lotId, u8 lotRole, u8 sceneSlot, u8 ownerQuestId)
{
    struct RogueRouteScenePlan *plan = GetCurrentScenePlan();

    if(plan == NULL || placementIndex >= ARRAY_COUNT(plan->placements))
        return;

    plan->placements[placementIndex] = PackPlacement(
        recipeId,
        lotId,
        lotRole,
        sceneSlot,
        ownerQuestId);
}
#endif

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

static u16 SelectEvilTeamTrainer(void)
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

    selectedIdx = RogueRandom() % eligibleCount;
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

void RogueRouteScenes_GenerateRoom(struct RogueAdvPathRoom *room)
{
    memset(&room->routeScenePlan, 0, sizeof(room->routeScenePlan));
}

static void HideSceneProp(u8 sceneSlot, u8 propId)
{
    u8 i;

    // The flag is shared by every prop that is already known to be hidden.
    // Visible conditional props keep flag 0, so one route-local flag can hide
    // any number of independently completed scenes after a quickload.
    FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    for(i = 0; i < gSaveBlock1Ptr->objectEventTemplatesCount; ++i)
    {
        const struct ObjectEventTemplate *objectEvent = &gSaveBlock1Ptr->objectEventTemplates[i];

        if(objectEvent->script == Rogue_RouteEvent_Prop
            && GetSceneObjectSlot(objectEvent->trainerRange_berryTreeId) == sceneSlot
            && GetSceneObjectProp(objectEvent->trainerRange_berryTreeId) == propId)
            RemoveObjectEventByLocalIdAndMap(objectEvent->localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    }
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

static void BuildStolenTradeCaseOffer(struct RogueRouteSceneRequest *request)
{
    request->recipeId = ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER;
    request->source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR;
    request->primaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    request->requestedItem = ITEM_TRADE_CASE;
    request->rewardItem = ITEM_BIG_POKEBLOCK_BUNDLE;
    request->trainerNum = SelectEvilTeamTrainer();
}

static void BuildHexedShrine(struct RogueRouteSceneRequest *request)
{
    request->recipeId = ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE;
    request->source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF;
    request->primaryGraphicsId = OBJ_EVENT_GFX_DEVIL_MAN;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_DEVIL_MAN;
    if(gRogueRun.temporaryDarkDealCurseItem != ITEM_NONE
        && (VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) & ROUTE_SCENE_HEXED_SHRINE_ACCEPTED) != 0)
    {
        request->requestedItem = gRogueRun.temporaryDarkDealCurseItem;
    }
    else
    {
        request->requestedItem = Rogue_SelectDarkDealCurseItem(RogueRandom());
    }
    request->rewardAmount = min(
        ROGUE_HEXED_SHRINE_REWARD_MAX,
        ROGUE_HEXED_SHRINE_REWARD_BASE + ROGUE_HEXED_SHRINE_REWARD_PER_DIFFICULTY * Rogue_GetCurrentDifficulty());
}

static void BuildAnomalousFossilOffer(struct RogueRouteSceneRequest *request)
{
    u16 eligibleCount = CountEligibleAnomalousFossils();
    u16 selected = eligibleCount == 0 ? 0 : RogueRandom() % eligibleCount;
    u16 i;

    request->recipeId = ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER;
    request->source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR;
    request->primaryGraphicsId = OBJ_EVENT_GFX_SCIENTIST_1;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_SCIENTIST_2;
    request->rewardAmount = RogueRandom();

    for(i = 0; i < ARRAY_COUNT(sAnomalousFossilItems); ++i)
    {
        u16 item = sAnomalousFossilItems[i];
        u16 species = RogueAdventureQuests_GetFossilSpecies(item);

        if(species != SPECIES_NONE
            && RoguePokedex_IsSpeciesEnabled(species)
            && selected-- == 0)
        {
            request->requestedItem = item;
            request->rewardItem = species;
            return;
        }
    }

    request->recipeId = ROGUE_ROUTE_SCENE_RECIPE_NONE;
}

static void BuildForbiddenStoneOffer(struct RogueRouteSceneRequest *request)
{
    request->recipeId = ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER;
    request->source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR;
    request->primaryGraphicsId = OBJ_EVENT_GFX_MISC_CHANNELER;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MISC_CHANNELER;
    request->requestedItem = ITEM_ODD_KEYSTONE;
    request->rewardItem = ITEM_ABILITY_PATCH;
    request->rewardAmount = RogueRandom();
}

static void BuildApricornGrove(struct RogueRouteSceneRequest *request)
{
    u8 choices[ARRAY_COUNT(sApricornItems)];
    u8 i;

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

    for(i = 0; i < ARRAY_COUNT(choices); ++i)
        choices[i] = i;
    for(i = 0; i < ROGUE_APRICORN_CHOICE_COUNT; ++i)
    {
        u8 selected = i + RogueRandom() % (ARRAY_COUNT(choices) - i);
        u8 temp = choices[i];

        choices[i] = choices[selected];
        choices[selected] = temp;
    }

    request->primaryGraphicsId = OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN;
    request->requestedItem = sApricornItems[choices[0]];
    request->rewardItem = sApricornItems[choices[1]];
    request->trainerNum = sApricornItems[choices[2]];
}

enum
{
    ROUTE_FALLBACK_FAMILY_STOLEN_TRADE_CASE,
    ROUTE_FALLBACK_FAMILY_HEXED_SHRINE,
    ROUTE_FALLBACK_FAMILY_ANOMALOUS_FOSSIL,
    ROUTE_FALLBACK_FAMILY_FORBIDDEN_STONE,
    ROUTE_FALLBACK_FAMILY_APRICORN_CRAFTING,
    ROUTE_FALLBACK_FAMILY_COUNT,
};

struct RogueRouteFallbackDefinition
{
    u8 recipeId;
    u8 weight;
    u8 familyId;
    bool8 (*isEligible)(u8 roomId);
};

struct RogueRouteRecipeDefinition
{
    void (*build)(struct RogueRouteSceneRequest *request);
    u8 source;
    u8 lotCount;
    u8 lotSizes[ROGUE_ROUTE_SCENE_MAX_ROLES];
    u8 objectCounts[ROGUE_ROUTE_SCENE_MAX_ROLES];
};

static const struct RogueRouteRecipeDefinition sRouteRecipes[ROGUE_ROUTE_SCENE_RECIPE_COUNT] =
{
    [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER] =
    {
        .build = BuildStolenTradeCaseOffer,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {3},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP] =
    {
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {4},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF] =
    {
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {3},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE] =
    {
        .build = BuildHexedShrine,
        .source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {4},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER] =
    {
        .build = BuildAnomalousFossilOffer,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {4},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION] =
    {
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {4},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER] =
    {
        .build = BuildForbiddenStoneOffer,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {4},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS] =
    {
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ROGUE_FORBIDDEN_STONE_SOUL_COUNT,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_SMALL, ROGUE_ROUTE_SCENE_LOT_SMALL, ROGUE_ROUTE_SCENE_LOT_SMALL},
        .objectCounts = {1, 1, 1},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF] =
    {
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_LARGE},
        .objectCounts = {4},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE] =
    {
        .build = BuildApricornGrove,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {3},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN] =
    {
        .build = BuildApricornGrove,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = 2,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM, ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {3, 3},
    },
    [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN] =
    {
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = 1,
        .lotSizes = {ROGUE_ROUTE_SCENE_LOT_MEDIUM},
        .objectCounts = {3},
    },
};

static const struct RogueRouteFallbackDefinition sRouteFallbacks[] =
{
    {ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER, 50, ROUTE_FALLBACK_FAMILY_STOLEN_TRADE_CASE, CanShowStolenTradeCaseOffer},
    {ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE, 50, ROUTE_FALLBACK_FAMILY_HEXED_SHRINE, CanShowHexedShrine},
    {ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER, 50, ROUTE_FALLBACK_FAMILY_ANOMALOUS_FOSSIL, CanShowAnomalousFossilOffer},
    {ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER, 50, ROUTE_FALLBACK_FAMILY_FORBIDDEN_STONE, CanShowForbiddenStoneOffer},
    {ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE, 25, ROUTE_FALLBACK_FAMILY_APRICORN_CRAFTING, CanShowApricornGrove},
    {ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN, 25, ROUTE_FALLBACK_FAMILY_APRICORN_CRAFTING, CanShowApricornGrove},
};

static const struct RogueRouteRecipeDefinition *GetRecipeDefinition(u8 recipeId)
{
    if(recipeId == ROGUE_ROUTE_SCENE_RECIPE_NONE
        || recipeId >= ROGUE_ROUTE_SCENE_RECIPE_COUNT
        || sRouteRecipes[recipeId].lotCount == 0)
        return NULL;

    return &sRouteRecipes[recipeId];
}

bool8 RogueRouteScenes_IsLotTemplate(const struct ObjectEventTemplate *objectEvent)
{
    return objectEvent->script == Rogue_RouteEvent_Interact
        && objectEvent->trainerType == TRAINER_TYPE_NONE
        && objectEvent->trainerRange_berryTreeId < ROGUE_ROUTE_SCENE_MAX_LOTS
        && objectEvent->movementRangeX < ROGUE_ROUTE_SCENE_LOT_SIZE_COUNT;
}

static const struct MapHeader *GetRouteMapHeader(u8 roomId)
{
    u8 routeIdx;

    if(roomId >= gRogueAdvPath.roomCount)
        return NULL;

    routeIdx = gRogueAdvPath.rooms[roomId].roomParams.roomIdx;
    if(routeIdx >= gRogueRouteTable.routeCount)
        return NULL;

    return Overworld_GetMapHeaderByGroupAndId(
        gRogueRouteTable.routes[routeIdx].map.group,
        gRogueRouteTable.routes[routeIdx].map.num);
}

static u8 CollectRouteLots(u8 roomId, struct RogueRouteLot *lots, u8 capacity, u8 *baseObjectCount)
{
    const struct MapHeader *mapHeader = GetRouteMapHeader(roomId);
    bool8 seenIds[ROGUE_ROUTE_SCENE_MAX_LOTS] = {FALSE};
    u8 count = 0;
    u8 i;

    *baseObjectCount = 0;
    if(mapHeader == NULL || mapHeader->events == NULL)
        return 0;

    for(i = 0; i < mapHeader->events->objectEventCount; ++i)
    {
        const struct ObjectEventTemplate *objectEvent = &mapHeader->events->objectEvents[i];

        if(RogueRouteScenes_IsLotTemplate(objectEvent))
        {
            u8 lotId = objectEvent->trainerRange_berryTreeId;

            if(lotId < ARRAY_COUNT(seenIds) && count < capacity && !seenIds[lotId])
            {
                lots[count].objectEvent = objectEvent;
                lots[count].id = lotId;
                lots[count].size = objectEvent->movementRangeX;
                seenIds[lotId] = TRUE;
                ++count;
            }
        }
        else
        {
            ++*baseObjectCount;
        }
    }

    return count;
}

static bool8 AddRecipeToPlan(
    struct RogueRouteScenePlan *plan,
    u8 *placementCount,
    u8 maxPlacements,
    u8 sceneSlot,
    u8 recipeId,
    u8 ownerQuestId,
    const struct RogueRouteLot *lots,
    u8 lotCount,
    u16 *usedLots,
    u8 *usedObjects,
    u8 objectBudget)
{
    const struct RogueRouteRecipeDefinition *definition = GetRecipeDefinition(recipeId);
    u16 pendingUsedLots = *usedLots;
    u8 selectedLots[ROGUE_ROUTE_SCENE_MAX_ROLES];
    u8 pendingObjects = *usedObjects;
    u8 role;

    if(definition == NULL
        || definition->lotCount > ROGUE_ROUTE_SCENE_MAX_ROLES
        || *placementCount + definition->lotCount > maxPlacements)
        return FALSE;

    for(role = 0; role < definition->lotCount; ++role)
    {
        u8 eligibleCount = 0;
        u8 selected;
        u8 i;

        for(i = 0; i < lotCount; ++i)
        {
            if((pendingUsedLots & (1 << lots[i].id)) == 0
                && lots[i].size >= definition->lotSizes[role])
                ++eligibleCount;
        }

        if(eligibleCount == 0)
            return FALSE;

        selected = RogueRandom() % eligibleCount;
        for(i = 0; i < lotCount; ++i)
        {
            if((pendingUsedLots & (1 << lots[i].id)) == 0
                && lots[i].size >= definition->lotSizes[role]
                && selected-- == 0)
            {
                selectedLots[role] = lots[i].id;
                pendingUsedLots |= 1 << lots[i].id;
                break;
            }
        }

        pendingObjects += definition->objectCounts[role];
        if(pendingObjects > objectBudget)
            return FALSE;
    }

    for(role = 0; role < definition->lotCount; ++role)
    {
        plan->placements[*placementCount] = PackPlacement(
            recipeId,
            selectedLots[role],
            role,
            sceneSlot,
            ownerQuestId);
        ++*placementCount;
    }

    *usedLots = pendingUsedLots;
    *usedObjects = pendingObjects;
    return TRUE;
}

static void BuildRouteScenePlan(u8 roomId, struct RogueRouteScenePlan *plan)
{
    struct RogueRouteSceneRequest questRequests[ROGUE_ROUTE_SCENE_MAX_PLACEMENTS] = {0};
    struct RogueRouteLot lots[ROGUE_ROUTE_SCENE_MAX_LOTS];
    bool8 usedFallbacks[ARRAY_COUNT(sRouteFallbacks)] = {FALSE};
    bool8 usedFallbackFamilies[ROUTE_FALLBACK_FAMILY_COUNT] = {FALSE};
    RAND_TYPE originalRng = gRngRogueValue;
    u16 usedLots = 0;
    u8 baseObjectCount;
    u8 objectBudget;
    u8 usedObjects = 0;
    u8 placementCount = 0;
    u8 sceneSlot = 0;
    u8 targetPlacements;
    u8 questCount;
    u8 lotCount;
    u8 i;

    memset(plan, 0, sizeof(*plan));
    lotCount = CollectRouteLots(roomId, lots, ARRAY_COUNT(lots), &baseObjectCount);
    if(lotCount == 0 || baseObjectCount >= OBJECT_EVENT_TEMPLATES_COUNT)
        return;

    objectBudget = OBJECT_EVENT_TEMPLATES_COUNT - baseObjectCount;
    SeedRogueRng(gRogueAdvPath.rooms[roomId].rngSeed ^ 0xA7E1);
    targetPlacements = 1 + RogueRandom() % ROGUE_ROUTE_SCENE_MAX_PLACEMENTS;

    questCount = RogueAdventureQuests_CollectSceneRequests(
        roomId,
        questRequests,
        ARRAY_COUNT(questRequests));
    targetPlacements = max(targetPlacements, questCount);

    for(i = 0; i < questCount && placementCount < ROGUE_ROUTE_SCENE_MAX_PLACEMENTS; ++i)
    {
        if(AddRecipeToPlan(
            plan,
            &placementCount,
            ROGUE_ROUTE_SCENE_MAX_PLACEMENTS,
            sceneSlot,
            questRequests[i].recipeId,
            questRequests[i].ownerQuestId,
            lots,
            lotCount,
            &usedLots,
            &usedObjects,
            objectBudget))
            ++sceneSlot;
    }

    while(placementCount < targetPlacements && sceneSlot < ROGUE_ROUTE_SCENE_MAX_PLACEMENTS)
    {
        u16 totalWeight = 0;
        u16 roll;
        u8 selectedFallback = ARRAY_COUNT(sRouteFallbacks);

        for(i = 0; i < ARRAY_COUNT(sRouteFallbacks); ++i)
        {
            if(!usedFallbacks[i]
                && !usedFallbackFamilies[sRouteFallbacks[i].familyId]
                && sRouteFallbacks[i].isEligible(roomId))
                totalWeight += sRouteFallbacks[i].weight;
        }

        if(totalWeight == 0)
            break;

        roll = RogueRandom() % totalWeight;
        for(i = 0; i < ARRAY_COUNT(sRouteFallbacks); ++i)
        {
            if(usedFallbacks[i]
                || usedFallbackFamilies[sRouteFallbacks[i].familyId]
                || !sRouteFallbacks[i].isEligible(roomId))
                continue;

            if(roll < sRouteFallbacks[i].weight)
            {
                selectedFallback = i;
                break;
            }
            roll -= sRouteFallbacks[i].weight;
        }

        if(selectedFallback >= ARRAY_COUNT(sRouteFallbacks))
            break;

        usedFallbacks[selectedFallback] = TRUE;
        if(AddRecipeToPlan(
            plan,
            &placementCount,
            ROGUE_ROUTE_SCENE_MAX_PLACEMENTS,
            sceneSlot,
            sRouteFallbacks[selectedFallback].recipeId,
            ROGUE_ADVENTURE_QUEST_INVALID_ID,
            lots,
            lotCount,
            &usedLots,
            &usedObjects,
            objectBudget))
        {
            usedFallbackFamilies[sRouteFallbacks[selectedFallback].familyId] = TRUE;
            ++sceneSlot;
        }
    }

    gRngRogueValue = originalRng;
}

u8 RogueRouteScenes_GetPlacementCount(void)
{
    const struct RogueRouteScenePlan *plan = GetCurrentScenePlan();
    u8 count = 0;

    if(plan == NULL)
        return 0;

    while(count < ARRAY_COUNT(plan->placements)
        && GetPlacementRecipe(&plan->placements[count]) != ROGUE_ROUTE_SCENE_RECIPE_NONE)
        ++count;

    return count;
}

bool8 RogueRouteScenes_GetPlacementRequest(u8 placementIndex, struct RogueRouteSceneRequest *request)
{
    const struct RogueRouteScenePlan *plan = GetCurrentScenePlan();
    const struct RogueRouteScenePlacement *placement;
    const struct RogueRouteRecipeDefinition *definition;
    RAND_TYPE originalRng;
    u8 recipeId;
    u8 routeIdx;

    if(plan == NULL || placementIndex >= ARRAY_COUNT(plan->placements))
        return FALSE;

    placement = &plan->placements[placementIndex];
    recipeId = GetPlacementRecipe(placement);
    definition = GetRecipeDefinition(recipeId);
    routeIdx = gRogueAdvPath.rooms[gRogueRun.adventureRoomId].roomParams.roomIdx;
    if(definition == NULL || routeIdx >= gRogueRouteTable.routeCount)
        return FALSE;

    memset(request, 0, sizeof(*request));
    request->recipeId = recipeId;
    request->environment = gRogueRouteTable.routes[routeIdx].environment;
    request->lotId = GetPlacementLot(placement);
    request->lotRole = GetPlacementRole(placement);
    request->sceneSlot = GetPlacementSceneSlot(placement);
    request->source = definition->source;
    request->ownerQuestId = definition->source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        ? GetPlacementOwner(placement)
        : ROGUE_ADVENTURE_QUEST_INVALID_ID;

    originalRng = gRngRogueValue;
    SeedRogueRng(gRogueAdvPath.rooms[gRogueRun.adventureRoomId].rngSeed
        ^ 0x5EED
        ^ (recipeId * 257)
        ^ (request->sceneSlot * 4051));

    if(definition->source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE)
    {
        struct RogueRouteSceneRequest content = {0};

        if(!RogueAdventureQuests_BuildSceneRequest(request->ownerQuestId, &content))
        {
            gRngRogueValue = originalRng;
            return FALSE;
        }

        request->rewardItem = content.rewardItem;
        request->requestedItem = content.requestedItem;
        request->trainerNum = content.trainerNum;
        request->primaryGraphicsId = content.primaryGraphicsId;
        request->secondaryGraphicsId = content.secondaryGraphicsId;
        request->rewardAmount = content.rewardAmount;
    }
    else if(definition->build != NULL)
    {
        definition->build(request);
    }

    request->recipeId = recipeId;
    request->source = definition->source;
    gRngRogueValue = originalRng;
    return TRUE;
}

static bool8 GetSceneRequestBySlotAndRole(u8 sceneSlot, u8 lotRole, struct RogueRouteSceneRequest *request)
{
    u8 i;

    for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
    {
        if(RogueRouteScenes_GetPlacementRequest(i, request)
            && request->sceneSlot == sceneSlot
            && request->lotRole == lotRole)
            return TRUE;
    }

    return FALSE;
}

static bool8 GetCurrentInteractionRequest(struct RogueRouteSceneRequest *request)
{
    u16 objectData;

    if(gSelectedObjectEvent >= OBJECT_EVENTS_COUNT)
        return FALSE;

    objectData = gObjectEvents[gSelectedObjectEvent].trainerRange_berryTreeId;
    return GetSceneRequestBySlotAndRole(
        GetSceneObjectSlot(objectData),
        GetSceneObjectRole(objectData),
        request);
}

void RogueRouteScenes_OnEnterRoute(void)
{
    struct RogueRouteScenePlan *plan;
    u8 i;

    if(gRogueRun.routeSceneRoomId != gRogueRun.adventureRoomId)
    {
        VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, 0);
        FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
        gRogueRun.routeSceneRoomId = gRogueRun.adventureRoomId;
    }

    plan = GetCurrentScenePlan();
    if(plan == NULL)
        return;

    BuildRouteScenePlan(gRogueRun.adventureRoomId, plan);

    // Won camps retain their collection-only state across same-route reloads.
    for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
    {
        struct RogueRouteSceneRequest scene;

        if(RogueRouteScenes_GetPlacementRequest(i, &scene)
            && scene.source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE)
        {
            const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(scene.ownerQuestId);

            if(scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
                && quest != NULL
                && quest->progress != 0
                && RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_COMPLETED)
                RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
            else if(scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS
                && quest != NULL
                && (quest->progress & ((1 << ROGUE_FORBIDDEN_STONE_SOUL_COUNT) - 1)) == (1 << ROGUE_FORBIDDEN_STONE_SOUL_COUNT) - 1)
                RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
            else if(scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF
                && quest != NULL
                && quest->progress != 0
                && RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_COMPLETED)
                RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        }
    }
}

void RogueRouteScenes_PrepareRouteTrainers(void)
{
    u8 placementIdx;

    for(placementIdx = 0; placementIdx < RogueRouteScenes_GetPlacementCount(); ++placementIdx)
    {
        struct RogueRouteSceneRequest scene;
        u8 trainerIdx;

        if(!RogueRouteScenes_GetPlacementRequest(placementIdx, &scene)
            || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP)
            continue;

        for(trainerIdx = 0; trainerIdx < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++trainerIdx)
        {
            if(Rogue_GetDynamicTrainer(trainerIdx) == scene.trainerNum)
                Rogue_SetDynamicTrainer(trainerIdx, TRAINER_NONE);
        }
    }
}

void RogueRouteScenes_OnExitRoute(void)
{
    bool8 advancedQuests[ROGUE_ADVENTURE_QUEST_CAPACITY] = {FALSE};
    u8 i;

    RogueAdventureQuests_EmitSignal(ROGUE_ADVENTURE_QUEST_SIGNAL_ROUTE_COMPLETED, 1);
    for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
    {
        struct RogueRouteSceneRequest scene;

        if(!RogueRouteScenes_GetPlacementRequest(i, &scene))
            continue;

        if(scene.source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
            && scene.ownerQuestId < ROGUE_ADVENTURE_QUEST_CAPACITY
            && !advancedQuests[scene.ownerQuestId]
            && RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        {
            advancedQuests[scene.ownerQuestId] = TRUE;
            RogueAdventureQuests_Advance(scene.ownerQuestId);
        }
        else if(scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN
            && scene.lotRole == 1
            && RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        {
            u8 questId = FindAdventureQuestId(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);

            if(questId < ROGUE_ADVENTURE_QUEST_CAPACITY && !advancedQuests[questId])
            {
                advancedQuests[questId] = TRUE;
                RogueAdventureQuests_Advance(questId);
            }
        }
    }

    RogueAdventureQuests_LeaveRoute(gRogueRun.adventureRoomId);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, 0);
    FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
}

static u8 FindFreeLocalId(const struct ObjectEventTemplate *objectEvents, u8 objectEventCount)
{
    u8 localId;
    u8 i;

    for(localId = 1; localId < OBJ_EVENT_ID_CAMERA; ++localId)
    {
        for(i = 0; i < objectEventCount; ++i)
        {
            if(objectEvents[i].localId == localId)
                break;
        }

        if(i == objectEventCount)
            return localId;
    }

    return 0;
}

static void AppendSceneObject(
    struct ObjectEventTemplate *objectEvents,
    u8 *objectEventCount,
    const struct ObjectEventTemplate *anchor,
    u8 localId,
    s8 xOffset,
    s8 yOffset,
    u16 graphicsId,
    const u8 *script,
    u16 objectData,
    u16 flagId)
{
    struct ObjectEventTemplate *objectEvent = &objectEvents[(*objectEventCount)++];

    memset(objectEvent, 0, sizeof(*objectEvent));
    objectEvent->localId = localId;
    objectEvent->graphicsId = graphicsId;
    objectEvent->x = anchor->x + xOffset;
    objectEvent->y = anchor->y + yOffset;
    objectEvent->elevation = anchor->elevation;
    objectEvent->movementType = MOVEMENT_TYPE_FACE_DOWN;
    objectEvent->trainerType = TRAINER_TYPE_NONE;
    objectEvent->trainerRange_berryTreeId = objectData;
    objectEvent->script = script;
    objectEvent->flagId = flagId;
}

static const u8 *GetSceneNpcScript(const struct RogueRouteSceneRequest *scene)
{
    switch(scene->recipeId)
    {
    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER:
        return Rogue_RouteEvent_StolenTradeCaseOffer;
    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP:
        return Rogue_RouteEvent_StolenTradeCaseCamp;
    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF:
        return Rogue_RouteEvent_StolenTradeCasePayoff;
    case ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE:
        return Rogue_RouteEvent_HexedShrine;
    case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER:
        return Rogue_RouteEvent_AnomalousFossilOffer;
    case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION:
        return Rogue_RouteEvent_AnomalousFossilRestoration;
    case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER:
        return Rogue_RouteEvent_ForbiddenStoneOffer;
    case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS:
        return Rogue_RouteEvent_ForbiddenStoneSoul;
    case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF:
        return Rogue_RouteEvent_ForbiddenStonePayoff;
    case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE:
        return Rogue_RouteEvent_ApricornTree;
    case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN:
        return scene->lotRole == 0 ? Rogue_RouteEvent_ApricornTree : Rogue_RouteEvent_ApricornArtisan;
    case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN:
        return Rogue_RouteEvent_ApricornArtisan;
    default:
        return NULL;
    }
}

static bool8 IsForbiddenStoneSoulCollected(const struct RogueRouteSceneRequest *scene)
{
    const struct RogueAdventureQuest *quest;

    if(scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS
        || scene->lotRole >= ROGUE_FORBIDDEN_STONE_SOUL_COUNT)
        return FALSE;

    quest = RogueAdventureQuests_Get(scene->ownerQuestId);
    return quest != NULL && (quest->progress & (1 << scene->lotRole)) != 0;
}

static bool8 RestoreSceneObject(
    struct ObjectEventTemplate *objectEvents,
    u8 objectEventCount,
    const struct ObjectEventTemplate *anchor,
    u8 localId,
    s8 xOffset,
    s8 yOffset,
    u16 graphicsId,
    const u8 *script,
    u16 objectData,
    u16 flagId)
{
    u8 i;

    for(i = 0; i < objectEventCount; ++i)
    {
        struct ObjectEventTemplate *objectEvent = &objectEvents[i];

        if(objectEvent->localId == localId
            && objectEvent->x == anchor->x + xOffset
            && objectEvent->y == anchor->y + yOffset)
        {
            u8 preservedLocalId = objectEvent->localId;

            memset(objectEvent, 0, sizeof(*objectEvent));
            objectEvent->localId = preservedLocalId;
            objectEvent->graphicsId = graphicsId;
            objectEvent->x = anchor->x + xOffset;
            objectEvent->y = anchor->y + yOffset;
            objectEvent->elevation = anchor->elevation;
            objectEvent->movementType = MOVEMENT_TYPE_FACE_DOWN;
            objectEvent->trainerType = TRAINER_TYPE_NONE;
            objectEvent->trainerRange_berryTreeId = objectData;
            objectEvent->script = script;
            objectEvent->flagId = flagId;
            return TRUE;
        }
    }

    return FALSE;
}

static void RestoreSceneProp(
    struct ObjectEventTemplate *objectEvents,
    u8 objectEventCount,
    const struct ObjectEventTemplate *anchor,
    s8 xOffset,
    s8 yOffset,
    u16 graphicsId,
    const u8 *script,
    u16 objectData,
    u16 flagId)
{
    u8 i;

    for(i = 0; i < objectEventCount; ++i)
    {
        const struct ObjectEventTemplate *objectEvent = &objectEvents[i];

        if(objectEvent->graphicsId == graphicsId
            && objectEvent->x == anchor->x + xOffset
            && objectEvent->y == anchor->y + yOffset)
        {
            RestoreSceneObject(objectEvents, objectEventCount, anchor, objectEvent->localId, xOffset, yOffset, graphicsId, script, objectData, flagId);
            return;
        }
    }
}

void RogueRouteScenes_RestoreObjectEvents(
    struct ObjectEventTemplate *objectEvents,
    u8 objectEventCount,
    const struct ObjectEventTemplate *baseObjectEvents,
    u8 baseObjectEventCount)
{
    u8 placementIdx;

    for(placementIdx = 0; placementIdx < RogueRouteScenes_GetPlacementCount(); ++placementIdx)
    {
        struct RogueRouteSceneRequest scene;
        const struct ObjectEventTemplate *lot = NULL;
        const u8 *npcScript;
        u16 mainData;
        u16 mainFlag = 0;
        u8 i;

        if(!RogueRouteScenes_GetPlacementRequest(placementIdx, &scene))
            continue;

        npcScript = GetSceneNpcScript(&scene);
        if(npcScript == NULL)
            continue;

        for(i = 0; i < baseObjectEventCount; ++i)
        {
            if(RogueRouteScenes_IsLotTemplate(&baseObjectEvents[i])
                && baseObjectEvents[i].trainerRange_berryTreeId == scene.lotId)
            {
                lot = &baseObjectEvents[i];
                break;
            }
        }

        mainData = PackSceneObjectData(scene.sceneSlot, scene.lotRole, 0);
        if(IsForbiddenStoneSoulCollected(&scene))
        {
            FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
            mainFlag = FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN;
        }
        if(lot == NULL
            || !RestoreSceneObject(objectEvents, objectEventCount, lot, lot->localId, 0, 0, scene.primaryGraphicsId, npcScript, mainData, mainFlag))
            continue;

#define RESTORE_PROP_FLAG(propId, x, y, gfx, script, flag) \
        RestoreSceneProp(objectEvents, objectEventCount, lot, x, y, gfx, script, PackSceneObjectData(scene.sceneSlot, scene.lotRole, propId), flag)
#define RESTORE_PROP(propId, x, y, gfx, script) \
        RESTORE_PROP_FLAG(propId, x, y, gfx, script, 0)

        switch(scene.recipeId)
        {
        case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER:
            RESTORE_PROP(1, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop);
            RESTORE_PROP(2, 1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP:
            if(RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
            {
                FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
                RESTORE_PROP_FLAG(1, 0, 1, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
            }
            else
                RESTORE_PROP(1, 0, 1, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop);
            RESTORE_PROP(2, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop);
            RESTORE_PROP(3, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF:
            if(RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
            {
                FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
                RESTORE_PROP_FLAG(1, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
            }
            else
                RESTORE_PROP(1, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop);
            RESTORE_PROP(2, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE:
            RESTORE_PROP(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_HexedShrineProp);
            RESTORE_PROP(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp);
            RESTORE_PROP(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER:
            RESTORE_PROP(1, 0, -1, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp);
            RESTORE_PROP(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp);
            RESTORE_PROP(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION:
            RESTORE_PROP(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_FossilProp);
            RESTORE_PROP(2, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_FossilProp);
            RESTORE_PROP(3, 1, 0, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER:
        case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF:
            RESTORE_PROP(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_ForbiddenStoneProp);
            RESTORE_PROP(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_ForbiddenStoneProp);
            RESTORE_PROP(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_ForbiddenStoneProp);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE:
            RESTORE_PROP(1, -1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree);
            RESTORE_PROP(2, 1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN:
            if(scene.lotRole == 0)
            {
                RESTORE_PROP(1, -1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree);
                RESTORE_PROP(2, 1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree);
            }
            else
            {
                RESTORE_PROP(1, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_ApricornProp);
                RESTORE_PROP(2, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_ApricornProp);
            }
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN:
            RESTORE_PROP(1, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_ApricornProp);
            RESTORE_PROP(2, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_ApricornProp);
            break;
        }

#undef RESTORE_PROP
#undef RESTORE_PROP_FLAG
    }
}

void RogueRouteScenes_ModifyObjectEvents(struct ObjectEventTemplate *objectEvents, u8 *objectEventCount, u8 objectEventCapacity)
{
    struct ObjectEventTemplate lots[ROGUE_ROUTE_SCENE_MAX_LOTS];
    bool8 foundLots[ROGUE_ROUTE_SCENE_MAX_LOTS] = {FALSE};
    u8 originalCount = *objectEventCount;
    u8 write = 0;
    u8 requiredCount = 0;
    u8 placementIdx;
    u8 i;

    for(i = 0; i < originalCount; ++i)
    {
        if(RogueRouteScenes_IsLotTemplate(&objectEvents[i]))
        {
            u8 lotId = objectEvents[i].trainerRange_berryTreeId;
            lots[lotId] = objectEvents[i];
            foundLots[lotId] = TRUE;
        }
        else
        {
            objectEvents[write++] = objectEvents[i];
        }
    }
    *objectEventCount = write;

    for(placementIdx = 0; placementIdx < RogueRouteScenes_GetPlacementCount(); ++placementIdx)
    {
        struct RogueRouteSceneRequest scene;
        const struct RogueRouteRecipeDefinition *definition;

        if(RogueRouteScenes_GetPlacementRequest(placementIdx, &scene)
            && !IsForbiddenStoneSoulCollected(&scene))
        {
            definition = GetRecipeDefinition(scene.recipeId);
            if(definition != NULL)
                requiredCount += definition->objectCounts[scene.lotRole];
        }
    }

    if(*objectEventCount + requiredCount > objectEventCapacity)
        return;

    for(placementIdx = 0; placementIdx < RogueRouteScenes_GetPlacementCount(); ++placementIdx)
    {
        struct RogueRouteSceneRequest scene;
        const struct ObjectEventTemplate *lot;
        const u8 *npcScript;
        u16 mainData;
        u8 localId;

        if(!RogueRouteScenes_GetPlacementRequest(placementIdx, &scene)
            || scene.lotId >= ARRAY_COUNT(lots)
            || !foundLots[scene.lotId]
            || IsForbiddenStoneSoulCollected(&scene))
            continue;

        lot = &lots[scene.lotId];
        npcScript = GetSceneNpcScript(&scene);
        if(npcScript == NULL)
            continue;

        mainData = PackSceneObjectData(scene.sceneSlot, scene.lotRole, 0);
        AppendSceneObject(objectEvents, objectEventCount, lot, lot->localId, 0, 0, scene.primaryGraphicsId, npcScript, mainData, 0);

#define APPEND_PROP(propId, x, y, gfx, script) \
        do \
        { \
            localId = FindFreeLocalId(objectEvents, *objectEventCount); \
            AppendSceneObject(objectEvents, objectEventCount, lot, localId, x, y, gfx, script, PackSceneObjectData(scene.sceneSlot, scene.lotRole, propId), 0); \
        } while(0)

        switch(scene.recipeId)
        {
        case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER:
            APPEND_PROP(1, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop);
            APPEND_PROP(2, 1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP:
            if(RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_COMPLETED)
                APPEND_PROP(1, 0, 1, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop);
            APPEND_PROP(2, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop);
            APPEND_PROP(3, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF:
            if(RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_COMPLETED)
                APPEND_PROP(1, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop);
            APPEND_PROP(2, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE:
            APPEND_PROP(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_HexedShrineProp);
            APPEND_PROP(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp);
            APPEND_PROP(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER:
            APPEND_PROP(1, 0, -1, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp);
            APPEND_PROP(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp);
            APPEND_PROP(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION:
            APPEND_PROP(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_FossilProp);
            APPEND_PROP(2, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_FossilProp);
            APPEND_PROP(3, 1, 0, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER:
        case ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF:
            APPEND_PROP(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_ForbiddenStoneProp);
            APPEND_PROP(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_ForbiddenStoneProp);
            APPEND_PROP(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_ForbiddenStoneProp);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE:
            APPEND_PROP(1, -1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree);
            APPEND_PROP(2, 1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree);
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN:
            if(scene.lotRole == 0)
            {
                APPEND_PROP(1, -1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree);
                APPEND_PROP(2, 1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree);
            }
            else
            {
                APPEND_PROP(1, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_ApricornProp);
                APPEND_PROP(2, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_ApricornProp);
            }
            break;
        case ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN:
            APPEND_PROP(1, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_ApricornProp);
            APPEND_PROP(2, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_ApricornProp);
            break;
        }

#undef APPEND_PROP
    }
}

static void ApplyAccentMetatile(const struct ObjectEventTemplate *anchor, s8 xOffset, s8 yOffset)
{
    s16 x = anchor->x + xOffset;
    s16 y = anchor->y + yOffset;
    u16 metatile;

    if(x < 0 || y < 0 || x >= gMapHeader.mapLayout->width || y >= gMapHeader.mapLayout->height)
        return;

    metatile = MapGridGetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET);
    if(gMapHeader.mapLayout->primaryTileset == &gTileset_General && metatile == METATILE_General_Grass)
        MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, METATILE_General_Grass_Stone);
    else if(gMapHeader.mapLayout->primaryTileset == &gTileset_GeneralHub && metatile == METATILE_GeneralHub_Grass)
        MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, METATILE_GeneralHub_Pebbles);
}

void RogueRouteScenes_ApplyMetatiles(void)
{
    const struct ObjectEventTemplate *lots[ROGUE_ROUTE_SCENE_MAX_LOTS] = {NULL};
    u8 i;

    if(gRogueAdvPath.currentRoomType != ADVPATH_ROOM_ROUTE)
        return;

    for(i = 0; i < gMapHeader.events->objectEventCount; ++i)
    {
        const struct ObjectEventTemplate *objectEvent = &gMapHeader.events->objectEvents[i];

        if(RogueRouteScenes_IsLotTemplate(objectEvent))
            lots[objectEvent->trainerRange_berryTreeId] = objectEvent;
    }

    for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
    {
        struct RogueRouteSceneRequest scene;
        const struct ObjectEventTemplate *lot;

        if(!RogueRouteScenes_GetPlacementRequest(i, &scene)
            || scene.lotId >= ARRAY_COUNT(lots)
            || (lot = lots[scene.lotId]) == NULL)
            continue;

        ApplyAccentMetatile(lot, -1, 0);
        ApplyAccentMetatile(lot, 1, 0);
        if(scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP)
            ApplyAccentMetatile(lot, 0, 1);
        else if(scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
            || scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
            || scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION
            || scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER
            || scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF)
            ApplyAccentMetatile(lot, 0, -1);
    }
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

    if(!GetCurrentInteractionRequest(&scene))
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
    if(!GetCurrentInteractionRequest(&scene)
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
    if(!GetCurrentInteractionRequest(&scene))
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
    bool8 alreadyHasCase;
    u16 state;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetCurrentInteractionRequest(&scene))
        return;
    state = RogueRouteScenes_GetState(scene.sceneSlot);
    if(scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_ACTIVE && state != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING))
        return;

    if(state == ROGUE_ROUTE_EVENT_STATE_ACTIVE
        && !RogueAdventureQuests_SetProgress(scene.ownerQuestId, 1))
        return;

    alreadyHasCase = CheckBagHasItem(scene.requestedItem, 1);
    if(!alreadyHasCase
        && (!CheckBagHasSpace(scene.requestedItem, 1) || !AddBagItem(scene.requestedItem, 1)))
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    HideSceneProp(scene.sceneSlot, 1);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryClaimStolenTradeCaseReward(void)
{
    struct RogueRouteSceneRequest scene;
    u16 state;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetCurrentInteractionRequest(&scene))
        return;
    state = RogueRouteScenes_GetState(scene.sceneSlot);
    if(scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
            && state != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING))
        return;

    if(!CheckBagHasItem(scene.requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    if(!RemoveBagItem(scene.requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    if(!AddBagItem(scene.rewardItem, 1))
    {
        AddBagItem(scene.requestedItem, 1);
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    AddMoney(&gSaveBlock1Ptr->money, ROGUE_STOLEN_TRADE_CASE_REWARD_MONEY);

    FlagSet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    HideSceneProp(scene.sceneSlot, 1);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryAcceptHexedShrine(void)
{
    struct RogueRouteSceneRequest scene;
    u32 money;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || gRogueRun.temporaryDarkDealCurseItem != ITEM_NONE)
        return;

    if(!CheckBagHasSpace(scene.requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    money = GetMoney(&gSaveBlock1Ptr->money);
    if(scene.rewardAmount > MAX_MONEY || money > MAX_MONEY - scene.rewardAmount)
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MONEY_FULL;
        return;
    }

    if(!Rogue_TryAddTemporaryDarkDealCurse(scene.requestedItem))
        return;

    AddMoney(&gSaveBlock1Ptr->money, scene.rewardAmount);
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
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL)
        || RogueAdventureQuests_GetFossilSpecies(scene.requestedItem) != scene.rewardItem)
        return;

    if(!CheckBagHasSpace(scene.requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    if(!AddBagItem(scene.requestedItem, 1))
        return;

    params.payload[0] = scene.requestedItem;
    params.payload[1] = scene.rewardAmount;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RemoveBagItem(scene.requestedItem, 1);
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
    if(!GetCurrentInteractionRequest(&scene)
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
    struct Pokemon mon;
    RAND_TYPE originalRng;
    u32 customMonId;
    u8 giveResult;
    u8 restoration = gSpecialVar_0x8004;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED
        || restoration > ROGUE_FOSSIL_RESTORATION_ADAPTIVE)
        return;

    if(!CheckBagHasItem(scene.requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    originalRng = gRngValue;
    SeedRng(scene.rewardAmount ^ (restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0x51A7 : 0xB4E3));
    customMonId = RogueGift_CreateDynamicMonIdRawWithTypingChance(
        UNIQUE_RARITY_RARE,
        scene.rewardItem,
        restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0 : 100);
    RogueGift_CreateMon(customMonId, &mon, scene.rewardItem, 1, USE_RANDOM_IVS);
    gRngValue = originalRng;

    if(!RemoveBagItem(scene.requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    giveResult = GiveTradedMonToPlayer(&mon);
    if(giveResult == MON_CANT_GIVE)
    {
        AddBagItem(scene.requestedItem, 1);
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
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE))
        return;

    if(!CheckBagHasSpace(ITEM_ODD_KEYSTONE, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    if(!AddBagItem(ITEM_ODD_KEYSTONE, 1))
        return;

    params.payload[0] = scene.rewardAmount;
    params.target = ROGUE_FORBIDDEN_STONE_SOUL_COUNT;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RemoveBagItem(ITEM_ODD_KEYSTONE, 1);
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
    if(!GetCurrentInteractionRequest(&scene)
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

    progress = quest->progress | (1 << scene.lotRole);
    if(!RogueAdventureQuests_SetProgress(scene.ownerQuestId, progress))
        return;

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
    if(!GetCurrentInteractionRequest(&scene)
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
    u32 money;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetCurrentInteractionRequest(&scene)
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
    if(quest->progress == 0 && !RogueAdventureQuests_SetProgress(scene.ownerQuestId, 1))
        return;

    if(!CheckBagHasItem(ITEM_ODD_KEYSTONE, 1))
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    money = GetMoney(&gSaveBlock1Ptr->money);
    if(money > MAX_MONEY - ROGUE_FORBIDDEN_STONE_REWARD_MONEY)
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MONEY_FULL;
        return;
    }

    if(!CheckBagHasSpace(ITEM_ABILITY_PATCH, 1))
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    if(!RemoveBagItem(ITEM_ODD_KEYSTONE, 1))
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    if(!AddBagItem(ITEM_ABILITY_PATCH, 1))
    {
        AddBagItem(ITEM_ODD_KEYSTONE, 1);
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    AddMoney(&gSaveBlock1Ptr->money, ROGUE_FORBIDDEN_STONE_REWARD_MONEY);
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
    if(!GetCurrentInteractionRequest(&scene)
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
            u8 choice = GetSceneObjectProp(gObjectEvents[gSelectedObjectEvent].trainerRange_berryTreeId);

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
    u16 apricorn;
    u16 ball;
    u8 choice;
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetCurrentInteractionRequest(&scene)
        || (scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE
            && scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN)
        || scene.lotRole != 0
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING)
        || gSelectedObjectEvent >= OBJECT_EVENTS_COUNT)
        return;

    choice = GetSceneObjectProp(gObjectEvents[gSelectedObjectEvent].trainerRange_berryTreeId);
    apricorn = GetApricornChoice(&scene, choice);
    ball = GetApricornBall(apricorn);
    if(apricorn == ITEM_NONE || ball == ITEM_NONE)
        return;

    if(!CheckBagHasSpace(apricorn, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }
    if(!AddBagItem(apricorn, 1))
        return;

    params.payload[0] = apricorn;
    params.payload[1] = ball;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RemoveBagItem(apricorn, 1);
        return;
    }

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryCraftApricornBalls(void)
{
    struct RogueRouteSceneRequest scene;
    const struct RogueAdventureQuest *quest;
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetCurrentInteractionRequest(&scene)
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

    if(!CheckBagHasItem(quest->payload[0], 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }
    if(!CheckBagHasSpace(quest->payload[1], ROGUE_APRICORN_BALL_REWARD_COUNT))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    if(!RemoveBagItem(quest->payload[0], 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }
    if(!AddBagItem(quest->payload[1], ROGUE_APRICORN_BALL_REWARD_COUNT))
    {
        AddBagItem(quest->payload[0], 1);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    Rogue_PushPopup_AddItem(quest->payload[1], ROGUE_APRICORN_BALL_REWARD_COUNT);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}
