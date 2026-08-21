#ifndef ROGUE_ADVENTUREPATHS_H
#define ROGUE_ADVENTUREPATHS_H

#include "global.h"

#define ROGUE_WARP_UNSET 0
#define ROGUE_WARP_TO_ROOM 1
#define ROGUE_WARP_TO_ADVPATH 2 

extern EWRAM_DATA struct RogueAdvPath gRogueAdvPath;

bool8 RogueAdv_GenerateAdventurePathsIfRequired();
void RogueAdv_CacheMiniBossPreviews();
void RogueAdv_ApplyAdventureMetatiles();
void RogueAdv_Debug_ForceRegenerateAdventurePaths();
bool8 RogueAdv_IsItemRoomRewardClaimed(u8 scheduleSlot);
bool8 RogueAdv_TryClaimItemRoomReward(u8 scheduleSlot, u16 itemId);
#ifdef ROGUE_DEBUG
u32 RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(u16 species);
u32 RogueAdv_Debug_GenerateUniqueDenCustomMonId(u16 species);
u16 RogueAdv_Debug_GetFullRestStopWeight(u8 difficulty);
bool8 RogueAdv_Debug_GetItemRoomSchedule(u8 slot, u8 *difficulty, u16 *itemId);
#endif

u8 RogueAdv_GetTileNum();
bool8 RogueAdv_IsViewingPath();

u8 RogueAdv_OverrideNextWarp(struct WarpData *warp);
void RogueAdv_ModifyObjectEvents(struct MapHeader *mapHeader, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity);
bool8 RogueAdv_CanUseEscapeRope(void);

u8 Rogue_GetTypeForHintForRoom(struct RogueAdvPathRoom const* room);
void RogueAdv_GetLastInteractedRoomParams();
void RogueAdv_WarpLastInteractedRoom();

#endif
