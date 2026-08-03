#ifndef GUARD_ROGUE_ROUTE_EVENTS_H
#define GUARD_ROGUE_ROUTE_EVENTS_H

#include "global.h"

struct RogueAdvPathRoom;

void RogueRouteEvents_GenerateRoom(struct RogueAdvPathRoom *room);
void RogueRouteEvents_OnEnterRoute(void);
void RogueRouteEvents_OnExitRoute(void);
void RogueRouteEvents_ModifyObjectEvents(struct ObjectEventTemplate *objectEvents, u8 objectEventCount);
bool8 RogueRouteEvents_IsMarkerTemplate(const struct ObjectEventTemplate *objectEvent);

void RogueRouteEvents_GetInteractionData(void);
void RogueRouteEvents_TryAcceptDelivery(void);
void RogueRouteEvents_TryCompleteDelivery(void);

#endif // GUARD_ROGUE_ROUTE_EVENTS_H
