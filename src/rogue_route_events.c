#include "global.h"

#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/metatile_labels.h"
#include "constants/rogue_route_events.h"
#include "constants/rogue_route_scenes.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "event_object_movement.h"
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

extern const u8 Rogue_RouteEvent_Interact[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseOffer[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseCamp[];
extern const u8 Rogue_RouteEvent_StolenTradeCasePayoff[];
extern const u8 Rogue_RouteEvent_Prop[];
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
    RAND_TYPE originalRng = gRngRogueValue;
    u8 routeIdx = room->roomParams.roomIdx;

    memset(&room->routeScene, 0, sizeof(room->routeScene));
    room->routeScene.ownerQuestId = ROGUE_ADVENTURE_QUEST_INVALID_ID;
    if(routeIdx >= gRogueRouteTable.routeCount)
        return;

    SeedRogueRng(room->rngSeed ^ 0xA7E1);

    room->routeScene.environment = gRogueRouteTable.routes[routeIdx].environment;
    room->routeScene.recipeId = ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER;
    room->routeScene.source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR;
    room->routeScene.anchor = RogueRandom() % ROGUE_ROUTE_EVENT_ANCHOR_COUNT;
    room->routeScene.primaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    room->routeScene.secondaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    room->routeScene.requestedItem = ITEM_TRADE_CASE;
    room->routeScene.rewardItem = ITEM_BIG_POKEBLOCK_BUNDLE;
    room->routeScene.trainerNum = SelectEvilTeamTrainer();

    gRngRogueValue = originalRng;
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

static void SelectRouteScene(u8 roomId, struct RogueRouteSceneRequest *selected)
{
    struct RogueRouteSceneCandidate selectedCandidate =
    {
        .request = *selected,
        .priority = 100,
    };
    u8 i;

    if(selectedCandidate.request.recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER
        && !CanShowStolenTradeCaseOffer(roomId))
    {
        selectedCandidate.request.recipeId = ROGUE_ROUTE_SCENE_RECIPE_NONE;
        selectedCandidate.request.ownerQuestId = ROGUE_ADVENTURE_QUEST_INVALID_ID;
        selectedCandidate.priority = 0;
    }

    // Content producers submit transient candidates here. Quest nodes outrank
    // the seeded fallback today; reactive producers can join without changing
    // the composer or quest graph implementation.
    for(i = 0; i < ARRAY_COUNT(sRouteSceneProducers); ++i)
    {
        struct RogueRouteSceneCandidate candidate =
        {
            .request = *selected,
        };

        if(sRouteSceneProducers[i](roomId, &candidate) && candidate.priority > selectedCandidate.priority)
            selectedCandidate = candidate;
    }

    *selected = selectedCandidate.request;
}

void RogueRouteScenes_OnEnterRoute(void)
{
    struct RogueRouteSceneRequest *scene;

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
    RogueAdventureQuests_EmitSignal(ROGUE_ADVENTURE_QUEST_SIGNAL_ROUTE_COMPLETED, 1);
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
            RestoreSceneObject(objectEvents, objectEventCount, anchor, objectEvent->localId, xOffset, yOffset, graphicsId, Rogue_RouteEvent_Prop, flagId);
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
        RestoreSceneProp(objectEvents, objectEventCount, anchor, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, 0);
        break;
    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP:
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 0, 1, OBJ_EVENT_GFX_MOVING_BOX, FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, 0);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, 0);
        break;
    case ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF:
        RestoreSceneProp(objectEvents, objectEventCount, anchor, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
        RestoreSceneProp(objectEvents, objectEventCount, anchor, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, 0);
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

    requiredCount = scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP ? 4 : 3;
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
    }
}

void RogueRouteEvents_GetInteractionData(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_ANCHOR_SENDER;
    gSpecialVar_0x8004 = ITEM_NONE;
    gSpecialVar_0x8005 = ITEM_NONE;
    gSpecialVar_0x8006 = TRAINER_NONE;

    if(scene == NULL)
        return;

    if(gSelectedObjectEvent < OBJECT_EVENTS_COUNT)
        gSpecialVar_Result = gObjectEvents[gSelectedObjectEvent].trainerRange_berryTreeId;
    gSpecialVar_0x8004 = scene->rewardItem;
    gSpecialVar_0x8005 = scene->requestedItem;
    gSpecialVar_0x8006 = scene->trainerNum;
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

    alreadyHasCase = CheckBagHasItem(scene->requestedItem, 1);
    if(!alreadyHasCase
        && (!CheckBagHasSpace(scene->requestedItem, 1) || !AddBagItem(scene->requestedItem, 1)))
    {
        VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    if(!RogueAdventureQuests_Advance(scene->ownerQuestId))
    {
        if(!alreadyHasCase)
            RemoveBagItem(scene->requestedItem, 1);
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
    u32 previousMoney;

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

    if(!CheckBagHasSpace(scene->rewardItem, 1) || !AddBagItem(scene->rewardItem, 1))
    {
        VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    if(!RemoveBagItem(scene->requestedItem, 1))
    {
        RemoveBagItem(scene->rewardItem, 1);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    previousMoney = GetMoney(&gSaveBlock1Ptr->money);
    AddMoney(&gSaveBlock1Ptr->money, ROGUE_STOLEN_TRADE_CASE_REWARD_MONEY);
    if(!RogueAdventureQuests_Advance(scene->ownerQuestId))
    {
        SetMoney(&gSaveBlock1Ptr->money, previousMoney);
        AddBagItem(scene->requestedItem, 1);
        RemoveBagItem(scene->rewardItem, 1);
        return;
    }

    FlagSet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    SyncPropVisibility();
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}
