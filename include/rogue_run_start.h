#ifndef ROGUE_RUN_START_H
#define ROGUE_RUN_START_H

#include "global.h"
#include "rogue.h"

enum RogueRunStartSource
{
    RUN_START_SOURCE_NORMAL,
    RUN_START_SOURCE_TRIAL,
    RUN_START_SOURCE_REPLAY,
    RUN_START_SOURCE_MULTIPLAYER_HOST,
    RUN_START_SOURCE_MULTIPLAYER_CLIENT,
};

enum RogueRunStartReadiness
{
    RUN_START_READY,
    RUN_START_READY_REQUIRES_PARTNER,
    RUN_START_BLOCKED_PARTY,
    RUN_START_BLOCKED_DAY_CARE,
    RUN_START_WAITING_FOR_HOST,
};

enum RogueRunStartReadinessReason
{
    RUN_START_REASON_NONE,
    RUN_START_REASON_PARTY_EMPTY,
    RUN_START_REASON_PARTY_CAPACITY,
    RUN_START_REASON_PARTY_ILLEGAL,
    RUN_START_REASON_DAY_CARE_ILLEGAL,
    RUN_START_REASON_SPECIES_CLAUSE,
    RUN_START_REASON_LEGENDARY_CLAUSE,
    RUN_START_REASON_TRIAL_REPLACES_PARTY,
};

enum RogueRunStartTeamPolicy
{
    RUN_START_TEAM_CURRENT,
    RUN_START_TEAM_OPTIONAL_PARTNER,
    RUN_START_TEAM_REQUIRED_PARTNER,
    RUN_START_TEAM_FIXED_TRIAL,
};

enum RogueRunStartTeamSource
{
    RUN_START_TEAM_SOURCE_CURRENT_PARTY,
    RUN_START_TEAM_SOURCE_STARTER_BAG,
    RUN_START_TEAM_SOURCE_FIXED_TRIAL,
};

enum RogueRunReviewAction
{
    RUN_REVIEW_ACTION_BACK,
    RUN_REVIEW_ACTION_START,
    RUN_REVIEW_ACTION_EDIT,
};

enum RogueRunStartEditOption
{
    RUN_START_EDIT_SETTINGS,
    RUN_START_EDIT_POKEDEX,
    RUN_START_EDIT_TRIAL,
    RUN_START_EDIT_DIFFICULTY,
    RUN_START_EDIT_TRIAL_POKEDEX,
    RUN_START_EDIT_STARTING_TEAM,
};

enum RogueTrialSetupStage
{
    TRIAL_SETUP_STAGE_TRIAL,
    TRIAL_SETUP_STAGE_DIFFICULTY,
    TRIAL_SETUP_STAGE_POKEDEX,
    TRIAL_SETUP_STAGE_REVIEW,
    TRIAL_SETUP_STAGE_EXIT,
};

struct RogueRunStartContext
{
    struct RogueDifficultyConfig effectiveConfig;
    u8 source;
    u8 readiness;
    u8 readinessReason;
    u8 trialId;
    u8 trialDifficulty;
    u8 pokedexVariant;
    u8 partyCapacity;
    u8 teamPolicy;
    u8 preferredTeamSource;
    u8 effectiveTeamSource;
    u8 eligibilityReason;
    u16 ineligibleSpecies;
    u16 eligibilityParam;
    u16 configRevision;
    bool8 isActive;
    bool8 canEdit;
    bool8 requiresRandomPartner;
    bool8 canUseCurrentParty;
    bool8 canUseStarterBag;
    bool8 hasPendingQuestRewards;
    bool8 mainQuestsDisabled;
    bool8 trialQuestsDisabled;
};

void RogueRunStart_PrepareStandard(void);
void RogueRunStart_PrepareTrial(void);
void RogueRunStart_Refresh(void);
void RogueRunStart_Commit(void);
void RogueRunStart_Clear(void);
const struct RogueRunStartContext *RogueRunStart_GetContext(void);
bool8 RogueRunStart_CanStart(void);

void RogueRunStart_CheckCanStart(void);
void RogueRunStart_RequiresRandomPartner(void);
void RogueRunStart_GetPreferredTeamSource(void);
void RogueRunStart_SetPreferredTeamSource(void);
void RogueRunStart_UsesStarterBag(void);
void RogueRunStart_UsesIntegratedTeamChoice(void);
void RogueRunStart_HasTeamSourceChoice(void);
void RogueRunStart_AppendTeamSourceOptions(void);
void RogueRunStart_AppendStandardEditOptions(void);
void RogueRunStart_AppendTrialEditOptions(void);

#endif // ROGUE_RUN_START_H
