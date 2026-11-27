#ifndef GUARD_ROGUE_ASCENSION_H
#define GUARD_ROGUE_ASCENSION_H

#include "global.h"

#define ASCENSION_MAX 12
#define ASCENSION_NONE 255
#define ASCENSION_RULES_VERSION 4

enum RogueAscensionLevel
{
    ASCENSION_GYM_BERRY = 1,
    ASCENSION_RIVAL_UNIQUE,
    ASCENSION_DIVERSE_TRAINERS,
    ASCENSION_ACE_SETS,
    ASCENSION_EXTRA_MON,
    ASCENSION_GYM_FIELDS,
    ASCENSION_BOSS_SETS,
    ASCENSION_ELITE_LEGEND,
    ASCENSION_PERFECT_BOSSES,
    ASCENSION_FRESH_START,
    ASCENSION_BOSS_UNIQUE,
    ASCENSION_FULL_TEAMS,
};

enum RogueAscensionEligibility
{
    ASCENSION_ELIGIBLE,
    ASCENSION_INELIGIBLE_MODE,
    ASCENSION_INELIGIBLE_TRIAL,
    ASCENSION_INELIGIBLE_REPLAY,
    ASCENSION_INELIGIBLE_MULTIPLAYER,
    ASCENSION_INELIGIBLE_CHARMS,
    ASCENSION_INELIGIBLE_DEBUG,
    ASCENSION_INELIGIBLE_SPECIAL,
};

struct RogueAdventureConfig;
u8 RogueAscension_CalculateIV(u8 ascension, u8 stage, bool8 boss);
u8 RogueAscension_PartySize(u8 ascension, u8 baseline, u8 capacity, bool8 boss);
u8 RogueAscension_GetUnlocked(u8 format);
bool8 RogueAscension_IsRevealed(void);
bool8 RogueAscension_IsUnlocked(u8 level, u8 format);
void RogueAscension_ResetProgress(void);
u8 RogueAscension_GetEligibility(const struct RogueAdventureConfig *config, u8 source);
const u8 *RogueAscension_EligibilityText(u8 reason);
void RogueAscension_RecordWin(void);
const u8 *RogueAscension_GetAddition(u8 level);
u8 RogueAscension_GetRuleLine(u8 level, u8 line, u8 *dest);
u8 RogueAscension_GetConfigRuleLine(const struct RogueAdventureConfig *config, u8 line, u8 *dest);
u8 RogueAscension_GetBaseLine(u8 line, u8 *dest);

#endif
