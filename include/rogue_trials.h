#ifndef ROGUE_TRIALS_H
#define ROGUE_TRIALS_H

#include "global.h"

enum RogueTrialId
{
    ROGUE_TRIAL_NONE,
    ROGUE_TRIAL_SOLO_CONTRACT,
    ROGUE_TRIAL_HOENN_CIRCUIT,
    ROGUE_TRIAL_CURSED_CIRCUIT,
    ROGUE_TRIAL_COUNT,
};

struct RogueTrialDefinition
{
    u8 id;
    const u8 *name;
    const u8 *description;
    u8 forcedPokedexVariant;
    bool8 blockCaughtMons;
};

struct RogueTrialRunState
{
    u8 trialId;
    u8 difficulty;
    u8 initialPartyCount;
    u8 invalidated : 1;
    u8 initialPartyCountSet : 1;
    u8 unused : 6;
};

const struct RogueTrialDefinition *RogueTrial_GetDefinition(u8 trialId);
bool8 RogueTrial_IsActive(void);
bool8 RogueTrial_IsActiveTrial(u8 trialId);
bool8 RogueTrial_IsInvalidated(void);
void RogueTrial_Invalidate(void);
void RogueTrial_ApplyPendingSelection(void);
u8 RogueTrial_GetPendingForcedPokedexVariant(void);

u16 RogueTrial_GetCharmItemCount(u8 effectType);
u16 RogueTrial_GetCurseItemCount(u8 effectType);
bool8 RogueTrial_CanAcceptCaughtMon(void);
bool8 RogueTrial_IsCompleteForQuest(u8 trialId);
void RogueTrial_OnMonGiven(void);

void RogueTrial_AppendTrialOptions(void);
void RogueTrial_AppendDifficultyOptions(void);
void RogueTrial_BufferTrialPreview(void);
void RogueTrial_SetPendingSelectionFromScript(void);
void RogueTrial_ClearPendingSelection(void);
void RogueTrial_CanUseTerminal(void);
void RogueTrial_RecordInitialParty(void);

#endif // ROGUE_TRIALS_H
