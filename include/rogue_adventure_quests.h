#ifndef GUARD_ROGUE_ADVENTURE_QUESTS_H
#define GUARD_ROGUE_ADVENTURE_QUESTS_H

#include "global.h"

struct RogueAdventureQuest;
struct RogueRouteSceneRequest;

struct RogueAdventureQuestCreateParams
{
    u16 payload[2];
    u8 target;
};

void RogueAdventureQuests_Clear(void);
u8 RogueAdventureQuests_Create(u8 definitionId, const struct RogueAdventureQuestCreateParams *params);
void RogueAdventureQuests_Remove(u8 questId);
bool8 RogueAdventureQuests_HasDefinition(u8 definitionId);
u8 RogueAdventureQuests_FindByDefinition(u8 definitionId);
bool8 RogueAdventureQuests_IsDefinitionSourceRoom(u8 definitionId, u8 roomId);
bool8 RogueAdventureQuests_IsItemProtected(u16 itemId);
bool8 RogueAdventureQuests_IsProgressTargetMet(u8 questId);
u16 RogueAdventureQuests_GetFossilSpecies(u16 fossilItem);
bool8 RogueAdventureQuests_BuildSceneRequest(u8 questId, struct RogueRouteSceneRequest *request);
u8 RogueAdventureQuests_CollectSceneRequests(u8 roomId, struct RogueRouteSceneRequest *requests, u8 capacity);
bool8 RogueAdventureQuests_TryCollectSceneRequest(u8 roomId, struct RogueRouteSceneRequest *request, u16 *priority);
void RogueAdventureQuests_LeaveRoute(u8 roomId);
void RogueAdventureQuests_EmitSignal(u8 signal, u16 value);
bool8 RogueAdventureQuests_EmitSignalForQuest(u8 questId, u8 signal, u16 value);

u8 RogueAdventureQuests_GetCount(void);
u8 RogueAdventureQuests_GetQuestIdAt(u8 displayIndex);
const struct RogueAdventureQuest *RogueAdventureQuests_Get(u8 questId);
u8 RogueAdventureQuests_GetState(u8 questId);
const u8 *RogueAdventureQuests_GetTitle(u8 questId);
void RogueAdventureQuests_BufferDescription(u8 questId, u8 *dest);

#endif // GUARD_ROGUE_ADVENTURE_QUESTS_H
