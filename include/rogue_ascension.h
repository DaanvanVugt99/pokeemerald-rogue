#ifndef GUARD_ROGUE_ASCENSION_H
#define GUARD_ROGUE_ASCENSION_H

#include "global.h"

#define ASCENSION_MAX 20
#define ASCENSION_NONE 255
#define ASCENSION_RULES_VERSION 1

enum RogueUpgradeDistribution
{
    ASCENSION_UPGRADE_NONE,
    ASCENSION_UPGRADE_ACES,
    ASCENSION_UPGRADE_BOSSES,
    ASCENSION_UPGRADE_TRAINERS,
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

struct RogueAscensionRules
{
    u8 natures;
    u8 moves;
    u8 items;
    u8 bossIvBonus;
    u8 trainerIv;
    bool8 perfectAces;
    bool8 perfectTeams;
    bool8 expandedParties;
    bool8 fullParties;
    bool8 diverseTrainers;
    bool8 limitedSupplies;
};

struct RogueAdventureConfig;
void RogueAscension_Resolve(u8 level, struct RogueAscensionRules *rules);
bool8 RogueAscension_Applies(u8 distribution, bool8 boss, bool8 ace);
u8 RogueAscension_CalculateIV(u8 ascension, u8 stage, bool8 boss, bool8 ace);
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
