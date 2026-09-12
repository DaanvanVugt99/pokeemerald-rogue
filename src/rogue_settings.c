#include "global.h"
#include "constants/layouts.h"
#include "event_data.h"
#include "gba/isagbprint.h"
#include "random.h"
#include "string_util.h"

#include "rogue_controller.h"
#include "rogue_charms.h"
#include "rogue_gifts.h"
#include "rogue_multiplayer.h"
#include "rogue_query.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_ascension.h"
#include "constants/rogue_pokedex.h"
#include "rogue_quest.h"
#include "rogue_trials.h"

#include "data/rogue/pokemon_nicknames.h"

STATIC_ASSERT(ARRAY_COUNT(sNicknameTable_Global) != 0, sNicknameTable_Global_IsntEmpty);

static const struct GameModeRules sGameModeRules[ROGUE_GAME_MODE_COUNT] = 
{
    [ROGUE_GAME_MODE_STANDARD] = {},
    [ROGUE_GAME_MODE_RAINBOW] = 
    {
        .trainerOrder = TRAINER_ORDER_RAINBOW,
    },
    [ROGUE_GAME_MODE_OFFICIAL] = 
    {
        .trainerOrder = TRAINER_ORDER_OFFICIAL,
        .disableTrialQuests = TRUE,
    },
    [ROGUE_GAME_MODE_GAUNTLET] = 
    {
        .initialLevelOverride = 100,
        .initialLevelOffset = 0,
        .levelOffsetInterval = 0,
        .enterPartySize = PARTY_SIZE,
        .trainerOrder = TRAINER_ORDER_DEFAULT,
        .disableTrialQuests = TRUE,
        .disablePerBadgeLvlCaps = TRUE,
        .forceEndGameTrainers = TRUE,
        .forceEndGameRouteItems = TRUE,
        .forceRandomanAlwaysActive = TRUE,
        .disableRivalEncounters = TRUE,
        .disableRouteTrainers = TRUE,
        .forceFullShopInventory = TRUE,
        .forceFullTutorMoves = TRUE,
        .adventureGenerator = ADV_GENERATOR_GAUNTLET,
    },
    [ROGUE_GAME_MODE_RAINBOW_GAUNTLET] = 
    {
        .initialLevelOverride = 100,
        .initialLevelOffset = 0,
        .levelOffsetInterval = 0,
        .enterPartySize = PARTY_SIZE,
        .trainerOrder = TRAINER_ORDER_RAINBOW,
        .disableTrialQuests = TRUE,
        .disablePerBadgeLvlCaps = TRUE,
        .forceEndGameTrainers = TRUE,
        .forceEndGameRouteItems = TRUE,
        .forceRandomanAlwaysActive = TRUE,
        .disableRivalEncounters = TRUE,
        .disableRouteTrainers = TRUE,
        .forceFullShopInventory = TRUE,
        .forceFullTutorMoves = TRUE,
        .adventureGenerator = ADV_GENERATOR_GAUNTLET,
    },
    [ROGUE_GAME_MODE_SLOW_PATH] =
    {
        .adventureGenerator = ADV_GENERATOR_SLOW_PATH,
    },
};

static EWRAM_DATA struct RogueAdventureConfig sRunStartConfigOverride;
static EWRAM_DATA bool8 sRunStartConfigOverrideActive;

#ifdef ROGUE_DEBUG
EWRAM_DATA struct RogueDebugConfig gRogueDebug = {0};
#endif

static const struct RogueAdventureConfig *GetReadableAdventureConfig(void)
{
    if (sRunStartConfigOverrideActive)
        return &sRunStartConfigOverride;
    if (Rogue_IsRunActive())
        return &gRogueSaveBlock->activeAdventureConfig;
    if (RogueMP_IsActive() && RogueMP_IsClient())
        return &gRogueMultiplayer->gameState.hub.adventureConfig;
    return &gRogueSaveBlock->adventureConfig;
}

bool8 Rogue_HasPostgameSettingsUnlocked(void)
{
    return FlagGet(FLAG_ROGUE_MET_POKABBIE)
        || RogueQuest_HasCollectedRewards(QUEST_ID_ONE_LAST_QUEST);
}

bool8 Rogue_IsAdventureModeAvailable(u8 mode)
{
    switch (mode)
    {
    case ROGUE_GAME_MODE_STANDARD:
    case ROGUE_GAME_MODE_RAINBOW:
    case ROGUE_GAME_MODE_OFFICIAL:
        return TRUE;
    case ROGUE_GAME_MODE_GAUNTLET:
    case ROGUE_GAME_MODE_RAINBOW_GAUNTLET:
        return Rogue_HasPostgameSettingsUnlocked();
    default:
        // Slow Path is unavailable for new adventures for now.
        return FALSE;
    }
}

void Rogue_CopyAdventureConfig(struct RogueAdventureConfig *dest)
{
    *dest = *GetReadableAdventureConfig();
}

void Rogue_SetRunStartConfigOverride(const struct RogueAdventureConfig *config)
{
    sRunStartConfigOverride = *config;
    sRunStartConfigOverrideActive = TRUE;
    RogueMonQuery_InvalidateSpeciesActiveCache();
}

void Rogue_ClearRunStartConfigOverride(void)
{
    sRunStartConfigOverrideActive = FALSE;
    RogueMonQuery_InvalidateSpeciesActiveCache();
}

bool8 Rogue_HasRunStartConfigOverride(void)
{
    return sRunStartConfigOverrideActive;
}

void Rogue_SetConfigToggleFor(struct RogueAdventureConfig *config, u16 elem, bool8 value)
{
    if (elem >= CONFIG_TOGGLE_TRAINER_ROGUE && elem <= CONFIG_TOGGLE_TRAINER_PALDEA)
    {
        u16 bit = 1 << (elem - CONFIG_TOGGLE_TRAINER_ROGUE);
        if (value) config->trainerRegions |= bit;
        else config->trainerRegions &= ~bit;
    }
    else if (elem == CONFIG_TOGGLE_OVERWORLD_MONS)
        config->overworldMons = value;
    else if (elem == CONFIG_TOGGLE_BAG_WIPE)
        config->trialFreshStart = value;
    // Battle clauses and conveniences are fixed, not editable configuration.
}

bool8 Rogue_GetConfigToggleFor(const struct RogueAdventureConfig *config, u16 elem)
{
    if (elem >= CONFIG_TOGGLE_TRAINER_ROGUE && elem <= CONFIG_TOGGLE_TRAINER_PALDEA)
        return (config->trainerRegions & (1 << (elem - CONFIG_TOGGLE_TRAINER_ROGUE))) != 0;
    switch (elem)
    {
    case CONFIG_TOGGLE_OVERWORLD_MONS: return config->overworldMons;
    case CONFIG_TOGGLE_BAG_WIPE: return config->trialFreshStart;
    case CONFIG_TOGGLE_DIVERSE_TRAINERS: return config->ascension >= 15;
    case CONFIG_TOGGLE_EXP_ALL:
    case CONFIG_TOGGLE_EV_GAIN:
    case CONFIG_TOGGLE_RELEASE_MONS:
    case CONFIG_TOGGLE_BAG_CLAUSE:
    case CONFIG_TOGGLE_SPECIES_CLAUSE:
    case CONFIG_TOGGLE_HELD_ITEM_CLAUSE:
    case CONFIG_TOGGLE_LEGENDARY_CLAUSE: return TRUE;
    default: return FALSE;
    }
}

void Rogue_SetConfigToggle(u16 elem, bool8 value)
{
    if (!Rogue_CanEditConfig()) return;
    Rogue_SetConfigToggleFor(&gRogueSaveBlock->adventureConfig, elem, value);
    RogueMonQuery_InvalidateSpeciesActiveCache();
}

bool8 Rogue_GetConfigToggle(u16 elem)
{
    return Rogue_GetConfigToggleFor(GetReadableAdventureConfig(), elem);
}

void Rogue_SetConfigRangeFor(struct RogueAdventureConfig *config, u16 elem, u8 value)
{
    switch (elem)
    {
    case CONFIG_RANGE_ASCENSION: config->ascension = min(value, ASCENSION_MAX); break;
    case CONFIG_RANGE_BATTLE_FORMAT: config->battleFormat = min(value, BATTLE_FORMAT_MIXED); break;
    case CONFIG_RANGE_POKEDEX_VARIANT: config->pokedexVariant = value; break;
    case CONFIG_RANGE_GAME_MODE_NUM: config->mode = min(value, ROGUE_GAME_MODE_COUNT - 1); break;
    case CONFIG_RANGE_TRAINER_ORDER: config->trainerOrder = min(value, TRAINER_ORDER_OFFICIAL); break;
    }
}

u8 Rogue_GetConfigRange(u16 elem)
{
    const struct RogueAdventureConfig *config = GetReadableAdventureConfig();
    switch (elem)
    {
    case CONFIG_RANGE_ASCENSION: return config->ascension;
    case CONFIG_RANGE_BATTLE_FORMAT: return config->battleFormat;
    case CONFIG_RANGE_POKEDEX_VARIANT: return config->pokedexVariant;
    case CONFIG_RANGE_GAME_MODE_NUM: return config->mode;
    case CONFIG_RANGE_TRAINER_ORDER: return config->trainerOrder;

    }
    return 0;
}

void Rogue_SetConfigRange(u16 elem, u8 value)
{
    struct RogueAdventureConfig *config = &gRogueSaveBlock->adventureConfig;
    if (!Rogue_CanEditConfig()) return;
    if (elem == CONFIG_RANGE_BATTLE_FORMAT && value <= BATTLE_FORMAT_MIXED)
    {
        gRogueSaveBlock->selectedAscension[config->battleFormat] = config->ascension;
        config->ascension = min(gRogueSaveBlock->selectedAscension[value], RogueAscension_GetUnlocked(value));
    }
    Rogue_SetConfigRangeFor(config, elem, value);
    RogueMonQuery_InvalidateSpeciesActiveCache();
}

void Rogue_SetConfigAscension(struct RogueAdventureConfig *config, u8 ascension)
{
    config->ascension = min(ascension, ASCENSION_MAX);
}

void Rogue_ApplyAdventureConfig(const struct RogueAdventureConfig *config)
{
    if (Rogue_IsRunActive()) return;
    gRogueSaveBlock->adventureConfig = *config;
    gRogueSaveBlock->selectedAscension[config->battleFormat] = config->ascension;
    RogueMonQuery_InvalidateSpeciesActiveCache();
}

void Rogue_SetAscension(u8 ascension)
{
    struct RogueAdventureConfig *config = &gRogueSaveBlock->adventureConfig;
    if (Rogue_CanEditConfig() && RogueAscension_IsUnlocked(ascension, config->battleFormat))
    {
        config->ascension = ascension;
        gRogueSaveBlock->selectedAscension[config->battleFormat] = ascension;
    }
}

u8 Rogue_GetAscension(void)
{
    return GetReadableAdventureConfig()->ascension;
}

bool8 Rogue_CanEditConfig()
{
    // Client can never change config
    if(RogueMP_IsActive() && RogueMP_IsClient())
        return FALSE;

    return !Rogue_IsRunActive();
}

struct GameModeRules const* Rogue_GetModeRules()
{
    static struct GameModeRules sGeneratedModeRules;
    u8 mode = Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM);
    u8 trainerOrder = Rogue_GetConfigRange(CONFIG_RANGE_TRAINER_ORDER);

    AGB_ASSERT(mode < ROGUE_GAME_MODE_COUNT);

    // Rainbow and Official used to be full game modes. Keep those legacy values
    // meaningful while exposing trainer order as its own setting in the UI.
    switch (mode)
    {
    case ROGUE_GAME_MODE_RAINBOW:
        mode = ROGUE_GAME_MODE_STANDARD;
        trainerOrder = TRAINER_ORDER_RAINBOW;
        break;
    case ROGUE_GAME_MODE_OFFICIAL:
        mode = ROGUE_GAME_MODE_STANDARD;
        trainerOrder = TRAINER_ORDER_OFFICIAL;
        break;
    case ROGUE_GAME_MODE_RAINBOW_GAUNTLET:
        mode = ROGUE_GAME_MODE_GAUNTLET;
        trainerOrder = TRAINER_ORDER_RAINBOW;
        break;
    }

    memcpy(&sGeneratedModeRules, &sGameModeRules[mode], sizeof(struct GameModeRules));
    sGeneratedModeRules.trainerOrder = trainerOrder;

    if(sGeneratedModeRules.trainerOrder == TRAINER_ORDER_OFFICIAL)
        sGeneratedModeRules.disableTrialQuests = TRUE;

    return &sGeneratedModeRules;
}

bool8 Rogue_ShouldDisableMainQuests()
{
    struct AdventureReplay const* replay = &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED];

    if(Rogue_IsRunActive() && FlagGet(FLAG_ROGUE_RUN_MAIN_QUESTS_DISABLED))
        return TRUE;

    if(!Rogue_IsRunActive() && Rogue_GetModeRules()->disableMainQuests)
        return TRUE;

    if(Rogue_IsRunActive() && FlagGet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE) && replay->isValid)
        return TRUE;
    
    return FALSE;
}

bool8 Rogue_ShouldDisableTrialQuests()
{
    struct AdventureReplay const* replay = &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED];

    if(RogueQuest_HasUnlockedTrials())
    {
        if(Rogue_IsRunActive() && FlagGet(FLAG_ROGUE_RUN_TRIAL_QUESTS_DISABLED))
            return TRUE;

        if(!Rogue_IsRunActive() && Rogue_GetModeRules()->disableTrialQuests)
            return TRUE;

        if(Rogue_IsRunActive() && FlagGet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE) && replay->isValid)
            return TRUE;
    }
    
    return FALSE;
}

#ifdef ROGUE_DEBUG

static u16 GetDebugElementOffset(u16 elem)
{
    AGB_ASSERT(elem >= DEBUG_START_VALUE);
    elem -= DEBUG_START_VALUE;
    return elem;
}

void RogueDebug_SetConfigToggle(u16 e, bool8 state)
{
    u16 elem = GetDebugElementOffset(e);
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(elem < DEBUG_TOGGLE_COUNT);
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueDebug.toggleBits));
    if(state)
    {
        gRogueDebug.toggleBits[idx] |= bitMask;
    }
    else
    {
        gRogueDebug.toggleBits[idx] &= ~bitMask;
    }
}

bool8 RogueDebug_GetConfigToggle(u16 e)
{
    u16 elem = GetDebugElementOffset(e);
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(elem < DEBUG_TOGGLE_COUNT);
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueDebug.toggleBits));
    return (gRogueDebug.toggleBits[idx] & bitMask) != 0;
}

void RogueDebug_SetConfigRange(u16 elem, u8 value)
{
    elem = GetDebugElementOffset(elem);

    AGB_ASSERT(elem < DEBUG_RANGE_COUNT);
    gRogueDebug.rangeValues[elem] = value;
}

u8 RogueDebug_GetConfigRange(u16 elem)
{
    elem = GetDebugElementOffset(elem);

    AGB_ASSERT(elem < DEBUG_RANGE_COUNT);
    return gRogueDebug.rangeValues[elem];
}

#else

void RogueDebug_SetConfigToggle(u16 elem, bool8 state)
{
}

bool8 RogueDebug_GetConfigToggle(u16 elem)
{
    return FALSE;
}

void RogueDebug_SetConfigRange(u16 elem, u8 value)
{
}

u8 RogueDebug_GetConfigRange(u16 elem)
{
    return 0;
}

#endif

void Rogue_ResetSettingsToDefaults(void)
{
    memset(&gRogueSaveBlock->adventureConfig, 0, sizeof(gRogueSaveBlock->adventureConfig));
    gRogueSaveBlock->adventureConfig.overworldMons = TRUE;
    gRogueSaveBlock->adventureConfig.trainerRegions = (1 << 1) | (1 << 2) | (1 << 3);
    gRogueSaveBlock->adventureConfig.pokedexVariant = POKEDEX_VARIANT_HOENN_RSE;
    RogueAscension_ResetProgress();
}

u8 Rogue_GetStartingMonCapacity()
{
    return PARTY_SIZE;
}

static u16 GetCurrentNicknameMode()
{
    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL)
        return gSaveBlock2Ptr->optionsNicknameMode;

    if(RogueTrial_IsActiveTrial(ROGUE_TRIAL_ROGUELOCKE))
        return OPTIONS_NICKNAME_MODE_ALWAYS;

    if(Rogue_InWildSafari())
        return OPTIONS_NICKNAME_MODE_NEVER;

    return gSaveBlock2Ptr->optionsNicknameMode;
}

static bool8 DoesPartyContainNickname(u8 const* str)
{
    u8 i;
    u8 nickname[POKEMON_NAME_LENGTH];

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        GetMonData(&gPlayerParty[i], MON_DATA_NICKNAME, nickname);
        if(StringCompareN(str, nickname, POKEMON_NAME_LENGTH) == 0)
            return TRUE;
    }

    return FALSE;
}

void Rogue_AssignAutomaticNicknameFromSeed(struct Pokemon *mon, u16 seed)
{
    u16 i;
    u16 nicknameIdx = seed % ARRAY_COUNT(sNicknameTable_Global);

    for(i = 0; i < ARRAY_COUNT(sNicknameTable_Global); ++i)
    {
        if(!DoesPartyContainNickname(sNicknameTable_Global[nicknameIdx]))
            break;

        nicknameIdx = (nicknameIdx + 1) % ARRAY_COUNT(sNicknameTable_Global);
    }

    SetMonData(mon, MON_DATA_NICKNAME, sNicknameTable_Global[nicknameIdx]);
}

bool8 Rogue_ShouldSkipAssignNickname(struct Pokemon* mon)
{
    u32 customMonId;

    // Never give snagged mons nicknames
    if(FlagGet(FLAG_ROGUE_IN_SNAG_BATTLE))
        return TRUE;

    // Don't give exotic mons nicknames
    customMonId = RogueGift_GetCustomMonId(mon);

    if(customMonId)
    {
        if(!RogueGift_CanRenameCustomMon(customMonId))
            return TRUE;
    }

    switch (GetCurrentNicknameMode())
    {
    case OPTIONS_NICKNAME_MODE_NEVER:
        return TRUE;

    case OPTIONS_NICKNAME_RANDOM:
        {
            u16 nicknameIdx;
            
            while(TRUE)
            {
                nicknameIdx = Random() % ARRAY_COUNT(sNicknameTable_Global);

                if(!DoesPartyContainNickname(sNicknameTable_Global[nicknameIdx]))
                    break;
            }

            SetMonData(mon, MON_DATA_NICKNAME, sNicknameTable_Global[nicknameIdx]);
            return TRUE;
        }
    }
    
    return FALSE;
}

bool8 Rogue_ShouldSkipAssignNicknameYesNoMessage(struct Pokemon* mon)
{
    switch (GetCurrentNicknameMode())
    {
    case OPTIONS_NICKNAME_MODE_ASK:
        return FALSE;
    }

    return TRUE;
}

bool8 Rogue_ShouldForceNicknameScreen()
{
    switch (GetCurrentNicknameMode())
    {
    case OPTIONS_NICKNAME_MODE_ALWAYS:
        return TRUE;
    }

    return FALSE;
}
