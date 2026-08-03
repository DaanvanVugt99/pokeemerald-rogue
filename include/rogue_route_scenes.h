#ifndef GUARD_ROGUE_ROUTE_SCENES_H
#define GUARD_ROGUE_ROUTE_SCENES_H

#include "global.h"

struct RogueAdvPathRoom;

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
bool8 RogueRouteScenes_IsAnchorTemplate(const struct ObjectEventTemplate *objectEvent);

#endif // GUARD_ROGUE_ROUTE_SCENES_H
