#include "global.h"

#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/flags.h"
#include "constants/rogue_route_events.h"
#include "constants/rogue_route_scenes.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "overworld.h"
#include "random.h"

#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_route_scene_internal.h"
#include "rogue_route_scenes.h"

extern const u8 Rogue_RouteEvent_Interact[];
extern const u8 Rogue_RouteEvent_Prop[];
extern const u8 Rogue_RouteEvent_AnomalousFossilProp[];
extern const u8 Rogue_RouteEvent_FossilWorkbench[];
extern const u8 Rogue_RouteEvent_ApricornProp[];
extern const u8 Rogue_RouteEvent_UnboundTutorProp[];
extern const u8 Rogue_RouteEvent_BreedersExchangePokemon[];
extern const u8 Rogue_RouteEvent_BuriedCacheSupplies[];
extern const u8 Rogue_RouteEvent_BuriedCacheSite[];
#define ROUTE_SCENE_RECIPE_SHIFT 0
#define ROUTE_SCENE_RECIPE_MASK  0x3F
#define ROUTE_SCENE_LOT_SHIFT    6
#define ROUTE_SCENE_LOT_MASK     0x0F
#define ROUTE_SCENE_ROLE_SHIFT   10
#define ROUTE_SCENE_ROLE_MASK    0x03
#define ROUTE_SCENE_OWNER_SHIFT  12
#define ROUTE_SCENE_OWNER_MASK   0x3F
#define ROUTE_SCENE_PAYLOAD_SHIFT 12
#define ROUTE_SCENE_PAYLOAD_MASK  0xFFFFF

#define ROUTE_SCENE_OBJECT_SLOT_MASK 0x03
#define ROUTE_SCENE_OBJECT_ROLE_SHIFT 2
#define ROUTE_SCENE_OBJECT_ROLE_MASK 0x03
#define ROUTE_SCENE_OBJECT_PROP_SHIFT 4
#define ROUTE_SCENE_OBJECT_PROP_MASK 0x0F

struct RogueRouteSpot
{
    const struct ObjectEventTemplate *objectEvent;
    u8 id;
    u8 type;
    u8 terrain;
};

static const struct ObjectEventTemplate *GetSceneObjectSpot(
    const struct ObjectEventTemplate *spots,
    u8 spotCount,
    const struct RogueRouteSceneRequest *scene,
    const struct RogueRouteSceneLotDefinition *lotDefinition,
    const struct RogueRouteSceneObjectDefinition *object);

void RogueRouteSceneRng_Seed(struct RogueRouteSceneRng *rng, u32 seed)
{
#ifdef ROGUE_FEATURE_HQ_RANDOM
    rng->state.seed = seed;
    rng->state.high = seed;
    rng->state.low = seed;
#else
    rng->state = (u16)seed;
#endif
}

u16 RogueRouteSceneRng_Next(struct RogueRouteSceneRng *rng)
{
#ifdef ROGUE_FEATURE_HQ_RANDOM
    return Random_16(RandomPCG32(&rng->state));
#else
    rng->state = ISO_RANDOMIZE1(rng->state);
    return rng->state >> 16;
#endif
}

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

static u8 GetPlacementSceneSlot(const struct RogueRouteScenePlan *plan, u8 placementIndex)
{
    u8 sceneSlot = 0;
    u8 i;

    for(i = 1; i <= placementIndex; ++i)
    {
        if(GetPlacementRole(&plan->placements[i]) == 0)
            ++sceneSlot;
    }

    return sceneSlot;
}

static u8 GetPlacementOwner(const struct RogueRouteScenePlacement *placement)
{
    return (placement->packed >> ROUTE_SCENE_OWNER_SHIFT) & ROUTE_SCENE_OWNER_MASK;
}

static u32 GetPlacementPayload(const struct RogueRouteScenePlacement *placement)
{
    return (placement->packed >> ROUTE_SCENE_PAYLOAD_SHIFT) & ROUTE_SCENE_PAYLOAD_MASK;
}

static struct RogueRouteScenePlacement PackPlacement(u8 recipeId, u8 lotId, u8 lotRole, u8 ownerQuestId, u32 payload)
{
    u32 ownerOrPayload = ownerQuestId == ROGUE_ADVENTURE_QUEST_INVALID_ID
        ? payload & ROUTE_SCENE_PAYLOAD_MASK
        : ownerQuestId & ROUTE_SCENE_OWNER_MASK;
    struct RogueRouteScenePlacement placement =
    {
        .packed = ((u32)recipeId << ROUTE_SCENE_RECIPE_SHIFT)
            | ((u32)lotId << ROUTE_SCENE_LOT_SHIFT)
            | ((u32)lotRole << ROUTE_SCENE_ROLE_SHIFT)
            | (ownerOrPayload << ROUTE_SCENE_OWNER_SHIFT),
    };

    return placement;
}

static bool8 SelectRecipePayload(u8 roomId, u8 recipeId, u8 sceneSlot, u32 *payload);

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
    const struct RogueRouteRecipeDefinition *definition = RogueRouteEvents_GetRecipeDefinition(recipeId);
    u32 payload = 0;

    if(plan == NULL || definition == NULL || placementIndex >= ARRAY_COUNT(plan->placements))
        return;

    if(definition->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        && !SelectRecipePayload(gRogueRun.adventureRoomId, recipeId, sceneSlot, &payload))
        return;

    plan->placements[placementIndex] = PackPlacement(
        recipeId,
        lotId,
        lotRole,
        ownerQuestId,
        payload);
}
#endif


void RogueRouteScenes_GenerateRoom(struct RogueAdvPathRoom *room)
{
    memset(&room->routeScenePlan, 0, sizeof(room->routeScenePlan));
}

void RogueRouteScenes_HideProp(u8 sceneSlot, u8 propId)
{
    u8 i;

    // The flag is shared by every prop that is already known to be hidden.
    // Visible conditional props keep flag 0, so one route-local flag can hide
    // any number of independently completed scenes after a quickload.
    FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    for(i = 0; i < gSaveBlock1Ptr->objectEventTemplatesCount; ++i)
    {
        struct ObjectEventTemplate *objectEvent = &gSaveBlock1Ptr->objectEventTemplates[i];

        if((objectEvent->script == Rogue_RouteEvent_Prop
                || objectEvent->script == Rogue_RouteEvent_AnomalousFossilProp
                || objectEvent->script == Rogue_RouteEvent_FossilWorkbench
                || objectEvent->script == Rogue_RouteEvent_ApricornProp
                || objectEvent->script == Rogue_RouteEvent_UnboundTutorProp
                || objectEvent->script == Rogue_RouteEvent_BreedersExchangePokemon
                || objectEvent->script == Rogue_RouteEvent_BuriedCacheSupplies
                || objectEvent->script == Rogue_RouteEvent_BuriedCacheSite)
            && GetSceneObjectSlot(objectEvent->trainerRange_berryTreeId) == sceneSlot
            && GetSceneObjectProp(objectEvent->trainerRange_berryTreeId) == propId)
        {
            objectEvent->flagId = FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN;
            RemoveObjectEventByLocalIdAndMap(objectEvent->localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
        }
    }
}

bool8 RogueRouteScenes_IsFollowMonSlotReserved(u8 slot)
{
    u8 placementIdx;

    if(slot != 1)
        return FALSE;

    for(placementIdx = 0; placementIdx < RogueRouteScenes_GetPlacementCount(); ++placementIdx)
    {
        struct RogueRouteSceneRequest scene;

        if(RogueRouteScenes_GetPlacementRequest(placementIdx, &scene)
            && scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE
            && RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_COMPLETED)
            return TRUE;
    }

    return FALSE;
}


bool8 RogueRouteScenes_IsLotTemplate(const struct ObjectEventTemplate *objectEvent)
{
    return objectEvent->script == Rogue_RouteEvent_Interact
        && objectEvent->trainerType == TRAINER_TYPE_NONE
        && objectEvent->trainerRange_berryTreeId < ROGUE_ROUTE_SCENE_MAX_SPOT_GROUPS
        && objectEvent->movementRangeX < ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT
        && objectEvent->movementRangeY < ROGUE_ROUTE_SCENE_TERRAIN_COUNT;
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

static u8 CollectRouteSpots(u8 roomId, struct RogueRouteSpot *spots, u8 capacity, u8 *baseObjectCount)
{
    const struct MapHeader *mapHeader = GetRouteMapHeader(roomId);
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
            if(count < capacity)
            {
                spots[count].objectEvent = objectEvent;
                spots[count].id = objectEvent->trainerRange_berryTreeId;
                spots[count].type = objectEvent->movementRangeX;
                spots[count].terrain = objectEvent->movementRangeY;
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

static bool8 SpotHasVisiblePairedProp(const struct RogueRouteSceneLotDefinition *lotDefinition)
{
    u8 i;

    if(lotDefinition->spotType == ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH)
        return FALSE;

    for(i = 0; i < lotDefinition->objectCount; ++i)
    {
        if(lotDefinition->objects[i].propId != 0)
            return TRUE;
    }

    return FALSE;
}

static bool8 HasPairedDecorSpot(const struct RogueRouteSpot *spots, u8 spotCount, u8 groupId, u8 spotType, u8 terrainMask)
{
    u8 i;

    for(i = 0; i < spotCount; ++i)
    {
        if(spots[i].id == groupId
            && spots[i].type == spotType
            && (terrainMask & ROGUE_ROUTE_SCENE_TERRAIN_MASK(spots[i].terrain)) != 0)
            return TRUE;
    }

    return FALSE;
}

static bool8 IsSpotRecipeValid(const struct RogueRouteSceneLotDefinition *lotDefinition)
{
    u8 i;
    bool8 hasPrimary = FALSE;
    u8 propCount = 0;

    if(lotDefinition->objectCount == 0
        || lotDefinition->objectCount > 3
        || lotDefinition->spotType >= ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT
        || lotDefinition->decorSpotType > ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT
        || (lotDefinition->terrainMask & ~ROGUE_ROUTE_SCENE_TERRAIN_MASK_ALL) != 0)
        return FALSE;

    for(i = 0; i < lotDefinition->objectCount; ++i)
    {
        if(lotDefinition->objects[i].propId == 0)
            hasPrimary = TRUE;
        else
            ++propCount;
    }

    if(!hasPrimary)
        return FALSE;

    if(lotDefinition->spotType == ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH)
    {
        if(propCount != 2 || lotDefinition->decorSpotType != ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT)
            return FALSE;
    }
    else
    {
        if(propCount > 1)
            return FALSE;

        if((propCount != 0) != (lotDefinition->decorSpotType != ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT))
            return FALSE;
    }

    return TRUE;
}

static bool8 IsPlantPatchObject(
    const struct RogueRouteSceneLotDefinition *lotDefinition,
    const struct RogueRouteSceneObjectDefinition *object)
{
    return lotDefinition->spotType == ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH
        && object->propId < ROGUE_APRICORN_CHOICE_COUNT;
}

static void ApplyPlantPatchOffset(struct ObjectEventTemplate *spot, u8 propId)
{
    s8 xOffset = 0;
    s8 yOffset = 0;

    switch(spot->movementType)
    {
    case MOVEMENT_TYPE_FACE_UP:
        xOffset = propId - 1;
        yOffset = -1;
        break;
    case MOVEMENT_TYPE_FACE_DOWN:
        xOffset = propId - 1;
        yOffset = 1;
        break;
    case MOVEMENT_TYPE_FACE_LEFT:
        xOffset = -1;
        yOffset = propId - 1;
        break;
    case MOVEMENT_TYPE_FACE_RIGHT:
        xOffset = 1;
        yOffset = propId - 1;
        break;
    default:
        xOffset = propId - 1;
        break;
    }

    spot->x += xOffset;
    spot->y += yOffset;
}

static bool8 GetResolvedSceneObjectSpot(
    struct ObjectEventTemplate *resolvedSpot,
    const struct ObjectEventTemplate *spots,
    u8 spotCount,
    const struct RogueRouteSceneRequest *scene,
    const struct RogueRouteSceneLotDefinition *lotDefinition,
    const struct RogueRouteSceneObjectDefinition *object)
{
    const struct ObjectEventTemplate *spot = GetSceneObjectSpot(spots, spotCount, scene, lotDefinition, object);

    if(spot == NULL)
        return FALSE;

    *resolvedSpot = *spot;
    if(IsPlantPatchObject(lotDefinition, object))
        ApplyPlantPatchOffset(resolvedSpot, object->propId);

    return TRUE;
}

static bool8 AddRecipeToPlan(
    struct RogueRouteScenePlan *plan,
    u8 *placementCount,
    u8 maxPlacements,
    u8 recipeId,
    u8 ownerQuestId,
    u32 payload,
    const struct RogueRouteSpot *spots,
    u8 spotCount,
    u16 *usedSpotGroups,
    u8 *usedObjects,
    u8 objectBudget,
    struct RogueRouteSceneRng *rng)
{
    const struct RogueRouteRecipeDefinition *definition = RogueRouteEvents_GetRecipeDefinition(recipeId);
    u16 pendingUsedSpotGroups = *usedSpotGroups;
    u8 selectedLots[ROGUE_ROUTE_SCENE_MAX_ROLES];
    u8 pendingObjects = *usedObjects;
    u8 role;

    if(definition == NULL
        || definition->lotCount > ROGUE_ROUTE_SCENE_MAX_ROLES
        || *placementCount + definition->lotCount > maxPlacements)
        return FALSE;

    for(role = 0; role < definition->lotCount; ++role)
    {
        const struct RogueRouteSceneLotDefinition *lotDefinition = &definition->lots[role];
        u8 eligibleCount = 0;
        u8 selected;
        u8 i;

        if(!IsSpotRecipeValid(lotDefinition))
            return FALSE;

        for(i = 0; i < spotCount; ++i)
        {
            if((pendingUsedSpotGroups & (1 << spots[i].id)) == 0
                && spots[i].type == lotDefinition->spotType
                && (lotDefinition->terrainMask & ROGUE_ROUTE_SCENE_TERRAIN_MASK(spots[i].terrain)) != 0
                && (!SpotHasVisiblePairedProp(lotDefinition)
                    || HasPairedDecorSpot(spots, spotCount, spots[i].id, lotDefinition->decorSpotType, lotDefinition->terrainMask)))
                ++eligibleCount;
        }

        if(eligibleCount == 0)
            return FALSE;

        selected = RogueRouteSceneRng_Next(rng) % eligibleCount;
        for(i = 0; i < spotCount; ++i)
        {
            if((pendingUsedSpotGroups & (1 << spots[i].id)) == 0
                && spots[i].type == lotDefinition->spotType
                && (lotDefinition->terrainMask & ROGUE_ROUTE_SCENE_TERRAIN_MASK(spots[i].terrain)) != 0
                && (!SpotHasVisiblePairedProp(lotDefinition)
                    || HasPairedDecorSpot(spots, spotCount, spots[i].id, lotDefinition->decorSpotType, lotDefinition->terrainMask))
                && selected-- == 0)
            {
                selectedLots[role] = spots[i].id;
                pendingUsedSpotGroups |= 1 << spots[i].id;
                break;
            }
        }

        pendingObjects += lotDefinition->objectCount;
        if(pendingObjects > objectBudget)
            return FALSE;
    }

    for(role = 0; role < definition->lotCount; ++role)
    {
        plan->placements[*placementCount] = PackPlacement(
            recipeId,
            selectedLots[role],
            role,
            ownerQuestId,
            payload);
        ++*placementCount;
    }

    *usedSpotGroups = pendingUsedSpotGroups;
    *usedObjects = pendingObjects;
    return TRUE;
}

static bool8 SelectRecipePayload(u8 roomId, u8 recipeId, u8 sceneSlot, u32 *payload)
{
    const struct RogueRouteRecipeDefinition *definition = RogueRouteEvents_GetRecipeDefinition(recipeId);
    struct RogueRouteSceneRequest request = {0};
    struct RogueRouteSceneRng rng;
    u8 routeIdx;

    if(definition == NULL || roomId >= gRogueAdvPath.roomCount)
        return FALSE;

    routeIdx = gRogueAdvPath.rooms[roomId].roomParams.roomIdx;
    if(routeIdx >= gRogueRouteTable.routeCount)
        return FALSE;

    *payload = 0;
    if(definition->selectPayload == NULL)
        return TRUE;

    request.recipeId = recipeId;
    request.environment = gRogueRouteTable.routes[routeIdx].environment;
    request.sceneSlot = sceneSlot;
    request.source = definition->source;
    request.ownerQuestId = ROGUE_ADVENTURE_QUEST_INVALID_ID;
    RogueRouteSceneRng_Seed(&rng, gRogueAdvPath.rooms[roomId].rngSeed
        ^ 0x5EED
        ^ (recipeId * 257)
        ^ (sceneSlot * 4051));

    if(!definition->selectPayload(&request, &rng, payload))
        return FALSE;

    AGB_ASSERT(*payload <= ROUTE_SCENE_PAYLOAD_MASK);
    return *payload <= ROUTE_SCENE_PAYLOAD_MASK;
}

static void BuildRouteScenePlan(u8 roomId, struct RogueRouteScenePlan *plan)
{
    struct RogueRouteSceneRequest questRequests[ROGUE_ROUTE_SCENE_MAX_PLACEMENTS] = {0};
    struct RogueRouteSpot spots[ROGUE_ROUTE_SCENE_MAX_SPOTS];
    bool8 usedFallbackRecipes[ROGUE_ROUTE_SCENE_RECIPE_COUNT] = {FALSE};
    bool8 usedFallbackFamilies[ROGUE_ROUTE_SCENE_RECIPE_COUNT] = {FALSE};
    struct RogueRouteSceneRng rng;
    u16 usedSpotGroups = 0;
    u8 baseObjectCount;
    u8 objectBudget;
    u8 usedObjects = 0;
    u8 placementCount = 0;
    u8 sceneSlot = 0;
    u8 targetPlacements;
    u8 fallbackCount = RogueRouteEvents_GetFallbackCount();
    u8 questCount;
    u8 spotCount;
    u8 i;

    memset(plan, 0, sizeof(*plan));
    spotCount = CollectRouteSpots(roomId, spots, ARRAY_COUNT(spots), &baseObjectCount);
    if(spotCount == 0 || baseObjectCount >= OBJECT_EVENT_TEMPLATES_COUNT)
        return;

    objectBudget = OBJECT_EVENT_TEMPLATES_COUNT - baseObjectCount;
    RogueRouteSceneRng_Seed(&rng, gRogueAdvPath.rooms[roomId].rngSeed ^ 0xA7E1);
    targetPlacements = 1 + RogueRouteSceneRng_Next(&rng) % ROGUE_ROUTE_SCENE_MAX_PLACEMENTS;

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
            questRequests[i].recipeId,
            questRequests[i].ownerQuestId,
            0,
            spots,
            spotCount,
            &usedSpotGroups,
            &usedObjects,
            objectBudget,
            &rng))
            ++sceneSlot;
    }

    while(placementCount < targetPlacements && sceneSlot < ROGUE_ROUTE_SCENE_MAX_PLACEMENTS)
    {
        u16 totalWeight = 0;
        u16 roll;
        u8 selectedFallback = fallbackCount;

        for(i = 0; i < fallbackCount; ++i)
        {
            const struct RogueRouteFallbackDefinition *fallback = RogueRouteEvents_GetFallbackDefinition(i);

            if(fallback != NULL
                && !usedFallbackRecipes[fallback->recipeId]
                && !usedFallbackFamilies[fallback->familyId]
                && !RogueRouteEvents_HasEncounteredFamily(fallback->familyId)
                && fallback->isEligible(roomId))
                totalWeight += fallback->weight;
        }

        if(totalWeight == 0)
            break;

        roll = RogueRouteSceneRng_Next(&rng) % totalWeight;
        for(i = 0; i < fallbackCount; ++i)
        {
            const struct RogueRouteFallbackDefinition *fallback = RogueRouteEvents_GetFallbackDefinition(i);

            if(fallback == NULL
                || usedFallbackRecipes[fallback->recipeId]
                || usedFallbackFamilies[fallback->familyId]
                || RogueRouteEvents_HasEncounteredFamily(fallback->familyId)
                || !fallback->isEligible(roomId))
                continue;

            if(roll < fallback->weight)
            {
                selectedFallback = i;
                break;
            }
            roll -= fallback->weight;
        }

        if(selectedFallback >= fallbackCount)
            break;

        {
            const struct RogueRouteFallbackDefinition *fallback = RogueRouteEvents_GetFallbackDefinition(selectedFallback);
            u32 payload = 0;

            if(fallback != NULL)
                usedFallbackRecipes[fallback->recipeId] = TRUE;

            if(fallback != NULL
                && SelectRecipePayload(roomId, fallback->recipeId, sceneSlot, &payload)
                && AddRecipeToPlan(
                    plan,
                    &placementCount,
                    ROGUE_ROUTE_SCENE_MAX_PLACEMENTS,
                    fallback->recipeId,
                    ROGUE_ADVENTURE_QUEST_INVALID_ID,
                    payload,
                    spots,
                    spotCount,
                    &usedSpotGroups,
                    &usedObjects,
                    objectBudget,
                    &rng))
            {
                usedFallbackFamilies[fallback->familyId] = TRUE;
                ++sceneSlot;
            }
        }
    }
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

static bool8 GetPlacementRequest(u8 placementIndex, struct RogueRouteSceneRequest *request, bool8 allowSuppressedGenerator)
{
    const struct RogueRouteScenePlan *plan = GetCurrentScenePlan();
    const struct RogueRouteScenePlacement *placement;
    const struct RogueRouteRecipeDefinition *definition;
    u8 recipeId;
    u8 routeIdx;

    if(plan == NULL || placementIndex >= ARRAY_COUNT(plan->placements))
        return FALSE;

    placement = &plan->placements[placementIndex];
    recipeId = GetPlacementRecipe(placement);
    definition = RogueRouteEvents_GetRecipeDefinition(recipeId);
    routeIdx = gRogueAdvPath.rooms[gRogueRun.adventureRoomId].roomParams.roomIdx;
    if(definition == NULL || routeIdx >= gRogueRouteTable.routeCount)
        return FALSE;
    if(definition->source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        && definition->linkedQuestDefinitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_NONE
        && RogueAdventureQuests_HasDefinition(definition->linkedQuestDefinitionId)
        && !allowSuppressedGenerator
        // The combined Apricorn recipe deliberately uses its second lot as
        // the same-route consumer after the grove creates the quest.
        && !(recipeId == ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN
            && GetPlacementRole(placement) == 1))
        return FALSE;

    memset(request, 0, sizeof(*request));
    request->recipeId = recipeId;
    request->environment = gRogueRouteTable.routes[routeIdx].environment;
    request->lotId = GetPlacementLot(placement);
    request->lotRole = GetPlacementRole(placement);
    request->sceneSlot = GetPlacementSceneSlot(plan, placementIndex);
    request->source = definition->source;
    request->ownerQuestId = definition->source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        ? GetPlacementOwner(placement)
        : ROGUE_ADVENTURE_QUEST_INVALID_ID;

    if(definition->source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE)
    {
        struct RogueRouteSceneRequest content = {0};

        if(!RogueAdventureQuests_BuildSceneRequest(request->ownerQuestId, &content))
            return FALSE;

        request->rewardItem = content.rewardItem;
        request->requestedItem = content.requestedItem;
        request->trainerNum = content.trainerNum;
        request->primaryGraphicsId = content.primaryGraphicsId;
        request->secondaryGraphicsId = content.secondaryGraphicsId;
        request->rewardAmount = content.rewardAmount;
    }
    else if(definition->expandPayload != NULL)
    {
        definition->expandPayload(request, GetPlacementPayload(placement));
    }

    request->recipeId = recipeId;
    request->source = definition->source;
    return TRUE;
}

bool8 RogueRouteScenes_GetPlacementRequest(u8 placementIndex, struct RogueRouteSceneRequest *request)
{
    return GetPlacementRequest(placementIndex, request, FALSE);
}

static bool8 GetSceneRequestBySlotAndRole(u8 sceneSlot, u8 lotRole, struct RogueRouteSceneRequest *request)
{
    u8 i;

    for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
    {
        if(GetPlacementRequest(i, request, TRUE)
            && request->sceneSlot == sceneSlot
            && request->lotRole == lotRole)
            return TRUE;
    }

    return FALSE;
}

bool8 RogueRouteScenes_GetCurrentInteractionRequest(struct RogueRouteSceneRequest *request)
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

u8 RogueRouteScenes_GetSelectedPropId(void)
{
    if(gSelectedObjectEvent >= OBJECT_EVENTS_COUNT)
        return 0;

    return GetSceneObjectProp(gObjectEvents[gSelectedObjectEvent].trainerRange_berryTreeId);
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

    if(RogueRouteScenes_GetPlacementCount() == 0)
        BuildRouteScenePlan(gRogueRun.adventureRoomId, plan);

    for(i = 0; i < RogueRouteScenes_GetPlacementCount(); ++i)
    {
        struct RogueRouteSceneRequest scene;

        if(RogueRouteScenes_GetPlacementRequest(i, &scene))
            RogueRouteEvents_OnEnterScene(&scene);
    }
}

void RogueRouteScenes_PrepareRouteTrainers(void)
{
    u8 placementIdx;

    for(placementIdx = 0; placementIdx < RogueRouteScenes_GetPlacementCount(); ++placementIdx)
    {
        struct RogueRouteSceneRequest scene;
        if(RogueRouteScenes_GetPlacementRequest(placementIdx, &scene))
            RogueRouteEvents_PrepareSceneTrainers(&scene);
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
        const struct RogueRouteScenePlan *plan = GetCurrentScenePlan();

        if(plan != NULL)
            RogueRouteEvents_MarkRecipeFamilyEncountered(GetPlacementRecipe(&plan->placements[i]));

        if(!RogueRouteScenes_GetPlacementRequest(i, &scene))
            continue;

        if(scene.source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
            && scene.ownerQuestId < ROGUE_ADVENTURE_QUEST_CAPACITY
            && !advancedQuests[scene.ownerQuestId]
            && RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        {
            u8 questId = scene.ownerQuestId;

            advancedQuests[scene.ownerQuestId] = TRUE;
            RogueAdventureQuests_EmitSignalForQuest(
                questId,
                ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
                1);
            if(RogueAdventureQuests_Get(questId) == NULL)
                RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
        }
        else
        {
            u8 questId = RogueRouteEvents_OnExitScene(&scene);

            if(questId < ROGUE_ADVENTURE_QUEST_CAPACITY && !advancedQuests[questId])
            {
                advancedQuests[questId] = TRUE;
                RogueAdventureQuests_EmitSignalForQuest(
                    questId,
                    ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
                    1);
                if(RogueAdventureQuests_Get(questId) == NULL)
                    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
            }
        }
    }

    RogueAdventureQuests_LeaveRoute(gRogueRun.adventureRoomId);
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, 0);
    FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
    FlagClear(FLAG_ROGUE_ROUTE_EVENT_PROP_B_HIDDEN);
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
}

static void AppendSceneObject(
    struct ObjectEventTemplate *objectEvents,
    u8 *objectEventCount,
    const struct ObjectEventTemplate *spot,
    u8 localId,
    u16 graphicsId,
    u8 movementType,
    const u8 *script,
    u16 objectData,
    u16 flagId)
{
    struct ObjectEventTemplate *objectEvent = &objectEvents[(*objectEventCount)++];

    memset(objectEvent, 0, sizeof(*objectEvent));
    objectEvent->localId = localId;
    objectEvent->graphicsId = graphicsId;
    objectEvent->x = spot->x;
    objectEvent->y = spot->y;
    objectEvent->elevation = spot->elevation;
    objectEvent->movementType = movementType;
    objectEvent->trainerType = TRAINER_TYPE_NONE;
    objectEvent->trainerRange_berryTreeId = objectData;
    objectEvent->script = script;
    objectEvent->flagId = flagId;
}

static const struct RogueRouteSceneLotDefinition *GetSceneLotDefinition(const struct RogueRouteSceneRequest *scene)
{
    const struct RogueRouteRecipeDefinition *recipe = RogueRouteEvents_GetRecipeDefinition(scene->recipeId);

    if(recipe == NULL || scene->lotRole >= recipe->lotCount)
        return NULL;

    return &recipe->lots[scene->lotRole];
}

static u16 ResolveSceneObjectGraphics(
    const struct RogueRouteSceneRequest *scene,
    const struct RogueRouteSceneObjectDefinition *object)
{
    static const u16 sSemanticGraphics[ROUTE_SCENE_SEMANTIC_PROP_COUNT][ROGUE_ROUTE_ENVIRONMENT_COUNT] =
    {
        [ROUTE_SCENE_SEMANTIC_PROP_SUPPLIES] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_ROUTE_PROP_DRIFTWOOD,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_WORKBENCH] =
        {
            OBJ_EVENT_GFX_WORK_TABLE,
            OBJ_EVENT_GFX_WORK_TABLE,
            OBJ_EVENT_GFX_WORK_TABLE,
            OBJ_EVENT_GFX_WORK_TABLE,
            OBJ_EVENT_GFX_WORK_TABLE,
            OBJ_EVENT_GFX_WORK_TABLE,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_SHRINE_STONE] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_CAMP] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_ROUTE_PROP_STUMP,
            OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
            OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
            OBJ_EVENT_GFX_ROUTE_PROP_DRIFTWOOD,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_RELIC] =
        {
            OBJ_EVENT_GFX_FOSSIL,
            OBJ_EVENT_GFX_FOSSIL,
            OBJ_EVENT_GFX_FOSSIL,
            OBJ_EVENT_GFX_FOSSIL,
            OBJ_EVENT_GFX_FOSSIL,
            OBJ_EVENT_GFX_FOSSIL,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_LANDMARK_0] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_STUMP,
            OBJ_EVENT_GFX_ROUTE_PROP_STUMP,
            OBJ_EVENT_GFX_ROUTE_PROP_SPLIT_ROCK,
            OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
            OBJ_EVENT_GFX_ROUTE_PROP_DRIFTWOOD,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_LANDMARK_1] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_ROUND_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_MOSSY_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPLIT_ROCK,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_SMALL_SIGN,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_LANDMARK_2] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_ROUTE_PROP_DRIFTWOOD,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_ROUTE_PROP_ROUND_STONE,
            OBJ_EVENT_GFX_SMALL_SIGN,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_OCCULT_ALTAR] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_OCCULT_ALTAR,
            OBJ_EVENT_GFX_ROUTE_PROP_OCCULT_ALTAR,
            OBJ_EVENT_GFX_ROUTE_PROP_OCCULT_ALTAR,
            OBJ_EVENT_GFX_ROUTE_PROP_OCCULT_ALTAR,
            OBJ_EVENT_GFX_ROUTE_PROP_OCCULT_ALTAR,
            OBJ_EVENT_GFX_ROUTE_PROP_OCCULT_ALTAR,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_SPIRIT_STONE] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
            OBJ_EVENT_GFX_ROUTE_PROP_SPIRIT_STONE,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_MERCHANT_STALL] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
            OBJ_EVENT_GFX_WORK_TABLE,
            OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
            OBJ_EVENT_GFX_ROUTE_PROP_DRIFTWOOD,
            OBJ_EVENT_GFX_ROUTE_PROP_SUPPLY_CRATE,
        },
        [ROUTE_SCENE_SEMANTIC_PROP_DIG_MARKER] =
        {
            OBJ_EVENT_GFX_ROUTE_PROP_DIG_MOUND,
            OBJ_EVENT_GFX_ROUTE_PROP_DIG_MOUND,
            OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
            OBJ_EVENT_GFX_ROUTE_PROP_STONE_PILE,
            OBJ_EVENT_GFX_ROUTE_PROP_DIG_MOUND,
            OBJ_EVENT_GFX_ROUTE_PROP_DIG_MOUND,
        },
    };
    u16 graphicsId = object->graphicsId;
    u8 semanticId;

    if(graphicsId == ROUTE_SCENE_GFX_PRIMARY)
        graphicsId = scene->primaryGraphicsId;
    else if(graphicsId == ROUTE_SCENE_GFX_SECONDARY)
        graphicsId = scene->secondaryGraphicsId;
    else if(graphicsId == ROUTE_SCENE_GFX_OFFERED_MON)
    {
        FollowMon_SetGraphics(1, scene->rewardItem, FALSE, 0);
        return OBJ_EVENT_GFX_FOLLOW_MON_1;
    }
    if(graphicsId <= ROUTE_SCENE_GFX_SEMANTIC_SUPPLIES
        && graphicsId >= ROUTE_SCENE_GFX_SEMANTIC_DIG_MARKER
        && scene->environment < ROGUE_ROUTE_ENVIRONMENT_COUNT)
    {
        semanticId = ROUTE_SCENE_GFX_SEMANTIC_SUPPLIES - graphicsId;
        return sSemanticGraphics[semanticId][scene->environment];
    }
    return graphicsId;
}

static bool8 IsSceneObjectVisible(
    const struct RogueRouteSceneRequest *scene,
    const struct RogueRouteSceneObjectDefinition *object)
{
    const struct RogueAdventureQuest *quest;
    u8 state = RogueRouteScenes_GetState(scene->sceneSlot);

    if((object->visibleStateMask & ROUTE_SCENE_STATE_MASK(state)) == 0)
        return FALSE;

    if((object->flags & ROUTE_SCENE_OBJECT_FLAG_HIDE_IF_QUEST_ROLE_COMPLETE) != 0)
    {
        quest = RogueAdventureQuests_Get(scene->ownerQuestId);
        if(quest != NULL && (quest->progress & (1 << scene->lotRole)) != 0)
            return FALSE;
    }

    return TRUE;
}

static u8 ResolveSceneObjectMovementType(
    const struct RogueRouteSceneLotDefinition *lotDefinition,
    const struct RogueRouteSceneObjectDefinition *object,
    const struct ObjectEventTemplate *spot)
{
    if(IsPlantPatchObject(lotDefinition, object))
        return MOVEMENT_TYPE_ROUTE_APRICORN_TREE;

    return spot->movementType;
}

static bool8 RestoreSceneObject(
    struct ObjectEventTemplate *objectEvents,
    u8 objectEventCount,
    const struct ObjectEventTemplate *spot,
    u8 localId,
    u16 graphicsId,
    u8 movementType,
    const u8 *script,
    u16 objectData,
    u16 flagId)
{
    u8 i;

    for(i = 0; i < objectEventCount; ++i)
    {
        struct ObjectEventTemplate *objectEvent = &objectEvents[i];

        if(objectEvent->localId == localId
            && objectEvent->x == spot->x
            && objectEvent->y == spot->y)
        {
            u8 preservedLocalId = objectEvent->localId;

            memset(objectEvent, 0, sizeof(*objectEvent));
            objectEvent->localId = preservedLocalId;
            objectEvent->graphicsId = graphicsId;
            objectEvent->x = spot->x;
            objectEvent->y = spot->y;
            objectEvent->elevation = spot->elevation;
            objectEvent->movementType = movementType;
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
    const struct ObjectEventTemplate *spot,
    u16 graphicsId,
    u8 movementType,
    const u8 *script,
    u16 objectData,
    u16 flagId)
{
    u8 i;

    for(i = 0; i < objectEventCount; ++i)
    {
        const struct ObjectEventTemplate *objectEvent = &objectEvents[i];

        if(objectEvent->graphicsId == graphicsId
            && objectEvent->x == spot->x
            && objectEvent->y == spot->y)
        {
            RestoreSceneObject(objectEvents, objectEventCount, spot, objectEvent->localId, graphicsId, movementType, script, objectData, flagId);
            return;
        }
    }
}

static bool8 IsObjectLocalIdUsed(
    const struct ObjectEventTemplate *objectEvents,
    u8 objectEventCount,
    const struct ObjectEventTemplate *reservedSpots,
    u8 reservedSpotCount,
    u8 allowedReservedLocalId,
    u8 localId)
{
    u8 i;

    for(i = 0; i < objectEventCount; ++i)
    {
        if(objectEvents[i].localId == localId)
            return TRUE;
    }

    for(i = 0; i < reservedSpotCount; ++i)
    {
        if(reservedSpots[i].localId != allowedReservedLocalId
            && reservedSpots[i].localId == localId)
            return TRUE;
    }

    return FALSE;
}

static u8 AllocSceneObjectLocalId(
    const struct ObjectEventTemplate *objectEvents,
    u8 objectEventCount,
    const struct ObjectEventTemplate *reservedSpots,
    u8 reservedSpotCount,
    u8 allowedReservedLocalId,
    u8 preferredLocalId)
{
    u16 localId;

    if(preferredLocalId != 0
        && !IsObjectLocalIdUsed(
            objectEvents,
            objectEventCount,
            reservedSpots,
            reservedSpotCount,
            allowedReservedLocalId,
            preferredLocalId))
        return preferredLocalId;

    for(localId = 1; localId <= 0xFF; ++localId)
    {
        if(!IsObjectLocalIdUsed(
            objectEvents,
            objectEventCount,
            reservedSpots,
            reservedSpotCount,
            allowedReservedLocalId,
            localId))
            return localId;
    }

    return 0;
}

static const struct ObjectEventTemplate *FindRouteSceneSpot(
    const struct ObjectEventTemplate *objectEvents,
    u8 objectEventCount,
    u8 groupId,
    u8 spotType)
{
    u8 i;

    for(i = 0; i < objectEventCount; ++i)
    {
        if(RogueRouteScenes_IsLotTemplate(&objectEvents[i])
            && objectEvents[i].trainerRange_berryTreeId == groupId
            && objectEvents[i].movementRangeX == spotType)
            return &objectEvents[i];
    }

    return NULL;
}

static const struct ObjectEventTemplate *GetSceneObjectSpot(
    const struct ObjectEventTemplate *spots,
    u8 spotCount,
    const struct RogueRouteSceneRequest *scene,
    const struct RogueRouteSceneLotDefinition *lotDefinition,
    const struct RogueRouteSceneObjectDefinition *object)
{
    if(object->propId == 0 || IsPlantPatchObject(lotDefinition, object))
        return FindRouteSceneSpot(spots, spotCount, scene->lotId, lotDefinition->spotType);

    return FindRouteSceneSpot(spots, spotCount, scene->lotId, lotDefinition->decorSpotType);
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
        const struct RogueRouteSceneLotDefinition *lotDefinition;
        u8 i;

        if(!RogueRouteScenes_GetPlacementRequest(placementIdx, &scene))
            continue;

        lotDefinition = GetSceneLotDefinition(&scene);
        if(lotDefinition == NULL)
            continue;

        for(i = 0; i < lotDefinition->objectCount; ++i)
        {
            const struct RogueRouteSceneObjectDefinition *object = &lotDefinition->objects[i];
            struct ObjectEventTemplate spot;
            u16 graphicsId = ResolveSceneObjectGraphics(&scene, object);
            u8 movementType;
            u16 flagId = 0;

            if(!GetResolvedSceneObjectSpot(
                &spot,
                baseObjectEvents,
                baseObjectEventCount,
                &scene,
                lotDefinition,
                object))
                continue;

            movementType = ResolveSceneObjectMovementType(lotDefinition, object, &spot);
            if(!IsSceneObjectVisible(&scene, object))
            {
                FlagSet(FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN);
                flagId = FLAG_ROGUE_ROUTE_EVENT_PROP_A_HIDDEN;
            }

            if(object->propId == 0)
            {
                RestoreSceneObject(
                    objectEvents,
                    objectEventCount,
                    &spot,
                    spot.localId,
                    graphicsId,
                    movementType,
                    object->script,
                    PackSceneObjectData(scene.sceneSlot, scene.lotRole, object->propId),
                    flagId);
            }
            else
            {
                RestoreSceneProp(
                    objectEvents,
                    objectEventCount,
                    &spot,
                    graphicsId,
                    movementType,
                    object->script,
                    PackSceneObjectData(scene.sceneSlot, scene.lotRole, object->propId),
                    flagId);
            }
        }
    }
}

void RogueRouteScenes_ModifyObjectEvents(struct ObjectEventTemplate *objectEvents, u8 *objectEventCount, u8 objectEventCapacity)
{
    struct ObjectEventTemplate spots[ROGUE_ROUTE_SCENE_MAX_SPOTS];
    u8 spotCount = 0;
    u8 originalCount = *objectEventCount;
    u8 write = 0;
    u8 requiredCount = 0;
    u8 placementIdx;
    u8 i;

    for(i = 0; i < originalCount; ++i)
    {
        if(RogueRouteScenes_IsLotTemplate(&objectEvents[i]))
        {
            if(spotCount < ARRAY_COUNT(spots))
                spots[spotCount++] = objectEvents[i];
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
        const struct RogueRouteSceneLotDefinition *lotDefinition;
        u8 objectIdx;

        if(!RogueRouteScenes_GetPlacementRequest(placementIdx, &scene))
            continue;

        lotDefinition = GetSceneLotDefinition(&scene);
        if(lotDefinition == NULL)
            continue;

        for(objectIdx = 0; objectIdx < lotDefinition->objectCount; ++objectIdx)
        {
            if(IsSceneObjectVisible(&scene, &lotDefinition->objects[objectIdx]))
                ++requiredCount;
        }
    }

    if(*objectEventCount + requiredCount > objectEventCapacity)
        return;

    for(placementIdx = 0; placementIdx < RogueRouteScenes_GetPlacementCount(); ++placementIdx)
    {
        struct RogueRouteSceneRequest scene;
        const struct RogueRouteSceneLotDefinition *lotDefinition;
        u8 objectIdx;

        if(!RogueRouteScenes_GetPlacementRequest(placementIdx, &scene))
            continue;

        lotDefinition = GetSceneLotDefinition(&scene);
        if(lotDefinition == NULL)
            continue;

        for(objectIdx = 0; objectIdx < lotDefinition->objectCount; ++objectIdx)
        {
            const struct RogueRouteSceneObjectDefinition *object = &lotDefinition->objects[objectIdx];
            struct ObjectEventTemplate spot;
            u8 localId;

            if(!IsSceneObjectVisible(&scene, object))
                continue;

            if(!GetResolvedSceneObjectSpot(
                &spot,
                spots,
                spotCount,
                &scene,
                lotDefinition,
                object))
                continue;

            localId = spot.localId;
            if(IsPlantPatchObject(lotDefinition, object) && object->propId != 0)
                localId = AllocSceneObjectLocalId(
                    objectEvents,
                    *objectEventCount,
                    spots,
                    spotCount,
                    spot.localId,
                    spot.localId + object->propId);
            if(localId == 0)
                continue;

            AppendSceneObject(
                objectEvents,
                objectEventCount,
                &spot,
                localId,
                ResolveSceneObjectGraphics(&scene, object),
                ResolveSceneObjectMovementType(lotDefinition, object, &spot),
                object->script,
                PackSceneObjectData(scene.sceneSlot, scene.lotRole, object->propId),
                0);
        }
    }
}

void RogueRouteScenes_ApplyMetatiles(void)
{
}
