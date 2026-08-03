#ifndef GUARD_ROGUE_ROUTE_SCENES_H
#define GUARD_ROGUE_ROUTE_SCENES_H

#include "global.h"

struct RogueAdvPathRoom;
struct RogueRouteSceneRequest;

// Route lifecycle and scene composition are content-agnostic. Individual
// interactions remain in rogue_route_events.h.
void RogueRouteScenes_GenerateRoom(struct RogueAdvPathRoom *room);
void RogueRouteScenes_OnEnterRoute(void);
void RogueRouteScenes_PrepareRouteTrainers(void);
void RogueRouteScenes_OnExitRoute(void);
void RogueRouteScenes_ModifyObjectEvents(struct ObjectEventTemplate *objectEvents, u8 *objectEventCount, u8 objectEventCapacity);
void RogueRouteScenes_RestoreObjectEvents(
    struct ObjectEventTemplate *objectEvents,
    u8 objectEventCount,
    const struct ObjectEventTemplate *baseObjectEvents,
    u8 baseObjectEventCount);
void RogueRouteScenes_ApplyMetatiles(void);
bool8 RogueRouteScenes_IsLotTemplate(const struct ObjectEventTemplate *objectEvent);
u8 RogueRouteScenes_GetPlacementCount(void);
bool8 RogueRouteScenes_GetPlacementRequest(u8 placementIndex, struct RogueRouteSceneRequest *request);
u8 RogueRouteScenes_GetState(u8 sceneSlot);
void RogueRouteScenes_SetState(u8 sceneSlot, u8 state);

#ifdef ROGUE_DEBUG
void RogueRouteScenes_DebugSetPlacement(u8 placementIndex, u8 recipeId, u8 lotId, u8 lotRole, u8 sceneSlot, u8 ownerQuestId);
#endif

#endif // GUARD_ROGUE_ROUTE_SCENES_H
