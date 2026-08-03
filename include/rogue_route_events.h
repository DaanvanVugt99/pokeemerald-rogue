#ifndef GUARD_ROGUE_ROUTE_EVENTS_H
#define GUARD_ROGUE_ROUTE_EVENTS_H

#include "global.h"

void RogueRouteEvents_GetInteractionData(void);
void RogueRouteEvents_TryAcceptStolenTradeCaseQuest(void);
void RogueRouteEvents_BeginStolenTradeCaseBattle(void);
void RogueRouteEvents_FinishStolenTradeCaseBattle(void);
void RogueRouteEvents_TryClaimStolenTradeCaseReward(void);
void RogueRouteEvents_TryAcceptHexedShrine(void);

#endif // GUARD_ROGUE_ROUTE_EVENTS_H
