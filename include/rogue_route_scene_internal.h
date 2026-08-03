#ifndef GUARD_ROGUE_ROUTE_SCENE_INTERNAL_H
#define GUARD_ROGUE_ROUTE_SCENE_INTERNAL_H

#include "global.h"
#include "constants/rogue_route_events.h"
#include "random.h"

struct RogueRouteSceneRequest;

// Stack-local deterministic RNG used while planning and rebuilding route
// scenes. It deliberately mirrors the configured Rogue RNG algorithm without
// reading or writing either global RNG stream.
struct RogueRouteSceneRng
{
    RAND_TYPE state;
};

enum
{
    ROUTE_SCENE_OBJECT_FLAG_NONE,
    // The object's lot role maps to one bit in the owning quest's progress.
    ROUTE_SCENE_OBJECT_FLAG_HIDE_IF_QUEST_ROLE_COMPLETE = (1 << 0),
};

enum
{
    ROUTE_SCENE_RESUME_NONE,
    ROUTE_SCENE_RESUME_REWARD_PENDING_IF_PROGRESS,
    ROUTE_SCENE_RESUME_COMPLETED_IF_TARGET_MET,
};

enum
{
    ROUTE_SCENE_RECIPE_FLAG_NONE,
    ROUTE_SCENE_RECIPE_FLAG_EXCLUDE_DYNAMIC_TRAINER = (1 << 0),
    ROUTE_SCENE_RECIPE_FLAG_COMPLETE_LINKED_QUEST_ON_EXIT = (1 << 1),
};

#define ROUTE_SCENE_GFX_PRIMARY 0xFFFF
#define ROUTE_SCENE_GFX_SECONDARY 0xFFFE
#define ROUTE_SCENE_HEXED_SHRINE_ACCEPTED (1 << 15)
#define ROUTE_SCENE_STATE_MASK(state) (1 << (state))
#define ROUTE_SCENE_STATE_MASK_ALL 0x0F
#define ROUTE_SCENE_STATE_MASK_UNTIL_COMPLETED \
    (ROUTE_SCENE_STATE_MASK(ROGUE_ROUTE_EVENT_STATE_NOT_STARTED) \
        | ROUTE_SCENE_STATE_MASK(ROGUE_ROUTE_EVENT_STATE_ACTIVE) \
        | ROUTE_SCENE_STATE_MASK(ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING))

struct RogueRouteSceneObjectDefinition
{
    const u8 *script;
    u16 graphicsId;
    s8 xOffset;
    s8 yOffset;
    u8 propId;
    u8 visibleStateMask;
    u8 flags;
};

struct RogueRouteSceneAccentDefinition
{
    s8 xOffset;
    s8 yOffset;
};

struct RogueRouteSceneLotDefinition
{
    const struct RogueRouteSceneObjectDefinition *objects;
    const struct RogueRouteSceneAccentDefinition *accents;
    u8 objectCount;
    u8 accentCount;
    u8 minimumSize;
};

struct RogueRouteRecipeDefinition
{
    bool8 (*selectPayload)(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload);
    void (*expandPayload)(struct RogueRouteSceneRequest *request, u32 payload);
    const struct RogueRouteSceneLotDefinition *lots;
    u8 source;
    u8 lotCount;
    u8 linkedQuestDefinitionId;
    u8 resumeBehavior;
    u8 flags;
    u8 completionLotRole;
};

void RogueRouteSceneRng_Seed(struct RogueRouteSceneRng *rng, u32 seed);
u16 RogueRouteSceneRng_Next(struct RogueRouteSceneRng *rng);

struct RogueRouteFallbackDefinition
{
    u8 recipeId;
    u8 weight;
    u8 familyId;
    bool8 (*isEligible)(u8 roomId);
};

const struct RogueRouteRecipeDefinition *RogueRouteEvents_GetRecipeDefinition(u8 recipeId);
const struct RogueRouteFallbackDefinition *RogueRouteEvents_GetFallbackDefinition(u8 fallbackId);
u8 RogueRouteEvents_GetFallbackCount(void);

void RogueRouteEvents_OnEnterScene(const struct RogueRouteSceneRequest *scene);
void RogueRouteEvents_PrepareSceneTrainers(const struct RogueRouteSceneRequest *scene);
u8 RogueRouteEvents_OnExitScene(const struct RogueRouteSceneRequest *scene);

bool8 RogueRouteScenes_GetCurrentInteractionRequest(struct RogueRouteSceneRequest *request);
u8 RogueRouteScenes_GetSelectedPropId(void);
void RogueRouteScenes_HideProp(u8 sceneSlot, u8 propId);

#endif // GUARD_ROGUE_ROUTE_SCENE_INTERNAL_H
