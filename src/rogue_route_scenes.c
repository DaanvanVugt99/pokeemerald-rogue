#include "global.h"

#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/flags.h"
#include "constants/metatile_labels.h"
#include "constants/rogue_route_events.h"
#include "constants/rogue_route_scenes.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
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
extern const u8 Rogue_RouteEvent_BreedersExchangePokemon[];
extern const struct Tileset gTileset_General;
extern const struct Tileset gTileset_GeneralHub;

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

struct RogueRouteLot
{
    const struct ObjectEventTemplate *objectEvent;
    u8 id;
    u8 size;
    u8 terrain;
};

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
        const struct ObjectEventTemplate *objectEvent = &gSaveBlock1Ptr->objectEventTemplates[i];

        if((objectEvent->script == Rogue_RouteEvent_Prop
                || objectEvent->script == Rogue_RouteEvent_BreedersExchangePokemon)
            && GetSceneObjectSlot(objectEvent->trainerRange_berryTreeId) == sceneSlot
            && GetSceneObjectProp(objectEvent->trainerRange_berryTreeId) == propId)
            RemoveObjectEventByLocalIdAndMap(objectEvent->localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
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
        && objectEvent->trainerRange_berryTreeId < ROGUE_ROUTE_SCENE_MAX_LOTS
        && objectEvent->movementRangeX < ROGUE_ROUTE_SCENE_LOT_SIZE_COUNT
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
                lots[count].terrain = objectEvent->movementRangeY;
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

static bool8 IsLotLayoutTraversable(const struct RogueRouteSceneLotDefinition *lotDefinition)
{
    u16 occupied = 0;
    u16 open;
    u16 visited = 0;
    u16 frontier;
    u8 i;

    // Dynamic objects may use the whole authored 3x3 clearing. Keep every
    // remaining tile connected and every object reachable from a cardinally
    // adjacent tile, so a recipe cannot create a wall or an inaccessible NPC.
    for(i = 0; i < lotDefinition->objectCount; ++i)
    {
        const struct RogueRouteSceneObjectDefinition *object = &lotDefinition->objects[i];
        u16 bit;

        if(abs(object->xOffset) > 1 || abs(object->yOffset) > 1)
            return FALSE;

        bit = 1 << ((object->yOffset + 1) * 3 + object->xOffset + 1);
        if(occupied & bit)
            return FALSE;
        occupied |= bit;
    }

    if((lotDefinition->requiredOpenMask & ~0x1FF) != 0
        || (occupied & lotDefinition->requiredOpenMask) != 0)
        return FALSE;

    open = (~occupied) & 0x1FF;
    for(i = 0; i < lotDefinition->objectCount; ++i)
    {
        const struct RogueRouteSceneObjectDefinition *object = &lotDefinition->objects[i];
        u8 x = object->xOffset + 1;
        u8 y = object->yOffset + 1;
        u16 neighbours = 0;

        if(x > 0)
            neighbours |= 1 << (y * 3 + x - 1);
        if(x < 2)
            neighbours |= 1 << (y * 3 + x + 1);
        if(y > 0)
            neighbours |= 1 << ((y - 1) * 3 + x);
        if(y < 2)
            neighbours |= 1 << ((y + 1) * 3 + x);
        if((neighbours & open) == 0)
            return FALSE;
    }

    frontier = open & -open;
    while(frontier != 0)
    {
        u16 next = 0;

        visited |= frontier;
        for(i = 0; i < 9; ++i)
        {
            u8 x = i % 3;
            u8 y = i / 3;

            if((frontier & (1 << i)) == 0)
                continue;
            if(x > 0)
                next |= 1 << (i - 1);
            if(x < 2)
                next |= 1 << (i + 1);
            if(y > 0)
                next |= 1 << (i - 3);
            if(y < 2)
                next |= 1 << (i + 3);
        }
        frontier = next & open & ~visited;
    }

    return visited == open;
}

static bool8 AddRecipeToPlan(
    struct RogueRouteScenePlan *plan,
    u8 *placementCount,
    u8 maxPlacements,
    u8 recipeId,
    u8 ownerQuestId,
    u32 payload,
    const struct RogueRouteLot *lots,
    u8 lotCount,
    u16 *usedLots,
    u8 *usedObjects,
    u8 objectBudget,
    struct RogueRouteSceneRng *rng)
{
    const struct RogueRouteRecipeDefinition *definition = RogueRouteEvents_GetRecipeDefinition(recipeId);
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
        const struct RogueRouteSceneLotDefinition *lotDefinition = &definition->lots[role];
        u8 eligibleCount = 0;
        u8 selected;
        u8 i;

        if(!IsLotLayoutTraversable(lotDefinition))
            return FALSE;

        for(i = 0; i < lotCount; ++i)
        {
            if((pendingUsedLots & (1 << lots[i].id)) == 0
                && lots[i].size >= lotDefinition->minimumSize
                && (lotDefinition->terrainMask & ROGUE_ROUTE_SCENE_TERRAIN_MASK(lots[i].terrain)) != 0)
                ++eligibleCount;
        }

        if(eligibleCount == 0)
            return FALSE;

        selected = RogueRouteSceneRng_Next(rng) % eligibleCount;
        for(i = 0; i < lotCount; ++i)
        {
            if((pendingUsedLots & (1 << lots[i].id)) == 0
                && lots[i].size >= lotDefinition->minimumSize
                && (lotDefinition->terrainMask & ROGUE_ROUTE_SCENE_TERRAIN_MASK(lots[i].terrain)) != 0
                && selected-- == 0)
            {
                selectedLots[role] = lots[i].id;
                pendingUsedLots |= 1 << lots[i].id;
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

    *usedLots = pendingUsedLots;
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
    struct RogueRouteLot lots[ROGUE_ROUTE_SCENE_MAX_LOTS];
    bool8 usedFallbackRecipes[ROGUE_ROUTE_SCENE_RECIPE_COUNT] = {FALSE};
    bool8 usedFallbackFamilies[ROGUE_ROUTE_SCENE_RECIPE_COUNT] = {FALSE};
    struct RogueRouteSceneRng rng;
    u16 usedLots = 0;
    u8 baseObjectCount;
    u8 objectBudget;
    u8 usedObjects = 0;
    u8 placementCount = 0;
    u8 sceneSlot = 0;
    u8 targetPlacements;
    u8 fallbackCount = RogueRouteEvents_GetFallbackCount();
    u8 questCount;
    u8 lotCount;
    u8 i;

    memset(plan, 0, sizeof(*plan));
    lotCount = CollectRouteLots(roomId, lots, ARRAY_COUNT(lots), &baseObjectCount);
    if(lotCount == 0 || baseObjectCount >= OBJECT_EVENT_TEMPLATES_COUNT)
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
            lots,
            lotCount,
            &usedLots,
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
                    lots,
                    lotCount,
                    &usedLots,
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

bool8 RogueRouteScenes_GetPlacementRequest(u8 placementIndex, struct RogueRouteSceneRequest *request)
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
        const struct RogueRouteSceneLotDefinition *lotDefinition;
        const struct ObjectEventTemplate *lot = NULL;
        u8 i;

        if(!RogueRouteScenes_GetPlacementRequest(placementIdx, &scene))
            continue;

        lotDefinition = GetSceneLotDefinition(&scene);
        if(lotDefinition == NULL)
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

        if(lot == NULL)
            continue;

        for(i = 0; i < lotDefinition->objectCount; ++i)
        {
            const struct RogueRouteSceneObjectDefinition *object = &lotDefinition->objects[i];
            u16 graphicsId = ResolveSceneObjectGraphics(&scene, object);
            u16 flagId = 0;

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
                    lot,
                    lot->localId,
                    object->xOffset,
                    object->yOffset,
                    graphicsId,
                    object->script,
                    PackSceneObjectData(scene.sceneSlot, scene.lotRole, object->propId),
                    flagId);
            }
            else
            {
                RestoreSceneProp(
                    objectEvents,
                    objectEventCount,
                    lot,
                    object->xOffset,
                    object->yOffset,
                    graphicsId,
                    object->script,
                    PackSceneObjectData(scene.sceneSlot, scene.lotRole, object->propId),
                    flagId);
            }
        }
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
        const struct ObjectEventTemplate *lot;
        u8 objectIdx;

        if(!RogueRouteScenes_GetPlacementRequest(placementIdx, &scene)
            || scene.lotId >= ARRAY_COUNT(lots)
            || !foundLots[scene.lotId])
            continue;

        lot = &lots[scene.lotId];
        lotDefinition = GetSceneLotDefinition(&scene);
        if(lotDefinition == NULL)
            continue;

        for(objectIdx = 0; objectIdx < lotDefinition->objectCount; ++objectIdx)
        {
            const struct RogueRouteSceneObjectDefinition *object = &lotDefinition->objects[objectIdx];
            u8 localId;

            if(!IsSceneObjectVisible(&scene, object))
                continue;

            localId = object->propId == 0
                ? lot->localId
                : FindFreeLocalId(objectEvents, *objectEventCount);
            AppendSceneObject(
                objectEvents,
                objectEventCount,
                lot,
                localId,
                object->xOffset,
                object->yOffset,
                ResolveSceneObjectGraphics(&scene, object),
                object->script,
                PackSceneObjectData(scene.sceneSlot, scene.lotRole, object->propId),
                0);
        }
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
        const struct RogueRouteSceneLotDefinition *lotDefinition;
        const struct ObjectEventTemplate *lot;
        u8 accentIdx;

        if(!RogueRouteScenes_GetPlacementRequest(i, &scene)
            || scene.lotId >= ARRAY_COUNT(lots)
            || (lot = lots[scene.lotId]) == NULL)
            continue;

        lotDefinition = GetSceneLotDefinition(&scene);
        if(lotDefinition == NULL)
            continue;

        for(accentIdx = 0; accentIdx < lotDefinition->accentCount; ++accentIdx)
        {
            ApplyAccentMetatile(
                lot,
                lotDefinition->accents[accentIdx].xOffset,
                lotDefinition->accents[accentIdx].yOffset);
        }
    }
}
