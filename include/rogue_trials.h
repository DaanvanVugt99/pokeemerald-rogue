#ifndef ROGUE_TRIALS_H
#define ROGUE_TRIALS_H

#include "global.h"

struct Pokemon;

enum RogueTrialId
{
    ROGUE_TRIAL_NONE,
    ROGUE_TRIAL_TYPE_NORMAL,
    ROGUE_TRIAL_TYPE_FIGHTING,
    ROGUE_TRIAL_TYPE_FLYING,
    ROGUE_TRIAL_TYPE_POISON,
    ROGUE_TRIAL_TYPE_GROUND,
    ROGUE_TRIAL_TYPE_ROCK,
    ROGUE_TRIAL_TYPE_BUG,
    ROGUE_TRIAL_TYPE_GHOST,
    ROGUE_TRIAL_TYPE_STEEL,
    ROGUE_TRIAL_TYPE_FIRE,
    ROGUE_TRIAL_TYPE_WATER,
    ROGUE_TRIAL_TYPE_GRASS,
    ROGUE_TRIAL_TYPE_ELECTRIC,
    ROGUE_TRIAL_TYPE_PSYCHIC,
    ROGUE_TRIAL_TYPE_ICE,
    ROGUE_TRIAL_TYPE_DRAGON,
    ROGUE_TRIAL_TYPE_DARK,
    ROGUE_TRIAL_TYPE_FAIRY,
    ROGUE_TRIAL_LITTLE_CUP,
    ROGUE_TRIAL_LOW_BST,
    ROGUE_TRIAL_RANDOMAN_ROULETTE,
    ROGUE_TRIAL_EQUALIZED,
    ROGUE_TRIAL_REGION_KANTO,
    ROGUE_TRIAL_REGION_JOHTO,
    ROGUE_TRIAL_REGION_HOENN,
    ROGUE_TRIAL_REGION_SINNOH,
    ROGUE_TRIAL_REGION_UNOVA,
    ROGUE_TRIAL_REGION_KALOS,
    ROGUE_TRIAL_REGION_ALOLA,
    ROGUE_TRIAL_REGION_GALAR,
    ROGUE_TRIAL_REGION_PALDEA,
    ROGUE_TRIAL_Z_A_ROYALE,
    ROGUE_TRIAL_ORRE_STYLE,
    ROGUE_TRIAL_ROGUELOCKE,
    ROGUE_TRIAL_CANT_PICK,
    ROGUE_TRIAL_CURSED_BODY,
    ROGUE_TRIAL_PRO_BUILDING,
    ROGUE_TRIAL_INSANE_MODE,
    ROGUE_TRIAL_IRON_MONO,
    ROGUE_TRIAL_IRON_KAIZO,
    ROGUE_TRIAL_CHAOS_MASTER,
    ROGUE_TRIAL_APOTHEOSIS,
    ROGUE_TRIAL_LIMITED_CAPTURE,
    ROGUE_TRIAL_COUNT,
};

#define ROGUE_TRIAL_NO_TYPE 0xFF
#define ROGUE_TRIAL_NO_TRAINER_TOGGLE 0

enum RogueTrialPokedexSet
{
    ROGUE_TRIAL_POKEDEX_SET_ALL,
    ROGUE_TRIAL_POKEDEX_SET_KANTO,
    ROGUE_TRIAL_POKEDEX_SET_JOHTO,
    ROGUE_TRIAL_POKEDEX_SET_HOENN,
    ROGUE_TRIAL_POKEDEX_SET_SINNOH,
    ROGUE_TRIAL_POKEDEX_SET_UNOVA,
    ROGUE_TRIAL_POKEDEX_SET_KALOS,
    ROGUE_TRIAL_POKEDEX_SET_ALOLA,
    ROGUE_TRIAL_POKEDEX_SET_GALAR,
    ROGUE_TRIAL_POKEDEX_SET_PALDEA,
    ROGUE_TRIAL_POKEDEX_SET_LEGENDS_ZA,
};

enum RogueTrialBattleGimmick
{
    ROGUE_TRIAL_GIMMICK_NONE,
    ROGUE_TRIAL_GIMMICK_MEGA,
    ROGUE_TRIAL_GIMMICK_Z_MOVE,
    ROGUE_TRIAL_GIMMICK_DYNAMAX,
    ROGUE_TRIAL_GIMMICK_TERASTALLIZATION,
    ROGUE_TRIAL_GIMMICK_COUNT,
};

struct RogueTrialBattleGimmickProfile
{
    u8 pokedexVariant;
    u8 gimmick;
};

struct RogueTrialDefinition
{
    u8 id;
    const u8 *name;
    const u8 *description;
    u8 forcedPokedexVariant;
    u8 pokedexSet;
    u8 forcedTrainerToggle;
    u8 requiredType;
    u16 maxBst;
    u16 normalizedBst;
    const struct RogueTrialBattleGimmickProfile *battleGimmickProfiles;
    u8 battleGimmickProfileCount;
    u8 battleLevel;
    u8 forcedTrainerOrder;
    u8 forcedBattleFormat;
    u8 forcedDifficulty;
    u8 curseEffect;
    u8 extraCurseEffect;
    u8 curseCount;
    u8 extraCurseCount;
    u8 captureLimit;
    const u16 *fixedStartingParty;
    u8 fixedStartingPartyCount;
    u8 requiresLittleCup : 1;
    u8 randomizePartyOnEnter : 1;
    u8 enableAllRegionalTrainers : 1;
    u8 hasCurseEffect : 1;
    u8 hasForcedTrainerOrder : 1;
    u8 hasForcedBattleFormat : 1;
    u8 forceRandomStarter : 1;
    u8 forceFreshStart : 1;
    u8 requiresStarterSpecies : 1;
    u8 forbidsLegendarySpecies : 1;
    u8 requiresLegendarySpecies : 1;
    u8 randomizeBeforeTrainerBattle : 1;
    u8 guaranteeCatch : 1;
    u8 disableDayCare : 1;
    u8 disableGifts : 1;
    u8 disableRandoman : 1;
    u8 enforceOpponentSpeciesLegality : 1;
    u8 disableTrainerBattleExp : 1;
    u8 hasForcedDifficulty : 1;
    u8 unused : 5;
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
u8 RogueTrial_GetRuleCount(u8 trialId, u8 pokedexVariant);
const u8 *RogueTrial_GetRuleText(u8 trialId, u8 pokedexVariant, u8 ruleIndex);
bool8 RogueTrial_GetBattleGimmick(u8 trialId, u8 pokedexVariant, u8 *gimmick);
bool8 RogueTrial_IsActive(void);
bool8 RogueTrial_IsActiveTrial(u8 trialId);
bool8 RogueTrial_IsInvalidated(void);
void RogueTrial_Invalidate(void);
void RogueTrial_ApplyPendingSelection(void);
void RogueTrial_ApplyRunBagItems(void);
void RogueTrial_ApplyBattleGimmickOverride(void);
u8 RogueTrial_GetPendingForcedPokedexVariant(void);

u16 RogueTrial_GetCharmItemCount(u8 effectType);
u16 RogueTrial_GetCurseItemCount(u8 effectType);
bool8 RogueTrial_CanAcceptMon(struct Pokemon *mon);
bool8 RogueTrial_IsSpeciesLegal(u16 species, u32 otId);
bool8 RogueTrial_TransformMonIfIllegal(struct Pokemon *mon);
bool8 RogueTrial_IsCompleteForQuest(u8 trialId);
void RogueTrial_OnMonGiven(struct Pokemon *mon);
void RogueTrial_OnCaughtMon(void);
bool8 RogueTrial_CanReceiveGift(void);
bool8 RogueTrial_CanThrowBall(void);
bool8 RogueTrial_IsCatchGuaranteed(void);
bool8 RogueTrial_IsDayCareDisabled(void);
bool8 RogueTrial_IsRandomanDisabled(void);
bool8 RogueTrial_IsTrainerBattleExpDisabled(void);
void RogueTrial_GetEffectiveSpeciesBaseStats(u16 species, u16 *stats, u8 statCount);
void RogueTrial_BufferDayCareDisabled(void);
void RogueTrial_BufferRandomanDisabled(void);
void RogueTrial_FilterMonQuery(void);
void RogueTrial_FilterOpponentMonQuery(void);
bool8 RogueTrial_EnforcesOpponentSpeciesLegality(void);
u8 RogueTrial_ModifyOpponentEvoLevel(u8 level);
bool8 RogueTrial_PendingAllowsSpecies(u16 species);
bool8 RogueTrial_PendingNeedsStarterFilter(void);
bool8 RogueTrial_PendingRequiresLegendarySpecies(void);
void RogueTrial_FilterPendingMonQuery(void);
void RogueTrial_OnEnterEncounter(void);
void RogueTrial_OnTrainerBattleStart(void);
void RogueTrial_CanStartTrainerBattle(void);
void RogueTrial_OnTrainerTeamReady(void);
void RogueTrial_OnTrainerBattleEnd(void);

void RogueTrial_AppendTrialOptions(void);
void RogueTrial_IsSelectedTrialGroup(void);
void RogueTrial_AppendSelectedTrialGroupOptions(void);
void RogueTrial_AppendDifficultyOptions(void);
void RogueTrial_SelectForcedDifficulty(void);
void RogueTrial_AppendPokedexOptions(void);
void RogueTrial_IsSelectedPokedexGroup(void);
void RogueTrial_AppendSelectedPokedexGroupOptions(void);
void RogueTrial_GetPokedexOptionCount(void);
void RogueTrial_SelectDefaultPokedexOption(void);
void RogueTrial_BufferTrialPreview(void);
void RogueTrial_SetPendingSelectionFromScript(void);
void RogueTrial_HasPendingSelection(void);
void RogueTrial_PendingHasFixedStartingParty(void);
void RogueTrial_PendingReplacesStartingParty(void);
void RogueTrial_ApplyPendingPartyCapacity(void);
void RogueTrial_CanUsePendingParty(void);
void RogueTrial_CanUsePendingDayCare(void);
void RogueTrial_CanStartPendingSelection(void);
void RogueTrial_ClearPendingSelection(void);
void RogueTrial_HasAvailableTrials(void);
void RogueTrial_LoadLastSelection(void);
void RogueTrial_CanUseAttendant(void);
void RogueTrial_RecordInitialParty(void);
bool8 RogueTrial_ApplyFixedStartingParty(u8 level);

#endif // ROGUE_TRIALS_H
