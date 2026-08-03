#ifndef GUARD_ROGUE_ROUTE_EVENTS_H
#define GUARD_ROGUE_ROUTE_EVENTS_H

#include "global.h"

void RogueRouteEvents_GetInteractionData(void);
void RogueRouteEvents_TryAcceptStolenTradeCaseQuest(void);
void RogueRouteEvents_BeginStolenTradeCaseBattle(void);
void RogueRouteEvents_FinishStolenTradeCaseBattle(void);
void RogueRouteEvents_TryClaimStolenTradeCaseReward(void);
void RogueRouteEvents_TryAcceptHexedShrine(void);
void RogueRouteEvents_TryAcceptAnomalousFossilQuest(void);
void RogueRouteEvents_BufferFossilRestorationData(void);
void RogueRouteEvents_TryRestoreAnomalousFossil(void);
void RogueRouteEvents_TryAcceptForbiddenStoneQuest(void);
void RogueRouteEvents_CollectForbiddenStoneSoul(void);
void RogueRouteEvents_PrepareForbiddenStoneBattle(void);
void RogueRouteEvents_FinishForbiddenStoneBattle(void);
void RogueRouteEvents_BufferApricornTreeData(void);
void RogueRouteEvents_TryChooseApricorn(void);
void RogueRouteEvents_TryCraftApricornBalls(void);
void RogueRouteEvents_PrepareUnboundTutor(void);
void RogueRouteEvents_FinishUnboundTutor(void);

#endif // GUARD_ROGUE_ROUTE_EVENTS_H
