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
extern const struct Tileset gTileset_General;
extern const struct Tileset gTileset_GeneralHub;

static const struct RogueRouteSceneRequest *GetCurrentSceneRequest(void)
{
    if(gRogueRun.adventureRoomId >= gRogueAdvPath.roomCount)
        return NULL;

    return &gRogueAdvPath.rooms[gRogueRun.adventureRoomId].routeScene;
}

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
    u8 routeIdx = room->roomParams.roomIdx;

    memset(&room->routeScene, 0, sizeof(room->routeScene));
    room->routeScene.ownerQuestId = ROGUE_ADVENTURE_QUEST_INVALID_ID;
    if(routeIdx >= gRogueRouteTable.routeCount)
        return;

    room->routeScene.environment = gRogueRouteTable.routes[routeIdx].environment;
}

static void SyncPropVisibility(void)
{
    u8 i;

    for(i = 0; i < gSaveBlock1Ptr->objectEventTemplatesCount; ++i)
    {
        const struct ObjectEventTemplate *objectEvent = &gSaveBlock1Ptr->objectEventTemplates[i];

        if(objectEvent->flagId != FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN
            && objectEvent->flagId != FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN)
            continue;

        if(FlagGet(objectEvent->flagId))
            RemoveObjectEventByLocalIdAndMap(objectEvent->localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
        else
            TrySpawnObjectEvent(objectEvent->localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    }
}

struct RogueRouteSceneCandidate
{
    struct RogueRouteSceneRequest request;
    u16 priority;
};

typedef bool8 (*RogueRouteSceneProducer)(u8 roomId, struct RogueRouteSceneCandidate *candidate);

static bool8 TryCollectQuestNodeCandidate(u8 roomId, struct RogueRouteSceneCandidate *candidate)
{
    return RogueAdventureQuests_TryCollectSceneRequest(roomId, &candidate->request, &candidate->priority);
}

static const RogueRouteSceneProducer sRouteSceneProducers[] =
{
    TryCollectQuestNodeCandidate,
};

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
        && VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) == ROGUE_ROUTE_EVENT_STATE_COMPLETED;
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
    request->requestedItem = Rogue_SelectDarkDealCurseItem(RogueRandom());
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

struct RogueRouteFallbackDefinition
{
    u8 weight;
    bool8 (*isEligible)(u8 roomId);
    void (*build)(struct RogueRouteSceneRequest *request);
};

static const struct RogueRouteFallbackDefinition sRouteFallbacks[] =
{
    {50, CanShowStolenTradeCaseOffer, BuildStolenTradeCaseOffer},
    {50, CanShowHexedShrine, BuildHexedShrine},
    {50, CanShowAnomalousFossilOffer, BuildAnomalousFossilOffer},
};

static void SelectFallbackScene(u8 roomId, struct RogueRouteSceneRequest *selected)
{
    RAND_TYPE originalRng = gRngRogueValue;
    u8 environment = selected->environment;
    u16 totalWeight = 0;
    u16 roll;
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sRouteFallbacks); ++i)
    {
        if(sRouteFallbacks[i].isEligible(roomId))
            totalWeight += sRouteFallbacks[i].weight;
    }

    memset(selected, 0, sizeof(*selected));
    selected->environment = environment;
    selected->ownerQuestId = ROGUE_ADVENTURE_QUEST_INVALID_ID;
    if(totalWeight == 0)
        return;

    SeedRogueRng(gRogueAdvPath.rooms[roomId].rngSeed ^ 0xA7E1);
    selected->anchor = RogueRandom() % ROGUE_ROUTE_EVENT_ANCHOR_COUNT;
    roll = RogueRandom() % totalWeight;

    for(i = 0; i < ARRAY_COUNT(sRouteFallbacks); ++i)
    {
        if(!sRouteFallbacks[i].isEligible(roomId))
            continue;

        if(roll < sRouteFallbacks[i].weight)
        {
            sRouteFallbacks[i].build(selected);
            break;
        }
        roll -= sRouteFallbacks[i].weight;
    }

    gRngRogueValue = originalRng;
}

static void SelectRouteScene(u8 roomId, struct RogueRouteSceneRequest *selected)
{
    struct RogueRouteSceneCandidate selectedCandidate = {0};
    struct RogueRouteSceneRequest baseRequest = *selected;
    u8 i;

    SelectFallbackScene(roomId, &selectedCandidate.request);
    selectedCandidate.request.environment = baseRequest.environment;

    // Content producers submit transient candidates here. Quest nodes outrank
    // the seeded fallback today; reactive producers can join without changing
    // the composer or quest graph implementation.
    for(i = 0; i < ARRAY_COUNT(sRouteSceneProducers); ++i)
    {
        struct RogueRouteSceneCandidate candidate =
        {
            .request = baseRequest,
        };

        if(sRouteSceneProducers[i](roomId, &candidate) && candidate.priority > selectedCandidate.priority)
            selectedCandidate = candidate;
    }

    *selected = selectedCandidate.request;
}

void RogueRouteScenes_OnEnterRoute(void)
{
    struct RogueRouteSceneRequest *scene;
    const struct RogueAdventureQuest *quest;

    if(gRogueRun.routeSceneRoomId != gRogueRun.adventureRoomId)
    {
        VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
        FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
        gRogueRun.routeSceneRoomId = gRogueRun.adventureRoomId;
    }

    if(gRogueRun.adventureRoomId >= gRogueAdvPath.roomCount)
        return;

    scene = &gRogueAdvPath.rooms[gRogueRun.adventureRoomId].routeScene;
    SelectRouteScene(gRogueRun.adventureRoomId, scene);

    // A won camp remains on its current node until the route is left. Restore
    // its collection-only interaction if it was rescheduled after a full Bag.
    if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        && scene->source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE)
    {
        quest = RogueAdventureQuests_Get(scene->ownerQuestId);
        if(quest != NULL
            && quest->progress != 0
            && VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_COMPLETED)
            VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
    }
}

void RogueRouteScenes_PrepareRouteTrainers(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    u8 i;

    if(scene == NULL || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP)
        return;

    for(i = 0; i < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++i)
    {
        if(Rogue_GetDynamicTrainer(i) == scene->trainerNum)
            Rogue_SetDynamicTrainer(i, TRAINER_NONE);
    }
}

void RogueRouteScenes_OnExitRoute(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();

    RogueAdventureQuests_EmitSignal(ROGUE_ADVENTURE_QUEST_SIGNAL_ROUTE_COMPLETED, 1);
    if(scene != NULL
        && scene->source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        && VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        RogueAdventureQuests_Advance(scene->ownerQuestId);
    else
        RogueAdventureQuests_LeaveRoute(gRogueRun.adventureRoomId);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
}

bool8 RogueRouteScenes_IsAnchorTemplate(const struct ObjectEventTemplate *objectEvent)
{
    return objectEvent->script == Rogue_RouteEvent_Interact
        && objectEvent->trainerType == TRAINER_TYPE_NONE
        && objectEvent->trainerRange_berryTreeId < ROGUE_ROUTE_EVENT_ANCHOR_COUNT;
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
    u16 role,
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
    objectEvent->trainerRange_berryTreeId = role;
    objectEvent->script = script;
    objectEvent->flagId = flagId;
}

static const u8 *GetSceneNpcScript(u8 recipeId)
{
    switch(recipeId)
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
    default:
        return NULL;
    }
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
            RestoreSceneObject(objectEvents, objectEventCount, anchor, objectEvent->localId, xOffset, yOffset, graphicsId, script, flagId);
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
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    const struct ObjectEventTemplate *anchor = NULL;
    const u8 *npcScript;
    u8 i;

    if(scene == NULL || scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_NONE)
        return;

    npcScript = GetSceneNpcScript(scene->recipeId);
    if(npcScript == NULL)
        return;

    for(i = 0; i < baseObjectEventCount; ++i)
    {
        if(RogueRouteScenes_IsAnchorTemplate(&baseObjectEvents[i])
            && baseObjectEvents[i].trainerRange_berryTreeId == scene->anchor)
        {
            anchor = &baseObjectEvents[i];
            break;
        }
    }

    if(anchor == NULL
        || !RestoreSceneObject(objectEvents, objectEventCount, anchor, anchor->localId, 0, 0, scene->primaryGraphicsId, npcScript, 0))
        return;

    switch(scene->recipeId)
    {
    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER:
        RestoreSceneProp(objectEvents, objectEventCount, anchor, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop, 0);
        break;
    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP:
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 0, 1, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, 0);
        break;
    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF:
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop, 0);
        break;
    case ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE:
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_HexedShrineProp, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp, 0);
        break;
    case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER:
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 0, -1, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp, 0);
        break;
    case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION:
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_FossilProp, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_FossilProp, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 1, 0, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp, 0);
        break;
    }
}

void RogueRouteScenes_ModifyObjectEvents(struct ObjectEventTemplate *objectEvents, u8 *objectEventCount, u8 objectEventCapacity)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    struct ObjectEventTemplate anchors[ROGUE_ROUTE_EVENT_ANCHOR_COUNT];
    bool8 foundAnchor[ROGUE_ROUTE_EVENT_ANCHOR_COUNT] = {FALSE};
    u8 requiredCount;
    u8 originalCount = *objectEventCount;
    u8 write = 0;
    u8 i;
    u8 localId;

    if(scene == NULL || scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_NONE)
        return;

    for(i = 0; i < originalCount; ++i)
    {
        if(RogueRouteScenes_IsAnchorTemplate(&objectEvents[i]))
        {
            u8 anchor = objectEvents[i].trainerRange_berryTreeId;
            anchors[anchor] = objectEvents[i];
            foundAnchor[anchor] = TRUE;
        }
    }

    if(!foundAnchor[ROGUE_ROUTE_EVENT_ANCHOR_SENDER] || !foundAnchor[ROGUE_ROUTE_EVENT_ANCHOR_RECIPIENT])
        return;

    requiredCount = scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        || scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
        || scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
        || scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION ? 4 : 3;
    if(originalCount - ROGUE_ROUTE_EVENT_ANCHOR_COUNT + requiredCount > objectEventCapacity)
        return;

    for(i = 0; i < originalCount; ++i)
    {
        if(!RogueRouteScenes_IsAnchorTemplate(&objectEvents[i]))
            objectEvents[write++] = objectEvents[i];
    }
    *objectEventCount = write;

    switch(scene->recipeId)
    {
    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_StolenTradeCaseOffer, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop, 0, 0);
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_StolenTradeCaseCamp, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 0, 1, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, 0, FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, 0, 0);
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_StolenTradeCasePayoff, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, 0, FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop, 0, 0);
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_HexedShrine, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_HexedShrineProp, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp, 0, 0);
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_AnomalousFossilOffer, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 0, -1, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp, 0, 0);
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_AnomalousFossilRestoration, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_FossilProp, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_FossilProp, 0, 0);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, 1, 0, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp, 0, 0);
        break;
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
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    const struct ObjectEventTemplate *anchors[ROGUE_ROUTE_EVENT_ANCHOR_COUNT] = {NULL};
    u8 i;

    if(scene == NULL || gRogueAdvPath.currentRoomType != ADVPATH_ROOM_ROUTE)
        return;

    for(i = 0; i < gMapHeader.events->objectEventCount; ++i)
    {
        const struct ObjectEventTemplate *objectEvent = &gMapHeader.events->objectEvents[i];

        if(RogueRouteScenes_IsAnchorTemplate(objectEvent))
            anchors[objectEvent->trainerRange_berryTreeId] = objectEvent;
    }

    if(anchors[ROGUE_ROUTE_EVENT_ANCHOR_SENDER] == NULL || anchors[ROGUE_ROUTE_EVENT_ANCHOR_RECIPIENT] == NULL)
        return;

    if(scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_NONE)
    {
        ApplyAccentMetatile(anchors[scene->anchor], -1, 0);
        ApplyAccentMetatile(anchors[scene->anchor], 1, 0);
        if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP)
            ApplyAccentMetatile(anchors[scene->anchor], 0, 1);
        else if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE)
            ApplyAccentMetatile(anchors[scene->anchor], 0, -1);
        else if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
            || scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION)
            ApplyAccentMetatile(anchors[scene->anchor], 0, -1);
    }
}

void RogueRouteEvents_GetInteractionData(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_ANCHOR_SENDER;
    gSpecialVar_0x8004 = ITEM_NONE;
    gSpecialVar_0x8005 = ITEM_NONE;
    gSpecialVar_0x8006 = TRAINER_NONE;
    gSpecialVar_0x8007 = 0;

    if(scene == NULL)
        return;

    if(gSelectedObjectEvent < OBJECT_EVENTS_COUNT)
        gSpecialVar_Result = gObjectEvents[gSelectedObjectEvent].trainerRange_berryTreeId;
    gSpecialVar_0x8004 = scene->rewardItem;
    gSpecialVar_0x8005 = scene->requestedItem;
    gSpecialVar_0x8006 = scene->trainerNum;
    gSpecialVar_0x8007 = scene->rewardAmount;
}

void RogueRouteEvents_TryAcceptStolenTradeCaseQuest(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    struct RogueAdventureQuestCreateParams params = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED)
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE)
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED)
        return;

    params.payload[1] = scene->trainerNum;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
        return;

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_BeginStolenTradeCaseBattle(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    u16 state = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED && state != ROGUE_ROUTE_EVENT_STATE_ACTIVE))
        return;

    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, scene->trainerNum);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_FinishStolenTradeCaseBattle(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    bool8 alreadyHasCase;
    u16 state = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_ACTIVE && state != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING))
        return;

    if(state == ROGUE_ROUTE_EVENT_STATE_ACTIVE
        && !RogueAdventureQuests_SetProgress(scene->ownerQuestId, 1))
        return;

    alreadyHasCase = CheckBagHasItem(scene->requestedItem, 1);
    if(!alreadyHasCase
        && (!CheckBagHasSpace(scene->requestedItem, 1) || !AddBagItem(scene->requestedItem, 1)))
    {
        VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    SyncPropVisibility();
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryClaimStolenTradeCaseReward(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
            && VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING))
        return;

    if(!CheckBagHasItem(scene->requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    if(!RemoveBagItem(scene->requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    if(!AddBagItem(scene->rewardItem, 1))
    {
        AddBagItem(scene->requestedItem, 1);
        VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    AddMoney(&gSaveBlock1Ptr->money, ROGUE_STOLEN_TRADE_CASE_REWARD_MONEY);

    FlagSet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    SyncPropVisibility();
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryAcceptHexedShrine(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    u32 money;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || gRogueRun.temporaryDarkDealCurseItem != ITEM_NONE)
        return;

    if(!CheckBagHasSpace(scene->requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    money = GetMoney(&gSaveBlock1Ptr->money);
    if(scene->rewardAmount > MAX_MONEY || money > MAX_MONEY - scene->rewardAmount)
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MONEY_FULL;
        return;
    }

    if(!Rogue_TryAddTemporaryDarkDealCurse(scene->requestedItem))
        return;

    AddMoney(&gSaveBlock1Ptr->money, scene->rewardAmount);
    Rogue_PushPopup_AddMoney(scene->rewardAmount);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryAcceptAnomalousFossilQuest(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    struct RogueAdventureQuestCreateParams params = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL)
        || RogueAdventureQuests_GetFossilSpecies(scene->requestedItem) != scene->rewardItem)
        return;

    if(!CheckBagHasSpace(scene->requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    if(!AddBagItem(scene->requestedItem, 1))
        return;

    params.payload[0] = scene->requestedItem;
    params.payload[1] = scene->rewardAmount;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RemoveBagItem(scene->requestedItem, 1);
        return;
    }

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
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
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();

    gStringVar1[0] = EOS;
    gStringVar2[0] = EOS;
    gStringVar3[0] = EOS;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE)
        return;

    BufferFossilCustomTyping(
        gStringVar1,
        scene->rewardItem,
        GenerateFossilCustomMonId(scene, ROGUE_FOSSIL_RESTORATION_STABLE));
    BufferFossilCustomTyping(
        gStringVar2,
        scene->rewardItem,
        GenerateFossilCustomMonId(scene, ROGUE_FOSSIL_RESTORATION_ADAPTIVE));
    StringCopy(gStringVar3, RoguePokedex_GetSpeciesName(scene->rewardItem));
}

void RogueRouteEvents_TryRestoreAnomalousFossil(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    struct Pokemon mon;
    RAND_TYPE originalRng;
    u32 customMonId;
    u8 giveResult;
    u8 restoration = gSpecialVar_0x8004;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) == ROGUE_ROUTE_EVENT_STATE_COMPLETED
        || restoration > ROGUE_FOSSIL_RESTORATION_ADAPTIVE)
        return;

    if(!CheckBagHasItem(scene->requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    originalRng = gRngValue;
    SeedRng(scene->rewardAmount ^ (restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0x51A7 : 0xB4E3));
    customMonId = RogueGift_CreateDynamicMonIdRawWithTypingChance(
        UNIQUE_RARITY_RARE,
        scene->rewardItem,
        restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0 : 100);
    RogueGift_CreateMon(customMonId, &mon, scene->rewardItem, 1, USE_RANDOM_IVS);
    gRngValue = originalRng;

    if(!RemoveBagItem(scene->requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    giveResult = GiveTradedMonToPlayer(&mon);
    if(giveResult == MON_CANT_GIVE)
    {
        AddBagItem(scene->requestedItem, 1);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;
        return;
    }

    GetSetPokedexSpeciesFlag(scene->rewardItem, FLAG_SET_CAUGHT);
    Rogue_PushPopup_AddPokemon(scene->rewardItem, TRUE, FALSE);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}
