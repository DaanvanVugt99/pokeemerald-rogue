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
extern const u8 Rogue_RouteEvent_Delivery[];
extern const u8 Rogue_RouteEvent_DeliveryPayoff[];
extern const u8 Rogue_RouteEvent_SupplyRequest[];
extern const u8 Rogue_RouteEvent_TrainerChallenge[];
extern const u8 Rogue_RouteEvent_Prop[];
extern const struct Tileset gTileset_General;
extern const struct Tileset gTileset_GeneralHub;

static const u16 sDeliveryRewards[] =
{
    ITEM_NUGGET,
    ITEM_RARE_CANDY,
    ITEM_PP_UP,
};

static const u16 sSupplyRequests[] =
{
    ITEM_POTION,
    ITEM_ANTIDOTE,
    ITEM_PARALYZE_HEAL,
    ITEM_REPEL,
    ITEM_ORAN_BERRY,
};

static const u16 sSupplyRewards[] =
{
    ITEM_SUPER_POTION,
    ITEM_ETHER,
    ITEM_REVIVE,
};

static const u8 sOneOffWeights[ROGUE_ROUTE_ENVIRONMENT_COUNT][ROGUE_ROUTE_SCENE_RECIPE_GENERATED_COUNT - 1] =
{
    [ROGUE_ROUTE_ENVIRONMENT_FIELD]      = {35, 40, 25},
    [ROGUE_ROUTE_ENVIRONMENT_FOREST]     = {30, 40, 30},
    [ROGUE_ROUTE_ENVIRONMENT_CAVE]       = {30, 25, 45},
    [ROGUE_ROUTE_ENVIRONMENT_MOUNTAIN]   = {30, 25, 45},
    [ROGUE_ROUTE_ENVIRONMENT_WATERFRONT] = {35, 40, 25},
    [ROGUE_ROUTE_ENVIRONMENT_URBAN]      = {40, 35, 25},
};

static const u16 sNpcGraphics[ROGUE_ROUTE_ENVIRONMENT_COUNT][4] =
{
    [ROGUE_ROUTE_ENVIRONMENT_FIELD]      = {OBJ_EVENT_GFX_CAMPER, OBJ_EVENT_GFX_PICNICKER, OBJ_EVENT_GFX_POKEFAN_F, OBJ_EVENT_GFX_GENTLEMAN},
    [ROGUE_ROUTE_ENVIRONMENT_FOREST]     = {OBJ_EVENT_GFX_BUG_CATCHER, OBJ_EVENT_GFX_PICNICKER, OBJ_EVENT_GFX_YOUNGSTER, OBJ_EVENT_GFX_POKEFAN_F},
    [ROGUE_ROUTE_ENVIRONMENT_CAVE]       = {OBJ_EVENT_GFX_HIKER, OBJ_EVENT_GFX_MANIAC, OBJ_EVENT_GFX_SCIENTIST_1, OBJ_EVENT_GFX_BLACK_BELT},
    [ROGUE_ROUTE_ENVIRONMENT_MOUNTAIN]   = {OBJ_EVENT_GFX_HIKER, OBJ_EVENT_GFX_CAMPER, OBJ_EVENT_GFX_BLACK_BELT, OBJ_EVENT_GFX_EXPERT_F},
    [ROGUE_ROUTE_ENVIRONMENT_WATERFRONT] = {OBJ_EVENT_GFX_SAILOR, OBJ_EVENT_GFX_FISHERMAN, OBJ_EVENT_GFX_SWIMMER_F, OBJ_EVENT_GFX_PICNICKER},
    [ROGUE_ROUTE_ENVIRONMENT_URBAN]      = {OBJ_EVENT_GFX_GENTLEMAN, OBJ_EVENT_GFX_SCIENTIST_1, OBJ_EVENT_GFX_WOMAN_1, OBJ_EVENT_GFX_RICH_BOY},
};

static const struct RogueRouteSceneRequest *GetCurrentSceneRequest(void)
{
    if(gRogueRun.adventureRoomId >= gRogueAdvPath.roomCount)
        return NULL;

    return &gRogueAdvPath.rooms[gRogueRun.adventureRoomId].routeScene;
}

static u8 SelectOneOffRecipe(u8 environment)
{
    u8 recipeId;
    u8 roll = RogueRandom() % 100;

    for(recipeId = ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER; recipeId < ROGUE_ROUTE_SCENE_RECIPE_GENERATED_COUNT; ++recipeId)
    {
        u8 weight = sOneOffWeights[environment][recipeId - 1];

        if(roll < weight)
            return recipeId;
        roll -= weight;
    }

    return ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE;
}

static u16 SelectChallengeTrainer(void)
{
    u16 trainerNum;
    u16 eligibleCount = 0;
    u16 selectedIdx;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        const struct RogueTrainer *trainer = &gRogueTrainers[trainerNum];

        if((trainer->trainerFlags & TRAINER_FLAG_CLASS_ROUTE) != 0
            && (trainer->classFlags & CLASS_FLAG_ANY_TEAM) == 0)
            ++eligibleCount;
    }

    AGB_ASSERT(eligibleCount != 0);
    selectedIdx = RogueRandom() % eligibleCount;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        const struct RogueTrainer *trainer = &gRogueTrainers[trainerNum];

        if((trainer->trainerFlags & TRAINER_FLAG_CLASS_ROUTE) != 0
            && (trainer->classFlags & CLASS_FLAG_ANY_TEAM) == 0
            && selectedIdx-- == 0)
            return trainerNum;
    }

    return TRAINER_NONE;
}

void RogueRouteScenes_GenerateRoom(struct RogueAdvPathRoom *room)
{
    RAND_TYPE originalRng = gRngRogueValue;
    u8 routeIdx = room->roomParams.roomIdx;
    u8 primaryIdx;
    u8 secondaryIdx;

    memset(&room->routeScene, 0, sizeof(room->routeScene));
    room->routeScene.ownerQuestId = ROGUE_ADVENTURE_QUEST_INVALID_ID;
    if(routeIdx >= gRogueRouteTable.routeCount)
        return;

    SeedRogueRng(room->rngSeed ^ 0xA7E1);

    room->routeScene.environment = gRogueRouteTable.routes[routeIdx].environment;
    room->routeScene.recipeId = SelectOneOffRecipe(room->routeScene.environment);
    room->routeScene.source = room->routeScene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER
        ? ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        : ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF;
    room->routeScene.anchor = RogueRandom() % ROGUE_ROUTE_EVENT_ANCHOR_COUNT;
    room->routeScene.variant = RogueRandom() % 2;

    primaryIdx = RogueRandom() % ARRAY_COUNT(sNpcGraphics[0]);
    secondaryIdx = (primaryIdx + 1 + RogueRandom() % (ARRAY_COUNT(sNpcGraphics[0]) - 1)) % ARRAY_COUNT(sNpcGraphics[0]);
    room->routeScene.primaryGraphicsId = sNpcGraphics[room->routeScene.environment][primaryIdx];
    room->routeScene.secondaryGraphicsId = sNpcGraphics[room->routeScene.environment][secondaryIdx];

    switch(room->routeScene.recipeId)
    {
    case ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER:
        room->routeScene.rewardItem = sDeliveryRewards[RogueRandom() % ARRAY_COUNT(sDeliveryRewards)];
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST:
        room->routeScene.requestedItem = sSupplyRequests[RogueRandom() % ARRAY_COUNT(sSupplyRequests)];
        room->routeScene.rewardItem = sSupplyRewards[RogueRandom() % ARRAY_COUNT(sSupplyRewards)];
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE:
        room->routeScene.rewardItem = sDeliveryRewards[RogueRandom() % ARRAY_COUNT(sDeliveryRewards)];
        room->routeScene.trainerNum = SelectChallengeTrainer();
        room->routeScene.primaryGraphicsId = Rogue_GetTrainerObjectEventGfx(room->routeScene.trainerNum);
        break;
    }

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

static void SelectRouteScene(u8 roomId, struct RogueRouteSceneRequest *selected)
{
    struct RogueRouteSceneCandidate selectedCandidate =
    {
        .request = *selected,
        .priority = 100,
    };
    u8 i;

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

    if(scene == NULL || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE)
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

static u16 GetScenePropGraphics(u8 environment, u8 propIdx)
{
    if(environment == ROGUE_ROUTE_ENVIRONMENT_CAVE || environment == ROGUE_ROUTE_ENVIRONMENT_MOUNTAIN)
        return propIdx == 0 ? OBJ_EVENT_GFX_BREAKABLE_ROCK : OBJ_EVENT_GFX_PUSHABLE_BOULDER;
    if(environment == ROGUE_ROUTE_ENVIRONMENT_URBAN || environment == ROGUE_ROUTE_ENVIRONMENT_WATERFRONT)
        return propIdx == 0 ? OBJ_EVENT_GFX_MOVING_BOX : OBJ_EVENT_GFX_BIRCHS_BAG;
    return propIdx == 0 ? OBJ_EVENT_GFX_BIRCHS_BAG : OBJ_EVENT_GFX_MOVING_BOX;
}

static void GetScenePropOffset(const struct RogueRouteSceneRequest *scene, u8 propIdx, s8 *xOffset, s8 *yOffset)
{
    if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER
        || scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_PAYOFF)
    {
        *xOffset = scene->variant == 0 ? 1 : -1;
        *yOffset = scene->variant == 0 ? -1 : 1;
    }
    else
    {
        *xOffset = propIdx == 0 ? -1 : 1;
        *yOffset = scene->variant == 0 ? 0 : (scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST ? 1 : -1);
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
    s8 propX;
    s8 propY;

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

    requiredCount = (scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER
        || scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_PAYOFF) ? 2 : 3;
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
    case ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_Delivery, ROGUE_ROUTE_EVENT_ANCHOR_SENDER, 0);
        GetScenePropOffset(scene, 0, &propX, &propY);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, propX, propY, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop, 0, FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_PAYOFF:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_DeliveryPayoff, ROGUE_ROUTE_EVENT_ANCHOR_RECIPIENT, 0);
        GetScenePropOffset(scene, 0, &propX, &propY);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, propX, propY, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop, 0, 0);
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_SupplyRequest, 0, 0);
        GetScenePropOffset(scene, 0, &propX, &propY);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, propX, propY, GetScenePropGraphics(scene->environment, 0), Rogue_RouteEvent_Prop, 0, 0);
        GetScenePropOffset(scene, 1, &propX, &propY);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, propX, propY, GetScenePropGraphics(scene->environment, 1), Rogue_RouteEvent_Prop, 0, 0);
        break;

    case ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE:
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], anchors[scene->anchor].localId, 0, 0, scene->primaryGraphicsId, Rogue_RouteEvent_TrainerChallenge, 0, 0);
        GetScenePropOffset(scene, 0, &propX, &propY);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, propX, propY, GetScenePropGraphics(scene->environment, 0), Rogue_RouteEvent_Prop, 0, 0);
        GetScenePropOffset(scene, 1, &propX, &propY);
        localId = FindFreeLocalId(objectEvents, *objectEventCount);
        AppendSceneObject(objectEvents, objectEventCount, &anchors[scene->anchor], localId, propX, propY, GetScenePropGraphics(scene->environment, 1), Rogue_RouteEvent_Prop, 0, 0);
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
    s8 propX;
    s8 propY;
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

    if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER
        || scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_PAYOFF)
    {
        GetScenePropOffset(scene, 0, &propX, &propY);
        ApplyAccentMetatile(anchors[scene->anchor], propX, propY);
    }
    else if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST)
    {
        GetScenePropOffset(scene, 0, &propX, &propY);
        ApplyAccentMetatile(anchors[scene->anchor], propX, propY);
        GetScenePropOffset(scene, 1, &propX, &propY);
        ApplyAccentMetatile(anchors[scene->anchor], propX, propY);
    }
    else if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE)
    {
        GetScenePropOffset(scene, 0, &propX, &propY);
        ApplyAccentMetatile(anchors[scene->anchor], propX, propY);
        GetScenePropOffset(scene, 1, &propX, &propY);
        ApplyAccentMetatile(anchors[scene->anchor], propX, propY);
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

void RogueRouteEvents_TryAcceptDelivery(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    struct RogueAdventureQuestCreateParams params = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_OFFER
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED)
        return;

    params.payload[0] = scene->rewardItem;
    params.payload[1] = scene->secondaryGraphicsId;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_PARCEL_DELIVERY, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
        return;

    if(!AddBagItem(ITEM_PARCEL, 1))
    {
        memset(&gRogueRun.adventureQuests[questId], 0, sizeof(gRogueRun.adventureQuests[questId]));
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    SyncPropVisibility();
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryCompleteDelivery(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_PAYOFF
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || !CheckBagHasItem(ITEM_PARCEL, 1))
        return;

    if(!CheckBagHasSpace(scene->rewardItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    if(!RemoveBagItem(ITEM_PARCEL, 1))
        return;
    if(!AddBagItem(scene->rewardItem, 1))
    {
        AddBagItem(ITEM_PARCEL, 1);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    if(!RogueAdventureQuests_Advance(scene->ownerQuestId))
    {
        RemoveBagItem(scene->rewardItem, 1);
        AddBagItem(ITEM_PARCEL, 1);
        return;
    }

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryCompleteSupplyRequest(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_SUPPLY_REQUEST
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED)
        return;

    if(!CheckBagHasItem(scene->requestedItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }
    if(!RemoveBagItem(scene->requestedItem, 1))
        return;
    if(!AddBagItem(scene->rewardItem, 1))
    {
        AddBagItem(scene->requestedItem, 1);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_BeginTrainerChallenge(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();
    u16 state = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF
        || (state != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED && state != ROGUE_ROUTE_EVENT_STATE_ACTIVE))
        return;

    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, scene->trainerNum);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryClaimTrainerReward(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING)
        return;

    if(!CheckBagHasSpace(scene->rewardItem, 1) || !AddBagItem(scene->rewardItem, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_FinishTrainerChallenge(void)
{
    const struct RogueRouteSceneRequest *scene = GetCurrentSceneRequest();

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_TRAINER_CHALLENGE
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_ACTIVE)
        return;

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
    RogueRouteEvents_TryClaimTrainerReward();
}

#if TESTING
u8 RogueRouteScenes_Test_GetOneOffWeight(u8 environment, u8 recipeId)
{
    if(environment >= ROGUE_ROUTE_ENVIRONMENT_COUNT
        || recipeId <= ROGUE_ROUTE_SCENE_RECIPE_NONE
        || recipeId >= ROGUE_ROUTE_SCENE_RECIPE_GENERATED_COUNT)
        return 0;

    return sOneOffWeights[environment][recipeId - 1];
}
#endif
