#ifndef ROGUE_SAVE_H
#define ROGUE_SAVE_H

#include "global.h"

extern struct RogueSaveBlock *gRogueSaveBlock;

// First internal development save schema. Bump this when the serialized
// Rogue block layout changes; the title screen requires a new game.
#define ROGUE_SAVE_VERSION 12

// Increment whenever dynamic Rogue item IDs are rearranged. Loading a save with
// an older layout removes dynamic items from both its current and restored bags.
#define ROGUE_DYNAMIC_ITEM_LAYOUT_VERSION 6

enum
{
    SAVE_VER_ID_UNKNOWN,
    SAVE_VER_ID_DEVELOPMENT_1_0,
    SAVE_VER_ID_DEVELOPMENT_2_0,
    SAVE_VER_ID_DEVELOPMENT_3_0,

    SAVE_VER_ID_LATEST = SAVE_VER_ID_DEVELOPMENT_3_0,
};

void RogueSave_UpdatePointers();

void RogueSave_ClearData();

void RogueSave_FormatForWriting();
bool8 RogueSave_FormatForReading(void);

u16 RogueSave_GetVersionId();

void RogueSave_OnSaveLoaded();

void RogueSave_SaveHubStates();
void RogueSave_LoadHubStates();

u16 RogueSave_GetHubBagItemIdAt(u16 index);
u16 RogueSave_GetHubBagItemQuantityAt(u16 index);

#endif //ROGUE_SAVE_H
