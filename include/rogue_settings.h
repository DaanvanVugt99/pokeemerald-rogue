#ifndef ROGUE_SETTINGS__H
#define ROGUE_SETTINGS__H

#include "global.h"
#include "rogue_ascension.h"

struct RogueAdventureConfig;

typedef void (*RogueAdventureMenuCallback)(void);

struct GameModeRules
{
    u8 initialLevelOverride;
    u8 initialLevelOffset;
    u8 levelOffsetInterval;
    u8 enterPartySize;
    u8 adventureGenerator;
    u8 trainerOrder : 2;
    u8 disableMainQuests : 1;
    u8 disableTrialQuests : 1;
    u8 disablePerBadgeLvlCaps : 1;
    u8 forceEndGameTrainers : 1;
    u8 forceEndGameRouteItems : 1;
    u8 forceRandomanAlwaysActive : 1;
    u8 disableRivalEncounters : 1;
    u8 disableRouteTrainers : 1;
    u8 forceFullShopInventory : 1;
    u8 forceFullTutorMoves : 1;
};

void Rogue_SetConfigToggle(u16 elem, bool8 state);
bool8 Rogue_GetConfigToggle(u16 elem);
bool8 Rogue_GetConfigToggleFor(const struct RogueAdventureConfig *config, u16 elem);
void Rogue_SetConfigToggleFor(struct RogueAdventureConfig *config, u16 elem, bool8 state);

void Rogue_SetConfigRange(u16 elem, u8 value);
u8 Rogue_GetConfigRange(u16 elem);
void Rogue_SetConfigRangeFor(struct RogueAdventureConfig *config, u16 elem, u8 value);

void Rogue_CopyAdventureConfig(struct RogueAdventureConfig *dest);
void Rogue_SetConfigAscension(struct RogueAdventureConfig *config, u8 preset);
void Rogue_ApplyAdventureConfig(const struct RogueAdventureConfig *config);
void Rogue_SetRunStartConfigOverride(const struct RogueAdventureConfig *config);
void Rogue_ClearRunStartConfigOverride(void);
bool8 Rogue_HasRunStartConfigOverride(void);

bool8 Rogue_CanEditConfig();
bool8 Rogue_IsAdventureModeAvailable(u8 mode);
bool8 Rogue_HasPostgameSettingsUnlocked(void);

struct GameModeRules const* Rogue_GetModeRules();
bool8 Rogue_ShouldDisableMainQuests();
bool8 Rogue_ShouldDisableTrialQuests();

void RogueDebug_SetConfigToggle(u16 elem, bool8 state);
bool8 RogueDebug_GetConfigToggle(u16 elem);

void RogueDebug_SetConfigRange(u16 elem, u8 value);
u8 RogueDebug_GetConfigRange(u16 elem);

void Rogue_ResetSettingsToDefaults();
void Rogue_SetAscension(u8 preset);
u8 Rogue_GetAscension(void);

u8 Rogue_GetStartingMonCapacity();

bool8 Rogue_ShouldSkipAssignNickname(struct Pokemon* mon);
bool8 Rogue_ShouldSkipAssignNicknameYesNoMessage();
bool8 Rogue_ShouldForceNicknameScreen();
void Rogue_AssignAutomaticNicknameFromSeed(struct Pokemon *mon, u16 seed);

// UI
void Rogue_OpenAdventureConfigMenu(RogueAdventureMenuCallback callback);
void CB2_InitAdventureConfigMenu(void);
void CB2_InitIntroAdventureConfigMenu(void);
void CB2_InitPokedexSelectionMenu(void);

#endif
