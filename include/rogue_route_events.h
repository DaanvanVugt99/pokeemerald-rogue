#ifndef GUARD_ROGUE_ROUTE_EVENTS_H
#define GUARD_ROGUE_ROUTE_EVENTS_H

#include "global.h"

void RogueRouteEvents_GetInteractionData(void);
void RogueRouteEvents_TryAcceptDelivery(void);
void RogueRouteEvents_TryCompleteDelivery(void);
void RogueRouteEvents_TryCompleteSupplyRequest(void);
void RogueRouteEvents_BeginTrainerChallenge(void);
void RogueRouteEvents_FinishTrainerChallenge(void);
void RogueRouteEvents_TryClaimTrainerReward(void);

#endif // GUARD_ROGUE_ROUTE_EVENTS_H
