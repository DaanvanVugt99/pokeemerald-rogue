#include "global.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/battle_string_ids.h"
#include "constants/berry.h"
#include "constants/event_objects.h"
#include "constants/form_change_types.h"
#include "constants/heal_locations.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/map_types.h"
#include "constants/rogue.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/trainer_types.h"
#include "constants/trainers.h"
#include "constants/weather.h"
#include "data.h"
#include "decompress.h"
#include "gba/isagbprint.h"

#include "battle.h"
#include "battle_util.h"
#include "battle_setup.h"
#include "battle_transition.h"
#include "berry.h"
#include "event_data.h"
#include "field_effect.h"
#include "graphics.h"
#include "international_string_util.h"
#include "item.h"
#include "item_icon.h"
#include "item_menu.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "field_player_avatar.h"
#include "load_save.h"
#include "malloc.h"
#include "main.h"
#include "menu.h"
#include "metatile_behavior.h"
#include "money.h"
#include "m4a.h"
#include "overworld.h"
#include "party_menu.h"
#include "palette.h"
#include "play_time.h"
#include "player_pc.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "rtc.h"
#include "safari_zone.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "siirtc.h"
#include "strings.h"
#include "string_util.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "text.h"
#include "trainer_card.h"

#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_assistant.h"
#include "rogue_automation.h"
#include "rogue_adventurepaths.h"
#include "rogue_campaign.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_debug.h"
#include "rogue_gameshow.h"
#include "rogue_gifts.h"
#include "rogue_followmon.h"
#include "rogue_hub.h"
#include "rogue_multiplayer.h"
#include "rogue_player_customisation.h"
#include "rogue_pokedex.h"
#include "rogue_popup.h"
#include "rogue_query.h"
#include "rogue_run_start.h"
#include "rogue_route_events.h"
#include "rogue_route_scenes.h"
#include "rogue_trials.h"
#include "rogue_quest.h"
#include "rogue_ridemon.h"
#include "rogue_safari.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_timeofday.h"
#include "rogue_trainers.h"

STATIC_ASSERT(sizeof(struct BoxPokemon) == sizeof(struct RogueBoxPokemonFacade), SizeOfRogueBoxPokemonFacade);
STATIC_ASSERT(sizeof(struct Pokemon) == sizeof(struct RoguePokemonFacade), SizeOfRoguePokemonFacade);

#define ROGUE_TRAINER_COUNT (FLAG_ROGUE_TRAINER_END - FLAG_ROGUE_TRAINER_START + 1)
#define ROGUE_ITEM_COUNT (FLAG_ROGUE_ITEM_END - FLAG_ROGUE_ITEM_START + 1)
#define ROUTE_SPECIAL_ITEM_CAPACITY 256
#define ROUTE_SPECIAL_ITEM_DROP_CHANCE_PER_MILLE 3
#define HIDEOUT_SPECIAL_ITEM_DROP_CHANCE_PER_MILLE 50
#define PARTY_SPECIAL_ITEM_WEIGHT 8
#define OTHER_SPECIAL_ITEM_WEIGHT 1
#define ROUTE_POKEBLOCK_BUNDLE_WEIGHT 4

#ifdef ROGUE_DEBUG
EWRAM_DATA u8 gDebug_CurrentTab = 0;
EWRAM_DATA u8 gDebug_WildOptionCount = 0;
EWRAM_DATA u8 gDebug_ItemOptionCount = 0;
EWRAM_DATA u8 gDebug_TrainerOptionCount = 0;

extern const u8 gText_RogueDebug_Header[];
extern const u8 gText_RogueDebug_Save[];
extern const u8 gText_RogueDebug_Room[];
extern const u8 gText_RogueDebug_BossRoom[];
extern const u8 gText_RogueDebug_Difficulty[];
extern const u8 gText_RogueDebug_PlayerLvl[];
extern const u8 gText_RogueDebug_WildLvl[];
extern const u8 gText_RogueDebug_WildCount[];
extern const u8 gText_RogueDebug_ItemCount[];
extern const u8 gText_RogueDebug_TrainerCount[];
extern const u8 gText_RogueDebug_Seed[];

extern const u8 gText_RogueDebug_AdvHeader[];
extern const u8 gText_RogueDebug_AdvCount[];
extern const u8 gText_RogueDebug_X[];
extern const u8 gText_RogueDebug_Y[];
#endif

extern const u8 gText_TrainerName_Default[];

struct RouteMonPreview
{
    u8 monSpriteId;
};

struct RogueCatchingContest
{
    u16 winningSpecies;
    u16 winningScore;
    u16 itemCountToRestore;
    u8 activeType;
    u8 activeStat;
    u8 spawnsRemaining;
    u8 isActive : 1;
};

struct RogueUniqueMonPalette
{
    u16 tempPalette[16];
};

// Temp data only ever stored in RAM
struct RogueLocalData
{
    struct RouteMonPreview encounterPreview[WILD_ENCOUNTER_GRASS_CAPACITY];
    struct RogueGameShow gameShow;
    struct RogueCatchingContest catchingContest;
    struct RogueUniqueMonPalette uniqueMonPalette;
    RAND_TYPE rngSeedToRestore;
    RAND_TYPE rngToRestore;
    RAND_TYPE rng2ToRestore;
    u32 totalMoneySpentOnMap;
    u32 wildBattleCustomMonId;
    u16 cachedObjIds[OBJ_EVENT_ID_MULTIPLAYER_COUNT];
    u16 partyRememberedHealth[PARTY_SIZE];
    u16 wildEncounterHistoryBuffer[3];
    u16 victoryLapHistoryBuffer[8];
    u16 recentObjectEventLoadedLayout;
    s16 autoPickupLastX;
    s16 autoPickupLastY;
    bool8 hasQuickLoadPending : 1;
    bool8 hasValidQuickSave : 1;
    bool8 hasNicknameMonMsgPending : 1;
    bool8 hasBattleEventOccurred : 1;
    bool8 hasUsePlayerTeamTempSave : 1;
    bool8 hasUseEnemyTeamTempSave : 1;
    bool8 hasBattleInputStarted : 1;
    bool8 hasPendingSnagBattle : 1;
    bool8 hasPendingRidemonTrappedCheck : 1;
    bool8 isShrineChallengeActive : 1;
};

typedef u16 hot_track_dat;

struct RogueHotTracking
{
    hot_track_dat initSeed;
    hot_track_dat rollingSeed;
    hot_track_dat triggerCount;
    hot_track_dat triggerMin;
    hot_track_dat triggerMax;
    hot_track_dat triggerAccumulation;
};

#ifdef ROGUE_FEATURE_HOT_TRACKING
static struct RogueHotTracking gRogueHotTracking;
#endif

EWRAM_DATA struct RogueRunData gRogueRun = {};

// Temporary data, that isn't remembered
EWRAM_DATA struct RogueLocalData gRogueLocal = {};
EWRAM_DATA struct RogueAdvPath gRogueAdvPath = {};
static EWRAM_DATA bool8 sSacredAshRecoveryPending = FALSE;
static EWRAM_DATA bool8 sPendingRoomEntrySetup = FALSE;
static EWRAM_DATA bool8 sExecutingDeferredRoomEntry = FALSE;

static void ResetHotTracking();
static void ClearRogueLocalData(void);

static u8 CalculateWildLevel(u8 variation);

static bool8 CanLearnMoveByLvl(u16 species, u16 move, s32 level);

static u8 GetCurrentWildEncounterCount(void);
static u8 GetCurrentWaterEncounterCount(void);
static u16 GetWildGrassEncounter(u8 index);
static u16 GetWildWaterEncounter(u8 index);
static u16 GetWildEncounterIndexFor(u16 species);
static void HandleForfeitingInCatchingContest();

void EnableRivalEncounterIfRequired();
static void ChooseLegendarysForNewAdventure();
static void ChooseUniqueDenForNewAdventure();
static void ChooseTeamEncountersForNewAdventure();
static void RememberPartyHeldItems();
static void RememberPartyHealth();
static void TryRestorePartyHeldItems(bool8 allowThief);
static void ClearPlayerTeam();
static void CheckAndNotifyForFaintedMons();

static void SwapMonItems(u8 aIdx, u8 bIdx, struct Pokemon *party);

static void RandomiseSafariWildEncounters(void);
static void RandomiseWildEncounters(void);
static void RandomiseFishingEncounters(void);
static void ResetTrainerBattles(void);
static void RandomiseEnabledTrainers(void);
static void RandomiseEnabledItems(void);
static void RandomiseMiniBossItemsAndRestoreSeed(void);
static void RandomiseBerryTrees(void);
static void RandomiseTRMoves();

static bool8 IsRareWeightedSpecies(u16 species);
static void RandomiseCharmItems(void);
static bool8 HasHoneyTreeEncounterPending(void);
static void ClearHoneyTreePokeblock(void);

static void SetupTrainerBattleInternal(u16 trainerNum);

u16 RogueRandomRange(u16 range, u8 flag)
{
    // Always use rogue random to avoid seeding issues based on flag
    u16 res = RogueRandom();

    if(range <= 1)
        return 0;

    return res % range;
}

bool8 RogueRandomChance(u8 chance, u16 seedFlag)
{
    if(chance == 0)
        return FALSE;
    else if(chance >= 100)
        return TRUE;

    return (RogueRandomRange(100, seedFlag) + 1) <= chance;
}

u16 Rogue_GetShinyOdds(u8 shinyRoll)
{
    u16 baseOdds = 0;

    switch (shinyRoll)
    {
    case SHINY_ROLL_DYNAMIC:
        baseOdds = 400;
        break;

    case SHINY_ROLL_STATIC:
        baseOdds = 100;
        break;

    case SHINY_ROLL_SHINY_LOCKED:
        baseOdds = 0;
        break;

    default:
        AGB_ASSERT(FALSE);
        break;
    }

    if(VarGet(VAR_ROGUE_ACTIVE_POKEBLOCK) == ITEM_POKEBLOCK_SHINY)
        baseOdds /= 2;


    if(IsCurseActive(EFFECT_SNAG_TRAINER_MON))
        baseOdds = 0;

    return baseOdds;
}

bool8 Rogue_RollShinyState(u8 shinyRoll)
{
    // Intentionally don't see shiny state
    u16 shinyOdds = Rogue_GetShinyOdds(shinyRoll);
    u16 rngValue = Random();
    return shinyOdds == 0 ? FALSE : (rngValue % shinyOdds) == 0;
}


static u16 GetEncounterChainShinyOdds(u8 count)
{
    u16 baseOdds = Rogue_GetShinyOdds(SHINY_ROLL_DYNAMIC);

    // By the time we reach 48 encounters, we want to be at max odds
    // Don't start increasing shiny rate until we pass 4 encounters
    if(count <= 4)
    {
        return baseOdds;
    }
    else
    {
        u16 range = ((VarGet(VAR_ROGUE_ACTIVE_POKEBLOCK) == ITEM_POKEBLOCK_SHINY) ? 24 : 48) - 4;
        u16 t = min(count - 4, range);
        u16 targetOdds = 16;

        return (targetOdds * t + baseOdds * (range - t)) / range;
    }
}

bool8 Rogue_IsRunActive(void)
{
    return FlagGet(FLAG_ROGUE_RUN_ACTIVE);
}

bool8 Rogue_IsVictoryLapActive(void)
{
    return Rogue_IsRunActive() && FlagGet(FLAG_ROGUE_IS_VICTORY_LAP);
}

bool8 Rogue_InWildSafari(void)
{
    return FlagGet(FLAG_ROGUE_WILD_SAFARI);
}

bool8 Rogue_UseSafariBattle()
{
    return Rogue_InWildSafari() || Rogue_IsCatchingContestActive();
}

bool8 Rogue_CanChangeSafariBall(void)
{
    return !Rogue_IsCatchingContestActive();
}

bool8 Rogue_SpeciesShareEvolutionLine(u16 speciesA, u16 speciesB)
{
#ifdef ROGUE_EXPANSION
    speciesA = GET_BASE_SPECIES_ID(speciesA);
    speciesB = GET_BASE_SPECIES_ID(speciesB);
#endif

    return speciesA == speciesB
        || Rogue_DoesEvolveInto(speciesA, speciesB)
        || Rogue_DoesEvolveInto(speciesB, speciesA);
}

bool8 Rogue_PartyContainsSpeciesChain(u16 checkSpecies, u8 ignoredSlot1, u8 ignoredSlot2)
{
    u8 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        u16 species;

        if(i == ignoredSlot1 || i == ignoredSlot2)
            continue;

        if(RogueRouteEvents_IsMysteryEggCourierEgg(&gPlayerParty[i]))
            continue;

#ifdef ROGUE_EXPANSION
        species = GET_BASE_SPECIES_ID(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES));
#else
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
#endif

        if(species != SPECIES_NONE && Rogue_SpeciesShareEvolutionLine(species, checkSpecies))
            return TRUE;
    }

    return FALSE;
}

bool8 Rogue_PartyHasDuplicateSpecies(struct Pokemon *mon, u8 ignoredSlot1, u8 ignoredSlot2)
{
    u16 species;

    if(!Rogue_IsSpeciesClauseActive())
        return FALSE;

    // A courier Egg is a temporary delivery object and is not allowed to hatch
    // during the run, so it must not participate in Species Clause checks.
    if(RogueRouteEvents_IsMysteryEggCourierEgg(mon))
        return FALSE;

#ifdef ROGUE_EXPANSION
    species = GET_BASE_SPECIES_ID(GetMonData(mon, MON_DATA_SPECIES));
#else
    species = GetMonData(mon, MON_DATA_SPECIES);
#endif

    if(species == SPECIES_NONE)
        return FALSE;

    return Rogue_PartyContainsSpeciesChain(species, ignoredSlot1, ignoredSlot2);
}

bool8 Rogue_PartyHasHeldItem(u16 itemId, u8 ignoredSlot1, u8 ignoredSlot2)
{
    u8 i;

    if(!Rogue_IsHeldItemClauseActive())
        return FALSE;

    if(itemId == ITEM_NONE)
        return FALSE;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(i == ignoredSlot1 || i == ignoredSlot2)
            continue;

        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE
            && GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM) == itemId)
            return TRUE;
    }

    return FALSE;
}

bool8 Rogue_TryRemoveDuplicateHeldItemForParty(struct Pokemon *mon, u8 ignoredSlot1, u8 ignoredSlot2)
{
    u16 item = GetMonData(mon, MON_DATA_HELD_ITEM);

    if(!Rogue_PartyHasHeldItem(item, ignoredSlot1, ignoredSlot2))
        return TRUE;

    if(!AddBagItem(item, 1))
        return FALSE;

    item = ITEM_NONE;
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    return TRUE;
}

static bool8 Rogue_CanStoreItemWithoutNewBagSlot(u16 item, u16 count)
{
    u8 i;
    u8 pocket;
    u16 slotCapacity;

    if(item == ITEM_NONE)
        return TRUE;

    pocket = ItemId_GetPocket(item);

    if(pocket == POCKET_NONE)
        return FALSE;

    --pocket;
    slotCapacity = Rogue_GetBagPocketAmountPerItem(pocket);

    for(i = 0; i < gBagPockets[pocket].capacity; ++i)
    {
        if(gBagPockets[pocket].itemSlots[i].itemId == item)
        {
            u16 ownedCount = GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);
            u16 freeCount = min(slotCapacity - ownedCount, count);

            count -= freeCount;

            if(count == 0)
                return TRUE;
        }
    }

    return FALSE;
}

static void Rogue_AddRequiredBagSlotForItem(u16 item, u16 count, u16 *reservedSlots, u16 *unreservedSlots)
{
    u8 pocket;

    if(item == ITEM_NONE || Rogue_CanStoreItemWithoutNewBagSlot(item, count))
        return;

    pocket = ItemId_GetPocket(item);

    if(ItemPocketUsesReservedSlots(pocket))
        ++*reservedSlots;
    else
        ++*unreservedSlots;
}

static bool8 Rogue_CanStoreCaughtMonHeldItemAfterRelease(struct Pokemon *mon, u8 releasedSlot)
{
    u16 caughtItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    u16 releasedItem = ITEM_NONE;
    u16 reservedSlots = 0;
    u16 unreservedSlots = 0;

    if(!Rogue_PartyHasHeldItem(caughtItem, releasedSlot, PARTY_SIZE))
        return TRUE;

    if(releasedSlot < PARTY_SIZE)
        releasedItem = GetMonData(&gPlayerParty[releasedSlot], MON_DATA_HELD_ITEM);

    if(caughtItem == releasedItem)
    {
        Rogue_AddRequiredBagSlotForItem(caughtItem, 2, &reservedSlots, &unreservedSlots);
    }
    else
    {
        Rogue_AddRequiredBagSlotForItem(releasedItem, 1, &reservedSlots, &unreservedSlots);
        Rogue_AddRequiredBagSlotForItem(caughtItem, 1, &reservedSlots, &unreservedSlots);
    }

    return reservedSlots <= GetBagReservedFreeSlots()
        && unreservedSlots <= GetBagUnreservedFreeSlots();
}

bool8 Rogue_CaughtMonFitsSpeciesClauseAfterRelease(struct Pokemon *mon, u8 releasedSlot)
{
    return !Rogue_PartyHasDuplicateSpecies(mon, releasedSlot, PARTY_SIZE);
}

bool8 Rogue_CaughtMonFitsHeldItemClauseAfterRelease(struct Pokemon *mon, u8 releasedSlot)
{
    u16 item = GetMonData(mon, MON_DATA_HELD_ITEM);

    if(!Rogue_PartyHasHeldItem(item, releasedSlot, PARTY_SIZE))
        return TRUE;

    return Rogue_CanStoreCaughtMonHeldItemAfterRelease(mon, releasedSlot);
}

bool8 Rogue_CanAddCaughtMonToParty(struct Pokemon *mon)
{
    if(Rogue_IsRunActive())
    {
        if(!RogueTrial_CanAcceptMon(mon))
            return FALSE;

        if(CalculatePlayerPartyCount() >= Rogue_GetMaxPartySize())
            return FALSE;

        if(!Rogue_CaughtMonFitsSpeciesClauseAfterRelease(mon, PARTY_SIZE))
            return FALSE;

        if(!Rogue_CaughtMonFitsHeldItemClauseAfterRelease(mon, PARTY_SIZE))
            return FALSE;
    }

    return TRUE;
}

bool8 Rogue_CanReleasePartyMonForCaughtMon(struct Pokemon *mon, u8 slot)
{
    if(!Rogue_IsRunActive())
        return TRUE;

    if(!RogueTrial_CanAcceptMon(mon))
        return FALSE;

    if(slot >= PARTY_SIZE || GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES) == SPECIES_NONE)
        return FALSE;

    return Rogue_CaughtMonFitsSpeciesClauseAfterRelease(mon, slot)
        && Rogue_CaughtMonFitsHeldItemClauseAfterRelease(mon, slot);
}

bool8 Rogue_IsBagClauseActive(void)
{
    return Rogue_IsRunActive() && Rogue_GetConfigToggle(CONFIG_TOGGLE_BAG_CLAUSE);
}

bool8 Rogue_IsSpeciesClauseActive(void)
{
    return Rogue_IsRunActive() && Rogue_GetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE);
}

bool8 Rogue_IsHeldItemClauseActive(void)
{
    return Rogue_IsRunActive() && Rogue_GetConfigToggle(CONFIG_TOGGLE_HELD_ITEM_CLAUSE);
}

u8 Rogue_GetCurrentDifficulty(void)
{
    return gRogueRun.currentDifficulty;
}

void Rogue_SetCurrentDifficulty(u8 difficulty)
{
    AGB_ASSERT(difficulty <= ROGUE_MAX_BOSS_COUNT);
    gRogueRun.currentDifficulty = difficulty;
}

bool8 Rogue_ForceExpAll(void)
{
    return Rogue_GetConfigToggle(CONFIG_TOGGLE_EXP_ALL);
}

u16* Rogue_GetVictoryLapHistoryBufferPtr()
{
    return gRogueLocal.victoryLapHistoryBuffer;
}
u32 Rogue_GetVictoryLapHistoryBufferSize()
{
    return ARRAY_COUNT(gRogueLocal.victoryLapHistoryBuffer);
}

bool8 Rogue_EnableExpGain(void)
{
    // Don't give exp in battle sim flat battles
    if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && Rogue_IsBattleSimTrainer(gTrainerBattleOpponent_A))
        return FALSE;

    return TRUE;
}

bool8 Rogue_EnableAffectionMechanics(void)
{
#ifdef ROGUE_EXPANSION
    return Rogue_GetConfigToggle(CONFIG_TOGGLE_AFFECTION);
#else
    return FALSE;
#endif
}

bool8 Rogue_ShouldReleaseFaintedMons(void)
{
    return Rogue_GetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS);
}

bool8 Rogue_FastBattleAnims(void)
{
    if(GetSafariZoneFlag())
    {
        return TRUE;
    }

    return !Rogue_UseKeyBattleAnims();
}

bool8 InBattleChoosingMoves();
bool8 InBattleRunningActions();

static u8 GetBattleSceneOption()
{
    if(Rogue_UseKeyBattleAnims())
        return gSaveBlock2Ptr->optionsBossBattleScene;
    else if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0)
        return gSaveBlock2Ptr->optionsTrainerBattleScene;
    else
        return gSaveBlock2Ptr->optionsWildBattleScene;
}

static bool8 IsCurrentExpTrainerBattle(void)
{
    return (gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && Rogue_IsExpTrainer(gTrainerBattleOpponent_A);
}

static u8 GetAnimSpeedScale(u8 option)
{
    if (JOY_HELD(L_BUTTON))
        return 1;

    AGB_ASSERT(option < OPTIONS_ANIM_SPEED_COUNT);
    return min(option, OPTIONS_ANIM_SPEED_COUNT - 1) + 1;
}

u8 Rogue_GetCutsceneSpeedScale(void)
{
    return GetAnimSpeedScale(gSaveBlock2Ptr->optionsCutsceneSpeed);
}

u8 Rogue_GetEvolutionSpeedScale(void)
{
    return GetAnimSpeedScale(gSaveBlock2Ptr->optionsEvolutionSpeed);
}

u8 Rogue_GetCatchingSpeedScale(void)
{
    return GetAnimSpeedScale(gSaveBlock2Ptr->optionsCatchingSpeed);
}

u8 Rogue_GetBattleSpeedScale(bool8 forHealthbar)
{
    u8 battleSceneOption = GetBattleSceneOption();

    // Hold L to slow down
    if(JOY_HELD(L_BUTTON))
        return 1;

    // We want to speed up all anims until input selection starts
    if(InBattleChoosingMoves())
        gRogueLocal.hasBattleInputStarted = TRUE;

    if(IsCurrentExpTrainerBattle())
        battleSceneOption = OPTIONS_BATTLE_SCENE_4X;

    if(gRogueLocal.hasBattleInputStarted)
    {
        // Always run at 1x speed here
        if(InBattleChoosingMoves())
            return 1;

        // When battle anims are turned off, it's a bit too hard to read text, so force running at normal speed
        if(!forHealthbar && battleSceneOption == OPTIONS_BATTLE_SCENE_DISABLED && InBattleRunningActions())
            return 1;
    }
    else
    {
        // Use menu transition speed here to speed it up, if requested
        if(gSaveBlock2Ptr->optionsFadeSpeed == OPTIONS_TEXT_SPEED_MID)
        {
            battleSceneOption = max(battleSceneOption, OPTIONS_BATTLE_SCENE_2X);
        }
        else if(gSaveBlock2Ptr->optionsFadeSpeed == OPTIONS_TEXT_SPEED_FAST)
        {
            battleSceneOption = max(battleSceneOption, OPTIONS_BATTLE_SCENE_4X);
        }
    }

    if(gRogueLocal.hasBattleInputStarted)
    {
        // We don't need to speed up health bar anymore as that passively happens now
        switch (battleSceneOption)
        {
        case OPTIONS_BATTLE_SCENE_1X:
            return forHealthbar ? 1 : 1;

        case OPTIONS_BATTLE_SCENE_2X:
            return forHealthbar ? 1 : 2;

        case OPTIONS_BATTLE_SCENE_3X:
            return forHealthbar ? 1 : 3;

        case OPTIONS_BATTLE_SCENE_4X:
            return forHealthbar ? 1 : 4;

        // Print text at a readable speed still
        case OPTIONS_BATTLE_SCENE_DISABLED:
            if(gRogueLocal.hasBattleInputStarted)
                return forHealthbar ? 10 : 1;
            else
                return 4;
        }
    }
    else // speeds for battle transitions
    {
        switch (battleSceneOption)
        {
        case OPTIONS_BATTLE_SCENE_1X:
            return 1;

        case OPTIONS_BATTLE_SCENE_2X:
            return 3;

        case OPTIONS_BATTLE_SCENE_3X:
            return 4;

        case OPTIONS_BATTLE_SCENE_4X:
        case OPTIONS_BATTLE_SCENE_DISABLED:
            return 6;
        }
    }

    return 1 ;
}

bool8 Rogue_UseKeyBattleAnims(void)
{
#if !TESTING
    if(Rogue_IsRunActive())
    {
        if(Rogue_IsVictoryLapActive())
            return FALSE;

        // Force slow anims for bosses
        if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && Rogue_IsKeyTrainer(gTrainerBattleOpponent_A))
            return TRUE;

        // Force slow anims for legendaries
        if((gBattleTypeFlags & BATTLE_TYPE_LEGENDARY) != 0)
            return TRUE;

        // If we've encountered a wild shiny or unique mon, we're going to treat it as an important battle
        if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) == 0 && (IsMonShiny(&gEnemyParty[0]) || gRogueLocal.wildBattleCustomMonId != 0))
            return TRUE;
    }
    else
    {
        // First catch is an important battle
        if(VarGet(VAR_ROGUE_INTRO_STATE) <= ROGUE_INTRO_STATE_REPORT_TO_PROF)
            return TRUE;
    }
#endif

    return FALSE;
}

bool8 Rogue_GetBattleAnimsEnabled(void)
{
    return GetBattleSceneOption() != OPTIONS_BATTLE_SCENE_DISABLED;
}

bool8 CheckOnlyTheseTrainersEnabled(u32 toggleToCheck);

bool8 Rogue_UseFinalQuestEffects(void)
{
    if(RogueQuest_IsQuestUnlocked(QUEST_ID_THE_FINAL_RUN))
    {
        if(RoguePokedex_GetDexVariant() != POKEDEX_VARIANT_NATIONAL_MAX)
            return FALSE;

        if(!CheckOnlyTheseTrainersEnabled(CONFIG_TOGGLE_TRAINER_ROGUE))
            return FALSE;

        if(Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM) != ROGUE_GAME_MODE_STANDARD)
            return FALSE;

        return TRUE;
    }

    return FALSE;
}

struct RogueGameShow* Rogue_GetGameShow()
{
    return &gRogueLocal.gameShow;
}

static bool8 HasFinalQuestBossAppliedSwap()
{
    return gRogueLocal.hasBattleEventOccurred != 0;
}

bool8 Rogue_IsFinalQuestFinalBoss(void)
{
    if(Rogue_UseFinalQuestEffects() && (gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && Rogue_GetCurrentDifficulty() >= ROGUE_FINAL_CHAMP_DIFFICULTY)
    {
        return Rogue_IsBossTrainer(gTrainerBattleOpponent_A);
    }

    return FALSE;
}

bool8 Rogue_ApplyFinalQuestFinalBossTeamSwap(void)
{
    if(Rogue_IsFinalQuestFinalBoss())
    {
        if(!HasFinalQuestBossAppliedSwap())
        {
            // Only do this once
            // Swap out team for the "final" mon a custom Wobbuffet
            u8 i;
            u32 fixedIVs = GetMonData(&gEnemyParty[0], MON_DATA_HP_IV);

            if(gEnemyPartyCount == PARTY_SIZE)
            {
                // Find an empty index which isn't sent out yet
                // (prefer these to go into the early spots)
                for(i = 0; i < PARTY_SIZE; ++i)
                {
                    if(i == gBattlerPartyIndexes[B_POSITION_OPPONENT_LEFT])
                        continue;

                    if((gBattleTypeFlags & BATTLE_TYPE_DOUBLE) && i == gBattlerPartyIndexes[B_POSITION_OPPONENT_RIGHT])
                        continue;

                    break;
                }
            }
            else
            {
                i = gEnemyPartyCount;
            }

            // Create custom wobbuffet
            RogueGift_CreateMon(CUSTOM_MON_WAHEY_WOBBUFFET, &gEnemyParty[i], SPECIES_WOBBUFFET, MAX_LEVEL, fixedIVs);

            // Apply different music ??
            //PlayBGM();

            gRogueLocal.hasBattleEventOccurred = TRUE;

            return TRUE;
        }
    }

    return FALSE;
}

bool8 Rogue_AssumeFinalQuestFakeChamp(void)
{
    // Present in a way that seems like this is the final champ only for us to reveal after that it wasn't
    return Rogue_UseFinalQuestEffects() && (Rogue_GetCurrentDifficulty() == ROGUE_CHAMP_START_DIFFICULTY && FlagGet(FLAG_ROGUE_FINAL_QUEST_MET_FAKE_CHAMP));
}

bool8 Rogue_Use100PercEffects(void)
{
    u16 mainPerc = RogueQuest_GetQuestCompletePercFor(QUEST_CONST_IS_MAIN_QUEST);

    // TODO - Also consider NG+
    if(mainPerc == 100 && !Rogue_Use200PercEffects())
    {
        return TRUE;
    }

    return FALSE;
}

bool8 Rogue_Use200PercEffects(void)
{
    u16 completionPerc = RogueQuest_GetDisplayCompletePerc();

    if(completionPerc == 100)
    {
        return TRUE;
    }

    return FALSE;
}

extern const struct Song gSongTable[];

u8 Rogue_ModifySoundVolume(struct MusicPlayerInfo *mplayInfo, u8 volume, u16 soundType)
{
    // 10 is eqv of 100%
    u8 audioLevel = 10;

    switch (soundType)
    {
    case ROGUE_SOUND_TYPE_CRY:
        // Don't modify this?
        break;

    default:
        if(mplayInfo == &gMPlayInfo_BGM)
        {
            // Fanfares are exempt
            if(
                mplayInfo->songHeader == gSongTable[MUS_LEVEL_UP].header ||
                mplayInfo->songHeader == gSongTable[MUS_OBTAIN_ITEM].header ||
                mplayInfo->songHeader == gSongTable[MUS_EVOLVED].header ||
                mplayInfo->songHeader == gSongTable[MUS_OBTAIN_TMHM].header ||
                mplayInfo->songHeader == gSongTable[MUS_HEAL].header ||
                mplayInfo->songHeader == gSongTable[MUS_DP_HEAL].header ||
                mplayInfo->songHeader == gSongTable[MUS_OBTAIN_BADGE].header ||
                mplayInfo->songHeader == gSongTable[MUS_MOVE_DELETED].header ||
                mplayInfo->songHeader == gSongTable[MUS_OBTAIN_BERRY].header ||
                mplayInfo->songHeader == gSongTable[MUS_AWAKEN_LEGEND].header ||
                mplayInfo->songHeader == gSongTable[MUS_SLOTS_JACKPOT].header ||
                mplayInfo->songHeader == gSongTable[MUS_SLOTS_WIN].header ||
                mplayInfo->songHeader == gSongTable[MUS_TOO_BAD].header ||
                mplayInfo->songHeader == gSongTable[MUS_RG_POKE_FLUTE].header ||
                mplayInfo->songHeader == gSongTable[MUS_RG_OBTAIN_KEY_ITEM].header ||
                mplayInfo->songHeader == gSongTable[MUS_RG_DEX_RATING].header ||
                mplayInfo->songHeader == gSongTable[MUS_OBTAIN_B_POINTS].header ||
                mplayInfo->songHeader == gSongTable[MUS_OBTAIN_SYMBOL].header ||
                mplayInfo->songHeader == gSongTable[MUS_REGISTER_MATCH_CALL].header ||
                mplayInfo->songHeader == gSongTable[MUS_HG_LEVEL_UP].header ||
                mplayInfo->songHeader == gSongTable[MUS_HG_EVOLVED].header ||
                mplayInfo->songHeader == gSongTable[MUS_DP_LEVEL_UP].header ||
                mplayInfo->songHeader == gSongTable[MUS_DP_EVOLVED].header
            )
            {
                // do nothing
            }
            else
            {
                audioLevel = gSaveBlock2Ptr->optionsSoundChannelBGM;
            }
        }
        else
        {
            if(
                mplayInfo->songHeader == gSongTable[SE_SELECT].header ||
                mplayInfo->songHeader == gSongTable[SE_DEX_SCROLL].header ||
                mplayInfo->songHeader == gSongTable[SE_PIN].header ||
                mplayInfo->songHeader == gSongTable[SE_WIN_OPEN].header ||
                mplayInfo->songHeader == gSongTable[SE_BALL].header
            )
            {
                // UI sound effects
                audioLevel = gSaveBlock2Ptr->optionsSoundChannelSE;
            }
            else if(gMain.inBattle)
            {
                // Assume all sounds are battle effects
                audioLevel = gSaveBlock2Ptr->optionsSoundChannelBattleSE;
            }
        }
        break;
    }

    if(audioLevel != 10)
    {
        return (volume * audioLevel) / 10;
    }

    return volume;
}

static u16 ModifyBattleSongByMap(u16 songNum, u32 mapFlags)
{
    switch (songNum)
    {
    case MUS_VS_WILD:
        if(mapFlags & ROUTE_FLAG_KANTO)
            return MUS_RG_VS_WILD;

        else if(mapFlags & ROUTE_FLAG_JOHTO)
            return MUS_HG_VS_WILD;

        else if(mapFlags & ROUTE_FLAG_SINNOH)
            return MUS_DP_VS_WILD;
        break;

    case MUS_VICTORY_WILD:
        if(mapFlags & ROUTE_FLAG_KANTO)
            return MUS_RG_VICTORY_WILD;

        else if(mapFlags & ROUTE_FLAG_JOHTO)
            return MUS_HG_VICTORY_WILD;

        else if(mapFlags & ROUTE_FLAG_SINNOH)
            return MUS_DP_VICTORY_WILD;
        break;

    case MUS_VS_TRAINER:
        if(mapFlags & ROUTE_FLAG_KANTO)
            return MUS_RG_VS_TRAINER;

        else if(mapFlags & ROUTE_FLAG_JOHTO)
            return MUS_HG_VS_TRAINER;

        else if(mapFlags & ROUTE_FLAG_SINNOH)
            return MUS_DP_VS_TRAINER;
        break;

    case MUS_VICTORY_TRAINER:
        if(mapFlags & ROUTE_FLAG_KANTO)
            return MUS_RG_VICTORY_TRAINER;

        else if(mapFlags & ROUTE_FLAG_JOHTO)
            return MUS_HG_VICTORY_TRAINER;

        else if(mapFlags & ROUTE_FLAG_SINNOH)
            return MUS_DP_VICTORY_TRAINER;
        break;
    }

    return songNum;
}

u16 Rogue_ModifyPlayBGM(u16 songNum)
{
    if(Rogue_InWildSafari() || Rogue_IsCatchingContestActive())
    {
        songNum = ModifyBattleSongByMap(songNum, ROUTE_FLAG_KANTO);
    }

    if(Rogue_IsRunActive())
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
        {
            u32 mapFlags = gRogueRouteTable.routes[gRogueRun.currentRouteIndex].mapFlags;
            songNum = ModifyBattleSongByMap(songNum, mapFlags);
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BATTLE_TOWER)
        {
            if(songNum == MUS_VS_TRAINER)
                songNum = MUS_DP_VS_TRAINER;
        }
    }
    else
    {
        if(VarGet(VAR_ROGUE_INTRO_STATE) == ROGUE_INTRO_STATE_LEAVE_LAB || VarGet(VAR_ROGUE_INTRO_STATE) == ROGUE_INTRO_STATE_CATCH_MON)
        {
            switch (songNum)
            {
            case MUS_LITTLEROOT:
            case MUS_HG_ELM_LAB:
                songNum = MUS_HELP;
                break;
            }
        }

        if(VarGet(VAR_ROGUE_SPECIAL_MODE) == ROGUE_SPECIAL_MODE_DECORATING)
        {
            switch (songNum)
            {
            case MUS_LITTLEROOT:
                songNum = MUS_DP_UNDERGROUND;
                break;
            }
        }

        switch (songNum)
        {
        case MUS_LITTLEROOT:
            switch(RogueToD_GetSeason())
            {
                case SEASON_SPRING:
                    songNum = MUS_HG_NEW_BARK;
                    break;
                case SEASON_SUMMER:
                    songNum = MUS_DP_SANDGEM_DAY;
                    break;
                case SEASON_AUTUMN:
                    songNum = MUS_DP_FLOAROMA_DAY;
                    break;
                case SEASON_WINTER:
                    songNum = MUS_DP_SNOWPOINT_DAY;
                    break;
            }
            break;
        }
    }

    switch (songNum)
    {
    //case MUS_EVOLUTION:
    //    songNum = MUS_HG_EVOLUTION;
    //    break;
    }

    return songNum;
}

u16 Rogue_ModifyPlaySE(u16 songNum)
{
    return songNum;
}

u16 Rogue_ModifyPlayFanfare(u16 songNum)
{
    switch (songNum)
    {
    case MUS_HEAL:
        return MUS_DP_HEAL;

    case MUS_LEVEL_UP:
        return MUS_HG_LEVEL_UP;
    }

    return songNum;
}

void Rogue_ModifyExpGained(struct Pokemon *mon, s32* expGain)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);

    if ((gBattleTypeFlags & BATTLE_TYPE_TRAINER) && !IsCurrentExpTrainerBattle() && RogueTrial_IsTrainerBattleExpDisabled())
    {
        *expGain = 0;
        return;
    }

    if(!Rogue_EnableExpGain())
    {
        *expGain = 0;
        return;
    }

    if(Rogue_IsRunActive() && species != SPECIES_NONE)
    {
        u8 targetLevel = Rogue_CalculatePlayerLvlCap();
        u8 maxLevel = Rogue_CalculatePlayerMaxLvl();
        u8 currentLevel = GetMonData(mon, MON_DATA_LEVEL);

        if(currentLevel != MAX_LEVEL)
        {
            u8 growthRate = gRogueSpeciesInfo[species].growthRate; // Was using GROWTH_FAST?
            u32 currLvlExp;
            u32 nextLvlExp;
            u32 lvlExp;

            s32 desiredExpPerc = 0;

            if(currentLevel < maxLevel)
            {
                if(currentLevel < targetLevel)
                {
                    s16 delta = targetLevel - currentLevel;

                    // Level up immediatly to the targetLevel (As it's the soft cap and moves with each fight)
                    desiredExpPerc = 100 * delta;
                }
            }
            else
            {
                if(Rogue_GetConfigToggle(CONFIG_TOGGLE_OVER_LVL))
                {
                    desiredExpPerc = 34;
                }
                else
                {
                    // No EXP once at target level
                    desiredExpPerc = 0;
                }
            }

            if(desiredExpPerc != 0)
            {
                // Pretty sure expGain get's casted to a u16 at some point so there's a limit to how much exp we can give
                s16 actualExpGain = 0;
                s16 lastExpGain = 0;

                //desiredExpPerc = 100;

                // Give levels
                while(desiredExpPerc > 0)
                {
                    currLvlExp = Rogue_ModifyExperienceTables(growthRate, currentLevel);
                    nextLvlExp = Rogue_ModifyExperienceTables(growthRate, currentLevel + 1);
                    lvlExp = (nextLvlExp - currLvlExp);

                    if(desiredExpPerc >= 100)
                    {
                        actualExpGain += lvlExp;
                        desiredExpPerc -= 100;

                        ++currentLevel;
                    }
                    else
                    {
                        s32 actualDelta = (lvlExp * desiredExpPerc) / 100;

                        // Give leftovers
                        actualExpGain += actualDelta;
                        desiredExpPerc = 0;
                    }

                    if(actualExpGain < lastExpGain)
                    {
                        // We must have overflowed :O
                        actualExpGain = 32767; // Max of a s16
                        break;
                    }

                    lastExpGain = actualExpGain;
                }

                *expGain = actualExpGain;
            }
            else
            {
                *expGain = 0;
            }
        }
    }
    else
    {
        // No EXP outside of runs
        *expGain = 0;
    }
}

void Rogue_ModifyEVGain(int* multiplier)
{
    // We don't give out EVs normally instead we reward at end of trainer battles based on nature
    *multiplier = 0;
}

void Rogue_ModifyCatchRate(u16 species, u16 ball, u16* catchRate, u16* ballMultiplier)
{
    if(GetSafariZoneFlag() || Rogue_UseSafariBattle() || RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INSTANT_CAPTURE) || RogueTrial_IsCatchGuaranteed())
    {
        *ballMultiplier = 12345; // Masterball equiv
    }
    else if(Rogue_IsRunActive())
    {
        u16 startMultiplier = *ballMultiplier;
        u8 difficulty = Rogue_GetCurrentDifficulty();
        u8 wildEncounterIndex = GetWildEncounterIndexFor(species);
        u8 speciesCatchCount = 0;

        if(wildEncounterIndex != WILD_ENCOUNTER_TOTAL_CAPACITY)
            speciesCatchCount = gRogueRun.wildEncounters.catchCounts[wildEncounterIndex];

        if(gBattleTypeFlags & BATTLE_TYPE_ROAMER)
        {
            // Roamers hard to make early captures possible but not impossible
            difficulty = ROGUE_GYM_MID_DIFFICULTY;

            // If you've have a lot of encounters with the roamer, drop the difficulty
            if(gRogueRun.wildEncounters.roamer.encounerCount >= 4)
                difficulty = ROGUE_GYM_MID_DIFFICULTY - 1;
            else if(gRogueRun.wildEncounters.roamer.encounerCount >= 6)
                difficulty = ROGUE_GYM_MID_DIFFICULTY - 2;

#ifdef ROGUE_EXPANSION
            // Quick ball is specifically nerfed for roamers
            if(ball == ITEM_QUICK_BALL)
                difficulty += 2;
#endif
        }

        if(difficulty <= 1) // First 2 badges
        {
            *ballMultiplier = *ballMultiplier * 8;
        }
        else if(difficulty <= 2)
        {
            *ballMultiplier = *ballMultiplier * 4;
        }
        else if(difficulty <= ROGUE_GYM_MID_DIFFICULTY - 1)
        {
            *ballMultiplier = *ballMultiplier * 3;
        }
        else if(difficulty <= ROGUE_ELITE_START_DIFFICULTY - 1)
        {
            // Minimum of 2x multiplier whilst doing gyms?
            *ballMultiplier = *ballMultiplier * 2;
        }
        else
        {
            // Elite 4 back to normal catch rates
        }

        // Modify the catch rate based on how many times we've caught this mon
        if(speciesCatchCount > 2)
        {
            // Already caught a few, so use the base multiplier
            *ballMultiplier = startMultiplier;
        }
        else if(speciesCatchCount > 4)
        {
            // Now we want to discourage catching more mons
            *ballMultiplier = max(1, startMultiplier / 2);
        }

        // Apply charms
        {
            u32 perc;
            u16 rateInc = GetCharmValue(EFFECT_CATCH_RATE);
            u16 rateDec = GetCurseValue(EFFECT_CATCH_RATE);

            if(rateInc > rateDec)
            {
                rateInc = rateInc - rateDec;
                rateDec = 0;
            }
            else
            {
                rateDec = rateDec - rateInc;
                rateInc = 0;
            }

            perc = 100;

            if(rateInc > 0)
            {
                perc = 100 + rateInc;
            }
            else if(rateDec > 0)
            {
                perc = (rateDec > 100 ? 0 : 100 - rateDec);
            }

            if(perc != 100)
                *ballMultiplier = ((u32)*ballMultiplier * perc) / 100;
        }

        // After we've caught a few remove the catch rate buff
        if(speciesCatchCount <= 3)
        {
            // Equiv to Snorlax
            if(*catchRate < 25)
                *catchRate = 25;
        }
    }
}

static void ModifyExistingMonToCustomMon(u32 customMonId, struct Pokemon* mon)
{
    u32 species = GetMonData(mon, MON_DATA_SPECIES);
    u32 level = GetMonData(mon, MON_DATA_LEVEL);
    u32 abilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM);
    u8 nature = GetNature(mon);
    u32 hpIV = GetMonData(mon, MON_DATA_HP_IV);
    u32 atkIV = GetMonData(mon, MON_DATA_ATK_IV);
    u32 defIV = GetMonData(mon, MON_DATA_DEF_IV);
    u32 speedIV = GetMonData(mon, MON_DATA_SPEED_IV);
    u32 spAtkIV = GetMonData(mon, MON_DATA_SPATK_IV);
    u32 spDefIV = GetMonData(mon, MON_DATA_SPDEF_IV);
    u32 isShiny = GetMonData(mon, MON_DATA_IS_SHINY);
    u32 isGenderFlag = GetMonData(mon, MON_DATA_GENDER_FLAG);
    u32 metLocation = GetMonData(mon, MON_DATA_MET_LOCATION);
    u32 metLevel = GetMonData(mon, MON_DATA_MET_LEVEL);
    u8 text[POKEMON_NAME_LENGTH + 1];
    bool8 hasCustomNickname = FALSE;
    bool8 preserveNativeAbility = RogueGift_GetCustomMonAbilityCount(customMonId) == 0;

    GetMonData(mon, MON_DATA_NICKNAME, text);

    if(StringCompareN(text, RoguePokedex_GetSpeciesName(species), POKEMON_NAME_LENGTH) != 0)
        hasCustomNickname = TRUE;

    RogueGift_CreateMon(customMonId, mon, species, level, 0);

    SetMonData(mon, MON_DATA_HP_IV, &hpIV);
    SetMonData(mon, MON_DATA_ATK_IV, &atkIV);
    SetMonData(mon, MON_DATA_DEF_IV, &defIV);
    SetMonData(mon, MON_DATA_SPEED_IV, &speedIV);
    SetMonData(mon, MON_DATA_SPATK_IV, &spAtkIV);
    SetMonData(mon, MON_DATA_SPDEF_IV, &spDefIV);
    SetNature(mon, nature);
    SetMonData(mon, MON_DATA_IS_SHINY, &isShiny);
    SetMonData(mon, MON_DATA_GENDER_FLAG, &isGenderFlag);
    SetMonData(mon, MON_DATA_MET_LOCATION, &metLocation);
    SetMonData(mon, MON_DATA_MET_LEVEL, &metLevel);

    // Reapplying custom data must not reroll a native ability selected when the
    // Pokemon was first created. Explicit custom abilities remain authoritative.
    if(preserveNativeAbility)
        SetMonData(mon, MON_DATA_ABILITY_NUM, &abilityNum);

    if(hasCustomNickname)
        SetMonData(mon, MON_DATA_NICKNAME, text);
}

void Rogue_ApplyCustomMonIdToMon(u32 customMonId, struct Pokemon* mon)
{
    ModifyExistingMonToCustomMon(customMonId, mon);
}

void Rogue_ModifyCaughtMon(struct Pokemon *mon)
{
    bool8 caughtInPremierBall = GetMonData(mon, MON_DATA_POKEBALL) == ITEM_PREMIER_BALL;
    bool8 caughtInFriendBall = GetMonData(mon, MON_DATA_POKEBALL) == ITEM_FRIEND_BALL;
    bool8 caughtInDreamBall = GetMonData(mon, MON_DATA_POKEBALL) == ITEM_DREAM_BALL;
    bool8 caughtInCherishBall = GetMonData(mon, MON_DATA_POKEBALL) == ITEM_CHERISH_BALL;

    if(Rogue_IsRunActive())
    {
        u16 hp = GetMonData(mon, MON_DATA_HP);
        u16 maxHp = GetMonData(mon, MON_DATA_MAX_HP);
        u32 statusAilment = 0; // STATUS1_NONE

        if(gRogueLocal.wildBattleCustomMonId != 0)
        {
            ModifyExistingMonToCustomMon(gRogueLocal.wildBattleCustomMonId, mon);
            RogueGift_RemoveDynamicCustomMon(gRogueLocal.wildBattleCustomMonId);
            gRogueLocal.wildBattleCustomMonId = 0;
        }

        hp = max(maxHp / 2, hp);

        // Heal up to 1/2 health and remove status effect
        SetMonData(mon, MON_DATA_HP, &hp);
        SetMonData(mon, MON_DATA_STATUS, &statusAilment);

        // Apply charms to IVs
        {
            u16 i;
            u16 value;

            u16 ivInc = GetCharmValue(EFFECT_WILD_IV_RATE);
            u16 ivDec = GetCurseValue(EFFECT_WILD_IV_RATE);

            if(ivInc > ivDec)
            {
                ivInc = ivInc - ivDec;
                ivDec = 0;
            }
            else
            {
                ivDec = ivDec - ivInc;
                ivInc = 0;
            }

            ivInc = min(ivInc, 31);
            ivDec = min(ivDec, 31);

            for(i = 0; i < 6; ++i)
            {
                value = GetMonData(mon, MON_DATA_HP_IV + i);

                if(ivInc != 0)
                {
                    value = max(ivInc, value);
                }

                if(ivDec != 0)
                {
                    value = min(31 - ivDec, value);
                }

                SetMonData(mon, MON_DATA_HP_IV + i, &value);
            }
        }

        // Increment catch counter for in route mons
        {
            u16 species = GetMonData(mon, MON_DATA_SPECIES);
            u8 index = GetWildEncounterIndexFor(species);

            if(index != WILD_ENCOUNTER_TOTAL_CAPACITY && gRogueRun.wildEncounters.catchCounts[index] != 255)
            {
                ++gRogueRun.wildEncounters.catchCounts[index];
            }
        }

        if(IsCurseActive(EFFECT_SNAG_TRAINER_MON) && FlagGet(FLAG_ROGUE_IN_SNAG_BATTLE))
        {
            mon->rogueExtraData.isSafariIllegal = TRUE;
            SetMonData(mon, MON_DATA_OT_NAME, Rogue_GetTrainerName(gTrainerBattleOpponent_A));
        }

        // Make sure we log if we end up replacing a fainted mon
        CheckAndNotifyForFaintedMons();
    }

    if(caughtInFriendBall)
    {
        u8 friendship = MAX_FRIENDSHIP;

        SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
    }

    if(caughtInPremierBall)
    {
        static const u8 sNeutralNatures[] =
        {
            NATURE_HARDY,
            NATURE_DOCILE,
            NATURE_SERIOUS,
            NATURE_BASHFUL,
            NATURE_QUIRKY,
        };

        SetNature(mon, sNeutralNatures[Random() % ARRAY_COUNT(sNeutralNatures)]);
    }

    if(caughtInDreamBall)
    {
        u16 species = GetMonData(mon, MON_DATA_SPECIES);
        u32 otId = GetMonData(mon, MON_DATA_OT_ID);
        u32 customMonId = RogueGift_GetCustomMonIdBySpecies(species, otId);
        u16 hiddenAbility = gSpeciesInfo[species].abilities[NUM_NORMAL_ABILITY_SLOTS];

        if(customMonId != CUSTOM_MON_NONE && RogueGift_GetCustomMonAbilityCount(customMonId) != 0)
            hiddenAbility = RogueGift_GetCustomMonAbility(customMonId, NUM_NORMAL_ABILITY_SLOTS);

        if(hiddenAbility != ABILITY_NONE)
        {
            u8 abilityNum = NUM_NORMAL_ABILITY_SLOTS;

            SetMonData(mon, MON_DATA_ABILITY_NUM, &abilityNum);
        }
    }

    if(caughtInCherishBall)
    {
        u8 i;
        u8 statIds[NUM_STATS] = {STAT_HP, STAT_ATK, STAT_DEF, STAT_SPEED, STAT_SPATK, STAT_SPDEF};
        u16 perfectIV = MAX_PER_STAT_IVS;

        // Select three distinct stats and guarantee that each has a perfect IV.
        for(i = 0; i < 3; ++i)
        {
            u8 selectedIndex = i + (Random() % (NUM_STATS - i));
            u8 selectedStat = statIds[selectedIndex];

            statIds[selectedIndex] = statIds[i];
            statIds[i] = selectedStat;
            SetMonData(mon, MON_DATA_HP_IV + selectedStat, &perfectIV);
        }
    }
}

void Rogue_OnAcceptCaughtMon(struct Pokemon *mon)
{
    if(Rogue_IsRunActive())
    {
        bool8 isLegendary;
        bool8 isUnique;
        u16 species = GetMonData(mon, MON_DATA_SPECIES);

        RogueTrial_OnCaughtMon();
        VarSet(VAR_ROGUE_TOTAL_RUN_CATCHES, VarGet(VAR_ROGUE_TOTAL_RUN_CATCHES) + 1);

        isLegendary = RoguePokedex_IsSpeciesLegendary(species);
        isUnique = RogueGift_GetCustomMonId(mon) != CUSTOM_MON_NONE;

        // Quest notifies
        if(isLegendary)
            RogueQuest_OnTrigger(QUEST_TRIGGER_MON_LEGEND_CAUGHT);

        if(isUnique)
            RogueQuest_OnTrigger(QUEST_TRIGGER_MON_UNIQUE_CAUGHT);

        if(isLegendary && isUnique)
            RogueQuest_OnTrigger(QUEST_TRIGGER_MON_UNIQUE_LEGEND_CAUGHT);

        if(IsMonShiny(mon))
            RogueQuest_OnTrigger(QUEST_TRIGGER_MON_SHINY_CAUGHT);
        else
            RogueQuest_OnTrigger(QUEST_TRIGGER_MON_NON_SHINY_CAUGHT);
    }
}

void Rogue_ModifyEggMon(struct Pokemon *mon)
{
    u32 eggCyclesLeft = 0;
    SetMonData(mon, MON_DATA_FRIENDSHIP, &eggCyclesLeft);
}

void Rogue_DiscardedCaughtMon(struct Pokemon *mon)
{
    if(Rogue_IsRunActive())
    {
        // Don't track discarded mons for catching contest unless they're shiny
        if(Rogue_IsCatchingContestActive() && !IsMonShiny(mon))
            return;

        RogueSafari_PushLowPriorityMon(mon);
    }
}

u16 Rogue_ModifyItemPickupAmount(u16 itemId, u16 amount)
{
    if(Rogue_IsRunActive())
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE
         || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_TEAM_HIDEOUT
         || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_RESTSTOP)
        {
            u8 pocket = ItemId_GetPocket(itemId);
            amount = 1;

            switch (pocket)
            {
            case POCKET_BERRIES:
                if(RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD2))
                    amount = 9;
                else if(RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD1))
                    amount = 7;
                else if(RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD0))
                    amount = 5;
                else
                    amount = 3;
                break;

            case POCKET_ITEMS:
            case POCKET_MEDICINE:
                amount = 3;
                break;

            case POCKET_TM_HM:
                //if(itemId >= ITEM_TR01 && itemId <= ITEM_TR50)
                //    amount = 3;
                //else
                //    amount = 1;
                amount = 1;
                break;

            case POCKET_POKE_BALLS:
                amount = 3;
                break;
            }

            switch (itemId)
            {
            case ITEM_MASTER_BALL:
            case ITEM_ESCAPE_ROPE:
                amount = 1;
                break;

            case ITEM_RARE_CANDY:
                amount = 10;
                break;

#ifdef ROGUE_EXPANSION
            case ITEM_ABILITY_CAPSULE:
            case ITEM_ABILITY_PATCH:
                amount = 1;
                break;
#endif
            }

            if(Rogue_IsEvolutionItem(itemId) || Rogue_IsFormItem(itemId))
                amount = 1;

#ifdef ROGUE_EXPANSION
            if(itemId >= ITEM_LONELY_MINT && itemId <= ITEM_SERIOUS_MINT)
                amount = 1;

            if(itemId >= ITEM_RED_NECTAR && itemId <= ITEM_PURPLE_NECTAR)
                amount = 1;

            if((itemId >= ITEM_BUG_TERA_SHARD && itemId <= ITEM_WATER_TERA_SHARD) || itemId == ITEM_STELLAR_TERA_SHARD)
                amount = 1;
#endif
        }
    }
    else
    {
        u8 pocket = ItemId_GetPocket(itemId);

        switch (pocket)
        {
        case POCKET_BERRIES:
            if(RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD2))
                amount = 9;
            else if(RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD1))
                amount = 7;
            else if(RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_HIGHER_YEILD0))
                amount = 5;
            else
                amount = 3;
            break;
        }
    }

    return amount;
}

bool8 Rogue_IsReusableItem(u16 itemId)
{
    if((itemId >= ITEM_TM01 && itemId <= ITEM_HM08)
     || (itemId >= ITEM_TR01 && itemId <= ITEM_TR50))
        return TRUE;

#ifdef ROGUE_EXPANSION
    if((itemId >= ITEM_BUG_TERA_SHARD && itemId <= ITEM_WATER_TERA_SHARD)
     || itemId == ITEM_STELLAR_TERA_SHARD)
        return TRUE;
#endif

    return FALSE;
}

#define PERCENT_FEMALE(percent) min(254, ((percent * 255) / 100))

u8 Rogue_ModifyGenderRatio(u8 genderRatio)
{
    if(genderRatio == MON_MALE || genderRatio == MON_FEMALE || genderRatio == MON_GENDERLESS)
    {
        return genderRatio;
    }

    return PERCENT_FEMALE(50.0);
}

#undef PERCENT_FEMALE

const void* Rogue_ModifyPaletteLoad(const void* input)
{
    if(input == &gObjectEventPal_PlayerPlaceholder[0])
    {
        return RoguePlayer_GetOverworldPalette();
    }

    if(input == &gObjectEventPal_NetPlayerPlaceholder[0])
    {
        return RogueNetPlayer_GetOverworldPalette();
    }

    //if(input == &gObjectEventPal_FollowMon0[0])
    //    return FollowMon_GetGraphicsForPalSlot(0);
//
    //if(input == &gObjectEventPal_FollowMon1[0])
    //    return FollowMon_GetGraphicsForPalSlot(1);
//
    //if(input == &gObjectEventPal_FollowMon2[0])
    //    return FollowMon_GetGraphicsForPalSlot(2);
//
    //if(input == &gObjectEventPal_FollowMon3[0])
    //    return FollowMon_GetGraphicsForPalSlot(3);
//
    //if(input == &gObjectEventPal_FollowMon4[0])
    //    return FollowMon_GetGraphicsForPalSlot(4);

    return input;
}

bool8 Rogue_ModifyObjectPaletteSlot(u16 graphicsId, u8* palSlot)
{
    if(graphicsId >= OBJ_EVENT_GFX_NET_PLAYER_FIRST && graphicsId <= OBJ_EVENT_GFX_NET_PLAYER_LAST)
    {
        *palSlot = 8;
        PatchObjectPalette(0x119C, *palSlot); // OBJ_EVENT_PAL_TAG_NET_PLAYER - todo should def pull this out correctly
        return TRUE;
    }

    if(graphicsId == OBJ_EVENT_GFX_FOLLOW_MON_PARTNER)
    {
        *palSlot = 1;
        LoadPalette(FollowMon_GetGraphicsForPalSlot(0), OBJ_PLTT_ID(*palSlot), PLTT_SIZE_4BPP);
        Rogue_ModifyOverworldPalette(OBJ_PLTT_ID(*palSlot), PLTT_SIZE_4BPP);
        return TRUE;
    }

    if(graphicsId >= OBJ_EVENT_GFX_FOLLOW_MON_0 && graphicsId <= OBJ_EVENT_GFX_FOLLOW_MON_3)
    {
        *palSlot = 6 + (graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0);
        LoadPalette(FollowMon_GetGraphicsForPalSlot(1 + graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0), OBJ_PLTT_ID(*palSlot), PLTT_SIZE_4BPP);
        Rogue_ModifyOverworldPalette(OBJ_PLTT_ID(*palSlot), PLTT_SIZE_4BPP);
        return TRUE;
    }

    if(graphicsId == OBJ_EVENT_GFX_FOLLOW_MON_4)
    {
        *palSlot = 10;
        LoadPalette(FollowMon_GetGraphicsForPalSlot(1 + graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0), OBJ_PLTT_ID(*palSlot), PLTT_SIZE_4BPP);
        Rogue_ModifyOverworldPalette(OBJ_PLTT_ID(*palSlot), PLTT_SIZE_4BPP);
        return TRUE;
    }

    if(graphicsId == OBJ_EVENT_GFX_FOLLOW_MON_5)
    {
        *palSlot = 1;
        LoadPalette(FollowMon_GetGraphicsForPalSlot(1 + graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0), OBJ_PLTT_ID(*palSlot), PLTT_SIZE_4BPP);
        Rogue_ModifyOverworldPalette(OBJ_PLTT_ID(*palSlot), PLTT_SIZE_4BPP);
        return TRUE;
    }

    return FALSE;
}

const u32 *Rogue_ModifyMonCompressedPalette(const u32 *compressedPal, u16 species, u8 gender, bool8 isShiny, u32 otId)
{
    u32 customMonId = RogueGift_GetCustomMonIdBySpecies(species, otId);

    if(customMonId != 0)
    {
        u16 const *inputPal = (u16 const*)&gPaletteDecompressionBuffer[0];
        LZ77UnCompWram(compressedPal, gPaletteDecompressionBuffer);

        if(RogueGift_TryApplyPaletteModify(customMonId, isShiny, inputPal, NULL, gRogueLocal.uniqueMonPalette.tempPalette))
            return gMonPalette_FrontPlaceholder;
    }

    return compressedPal;
}

bool8 Rogue_ModifyPaletteDecompress(const u32* input, void* buffer)
{
    const u16* overrideBuffer = NULL;

    if(input == gTrainerPalette_PlayerFrontPlaceholder)
    {
        overrideBuffer = RoguePlayer_GetTrainerFrontPalette();
    }

    if(input == gTrainerPalette_PlayerBackPlaceholder)
    {
        overrideBuffer = RoguePlayer_GetTrainerBackPalette();
    }

    if(input == gMonPalette_FrontPlaceholder)
    {
        overrideBuffer = gRogueLocal.uniqueMonPalette.tempPalette;
    }

    if(overrideBuffer != NULL)
    {
        // Copy over from override buffer into actual output (only really needed if buffer isn't gDecompressBuffer)
        if(overrideBuffer != buffer)
        {
            u8 i;
            u16* writeBuffer = (u16*)buffer;

            for(i = 0; i < 16; ++i) // assume 16 palette slots (We currently don't have any use cases outside of this anyway)
                writeBuffer[i] = overrideBuffer[i];
        }

        return TRUE;
    }

    return FALSE;
}

void Rogue_ModifyOverworldPalette(u16 offset, u16 count)
{
    RogueToD_ModifyOverworldPalette(offset, count);
}

void Rogue_ModifyBattlePalette(u16 offset, u16 count)
{
    RogueToD_ModifyBattlePalette(offset, count);
}

extern const u8 gPlaceholder_Gym_PreBattleOpenning[];
extern const u8 gPlaceholder_Gym_PreBattleTaunt[];
extern const u8 gPlaceholder_Gym_PostBattleTaunt[];
extern const u8 gPlaceholder_Gym_PostBattleCloser[];

extern const u8 gPlaceholder_Trainer_PreBattleOpenning[];
extern const u8 gPlaceholder_Trainer_PreBattleTaunt[];
extern const u8 gPlaceholder_Trainer_PostBattleTaunt[];
extern const u8 gPlaceholder_Trainer_PostBattleCloser[];

const u8* Rogue_ModifyFieldMessage(const u8* str)
{
    const u8* overrideStr = NULL;

    if(Rogue_IsRunActive())
    {
        if(Rogue_IsVictoryLapActive())
        {
            u16 trainerNum = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA);

            if(str == gPlaceholder_Gym_PreBattleOpenning)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_PRE_BATTLE_OPENNING);
            else if(str == gPlaceholder_Gym_PreBattleTaunt)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_PRE_BATTLE_TAUNT);
            else if(str == gPlaceholder_Gym_PostBattleTaunt)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_POST_BATTLE_TAUNT);
            else if(str == gPlaceholder_Gym_PostBattleCloser)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_POST_BATTLE_CLOSER);
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS)
        {
            u16 trainerNum;

            // We don't technically have a new room so force the trainer num here
            if(Rogue_UseFinalQuestEffects() && Rogue_GetCurrentDifficulty() >= ROGUE_FINAL_CHAMP_DIFFICULTY)
                trainerNum = gRogueRun.bossTrainerNums[ROGUE_FINAL_CHAMP_DIFFICULTY];
            else
                trainerNum = gRogueAdvPath.currentRoomParams.perType.boss.trainerNum;

            if(str == gPlaceholder_Gym_PreBattleOpenning)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_PRE_BATTLE_OPENNING);
            else if(str == gPlaceholder_Gym_PreBattleTaunt)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_PRE_BATTLE_TAUNT);
            else if(str == gPlaceholder_Gym_PostBattleTaunt)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_POST_BATTLE_TAUNT);
            else if(str == gPlaceholder_Gym_PostBattleCloser)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_POST_BATTLE_CLOSER);
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_MINIBOSS)
        {
            u16 trainerNum = gRogueAdvPath.currentRoomParams.perType.miniboss.trainerNum;

            if(str == gPlaceholder_Gym_PreBattleOpenning)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_PRE_BATTLE_OPENNING);
            else if(str == gPlaceholder_Gym_PreBattleTaunt)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_PRE_BATTLE_TAUNT);
            else if(str == gPlaceholder_Gym_PostBattleTaunt)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_POST_BATTLE_TAUNT);
            else if(str == gPlaceholder_Gym_PostBattleCloser)
                overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_POST_BATTLE_CLOSER);
        }

        // Overworld trainer messages
        if(str == gPlaceholder_Trainer_PreBattleOpenning)
        {
            u16 trainerNum = Rogue_GetTrainerNumFromLastInteracted();
            overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_PRE_BATTLE_OPENNING);
        }
        else if(str == gPlaceholder_Trainer_PreBattleTaunt)
        {
            u16 trainerNum = Rogue_GetTrainerNumFromLastInteracted();
            overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_PRE_BATTLE_TAUNT);
        }
        else if(str == gPlaceholder_Trainer_PostBattleTaunt)
        {
            u16 trainerNum = Rogue_GetTrainerNumFromLastInteracted();
            overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_POST_BATTLE_TAUNT);
        }
        else if(str == gPlaceholder_Trainer_PostBattleCloser)
        {
            u16 trainerNum = Rogue_GetTrainerNumFromLastInteracted();
            overrideStr = Rogue_GetTrainerString(trainerNum, TRAINER_STRING_POST_BATTLE_CLOSER);
        }
    }

    return overrideStr != NULL ? overrideStr : str;
}

extern const u8* const gBattleStringsTable[];
extern const u8 sText_Trainer1SentOutPkmn2[];

static const u8 sText_FinalQuestFinalMonSendOut[] = _("Wahey!\nI'm not through yet!");

const u8* Rogue_ModifyBattleMessage(const u8* str)
{
    const u8* overrideStr = NULL;

    if(Rogue_ShouldSkipAssignNicknameYesNoMessage())
    {
        // Don't display "Would you like to nickname" msg
        if(str == gBattleStringsTable[STRINGID_GIVENICKNAMECAPTURED - BATTLESTRINGS_TABLE_START])
            overrideStr = gText_EmptyString2;
    }

    if(Rogue_IsFinalQuestFinalBoss() && HasFinalQuestBossAppliedSwap())
    {
        if(str == sText_Trainer1SentOutPkmn2)
            overrideStr = sText_FinalQuestFinalMonSendOut;
    }

    return overrideStr != NULL ? overrideStr : str;
}

void Rogue_ModifyBattleMon(u8 monId, struct BattlePokemon* battleMon, bool8 isPlayer)
{
    // Note monId is a relic and is always 0

    if(isPlayer)
    {
        if(IsCurseActive(EFFECT_TORMENT_STATUS))
            battleMon->status2 |= STATUS2_TORMENT;
    }
    else
    {
        if(IsCharmActive(EFFECT_TORMENT_STATUS))
            battleMon->status2 |= STATUS2_TORMENT;
    }
}

// Base on Vanilla calcs
static u32 CalculateBattleWinnings(u16 trainerNum)
{
    u32 i = 0;
    u32 lastMonLevel = 0;
    u32 moneyReward;

    {
        struct Trainer trainer;

        Rogue_ModifyTrainer(trainerNum, &trainer);

        // Base calcs of player current level cap
        lastMonLevel = Rogue_CalculatePlayerMonLvl();


        for (; gTrainerMoneyTable[i].classId != 0xFF; i++)
        {
            if (gTrainerMoneyTable[i].classId == trainer.trainerClass)
                break;
        }

        // Default multiplier is 4
        if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
            moneyReward = 6 * lastMonLevel * gTrainerMoneyTable[i].value;
        else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
            moneyReward = 6 * lastMonLevel * 2 * gTrainerMoneyTable[i].value;
        else
            moneyReward = 6 * lastMonLevel * gTrainerMoneyTable[i].value;

        if(Rogue_IsRunActive() && gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BATTLE_TOWER)
        {
            // 66% boost
            moneyReward = (moneyReward * 5) / 3;
        }
    }

    return moneyReward;
}

void Rogue_ModifyBattleWinnings(u16 trainerNum, u32* money)
{
    if(Rogue_IsRunActive())
    {
        // Increase by 20%
        *money = (CalculateBattleWinnings(trainerNum) * 120) / 100;

        if(Rogue_IsExpTrainer(trainerNum) || Rogue_IsBattleSimTrainer(trainerNum) || Rogue_IsVictoryLapActive())
        {
            *money = 0;
            return;
        }

        switch (gRogueAdvPath.currentRoomType)
        {
        case ADVPATH_ROOM_BOSS:
            {
                u8 difficulty = Rogue_GetCurrentDifficulty();
                *money = (difficulty + 1) * 1000;
            }
            break;

        case ADVPATH_ROOM_TEAM_HIDEOUT:
            if(Rogue_IsKeyTrainer(trainerNum))
            {
                u8 difficulty = Rogue_GetCurrentDifficulty();
                *money = (difficulty + 1) * 1000;
            }
            else
            {
                // Give slightly more money here
                *money *= 2;
            }
            break;
        }

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD)
            *money = ((*money) * 125) / 100;

        //if(FlagGet(FLAG_ROGUE_HARD_ITEMS))
        //{
        //    if(difficulty <= 11)
        //    {
        //        if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_TOUGH) // Hard
        //            *money = *money / 2;
        //        else
        //            *money = *money / 3;
        //    }
        //    else
        //    {
        //        // Kinder but not by much ;)
        //        if(difficultyModifier != ADVPATH_SUBROOM_ROUTE_TOUGH) // !Hard
        //            *money = *money / 2;
        //    }
        //}
        //else if(!FlagGet(FLAG_ROGUE_EASY_ITEMS))
        //{
        //    if(difficulty <= 11)
        //    {
        //        if(difficultyModifier != ADVPATH_SUBROOM_ROUTE_TOUGH) // !Hard
        //            *money = *money / 2;
        //    }
        //}

        // Snap/Floor to multiple of ten
        if(*money > 100)
        {
            *money /= 10;
            *money *= 10;
        }
        else if(*money != 0)
        {
            *money = 100;
        }
    }
}

void Rogue_ModifyBattleWaitTime(u16* waitTime, bool8 awaitingMessage)
{
    u8 difficulty = Rogue_IsRunActive() ? Rogue_GetCurrentDifficulty() : 0;

    // We won't modify absolute wait flags
    if(*waitTime & B_WAIT_TIME_ABSOLUTE)
    {
        *waitTime &= ~B_WAIT_TIME_ABSOLUTE;
    }
    else
    {
        if(Rogue_FastBattleAnims())
        {
            *waitTime = awaitingMessage ? 8 : 0;
        }
        else if(difficulty < ROGUE_FINAL_CHAMP_DIFFICULTY) // Go at default speed for final fight
        {
            if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && Rogue_IsKeyTrainer(gTrainerBattleOpponent_A))
            {
                // Still run faster and default game because it's way too slow :(
                if(difficulty < ROGUE_ELITE_START_DIFFICULTY)
                    *waitTime = *waitTime / 4;
                else
                    *waitTime = *waitTime / 2;
            }
            else
                // Go faster, but not quite gym leader slow
                *waitTime = *waitTime / 6;
        }

        if(!Rogue_GetBattleAnimsEnabled())
        {
            // If we don't have anims on wait message for at least a little bit
            *waitTime = max(4, *waitTime);
        }
    }

    // Now apply speed scale
    //*waitTime = max(1, *waitTime / Rogue_GetBattleSpeedScale(FALSE));
}

s16 Rogue_ModifyBattleSlideAnim(s16 rate)
{
    u8 difficulty = Rogue_IsRunActive() ? Rogue_GetCurrentDifficulty() : 0;

    if(Rogue_FastBattleAnims())
    {
        if(rate < 0)
            return rate * 2 - 1;
        else
            return rate * 2 + 1;
    }
    else if(difficulty < ROGUE_FINAL_CHAMP_DIFFICULTY)
    {
        // Go at default speed for final fight
        return rate * 2;
    }

    // Still run faster and default game because it's way too slow :(
    return rate;
}

bool8 Rogue_UseFastLevelUpAnim()
{
    return gSaveBlock2Ptr->optionsTextSpeed != OPTIONS_TEXT_SPEED_SLOW;
}

const u8* Rogue_ModifyOverworldInteractionScript(struct MapPosition *position, u16 metatileBehavior, u8 direction, u8 const* script)
{
    u16 specialState = VarGet(VAR_ROGUE_SPECIAL_MODE);

    if(specialState == ROGUE_SPECIAL_MODE_DECORATING)
    {
        script = RogueHub_GetDecoratingScriptFor(gMapHeader.mapLayoutId, position, metatileBehavior, direction, script);
    }

    return script;
}

u16 Rogue_ModifyOverworldMapWeather(u16 weather)
{
    if(gMapHeader.mapType != MAP_TYPE_INDOOR && gMapHeader.mapType != MAP_TYPE_UNDERGROUND)
    {
        if(Rogue_IsRunActive())
        {
            switch (gRogueAdvPath.currentRoomType)
            {
            case ADVPATH_ROOM_ROUTE:
                return VarGet(VAR_ROGUE_DESIRED_WEATHER);
            }
        }
        else if(VarGet(VAR_ROGUE_INTRO_STATE) < ROGUE_INTRO_STATE_COMPLETE)
        {
            // Don't have any special weather until player has completed tutorial
            return WEATHER_NONE;
        }
        else
        {
            u16 weatherState = RogueHub_GetWeatherState();

            switch(RogueToD_GetSeason())
            {
                case SEASON_SPRING:
                {
                    switch (weatherState % 5)
                    {
                    case 0:
                        return WEATHER_RAIN;
                    case 1:
                        return WEATHER_RAIN_THUNDERSTORM;
                    case 2:
                        return WEATHER_MISTY_FOG;

                    case 3:
                    case 4:
                        return WEATHER_NONE;
                    }
                }
                case SEASON_SUMMER:
                {
                    switch (weatherState % 5)
                    {
                    case 0:
                        return WEATHER_RAIN_THUNDERSTORM;

                    case 1:
                    case 2:
                    case 3:
                    case 4:
                        return WEATHER_NONE;
                    }
                }
                case SEASON_AUTUMN:
                {
                    switch (weatherState % 6)
                    {
                    case 0:
                        return WEATHER_RAIN;
                    case 1:
                        return WEATHER_RAIN_THUNDERSTORM;
                    case 2:
                    case 3:
                        return WEATHER_LEAVES;

                    case 4:
                    case 5:
                        return WEATHER_NONE;
                    }
                }
                case SEASON_WINTER:
                {
                    switch (weatherState % 6)
                    {
                    case 0:
                    case 1:
                    case 2:
                        return WEATHER_SNOW;

                    case 3:
                        return WEATHER_SHADE;

                    case 4:
                    case 5:
                        return WEATHER_NONE;
                    }
                }
                    return WEATHER_SNOW;
            }
        }
    }

    return weather;
}

const struct Tileset * Rogue_ModifyOverworldTileset(const struct Tileset * tileset)
{
    if(Rogue_IsRunActive())
    {
        return tileset;
    }
    else
    {
        return RogueHub_ModifyOverworldTileset(tileset);
    }
}

bool8 Rogue_CanRenameMon(struct Pokemon* mon)
{
    u32 customMonId;
    u32 otId = GetMonData(mon, MON_DATA_OT_ID);

    if(!IsOtherTrainer(otId))
        return TRUE;

    customMonId = RogueGift_GetCustomMonId(mon);

    if(customMonId)
        return RogueGift_CanRenameCustomMon(customMonId);

    return FALSE;
}

u8 SpeciesToGen(u16 species)
{
    if(species >= SPECIES_BULBASAUR && species <= SPECIES_MEW)
        return 1;
    if(species >= SPECIES_CHIKORITA && species <= SPECIES_CELEBI)
        return 2;
    if(species >= SPECIES_TREECKO && species <= SPECIES_DEOXYS)
        return 3;
#ifdef ROGUE_EXPANSION
    if(species >= SPECIES_TURTWIG && species <= SPECIES_ARCEUS)
        return 4;
    if(species >= SPECIES_VICTINI && species <= SPECIES_GENESECT)
        return 5;
    if(species >= SPECIES_CHESPIN && species <= SPECIES_VOLCANION)
        return 6;
    if(species >= SPECIES_ROWLET && species <= SPECIES_MELMETAL)
        return 7;
    if(species >= SPECIES_GROOKEY && species <= SPECIES_CALYREX)
        return 8;
    // Hisui is classes as gen8
    if(species >= SPECIES_WYRDEER && species <= SPECIES_ENAMORUS)
        return 8;

    if(species >= SPECIES_SPRIGATITO && species <= SPECIES_PECHARUNT)
        return 9;

    if(species == SPECIES_FLOETTE_ETERNAL_FLOWER)
        return 9;

    if(species >= SPECIES_RATTATA_ALOLAN && species <= SPECIES_MAROWAK_ALOLAN)
        return 7;
    if(species >= SPECIES_MEOWTH_GALARIAN && species <= SPECIES_STUNFISK_GALARIAN)
        return 8;

    // Hisui is classes as gen8
    if(species >= SPECIES_GROWLITHE_HISUIAN && species <= SPECIES_DECIDUEYE_HISUIAN)
        return 8;

    if(species >= SPECIES_BURMY_SANDY_CLOAK && species <= SPECIES_ARCEUS_FAIRY)
        return 4;

    // Just treat megas as gen 1 as they are controlled by a different mechanism
    if(species >= SPECIES_VENUSAUR_MEGA && species <= SPECIES_GROUDON_PRIMAL)
        return 1;
    if(species >= SPECIES_VENUSAUR_GIGANTAMAX && species <= SPECIES_URSHIFU_RAPID_STRIKE_STYLE_GIGANTAMAX)
        return 1;

    switch(species)
    {
        case SPECIES_GIRATINA_ORIGIN:
            return 4;

        case SPECIES_PALKIA_ORIGIN:
        case SPECIES_DIALGA_ORIGIN:
            return 8;

        case SPECIES_KYUREM_WHITE:
        case SPECIES_KYUREM_BLACK:
            return 5;

        //case SPECIES_ZYGARDE_COMPLETE:
        //    return 6;

        case SPECIES_NECROZMA_DUSK_MANE:
        case SPECIES_NECROZMA_DAWN_WINGS:
        case SPECIES_NECROZMA_ULTRA:
            return 7;

        case SPECIES_ZACIAN_CROWNED_SWORD:
        case SPECIES_ZAMAZENTA_CROWNED_SHIELD:
        case SPECIES_ETERNATUS_ETERNAMAX:
        case SPECIES_URSHIFU_RAPID_STRIKE_STYLE:
        case SPECIES_ZARUDE_DADA:
        case SPECIES_CALYREX_ICE_RIDER:
        case SPECIES_CALYREX_SHADOW_RIDER:
            return 8;

        case SPECIES_ENAMORUS_THERIAN:
            return 8;
    }

    // Alternate forms
    switch(species)
    {
        case SPECIES_MEOWSTIC_FEMALE:
            return 7;

        case SPECIES_INDEEDEE_FEMALE:
            return 8;
    }

    if(species >= SPECIES_LYCANROC_MIDNIGHT && species <= SPECIES_LYCANROC_DUSK)
        return 7;

    if(species >= SPECIES_TOXTRICITY_LOW_KEY && species <= SPECIES_ALCREMIE_STRAWBERRY_RAINBOW_SWIRL)
        return 8;

    if(species >= SPECIES_ALCREMIE_BERRY_VANILLA_CREAM && species <= SPECIES_ALCREMIE_RIBBON_RAINBOW_SWIRL)
        return 8;
#endif

    return 0;
}

static u8 ItemToGen(u16 item)
{
    if(!Rogue_IsRunActive() && FlagGet(FLAG_ROGUE_MET_POKABBIE))
    {
        // We want all items to appear in the hub, so long as we've unlocked the expanded mode
        return 1;
    }

#ifdef ROGUE_EXPANSION
    if(item >= ITEM_FLAME_PLATE && item <= ITEM_PIXIE_PLATE)
        return 4;

    if(item >= ITEM_DOUSE_DRIVE && item <= ITEM_CHILL_DRIVE)
        return 5;

    if(item >= ITEM_FIRE_MEMORY && item <= ITEM_FAIRY_MEMORY)
        return 7;

    if(item >= ITEM_RED_NECTAR && item <= ITEM_PURPLE_NECTAR)
        return 7;

    // Mega stones are gonna be gen'd by the mons as we already feature toggle them based on key items
    if(item >= ITEM_VENUSAURITE && item <= ITEM_MEWTWONITE_Y)
        return 1;
    if(item >= ITEM_AMPHAROSITE && item <= ITEM_TYRANITARITE)
        return 2;
    if(item >= ITEM_SCEPTILITE && item <= ITEM_LATIOSITE)
        return 3;
    if(item >= ITEM_LOPUNNITE && item <= ITEM_GALLADITE)
        return 4;
    if(item == ITEM_AUDINITE)
        return 5;
    if(item == ITEM_DIANCITE)
        return 6;

    // Z-crystals are key item feature toggled so leave them as always on except to for mon specific ones
    if(item >= ITEM_NORMALIUM_Z && item <= ITEM_MEWNIUM_Z)
        return 1;
    if(item >= ITEM_DECIDIUM_Z && item <= ITEM_ULTRANECROZIUM_Z)
        return 7;

    switch(item)
    {
        case ITEM_SWEET_HEART:
            return 5;

        case ITEM_PEWTER_CRUNCHIES:
            return 1;
        case ITEM_RAGE_CANDY_BAR:
            return 2;
        case ITEM_LAVA_COOKIE:
            return 3;
        case ITEM_OLD_GATEAU:
            return 4;
        case ITEM_CASTELIACONE:
            return 5;
        case ITEM_LUMIOSE_GALETTE:
            return 6;
        case ITEM_SHALOUR_SABLE:
            return 7;
        case ITEM_BIG_MALASADA:
            return 8;

        case ITEM_SUN_STONE:
        case ITEM_DRAGON_SCALE:
        case ITEM_UPGRADE:
        case ITEM_KINGS_ROCK:
        case ITEM_METAL_COAT:
            return 2;

        case ITEM_SHINY_STONE:
        case ITEM_DUSK_STONE:
        case ITEM_DAWN_STONE:
        case ITEM_ICE_STONE: // needed for gen 4 evos
        case ITEM_PROTECTOR:
        case ITEM_ELECTIRIZER:
        case ITEM_MAGMARIZER:
        case ITEM_DUBIOUS_DISC:
        case ITEM_REAPER_CLOTH:
        case ITEM_OVAL_STONE:
        case ITEM_RAZOR_FANG:
        case ITEM_RAZOR_CLAW:
            return 4;

        case ITEM_PRISM_SCALE:
            return 5;

        case ITEM_WHIPPED_DREAM:
        case ITEM_SACHET:
            return 6;

        case ITEM_SWEET_APPLE:
        case ITEM_TART_APPLE:
        case ITEM_CRACKED_POT:
        case ITEM_CHIPPED_POT:
        case ITEM_GALARICA_CUFF:
        case ITEM_GALARICA_WREATH:
        case ITEM_STRAWBERRY_SWEET:
        case ITEM_LOVE_SWEET:
        case ITEM_BERRY_SWEET:
        case ITEM_CLOVER_SWEET:
        case ITEM_FLOWER_SWEET:
        case ITEM_STAR_SWEET:
        case ITEM_RIBBON_SWEET:
            return 8;

        case ITEM_RUSTED_SWORD:
        case ITEM_RUSTED_SHIELD:
            return 8;

        case ITEM_RED_ORB:
        case ITEM_BLUE_ORB:
            return 3;

        case ITEM_LIGHT_BALL:
        case ITEM_LEEK:
        case ITEM_THICK_CLUB:
            return 1;

        case ITEM_DEEP_SEA_SCALE:
        case ITEM_DEEP_SEA_TOOTH:
        case ITEM_SOUL_DEW:
            return 3;

        case ITEM_ADAMANT_ORB:
        case ITEM_LUSTROUS_ORB:
        case ITEM_GRISEOUS_ORB:
        case ITEM_GRISEOUS_CORE: // <- Always have griseous core avaliable, so can transform as we did in og game
            return 4;

        case ITEM_ROTOM_CATALOG:
        case ITEM_GRACIDEA:
            return 4;

        case ITEM_REVEAL_GLASS:
        case ITEM_DNA_SPLICERS:
            return 5;

        case ITEM_ZYGARDE_CUBE:
        case ITEM_PRISON_BOTTLE:
            return 6;

        case ITEM_N_SOLARIZER:
        case ITEM_N_LUNARIZER:
            return 7;

        case ITEM_REINS_OF_UNITY:
            return 8;

        // Hisui items gen8
        case ITEM_BLACK_AUGURITE:
        case ITEM_PEAT_BLOCK:
        case ITEM_ADAMANT_CRYSTAL:
        case ITEM_LUSTROUS_GLOBE:
            return 8;

        // Custom items
        case ITEM_ALOLA_STONE:
            return 7;
        case ITEM_GALAR_STONE:
            return 8;
        case ITEM_HISUI_STONE:
            return 8;
    };

#else
    // Item ranges are different so handle differently for EE and vanilla
    switch(item)
    {
        case ITEM_SUN_STONE:
        case ITEM_DRAGON_SCALE:
        case ITEM_UP_GRADE:
        case ITEM_KINGS_ROCK:
        case ITEM_METAL_COAT:
            return 2;

        case ITEM_DEEP_SEA_SCALE:
        case ITEM_DEEP_SEA_TOOTH:
        case ITEM_SOUL_DEW:
        case ITEM_RED_ORB:
        case ITEM_BLUE_ORB:
        case ITEM_LAVA_COOKIE:
            return 3;
    };
#endif

    // Assume gen 1 if we get here (i.e. always on)
    return 1;
}

extern const u16 gRogueBake_EvoItems_Count;
extern const u16 gRogueBake_FormItems[];
extern const u16 gRogueBake_FormItems_Count;

static void SetEvolutionItemFlag(u16 itemId, bool8 state)
{
    u16 elem = Rogue_GetEvolutionItemIndex(itemId);
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(elem < gRogueBake_EvoItems_Count);
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueRun.activeEvoItemFlags));

    if(state)
        gRogueRun.activeEvoItemFlags[idx] |= bitMask;
    else
        gRogueRun.activeEvoItemFlags[idx] &= ~bitMask;
}

static bool8 GetEvolutionItemFlag(u16 itemId)
{
    u16 elem = Rogue_GetEvolutionItemIndex(itemId);
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(idx < ARRAY_COUNT(gRogueRun.activeEvoItemFlags));

    return (gRogueRun.activeEvoItemFlags[idx] & bitMask) != 0;
}

static void SetFormItemFlag(u16 itemId, bool8 state)
{
    u16 elem = Rogue_GetFormItemIndex(itemId);
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(elem < gRogueBake_FormItems_Count);
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueRun.activeFormItemFlags));

    if(state)
        gRogueRun.activeFormItemFlags[idx] |= bitMask;
    else
        gRogueRun.activeFormItemFlags[idx] &= ~bitMask;
}

static bool8 GetFormItemFlag(u16 itemId)
{
    u16 elem = Rogue_GetFormItemIndex(itemId);
    u16 idx = elem / 8;
    u16 bit = elem % 8;
    u8 bitMask = 1 << bit;

    AGB_ASSERT(idx < ARRAY_COUNT(gRogueRun.activeFormItemFlags));

    return (gRogueRun.activeFormItemFlags[idx] & bitMask) != 0;
}

extern const struct RogueItem gRogueItems[];

static bool8 IsBattleOnlyBagClauseReplacementItem(u16 itemId)
{
    switch(itemId)
    {
    case ITEM_X_ATTACK:
    case ITEM_X_DEFENSE:
    case ITEM_X_SP_ATK:
    case ITEM_X_SP_DEF:
    case ITEM_X_SPEED:
    case ITEM_X_ACCURACY:
    case ITEM_DIRE_HIT:
    case ITEM_GUARD_SPEC:
    case ITEM_POKE_DOLL:
    case ITEM_FLUFFY_TAIL:
#ifdef ROGUE_EXPANSION
    case ITEM_POKE_TOY:
#endif
        return TRUE;

    default:
        return FALSE;
    }
}

static bool8 IsBagClauseDisabledBattleOnlyItem(u16 itemId)
{
    return Rogue_GetConfigToggle(CONFIG_TOGGLE_BAG_CLAUSE)
        && IsBattleOnlyBagClauseReplacementItem(itemId);
}

static u16 SanitizeRouteRewardItem(u16 itemId)
{
    if(itemId == ITEM_NONE || itemId == ITEM_LIST_END)
        return itemId;

    if(IsBattleOnlyBagClauseReplacementItem(itemId) || !Rogue_IsItemEnabled(itemId))
        return ITEM_POKEBLOCK_BUNDLE;

    return itemId;
}

static bool8 CanReplaceRouteRewardWithPokeblockBundle(u16 itemId)
{
    switch(itemId)
    {
    case ITEM_POKEBLOCK_BUNDLE:
    case ITEM_BIG_POKEBLOCK_BUNDLE:
    case ITEM_MASTER_BALL:
    case ITEM_RARE_CANDY:
    case ITEM_ESCAPE_ROPE:
#ifdef ROGUE_EXPANSION
    case ITEM_ABILITY_CAPSULE:
    case ITEM_ABILITY_PATCH:
#endif
        return FALSE;
    }

    if(Rogue_IsEvolutionItem(itemId) || Rogue_IsFormItem(itemId))
        return FALSE;

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_LONELY_MINT && itemId <= ITEM_SERIOUS_MINT)
        return FALSE;

    if((itemId >= ITEM_BUG_TERA_SHARD && itemId <= ITEM_WATER_TERA_SHARD) || itemId == ITEM_STELLAR_TERA_SHARD)
        return FALSE;
#endif

    switch(ItemId_GetPocket(itemId))
    {
    case POCKET_ITEMS:
    case POCKET_MEDICINE:
        return TRUE;

    default:
        return FALSE;
    }
}

bool8 Rogue_IsItemEnabled(u16 itemId)
{
    // Handle perma banned entries
    // (There is no scenario in which we will allow these)
    {
        if(itemId >= ITEM_ROGUE_ITEM_FIRST && itemId <= ITEM_ROGUE_ITEM_LAST)
        {
            if(itemId >= ITEM_TR01 && itemId <= ITEM_TR50)
            {
                // do nothing, the ID is never set on these and that's intentional as they copy from a template
            }
            // Item isn't enabled in this build config / doesn't have any data associated with this ID
            else if(gRogueItems[itemId - ITEM_ROGUE_ITEM_FIRST].itemId != itemId)
                return FALSE;
        }

        if(itemId >= FIRST_MAIL_INDEX && itemId <= LAST_MAIL_INDEX)
            return FALSE;

        if(itemId >= ITEM_RED_SCARF && itemId <= ITEM_YELLOW_SCARF)
            return FALSE;

        if(itemId >= ITEM_RED_SHARD && itemId <= ITEM_GREEN_SHARD)
            return FALSE;

        if(itemId >= ITEM_BLUE_FLUTE && itemId <= ITEM_WHITE_FLUTE)
            return FALSE;

        if(IsBagClauseDisabledBattleOnlyItem(itemId))
            return FALSE;

#ifdef ROGUE_EXPANSION
        if(itemId >= ITEM_GROWTH_MULCH && itemId <= ITEM_BLACK_APRICORN)
            return FALSE;

        // Exclude all treasures then turn on the ones we want to use
        if(itemId >= ITEM_BOTTLE_CAP && itemId <= ITEM_STRANGE_SOUVENIR)
            return FALSE;

        // These TMs aren't setup
        if(itemId >= ITEM_TM55 && itemId <= ITEM_TM100)
            return FALSE;

        // Ignore fossils for now
        if(itemId >= ITEM_HELIX_FOSSIL && itemId <= ITEM_FOSSILIZED_DINO)
            return FALSE;

        // Ignore sweets, as they are not used
        if(itemId >= ITEM_STRAWBERRY_SWEET && itemId <= ITEM_RIBBON_SWEET)
            return FALSE;

        // No dynamax
        if(itemId >= ITEM_EXP_CANDY_XS && itemId <= ITEM_DYNAMAX_CANDY)
            return FALSE;

        // No mochi
        if(itemId >= ITEM_HEALTH_MOCHI && itemId <= ITEM_FRESH_START_MOCHI)
            return FALSE;
#endif

        if(Rogue_IsRunActive())
        {
            // Berries are not functional outside of the hub
            switch (itemId)
            {
                case ITEM_DURIN_BERRY:
                case ITEM_PAMTRE_BERRY:
                case ITEM_NOMEL_BERRY:
                case ITEM_BELUE_BERRY:
                case ITEM_WATMEL_BERRY:
                case ITEM_SPELON_BERRY:
                case ITEM_RABUTA_BERRY:
                case ITEM_CORNN_BERRY:
                case ITEM_MAGOST_BERRY:
                    return FALSE;

#ifdef ROGUE_EXPANSION
                // Specific held items which don't trigger form changes, so won't be caught by the logic below
                // we don't want unless the mon is avaliable
                case ITEM_SOUL_DEW:
                    return Query_IsSpeciesEnabledForceDexChecking(SPECIES_LATIAS) || Query_IsSpeciesEnabledForceDexChecking(SPECIES_LATIOS);

                case ITEM_ADAMANT_ORB:
                    return Query_IsSpeciesEnabledForceDexChecking(SPECIES_DIALGA);

                case ITEM_LUSTROUS_ORB:
                    return Query_IsSpeciesEnabledForceDexChecking(SPECIES_PALKIA);

                case ITEM_GRISEOUS_ORB:
                    return Query_IsSpeciesEnabledForceDexChecking(SPECIES_GIRATINA);
#endif
            }
        }

        switch (itemId)
        {
        case ITEM_REVIVE:
        case ITEM_MAX_REVIVE:
            if(Rogue_IsRunActive() && !Rogue_ShouldReleaseFaintedMons())
            {
                return TRUE;
            }
            return FALSE;
            break;

#ifdef ROGUE_EXPANSION
        case ITEM_MAX_MUSHROOMS:
            return Rogue_IsRunActive() && IsDynamaxEnabled();

        // Only active in hub via quest reward
        case ITEM_BERSERK_GENE:
            return !Rogue_IsRunActive();
#endif

        case ITEM_SACRED_ASH:
        case ITEM_REVIVAL_HERB:
        case ITEM_RARE_CANDY:
        case ITEM_HEART_SCALE:
        case ITEM_LUCKY_EGG:
        case ITEM_EXP_SHARE:
        case ITEM_SHOAL_SALT:
        case ITEM_SHOAL_SHELL:
        case ITEM_FLUFFY_TAIL:
        case ITEM_SOOTHE_BELL:
        case ITEM_EVERSTONE:

        case ITEM_SMALL_COIN_CASE:
        case ITEM_LARGE_COIN_CASE:

        case ITEM_PINAP_BERRY:
        case ITEM_NANAB_BERRY:
        case ITEM_RAZZ_BERRY:
        case ITEM_ENIGMA_BERRY:
        case ITEM_WEPEAR_BERRY:
        case ITEM_BLUK_BERRY:

        // Berries that may confuse
        case ITEM_AGUAV_BERRY:
        case ITEM_WIKI_BERRY:
        case ITEM_PERSIM_BERRY:
        case ITEM_IAPAPA_BERRY:
        case ITEM_MAGO_BERRY:
        case ITEM_FIGY_BERRY:
#ifdef ROGUE_EXPANSION
        case ITEM_ENIGMA_BERRY_E_READER:

        // Not implemented/needed
        case ITEM_MAX_HONEY:
        case ITEM_LURE:
        case ITEM_SUPER_LURE:
        case ITEM_MAX_LURE:
        case ITEM_WISHING_PIECE:
        case ITEM_ARMORITE_ORE:
        case ITEM_DYNITE_ORE:
        case ITEM_GALARICA_TWIG:
        case ITEM_SWEET_HEART:
        case ITEM_POKE_TOY:

        case ITEM_BIG_BAMBOO_SHOOT:
        case ITEM_TINY_BAMBOO_SHOOT:

        // Link cable is Rogue's item
        case ITEM_LINKING_CORD:

        // Exclude all treasures then turn on the ones we want to use
        //case ITEM_NUGGET:
        //case ITEM_PEARL:
        //case ITEM_BIG_PEARL:
        //case ITEM_STARDUST:
        //case ITEM_STAR_PIECE:

        // Ignore these, as mons/form swaps currently not enabled
        case ITEM_PIKASHUNIUM_Z:
#endif
            return FALSE;
        }
    }

    // Run only excludes
    if(Rogue_IsRunActive())
    {
        u8 genLimit = RoguePokedex_GetDexGenLimit();

        if(!Rogue_GetConfigToggle(CONFIG_TOGGLE_EV_GAIN))
        {
#if defined(ROGUE_EXPANSION)
            if(itemId >= ITEM_HP_UP && itemId <= ITEM_CARBOS)
                return FALSE;

            if(itemId >= ITEM_HEALTH_FEATHER && itemId <= ITEM_SWIFT_FEATHER)
                return FALSE;

            if(itemId >= ITEM_MACHO_BRACE && itemId <= ITEM_POWER_ANKLET)
                return FALSE;
#else
            if((itemId >= ITEM_HP_UP && itemId <= ITEM_CALCIUM) || itemId == ITEM_ZINC)
                return FALSE;
#endif
        }

#ifdef ROGUE_EXPANSION
        // Mass exclude mega, Z moves & Tera Shards
        // Only show tera shards if we have teras enabled
        if((itemId >= ITEM_BUG_TERA_SHARD && itemId <= ITEM_WATER_TERA_SHARD) || itemId == ITEM_STELLAR_TERA_SHARD)
        {
            if(Rogue_IsRunActive())
                return IsTerastallizeEnabled();
            else
                return TRUE;
        }

        if((itemId >= ITEM_RED_ORB && itemId <= ITEM_BLUE_ORB) || IS_MEGA_STONE_ITEM(itemId))
        {
            return IsMegaEvolutionEnabled()
                && Rogue_IsFormItem(itemId)
                && GetFormItemFlag(itemId);
        }

        if(itemId >= ITEM_NORMALIUM_Z && itemId <= ITEM_ULTRANECROZIUM_Z)
        {
            if(Rogue_IsRunActive())
                return IsZMovesEnabled();
            else
                return TRUE;
        }

        // Regional treat (Avoid spawning in multiple)
        if(itemId >= ITEM_PEWTER_CRUNCHIES && itemId <= ITEM_BIG_MALASADA)
        {
            switch(genLimit)
            {
                case 1:
                    if(itemId != ITEM_PEWTER_CRUNCHIES)
                        return FALSE;
                    break;
                case 2:
                    if(itemId != ITEM_RAGE_CANDY_BAR)
                        return FALSE;
                    break;
                case 3:
                    if(itemId != ITEM_LAVA_COOKIE)
                        return FALSE;
                    break;
                case 4:
                    if(itemId != ITEM_OLD_GATEAU)
                        return FALSE;
                    break;
                case 5:
                    if(itemId != ITEM_CASTELIACONE)
                        return FALSE;
                    break;
                case 6:
                    if(itemId != ITEM_LUMIOSE_GALETTE)
                        return FALSE;
                    break;
                case 7:
                    if(itemId != ITEM_SHALOUR_SABLE)
                        return FALSE;
                    break;
                //case 8:
                default:
                    if(itemId != ITEM_BIG_MALASADA)
                        return FALSE;
                    break;
            }
        }
#endif

        if(Rogue_IsEvolutionItem(itemId))
        {
            // Only include the active evo items
            return GetEvolutionItemFlag(itemId) != FALSE;
        }

        if(Rogue_IsFormItem(itemId))
        {
            // Only include the active form change items
            return GetFormItemFlag(itemId) != FALSE;
        }

        if(ItemToGen(itemId) > genLimit)
            return FALSE;
    }
    // Hub only excludes
    else
    {
        if(itemId >= ITEM_TR01 && itemId <= ITEM_TR50)
            return FALSE;
    }

    // Only return true if the actual item is valid
    {
        struct Item item;
        Rogue_ModifyItem(itemId, &item);

        // Best entry to check that this item is valid
        return (item.description != NULL);
    }
}

bool8 Rogue_IsItemRoomReward(u16 itemId)
{
    return itemId >= FIRST_ITEM_ROOM_REWARD && itemId <= LAST_ITEM_ROOM_REWARD;
}

bool8 IsMegaEvolutionEnabled(void)
{
#if TESTING && defined(ROGUE_EXPANSION)
    // todo - once we have Rogue specific tests, should come up with a good way to make this testable
    return TRUE;
#elif defined(ROGUE_EXPANSION)
    if(Rogue_IsRunActive())
        return gRogueRun.megasEnabled; // cached result
    else
        return CheckBagHasItem(ITEM_MEGA_RING, 1);

#else
    return FALSE;
#endif
}

bool8 IsZMovesEnabled(void)
{
#if TESTING && defined(ROGUE_EXPANSION)
    // todo - once we have Rogue specific tests, should come up with a good way to make this testable
    return TRUE;
#elif defined(ROGUE_EXPANSION)
    if(Rogue_IsRunActive())
        return gRogueRun.zMovesEnabled; // cached result
    else
        return CheckBagHasItem(ITEM_Z_POWER_RING, 1);
#else
    return FALSE;
#endif
}

bool8 IsDynamaxEnabled(void)
{
#if TESTING && defined(ROGUE_EXPANSION)
    // todo - once we have Rogue specific tests, should come up with a good way to make this testable
    return TRUE;
#elif defined(ROGUE_EXPANSION)
    if(Rogue_IsRunActive())
        return gRogueRun.dynamaxEnabled; // cached result
    else
        return CheckBagHasItem(ITEM_DYNAMAX_BAND, 1);
#else
    return FALSE;
#endif
}

bool8 IsTerastallizeEnabled(void)
{
#if TESTING && defined(ROGUE_EXPANSION)
    // todo - once we have Rogue specific tests, should come up with a good way to make this testable
    return TRUE;
#elif defined(ROGUE_EXPANSION)
    if(Rogue_IsRunActive())
        return gRogueRun.terastallizeEnabled; // cached result
    else
        return CheckBagHasItem(ITEM_TERA_ORB, 1);
#else
    return FALSE;
#endif
}

bool8 IsHealingFlaskEnabled(void)
{
    return CheckBagHasItem(ITEM_HEALING_FLASK, 1);
}

static bool8 IsRareShopActiveInternal()
{
    u16 itemId;

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
    {
        if(ItemId_GetPocket(itemId) == POCKET_STONES)
        {
            if(Rogue_IsItemEnabled(itemId))
                return TRUE;
        }
    }

    return FALSE;
}

static bool8 IsRareShopActive()
{
    if(Rogue_IsRunActive())
        return gRogueRun.rareShopEnabled; // cached result
    else
        return TRUE; // just assume it's active when in hub
}

#if defined(ROGUE_DEBUG)

u16 Debug_MiniMenuHeight(void)
{
    u16 height = 10;
    return height;
}

static u8* AppendNumberField(u8* strPointer, const u8* field, u32 num)
{
    u8 pow = 2;

    if(num >= 1000)
    {
        pow = 9;
    }
    else if(num >= 100)
    {
        pow = 3;
    }

    ConvertUIntToDecimalStringN(gStringVar1, num, STR_CONV_MODE_RIGHT_ALIGN, pow);

    strPointer = StringAppend(strPointer, field);
    return StringAppend(strPointer, gStringVar1);
}

u8* Debug_GetMiniMenuContent(void)
{
    u8* strPointer = &gStringVar4[0];
    *strPointer = EOS;

    if(JOY_NEW(R_BUTTON) && gDebug_CurrentTab != 2)
    {
        ++gDebug_CurrentTab;
    }
    else if(JOY_NEW(L_BUTTON) && gDebug_CurrentTab != 0)
    {
        --gDebug_CurrentTab;
    }

    // Main tab
    //
    if(gDebug_CurrentTab == 0)
    {
        u8 difficultyLevel = Rogue_GetCurrentDifficulty();
        u8 playerLevel = Rogue_CalculatePlayerMonLvl();
        u8 wildLevel = CalculateWildLevel(0);

        strPointer = StringAppend(strPointer, gText_RogueDebug_Header);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Save, RogueSave_GetVersionId());
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Room, gRogueRun.enteredRoomCounter);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Difficulty, difficultyLevel);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_PlayerLvl, playerLevel);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_WildLvl, wildLevel);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_WildCount, gDebug_WildOptionCount);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_ItemCount, gDebug_ItemOptionCount);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_TrainerCount, gDebug_TrainerOptionCount);
    }
    // Adventure path tab
    //
    else if(gDebug_CurrentTab == 1)
    {
        strPointer = StringAppend(strPointer, gText_RogueDebug_AdvHeader);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_AdvCount, gRogueAdvPath.roomCount);
        //strPointer = AppendNumberField(strPointer, gText_RogueDebug_X, gRogueAdvPath.currentNodeX);
        //strPointer = AppendNumberField(strPointer, gText_RogueDebug_Y, gRogueAdvPath.currentNodeY);
    }
#ifdef ROGUE_FEATURE_AUTOMATION
    // Automation tab
    //
    else
    {
        u16 i;

        strPointer = StringAppend(strPointer, gText_RogueDebug_Header);

        for(i = 0; i < 8; ++i) // Rogue_AutomationBufferSize()
        {
            strPointer = AppendNumberField(strPointer, gText_RogueDebug_X, Rogue_ReadAutomationBuffer(i));
        }
    }
#else
    // Misc. debug tab
    //
    else
    {

        strPointer = StringAppend(strPointer, gText_RogueDebug_Header);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Seed, gSaveBlock1Ptr->dewfordTrends[0].words[0]);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Seed, gSaveBlock1Ptr->dewfordTrends[0].words[1]);
    }
#endif

    return gStringVar4;
}

#endif

bool8 Rogue_ShouldShowMiniMenu(void)
{
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return TRUE;

    if(GetSafariZoneFlag())
        return FALSE;

    return TRUE;
}

u16 Rogue_MiniMenuHeight(void)
{
    u16 height = Rogue_IsRunActive() ? 3 : 1;

#if defined(ROGUE_DEBUG)
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return Debug_MiniMenuHeight();
#endif

    if (RogueTrial_IsActive())
    {
        ++height;
        if (RogueTrial_IsActiveTrial(ROGUE_TRIAL_LIMITED_CAPTURE))
            ++height;
    }

    if(GetSafariZoneFlag())
    {
        height = 3;
    }
    else
    {
        if(Rogue_IsActiveCampaignScored())
        {
            ++height;
        }
    }

    return height * 2;
}

extern const u8 gText_StatusRoute[];
extern const u8 gText_StatusBadges[];
extern const u8 gText_StatusTrial[];
extern const u8 gText_StatusCaptures[];
extern const u8 gText_StatusScore[];
extern const u8 gText_StatusTimer[];
extern const u8 gText_StatusClock[];
extern const u8 gText_StatusSeasonSpring[];
extern const u8 gText_StatusSeasonSummer[];
extern const u8 gText_StatusSeasonAutumn[];
extern const u8 gText_StatusSeasonWinter[];

u8* Rogue_GetMiniMenuContent(void)
{
    u8* strPointer = &gStringVar4[0];

#if defined(ROGUE_DEBUG)
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return Debug_GetMiniMenuContent();
#endif

    *strPointer = EOS;

    // Season stamp
    switch(RogueToD_GetSeason())
    {
        case SEASON_SPRING:
            strPointer = StringAppend(strPointer, gText_StatusSeasonSpring);
            break;
        case SEASON_SUMMER:
            strPointer = StringAppend(strPointer, gText_StatusSeasonSummer);
            break;
        case SEASON_AUTUMN:
            strPointer = StringAppend(strPointer, gText_StatusSeasonAutumn);
            break;
        case SEASON_WINTER:
            strPointer = StringAppend(strPointer, gText_StatusSeasonWinter);
            break;
    }

    // Clock
    ConvertIntToDecimalStringN(gStringVar1, RogueToD_GetHours(), STR_CONV_MODE_RIGHT_ALIGN, 2);
    ConvertIntToDecimalStringN(gStringVar2, RogueToD_GetMinutes(), STR_CONV_MODE_LEADING_ZEROS, 2);

    StringExpandPlaceholders(gStringVar3, gText_StatusClock);
    strPointer = StringAppend(strPointer, gStringVar3);

    if(Rogue_IsRunActive())
    {
        // Run time
        ConvertIntToDecimalStringN(gStringVar1, gSaveBlock2Ptr->playTimeHours, STR_CONV_MODE_RIGHT_ALIGN, 3);
        ConvertIntToDecimalStringN(gStringVar2, gSaveBlock2Ptr->playTimeMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
        StringExpandPlaceholders(gStringVar3, gText_StatusTimer);
        strPointer = StringAppend(strPointer, gStringVar3);

        // Badges
        ConvertIntToDecimalStringN(gStringVar1, Rogue_GetCurrentDifficulty(), STR_CONV_MODE_RIGHT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar3, gText_StatusBadges);
        strPointer = StringAppend(strPointer, gStringVar3);

        if (RogueTrial_IsActive())
        {
            const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);

            StringCopy(gStringVar1, trial->name);
            StringExpandPlaceholders(gStringVar3, gText_StatusTrial);
            strPointer = StringAppend(strPointer, gStringVar3);

            if (RogueTrial_IsActiveTrial(ROGUE_TRIAL_LIMITED_CAPTURE))
            {
                ConvertIntToDecimalStringN(gStringVar1, VarGet(VAR_ROGUE_TOTAL_RUN_CATCHES), STR_CONV_MODE_LEFT_ALIGN, 1);
                StringExpandPlaceholders(gStringVar3, gText_StatusCaptures);
                strPointer = StringAppend(strPointer, gStringVar3);
            }
        }
    }

    // Score
    if(Rogue_IsActiveCampaignScored())
    {
        ConvertIntToDecimalStringN(gStringVar1, Rogue_GetCampaignScore(), STR_CONV_MODE_RIGHT_ALIGN, 6);

        StringExpandPlaceholders(gStringVar3, gText_StatusScore);
        strPointer = StringAppend(strPointer, gStringVar3);
    }

    *(strPointer - 1) = EOS; // Remove trailing \n
    return gStringVar4;
}

static u16 GetMenuWildEncounterCount()
{
    if(Rogue_IsRideMonSwimming())
        return GetCurrentWaterEncounterCount();

    return GetCurrentWildEncounterCount();
}

static u16 GetMenuWildEncounterSpecies(u8 i)
{
    if(Rogue_IsRideMonSwimming())
        return GetWildWaterEncounter(i);

    return GetWildGrassEncounter(i);
}

void Rogue_CreateMiniMenuExtraGFX(void)
{
    u8 i;
    u8 palIndex;
    u8 oamPriority = 0; // Render infront of background
    u16 palBuffer[16];

#if defined(ROGUE_DEBUG)
    // Don't show whilst info panel is visible
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return;
#endif

    // Ensure we have a palette free
    FieldEffectFreeAllSprites();

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE || GetSafariZoneFlag())
    {
        bool8 isVisible;
        u16 yOffset = 24 + Rogue_MiniMenuHeight() * 8;

        //LoadMonIconPalettes();

        for(i = 0; i < GetMenuWildEncounterCount(); ++i)
        {
            //u8 paletteOffset = i;
            u8 paletteOffset = 0; // No palette offset as we're going to greyscale and share anyway
            u16 targetSpecies = GetMenuWildEncounterSpecies(i);

            isVisible = GetSetPokedexSpeciesFlag(targetSpecies, FLAG_GET_SEEN);

            if(isVisible)
            {
                LoadMonIconPaletteCustomOffset(targetSpecies, paletteOffset);

                gRogueLocal.encounterPreview[i].monSpriteId = CreateMonIconCustomPaletteOffset(targetSpecies, SpriteCallbackDummy, (14 + (i % 3) * 32), yOffset + (i / 3) * 32, oamPriority, paletteOffset);
            }
            else
            {
                LoadMonIconPaletteCustomOffset(SPECIES_NONE, paletteOffset);
                gRogueLocal.encounterPreview[i].monSpriteId = CreateMissingMonIcon(SpriteCallbackDummy, (14 + (i % 3) * 32), yOffset + (i / 3) * 32, 0, paletteOffset);
            }

            // Have to grey out icon as I can't figure out why custom palette offsets still seem to be stomping over each other
            // The best guess I have is that the overworld palette is doing some extra behaviour but who knows
            palIndex = IndexOfSpritePaletteTag(gSprites[gRogueLocal.encounterPreview[i].monSpriteId].template->paletteTag);
            CpuCopy16(&gPlttBufferUnfaded[0x100 + palIndex * 16], &palBuffer[0], 32);
            //TintPalette_CustomTone(&palBuffer[0], 16, 510, 510, 510);
            TintPalette_GrayScale2(&palBuffer[0], 16);
            LoadPalette(&palBuffer[0], 0x100 + palIndex * 16, 32);
        }
    }
}

void Rogue_RemoveMiniMenuExtraGFX(void)
{
    u8 i;

#if defined(ROGUE_DEBUG)
    // Don't show whilst info panel is visible
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return;
#endif

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE || GetSafariZoneFlag())
    {
        bool8 isVisible;

        for(i = 0; i < GetMenuWildEncounterCount(); ++i)
        {
            u8 paletteOffset = i;
            u16 targetSpecies = GetMenuWildEncounterSpecies(i);

            isVisible = GetSetPokedexSpeciesFlag(targetSpecies, FLAG_GET_SEEN);

            if(isVisible)
                FreeMonIconPaletteCustomOffset(GetIconSpeciesNoPersonality(targetSpecies), paletteOffset);
            else
                FreeMonIconPaletteCustomOffset(GetIconSpeciesNoPersonality(SPECIES_NONE), paletteOffset);

            if(gRogueLocal.encounterPreview[i].monSpriteId != SPRITE_NONE)
                FreeAndDestroyMonIconSprite(&gSprites[gRogueLocal.encounterPreview[i].monSpriteId]);

            gRogueLocal.encounterPreview[i].monSpriteId = SPRITE_NONE;
        }

        //FreeMonIconPalettes();
    }
}

struct StarterSelectionData
{
    u16 species[3];
    bool8 shinyState[3];
    u8 count;
};

static const u8 sStarterTypeTriangles[] =
{
    TYPE_WATER, TYPE_GRASS, TYPE_FIRE,
    TYPE_BUG, TYPE_ROCK, TYPE_GRASS,
#ifdef ROGUE_EXPANSION
    TYPE_FAIRY, TYPE_STEEL, TYPE_FIGHTING,
#endif

    // dragon, dragon, dragon
    TYPE_ROCK, TYPE_GRASS, TYPE_FIRE,
    TYPE_DARK, TYPE_FIGHTING, TYPE_PSYCHIC,

    TYPE_PSYCHIC, TYPE_BUG, TYPE_POISON,
    // ghost, ghost, ghost
    TYPE_ICE, TYPE_FIGHTING, TYPE_FLYING,

    TYPE_ELECTRIC, TYPE_GROUND, TYPE_WATER,
    TYPE_GRASS, TYPE_ICE, TYPE_ROCK,
    TYPE_POISON, TYPE_GROUND, TYPE_GRASS,

    TYPE_FIRE, TYPE_GROUND, TYPE_ICE,
    TYPE_ROCK, TYPE_FIGHTING, TYPE_PSYCHIC,
    TYPE_FIGHTING, TYPE_FLYING, TYPE_ROCK,

    TYPE_GROUND, TYPE_ICE, TYPE_STEEL,
    TYPE_FLYING, TYPE_ROCK, TYPE_GRASS,
    TYPE_STEEL, TYPE_FIRE, TYPE_ROCK
};

static void BuildTrialStarterMonQuery(bool8 limitToCurrentDex)
{
    RogueMonQuery_Begin();

    RogueMonQuery_IsSpeciesActive();
    if (limitToCurrentDex)
        RogueMonQuery_IsBaseSpeciesInCurrentDex(QUERY_FUNC_INCLUDE);
    if (RogueTrial_PendingRequiresLegendarySpecies())
    {
        RogueTrial_FilterPendingMonQuery();
        return;
    }
    else
    {
        RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);
    }

    RogueMonQuery_TransformIntoEggSpecies();
    RogueMonQuery_TransformIntoEvos(2, FALSE, FALSE); // to force mons to fit gen settings
    RogueTrial_FilterPendingMonQuery();
    RogueMonQuery_AnyActiveEvos(QUERY_FUNC_INCLUDE);
}

static bool8 TrySelectTrialStarterMons(struct StarterSelectionData *starters, bool8 isSeeded, bool8 limitToCurrentDex)
{
    u8 i;
    u8 selectedCount = 0;
    u16 selectedPool[ARRAY_COUNT(starters->species)];

    BuildTrialStarterMonQuery(limitToCurrentDex);

    if (!RogueMiscQuery_AnyActiveElements())
    {
        RogueMonQuery_End();
        return FALSE;
    }

    for (i = 0; i < ARRAY_COUNT(starters->species); ++i)
    {
        u16 randValue = isSeeded ? RogueRandom() : Random();

        if (RogueMiscQuery_AnyActiveElements())
        {
            starters->species[i] = RogueMiscQuery_SelectRandomElement(randValue);
            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, starters->species[i]);
            selectedPool[selectedCount++] = starters->species[i];
        }
        else
        {
            // Tiny Trial pools may contain fewer than three legal species. Exhaust
            // every option before allowing duplicates so the starter bag remains full.
            AGB_ASSERT(selectedCount != 0);
            starters->species[i] = selectedPool[randValue % selectedCount];
        }

        starters->shinyState[i] = Rogue_RollShinyState(SHINY_ROLL_DYNAMIC);
    }

    starters->count = ARRAY_COUNT(starters->species);

    RogueMonQuery_End();
    return TRUE;
}

static struct StarterSelectionData SelectTrialStarterMons(bool8 isSeeded)
{
    struct StarterSelectionData starters;

    memset(&starters, 0, sizeof(starters));

    if (!TrySelectTrialStarterMons(&starters, isSeeded, TRUE))
        TrySelectTrialStarterMons(&starters, isSeeded, FALSE);

    return starters;
}

static struct StarterSelectionData SelectStarterMons(bool8 isSeeded)
{
    struct StarterSelectionData starters;
    u8 i;
    bool8 isValidTriangle = FALSE;
    u16 typeTriangleOffset = (isSeeded ? RogueRandom() : Random());

    if (RogueTrial_PendingNeedsStarterFilter())
        return SelectTrialStarterMons(isSeeded);

    while(!isValidTriangle)
    {
        u16 triangleCount = ARRAY_COUNT(sStarterTypeTriangles) / 3;

        typeTriangleOffset = (typeTriangleOffset + 1) % triangleCount;
        isValidTriangle = TRUE;

        for(i = 0; i < 3; ++i)
        {
            u32 typeFlags = MON_TYPE_VAL_TO_FLAGS(sStarterTypeTriangles[typeTriangleOffset * 3 + i]);
            RogueMonQuery_Begin();

            RogueMonQuery_IsSpeciesActive();
            RogueMonQuery_IsBaseSpeciesInCurrentDex(QUERY_FUNC_INCLUDE);
            RogueMonQuery_EvosContainType(QUERY_FUNC_INCLUDE, typeFlags);
            RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);

            RogueMonQuery_TransformIntoEggSpecies();
            RogueMonQuery_TransformIntoEvos(2, FALSE, FALSE); // to force mons to fit gen settings
            RogueMonQuery_AnyActiveEvos(QUERY_FUNC_INCLUDE);
            RogueTrial_FilterPendingMonQuery();

            RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, typeFlags);

            // Exclude other types in triangle
            typeFlags = 0;
            if(i != 0)
                typeFlags |= MON_TYPE_VAL_TO_FLAGS(sStarterTypeTriangles[typeTriangleOffset * 3 + 0]);
            if(i != 1)
                typeFlags |= MON_TYPE_VAL_TO_FLAGS(sStarterTypeTriangles[typeTriangleOffset * 3 + 1]);
            if(i != 2)
                typeFlags |= MON_TYPE_VAL_TO_FLAGS(sStarterTypeTriangles[typeTriangleOffset * 3 + 2]);

            RogueMonQuery_IsOfType(QUERY_FUNC_EXCLUDE, typeFlags);

            RogueWeightQuery_Begin();
            {
                RogueWeightQuery_FillWeights(1);

                // No valid starter for this type so exit this triangle here
                if(!RogueWeightQuery_HasAnyWeights())
                {
                    RogueWeightQuery_End();
                    RogueMonQuery_End();

                    isValidTriangle = FALSE;
                    break;
                }

                starters.species[i] = RogueWeightQuery_SelectRandomFromWeights(isSeeded ? RogueRandom() : Random());
                starters.shinyState[i] = Rogue_RollShinyState(SHINY_ROLL_DYNAMIC);
            }
            RogueWeightQuery_End();

            RogueMonQuery_End();
        }
    }

    return starters;
}

void Rogue_RandomiseStarters()
{
    struct StarterSelectionData starters;
    u8 forcedDexVariant = RogueTrial_GetPendingForcedPokedexVariant();
    u8 dexVariantToRestore = POKEDEX_VARIANT_NONE;

    if (forcedDexVariant != POKEDEX_VARIANT_NONE)
    {
        dexVariantToRestore = RoguePokedex_GetDexVariant();
        RoguePokedex_SetDexVariant(forcedDexVariant);
    }

    starters = SelectStarterMons(FALSE);

    if (forcedDexVariant != POKEDEX_VARIANT_NONE)
        RoguePokedex_SetDexVariant(dexVariantToRestore);

    VarSet(VAR_ROGUE_STARTER0, starters.species[0]);
    VarSet(VAR_ROGUE_STARTER1, starters.species[1]);
    VarSet(VAR_ROGUE_STARTER2, starters.species[2]);
}

u16 Rogue_SelectFallbackStarterSpecies(void)
{
    struct StarterSelectionData starters = SelectStarterMons(FALSE);
    return starters.species[0];
}

void Rogue_GenerateFallbackStarter(void)
{
    gSpecialVar_Result = Rogue_SelectFallbackStarterSpecies();
}

static void UNUSED ClearPokemonHeldItems(void)
{
    struct BoxPokemon* boxMon;
    u16 boxId, boxPosition;
    u16 itemId = ITEM_NONE;

    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++)
        {
            boxMon = GetBoxedMonPtr(boxId, boxPosition);

            if(GetBoxMonData(boxMon, MON_DATA_SPECIES) != SPECIES_NONE)
                SetBoxMonData(boxMon, MON_DATA_HELD_ITEM, &itemId);
        }
    }

    for(boxId = 0; boxId < gPlayerPartyCount; ++boxId)
    {
        if(GetMonData(&gPlayerParty[boxId], MON_DATA_SPECIES) != SPECIES_NONE)
                SetMonData(&gPlayerParty[boxId], MON_DATA_HELD_ITEM, &itemId);
    }
}

void Rogue_ResetConfigHubSettings(void)
{
    // TODO - Replace this??

    // Seed settings
    FlagClear(FLAG_SET_SEED_ENABLED);
    FlagSet(FLAG_SET_SEED_ITEMS);
    FlagSet(FLAG_SET_SEED_TRAINERS);
    FlagSet(FLAG_SET_SEED_BOSSES);
    FlagSet(FLAG_SET_SEED_WILDMONS);

    // Expansion Room settings
    VarSet(VAR_ROGUE_ENABLED_GEN_LIMIT, 3);
    VarSet(VAR_ROGUE_REGION_DEX_LIMIT, 0);
}

static void ChooseRandomPokeballReward()
{
    if(VarGet(VAR_ROGUE_FREE_POKE_BALL) == ITEM_NONE)
    {
        RogueItemQuery_Begin();

        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_POKE_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_GREAT_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_ULTRA_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_PREMIER_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_LUXURY_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_TIMER_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_REPEAT_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_NEST_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_DIVE_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_NET_BALL);

#ifdef ROGUE_EXPANSION
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_HEAL_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_DUSK_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_QUICK_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_LEVEL_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_LURE_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_MOON_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_FRIEND_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_LOVE_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_FAST_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_HEAVY_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_DREAM_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_BEAST_BALL);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_CHERISH_BALL);
#endif

        VarSet(VAR_ROGUE_FREE_POKE_BALL, RogueMiscQuery_SelectRandomElement(Random()));

        RogueItemQuery_End();
    }
}

void Rogue_OnNewGame(void)
{
    RogueSave_ClearData();

    gSaveBlock1Ptr->bagSortMode = ITEM_SORT_MODE_TYPE;
    gSaveBlock1Ptr->bagCapacityUpgrades = 0;

    RoguePlayer_SetNewGameOutfit();
    RogueQuest_OnNewGame();

    StringCopy(gSaveBlock2Ptr->playerName, gText_TrainerName_Default);
    StringCopy(gSaveBlock2Ptr->pokemonHubName, gText_ExpandedPlaceholder_PokemonHub);
    memset(&gRogueRun.completedBadges[0], TYPE_NONE, sizeof(gRogueRun.completedBadges));
    memset(&gRogueRun.lastShopVisitDifficulty[0], ROGUE_MAX_BOSS_COUNT, sizeof(gRogueRun.lastShopVisitDifficulty));

    SetMoney(&gSaveBlock1Ptr->money, 0);
    memset(&gRogueLocal, 0, sizeof(gRogueLocal));

    FlagSet(FLAG_ROGUE_SETTINGS_MENU_DISPLAY_HIGHLIGHT);

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_ROGUE_IS_VICTORY_LAP);
    FlagClear(FLAG_ROGUE_WILD_SAFARI);
    FlagClear(FLAG_ROGUE_LVL_TUTORIAL);
    FlagSet(FLAG_ROGUE_HIDE_WORKBENCHES);

    FlagClear(FLAG_ROGUE_PRE_RELEASE_COMPAT_WARNING);

#ifdef ROGUE_EXPANSION
    FlagSet(FLAG_ROGUE_EXPANSION_ACTIVE);
#else
    FlagClear(FLAG_ROGUE_EXPANSION_ACTIVE);
#endif

    VarSet(VAR_ROGUE_DESIRED_CAMPAIGN, ROGUE_CAMPAIGN_NONE);

    Rogue_ResetSettingsToDefaults();
    Rogue_ResetConfigHubSettings();
    ChooseRandomPokeballReward();

    VarSet(VAR_ROGUE_DIFFICULTY, 0);
    VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, 0);
    VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, 0);
    VarSet(VAR_ROGUE_ADVENTURE_MONEY_TIER, 0);
    VarSet(VAR_ROGUE_DESIRED_WEATHER, WEATHER_NONE);

    VarSet(VAR_ROGUE_REGION_DEX_LIMIT, 0);
    VarSet(VAR_ROGUE_DESIRED_CAMPAIGN, ROGUE_CAMPAIGN_NONE);

    FlagSet(FLAG_SYS_B_DASH);
    EnableNationalPokedex();

    RogueToD_SetTime(60 * 10);
    RogueHub_UpdateWeatherState();

    SetLastHealLocationWarp(HEAL_LOCATION_ROGUE_HUB);

    ClearBerryTrees();

    // Plant default berries (these are the free gifts you get)
    PlantBerryTree(BERRY_TREE_HUB_11, ItemIdToBerryType(ITEM_ORAN_BERRY), BERRY_STAGE_BERRIES, FALSE);
    PlantBerryTree(BERRY_TREE_HUB_12, ItemIdToBerryType(ITEM_RAWST_BERRY), BERRY_STAGE_BERRIES, FALSE);
    PlantBerryTree(BERRY_TREE_HUB_13, ItemIdToBerryType(ITEM_CHERI_BERRY), BERRY_STAGE_BERRIES, FALSE);
    PlantBerryTree(BERRY_TREE_HUB_14, ItemIdToBerryType(ITEM_CHESTO_BERRY), BERRY_STAGE_BERRIES, FALSE);
    PlantBerryTree(BERRY_TREE_HUB_15, ItemIdToBerryType(ITEM_PECHA_BERRY), BERRY_STAGE_BERRIES, FALSE);

    Rogue_ResetCampaignAfter(0);
    RogueHub_ClearProgress();

#ifdef ROGUE_DEBUG
    FlagClear(FLAG_ROGUE_DEBUG_DISABLED);
#else
    FlagSet(FLAG_ROGUE_DEBUG_DISABLED);
#endif

    memset(gRogueSaveBlock->safariMons, 0, sizeof(gRogueSaveBlock->safariMons));
    memset(gRogueSaveBlock->dynamicUniquePokemon, 0, sizeof(gRogueSaveBlock->dynamicUniquePokemon));
    memset(gRogueSaveBlock->daycarePokemon, 0, sizeof(gRogueSaveBlock->daycarePokemon));
    memset(gRogueSaveBlock->adventureReplay, 0, sizeof(gRogueSaveBlock->adventureReplay));
    memset(gRogueSaveBlock->monMasteryFlags, 0, sizeof(gRogueSaveBlock->monMasteryFlags));
    gRogueSaveBlock->lastKnownNumSpecies = NUM_SPECIES;

    Rogue_ClearPopupQueue();
}

void Rogue_GameClear(void)
{
    SetContinueGameWarpToHealLocation(HEAL_LOCATION_ROGUE_HUB);
}

void Rogue_SetDefaultOptions(void)
{
#ifdef ROGUE_DEBUG
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
    gSaveBlock2Ptr->optionsFadeSpeed = OPTIONS_TEXT_SPEED_FAST;
#else
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_MID;
    gSaveBlock2Ptr->optionsFadeSpeed = OPTIONS_TEXT_SPEED_MID;
#endif
    //gSaveBlock2Ptr->optionsSound = OPTIONS_SOUND_MONO;
    //gSaveBlock2Ptr->optionsBattleSceneOff = FALSE;
    //gSaveBlock2Ptr->regionMapZoom = FALSE;
}

extern const u8 Rogue_QuickSaveLoad[];
extern const u8 Rogue_ForceNicknameMon[];
extern const u8 Rogue_AskNicknameMon[];
extern const u8 Rogue_Encounter_RestStop_RandomMan[];
extern const u8 Rogue_EventScript_AttemptSnagBattle[];
extern const u8 Rogue_EventScript_AttemptSacredAshRecovery[];
extern const u8 Rogue_Ridemon_PlayerIsTrapped[];

void Rogue_NotifySaveLoaded(void)
{
    RogueQuest_OnLoadGame();
    FollowMon_RecountActiveObjects();

    gRogueLocal.hasQuickLoadPending = FALSE;

    if(Rogue_IsRunActive() && !FlagGet(FLAG_ROGUE_RUN_COMPLETED))
    {
        gRogueLocal.hasQuickLoadPending = TRUE;
    }

    RogueQuest_OnTrigger(QUEST_TRIGGER_MISC_UPDATE);
}

bool8 Rogue_IsObjectEventExcludedFromSave(struct ObjectEvent* objectEvent)
{
    // Don't save MP objects
    if(objectEvent->localId >= OBJ_EVENT_ID_MULTIPLAYER_FIRST && objectEvent->localId <= OBJ_EVENT_ID_MULTIPLAYER_LAST)
        return TRUE;

    // We probably don't need this, as the template should still be setup in the same order they're saved in so this should be fine
    //if(RogueHub_IsPlayerBaseLayout(gMapHeader.mapLayoutId))
    //{
    //    // To avoid scripts being setup incorrectly, always reload the props dynamically
    //    if(objectEvent->localId != OBJ_EVENT_ID_PLAYER && objectEvent->localId != OBJ_EVENT_ID_FOLLOWER)
    //        return TRUE;
    //}

    return FALSE;
}

void Rogue_OnSecondPassed(void)
{

}

void Rogue_OnMinutePassed(void)
{
    RogueGift_CountDownDynamicCustomMons();
}

void Rogue_OnHourPassed(void)
{

}

void ForceRunRidemonTrappedCheck()
{
    gRogueLocal.hasPendingRidemonTrappedCheck = TRUE;
}

bool8 Rogue_OnProcessPlayerFieldInput(void)
{
    if(sSacredAshRecoveryPending)
    {
        ScriptContext_SetupScript(Rogue_EventScript_AttemptSacredAshRecovery);
        return TRUE;
    }
    else if(gRogueLocal.hasNicknameMonMsgPending)
    {
        gRogueLocal.hasNicknameMonMsgPending = FALSE;
        if(Rogue_ShouldSkipAssignNicknameYesNoMessage())
            ScriptContext_SetupScript(Rogue_ForceNicknameMon);
        else
            ScriptContext_SetupScript(Rogue_AskNicknameMon);
        return TRUE;
    }
    else if(!RogueDebug_GetConfigToggle(DEBUG_TOGGLE_ALLOW_SAVE_SCUM) && gRogueLocal.hasQuickLoadPending)
    {
        gRogueLocal.hasQuickLoadPending = FALSE;

        VarSet(VAR_0x8004, gRogueRun.isQuickSaveValid);
        gRogueRun.isQuickSaveValid = FALSE;

        ScriptContext_SetupScript(Rogue_QuickSaveLoad);
        return TRUE;
    }
    else if(gRogueLocal.hasPendingSnagBattle)
    {
        gRogueLocal.hasPendingSnagBattle = FALSE;
        gSpecialVar_0x800A = RogueRandom() % gEnemyPartyCount;
        gSpecialVar_0x800B = GetMonData(&gEnemyParty[gSpecialVar_0x800A], MON_DATA_SPECIES);
        StringCopy_Nickname(gStringVar1, RoguePokedex_GetSpeciesName(gSpecialVar_0x800B));
        ScriptContext_SetupScript(Rogue_EventScript_AttemptSnagBattle);
        return TRUE;
    }
    else if(gRogueLocal.hasPendingRidemonTrappedCheck && !Rogue_IsRideMonFlying()) // wait until we've landed to run the script
    {
        gRogueLocal.hasPendingRidemonTrappedCheck = FALSE;

        if(Rogue_ShouldRunRidemonTrappedScript())
        {
            ScriptContext_SetupScript(Rogue_Ridemon_PlayerIsTrapped);
            return TRUE;
        }

        return FALSE;
    }
    else if(RogueMP_IsActive() && RogueMP_TryExecuteScripts())
    {
        // Do nothing here, as we expect any update to be done above
        return TRUE;
    }
    else if(FollowMon_ProcessMonInteraction() == TRUE)
    {
        return TRUE;
    }
    else if(Rogue_HandleRideMonInput() == TRUE)
    {
        return TRUE;
    }

    return FALSE;
}

#ifdef ROGUE_FEATURE_HOT_TRACKING
static hot_track_dat HotTrackingRtcToCounter(struct SiiRtcInfo* rtc)
{
    return RtcGetDayCount(rtc) * 24 * 60 * 60 +
           ConvertBcdToBinary(rtc->hour) * 60 * 60 +
           ConvertBcdToBinary(rtc->minute) * 60 +
           ConvertBcdToBinary(rtc->second);
}

static hot_track_dat HotTrackingLocalRtcToCounter(void)
{
    struct SiiRtcInfo localRtc;
    RtcGetRawInfo(&localRtc);

    return HotTrackingRtcToCounter(&localRtc);
}
#endif

static void ResetHotTracking()
{
#ifdef ROGUE_FEATURE_HOT_TRACKING
    gRogueHotTracking.initSeed = HotTrackingLocalRtcToCounter();
    gRogueHotTracking.rollingSeed = gRogueHotTracking.initSeed;
    gRogueHotTracking.triggerCount = 0;
    gRogueHotTracking.triggerMin = (hot_track_dat)-1;
    gRogueHotTracking.triggerMax = 0;
    gRogueHotTracking.triggerAccumulation = 0;

    DebugPrintf("HotTracking init:%d roll:%d", gRogueHotTracking.initSeed, gRogueHotTracking.rollingSeed);
#endif
}

static void UpdateHotTracking()
{
#ifdef ROGUE_FEATURE_HOT_TRACKING
    hot_track_dat localCounter = HotTrackingLocalRtcToCounter();
    hot_track_dat rollingCounter = localCounter - gRogueHotTracking.rollingSeed;
    DEBUG_CODE(hot_track_dat seedCounter = localCounter - gRogueHotTracking.initSeed);

    if(rollingCounter > 1)
    {
        gRogueHotTracking.initSeed = localCounter;
        gRogueHotTracking.rollingSeed = localCounter;

        ++gRogueHotTracking.triggerCount;
        gRogueHotTracking.triggerMin = min(gRogueHotTracking.triggerMin, rollingCounter);
        gRogueHotTracking.triggerMax = max(gRogueHotTracking.triggerMax, rollingCounter);
        gRogueHotTracking.triggerAccumulation += rollingCounter;

        DebugPrintf("HotTracking trigger:%d roll:%d trigger:%d", seedCounter, rollingCounter, gRogueHotTracking.triggerCount);
    }
    else
    {
        gRogueHotTracking.rollingSeed = localCounter;
    }
#endif
}

void Rogue_MainInit(void)
{
    u32 i;

    for(i = 0; i < OBJ_EVENT_ID_MULTIPLAYER_COUNT; ++i)
        gRogueLocal.cachedObjIds[i] = OBJECT_EVENTS_COUNT;

    ResetHotTracking();

    FollowMon_ClearCachedPartnerSpecies();

    RogueQuery_Init();
    Rogue_RideMonInit();
    Rogue_AssistantInit();

#ifdef ROGUE_FEATURE_AUTOMATION
    Rogue_AutomationInit();
#endif

    RogueDebug_MainInit();
}

void Rogue_MainEarlyCB(void)
{
    // Want to process before overworld update
    Rogue_AssistantMainCB();
}

void Rogue_MainLateCB(void)
{
    //Additional 3rd maincallback which is always called

    if(Rogue_GetActiveCampaign() != ROGUE_CAMPAIGN_NONE)
    {
        UpdateHotTracking();
    }

#ifdef ROGUE_FEATURE_AUTOMATION
    Rogue_AutomationCallback();
#endif
    RogueDebug_MainCB();
}

static void ClearRogueLocalData(void)
{
    memset(&gRogueLocal, 0, sizeof(gRogueLocal));
}

static void TryAutoItemPickup(void)
{
    u8 i, elevation, direction;
    s16 x, y;
    struct ObjectEventTemplate *template;

    if (ScriptContext_IsEnabled() || ArePlayerFieldControlsLocked())
        return;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    elevation = PlayerGetElevation();
    direction = GetPlayerFacingDirection();

    // Normal interactions look one tile beyond counters so objects placed on
    // tables and shop counters can be reached from the other side. Auto Pickup
    // must resolve the same target instead of stopping on the counter itself.
    if (MetatileBehavior_IsCounter(MapGridGetMetatileBehaviorAt(x, y)))
    {
        x += gDirectionToVectors[direction].x;
        y += gDirectionToVectors[direction].y;
    }

    if ((gRogueLocal.autoPickupLastX == x && gRogueLocal.autoPickupLastY == y) || Rogue_IsRideMonFlying())
        return;

    if (Rogue_IsRunActive()
     && (gRogueAdvPath.currentRoomType == ADVPATH_ROOM_DARK_DEAL
      || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LAB))
        return;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (!gObjectEvents[i].active || i == gPlayerAvatar.objectEventId)
            continue;

        if (gObjectEvents[i].currentCoords.x != x
         || gObjectEvents[i].currentCoords.y != y
         || gObjectEvents[i].currentElevation != elevation
         || gObjectEvents[i].invisible)
            continue;

        template = GetBaseTemplateForObjectEvent(&gObjectEvents[i]);
        if (template != NULL)
        {
            if (template->flagId >= FLAG_ROGUE_ITEM_START && template->flagId <= FLAG_ROGUE_ITEM_END)
            {
                u16 amount;
                u16 idx = template->flagId - FLAG_ROGUE_ITEM_START;
                u16 itemId = SanitizeRouteRewardItem(VarGet(VAR_ROGUE_ITEM_START + idx));

                VarSet(VAR_0x8001, itemId);
                VarSet(VAR_ROGUE_ITEM_START + idx, itemId);
                amount = Rogue_ModifyItemPickupAmount(itemId, 1);

                if (AddBagItem(itemId, amount))
                {
                    Rogue_PushPopup_AddItem(itemId, amount);
                    RemoveObjectEventByLocalIdAndMap(gObjectEvents[i].localId, gObjectEvents[i].mapNum, gObjectEvents[i].mapGroup);
                }
            }
            else if (template->graphicsId == OBJ_EVENT_GFX_ITEM_RARE_CANDY || template->graphicsId == OBJ_EVENT_GFX_ITEM_MASTER_BALL)
            {
                u16 amount;
                u16 itemId = ITEM_NONE;

                switch (template->graphicsId)
                {
                case OBJ_EVENT_GFX_ITEM_RARE_CANDY:
                    itemId = ITEM_RARE_CANDY;
                    break;

                case OBJ_EVENT_GFX_ITEM_MASTER_BALL:
                    itemId = ITEM_MASTER_BALL;
                    break;
                }

                VarSet(VAR_0x8001, itemId);
                amount = Rogue_ModifyItemPickupAmount(itemId, 1);

                if (AddBagItem(itemId, amount))
                {
                    Rogue_PushPopup_AddItem(itemId, amount);
                    RemoveObjectEventByLocalIdAndMap(gObjectEvents[i].localId, gObjectEvents[i].mapNum, gObjectEvents[i].mapGroup);
                }
            }
            else if (template->movementType == MOVEMENT_TYPE_BERRY_TREE_GROWTH)
            {
                u16 stage;

                gSelectedObjectEvent = i;
                gSpecialVar_LastTalked = gObjectEvents[i].localId;

                ObjectEventInteractionGetBerryTreeData();
                stage = gSpecialVar_0x8004;

                if (stage == BERRY_STAGE_BERRIES)
                {
                    u8 id = GetObjectEventBerryTreeId(i);
                    u16 berry = GetBerryTypeByBerryTreeId(id);
                    u16 itemId = BerryTypeToItemId(berry);
                    u16 amount = Rogue_ModifyItemPickupAmount(itemId, GetBerryCountByBerryTreeId(id));

                    if (AddBagItem(itemId, amount))
                    {
                        Rogue_PushPopup_AddItem(itemId, amount);
                        ObjectEventInteractionRemoveBerryTree();
                    }
                }
            }
            else if (template->graphicsId == OBJ_EVENT_GFX_BREAKABLE_ROCK
                  || template->graphicsId == OBJ_EVENT_GFX_CUTTABLE_TREE
                  || template->graphicsId == OBJ_EVENT_GFX_PUSHABLE_BOULDER)
            {
                // Once Strength is active, movement handles pushing boulders.
                // Re-running their interaction script only repeats the reminder popup.
                if (template->graphicsId != OBJ_EVENT_GFX_PUSHABLE_BOULDER || !FlagGet(FLAG_SYS_USE_STRENGTH))
                {
                    gSelectedObjectEvent = i;
                    gSpecialVar_LastTalked = gObjectEvents[i].localId;
                    ScriptContext_SetupScript(template->script);
                }
            }
        }

        break;
    }

    gRogueLocal.autoPickupLastX = x;
    gRogueLocal.autoPickupLastY = y;
}

void Rogue_OverworldCB(u16 newKeys, u16 heldKeys, bool8 inputActive)
{
    if(inputActive)
    {
        if(!(gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE | PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER | PLAYER_AVATAR_FLAG_CONTROLLABLE)))
        {
            // Update running toggle
            if(gSaveBlock2Ptr->optionsAutoRunToggle && (newKeys & B_BUTTON) != 0)
            {
                gSaveBlock2Ptr->optionsAutoRunActive = !gSaveBlock2Ptr->optionsAutoRunActive;
            }
        }

        if(gSaveBlock2Ptr->optionsItemPickupAutomatic)
            TryAutoItemPickup();
    }

    START_TIMER(ROGUE_ASSISTANT_CALLBACK);
    Rogue_AssistantOverworldCB();
    STOP_TIMER(ROGUE_ASSISTANT_CALLBACK);
}

void Rogue_OnReturnToField()
{
    if(!Rogue_IsRunActive())
    {
        RogueHub_ReloadObjectsAndTiles();
    }
}

bool8 Rogue_IsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider)
{
    if(FollowMon_IsItemRoomSableyeObject(obstacle) || FollowMon_IsItemRoomSableyeObject(collider))
        return FALSE;

    if(Rogue_RideMonIsCollisionExempt(obstacle, collider))
        return TRUE;

    if(RogueRouteScenes_IsBreedersExchangePokemonObject(obstacle)
        || RogueRouteScenes_IsBreedersExchangePokemonObject(collider))
        return FALSE;

    if(FollowMon_IsCollisionExempt(obstacle, collider))
        return TRUE;

    if (obstacle->localId >= OBJ_EVENT_ID_MULTIPLAYER_FIRST && obstacle->localId <= OBJ_EVENT_ID_MULTIPLAYER_LAST)
    {
        // Don't collide in the map screen
        if(RogueAdv_IsViewingPath()) //Rogue_IsRunActive())
            return TRUE;
    }

    return FALSE;
}

bool8 Rogue_IsRunningToggledOn()
{
    return gSaveBlock2Ptr->optionsAutoRunActive;
}

#define ITEM_ROOM_OVERWORLD_ICON_TAG 0xF531

static bool8 IsItemRoomRewardObject(struct ObjectEvent *objectEvent)
{
    return gMapHeader.mapLayoutId == LAYOUT_ROGUE_ENCOUNTER_ITEM_ROOM
        && objectEvent->graphicsId == OBJ_EVENT_GFX_ITEM_HOLD_ITEM;
}

static void SpriteCB_ItemRoomOverworldIcon(struct Sprite *sprite)
{
    u8 objectEventId = sprite->data[0];

    if(objectEventId >= OBJECT_EVENTS_COUNT
        || !gObjectEvents[objectEventId].active
        || !IsItemRoomRewardObject(&gObjectEvents[objectEventId]))
    {
        sprite->invisible = TRUE;
        return;
    }
    else
    {
        struct ObjectEvent *objectEvent = &gObjectEvents[objectEventId];
        struct Sprite *objectSprite = &gSprites[objectEvent->spriteId];

        // The visible icon occupies the top-left 24x24 pixels of a 32x32
        // sprite. Centre it horizontally and lift it onto the plinth.
        sprite->x = objectSprite->x + 4;
        sprite->y = objectSprite->y;
        sprite->coordOffsetEnabled = objectSprite->coordOffsetEnabled;
        sprite->oam.priority = objectSprite->oam.priority;
        sprite->subpriority = objectSprite->subpriority;
        sprite->invisible = objectEvent->offScreen;

        // Retain the object event as the solid, interactable anchor without
        // drawing its generic held-item marker underneath the real icon.
        // Store this on the object event as well as the sprite: overworld
        // visibility updates can rewrite a sprite's `invisible` bit while a
        // field menu is open, so changing only the sprite lets the generic
        // held-item marker reappear when the preview closes.
        objectEvent->invisible = TRUE;
        objectSprite->invisible = TRUE;
    }
}

static void DestroyItemRoomOverworldIcon(void)
{
    u8 spriteId;

    for(spriteId = 0; spriteId < MAX_SPRITES; ++spriteId)
    {
        if(gSprites[spriteId].inUse
            && gSprites[spriteId].callback == SpriteCB_ItemRoomOverworldIcon)
        {
            DestroySprite(&gSprites[spriteId]);
        }
    }

    FreeSpriteTilesByTag(ITEM_ROOM_OVERWORLD_ICON_TAG);
    FreeSpritePaletteByTag(ITEM_ROOM_OVERWORLD_ICON_TAG);
}

static void CreateItemRoomOverworldIcon(u8 objectEventId)
{
    u16 itemId = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA);
    u8 spriteId;

    DestroyItemRoomOverworldIcon();
    if(itemId == ITEM_NONE)
        return;

    spriteId = AddItemIconSprite(
        ITEM_ROOM_OVERWORLD_ICON_TAG,
        ITEM_ROOM_OVERWORLD_ICON_TAG,
        itemId
    );
    if(spriteId == MAX_SPRITES)
        return;

    gSprites[spriteId].data[0] = objectEventId;
    gSprites[spriteId].callback = SpriteCB_ItemRoomOverworldIcon;
    SpriteCB_ItemRoomOverworldIcon(&gSprites[spriteId]);
}

void Rogue_OnSpawnObjectEvent(struct ObjectEvent *objectEvent, u8 objectEventId)
{
    if(FollowMon_IsMonObject(objectEvent, TRUE))
    {
        FollowMon_OnObjectEventSpawned(objectEvent);
    }

    if(IsItemRoomRewardObject(objectEvent))
        CreateItemRoomOverworldIcon(objectEventId);

    if (objectEvent->localId >= OBJ_EVENT_ID_MULTIPLAYER_FIRST && objectEvent->localId <= OBJ_EVENT_ID_MULTIPLAYER_LAST)
    {
        gRogueLocal.cachedObjIds[objectEvent->localId - OBJ_EVENT_ID_MULTIPLAYER_FIRST] = objectEventId;
    }
}

void Rogue_OnRemoveObjectEvent(struct ObjectEvent *objectEvent)
{
    if(FollowMon_IsMonObject(objectEvent, TRUE))
    {
        FollowMon_OnObjectEventRemoved(objectEvent);
    }

    if(IsItemRoomRewardObject(objectEvent))
        DestroyItemRoomOverworldIcon();

    if (objectEvent->localId >= OBJ_EVENT_ID_MULTIPLAYER_FIRST && objectEvent->localId <= OBJ_EVENT_ID_MULTIPLAYER_LAST)
    {
        gRogueLocal.cachedObjIds[objectEvent->localId - OBJ_EVENT_ID_MULTIPLAYER_FIRST] = OBJECT_EVENTS_COUNT;
    }
}

void Rogue_OnMovementType_Player(struct Sprite *sprite)
{
    //Rogue_UpdateRideMons();
}

void Rogue_OnObjectEventMovement(u8 objectEventId)
{
    Rogue_HandleRideMonMovementIfNeeded(objectEventId);
}

void Rogue_OnResumeMap()
{
}

void Rogue_OnObjectEventsInit()
{
    u32 i;

    gRogueLocal.hasPendingRidemonTrappedCheck = TRUE;
    SetupFollowParterMonObjectEvent();

    // Clear
    for(i = 0; i < OBJ_EVENT_ID_MULTIPLAYER_COUNT; ++i)
        gRogueLocal.cachedObjIds[i] = OBJECT_EVENTS_COUNT;

    // Repopulate with existing object events
    for(i = 0; i < OBJECT_EVENTS_COUNT; ++i)
    {
        if(gObjectEvents[i].active && gObjectEvents[i].localId >= OBJ_EVENT_ID_MULTIPLAYER_FIRST && gObjectEvents[i].localId <= OBJ_EVENT_ID_MULTIPLAYER_LAST)
            gRogueLocal.cachedObjIds[gObjectEvents[i].localId - OBJ_EVENT_ID_MULTIPLAYER_FIRST] = i;
    }
}

void Rogue_OnResetAllSprites()
{
    Rogue_OnResetRideMonSprites();
}

bool8 Rogue_TryGetCachedObjectEventId(u32 localId, u8* eventObjectId)
{
    if (localId >= OBJ_EVENT_ID_MULTIPLAYER_FIRST && localId <= OBJ_EVENT_ID_MULTIPLAYER_LAST)
    {
        *eventObjectId = gRogueLocal.cachedObjIds[localId - OBJ_EVENT_ID_MULTIPLAYER_FIRST];
        return TRUE;
    }

    *eventObjectId = OBJECT_EVENTS_COUNT;
    return FALSE;
}

void Rogue_GetHotTrackingData(u16* count, u16* average, u16* min, u16* max)
{
#ifdef ROGUE_FEATURE_HOT_TRACKING
    *count = gRogueHotTracking.triggerCount;
    *average = gRogueHotTracking.triggerAccumulation / gRogueHotTracking.triggerCount;
    *min = gRogueHotTracking.triggerMin;
    *max = gRogueHotTracking.triggerMax;
#else
    *count = 0;
    *average = 0;
    *min = 0;
    *max = 0;
#endif
}


void Rogue_OnLoadMap(void)
{
    if(GetSafariZoneFlag())
    {
        // Reset preview data
        memset(&gRogueLocal.encounterPreview[0], 0, sizeof(gRogueLocal.encounterPreview));
        Rogue_ClearPopupQueue();

        RandomiseSafariWildEncounters();
        //Rogue_PushPopup(POPUP_MSG_SAFARI_ENCOUNTERS, 0);
    }
    else if(Rogue_IsRunActive())
    {
        if(RogueAdv_IsViewingPath())
            RogueAdv_ApplyAdventureMetatiles();
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
            RogueRouteScenes_ApplyMetatiles();
    }
    else if(!Rogue_IsRunActive())
    {
        // Apply metatiles for the map we're in
        RogueHub_UpdateWarpStates();
        RogueHub_ApplyMapMetatiles();
    }
}

bool8 Rogue_ShouldSkipReloadMapTileView()
{
    if(!Rogue_IsRunActive())
    {
        // If actually updated tiles only?
        return TRUE;
    }

    return FALSE;
}

u16 GetStartDifficulty(void)
{
    u16 skipToDifficulty = 0;

    if(RogueDebug_GetConfigRange(DEBUG_RANGE_START_DIFFICULTY) != 0)
    {
        skipToDifficulty = RogueDebug_GetConfigRange(DEBUG_RANGE_START_DIFFICULTY);
    }

    return skipToDifficulty;
}

static bool8 HasAnyActiveEvos(u16 species)
{
    u8 i;
    struct Evolution evo;
    u8 evoCount = Rogue_GetMaxEvolutionCount(species);

    for(i = 0; i < evoCount; ++i)
    {
        Rogue_ModifyEvolution(species, i, &evo);

        if(evo.targetSpecies != SPECIES_NONE)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static void GiveMonPartnerRibbon(void)
{
    u8 i;
    u16 species;
    bool8 ribbonSet = TRUE;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        if(species != SPECIES_NONE)
        {
            SetMonData(&gPlayerParty[i], MON_DATA_TEMP_PARTNER_RIBBON, &ribbonSet);

            if(Rogue_GetMaxEvolutionCount(species) != 0 && !HasAnyActiveEvos(species))
                Rogue_PushPopup_UnableToEvolve(i);
        }
    }
}

bool8 Rogue_IsPartnerMonInTeam(void)
{
    u8 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(&gPlayerParty[i], MON_DATA_HP) != 0)
        {
            if(GetMonData(&gPlayerParty[i], MON_DATA_TEMP_PARTNER_RIBBON))
                return TRUE;
        }
    }

    return FALSE;
}

static u16 CalculateRewardLvlMonCount()
{
    u8 i;
    u16 validCount = 0;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(&gPlayerParty[i], MON_DATA_LEVEL) != MAX_LEVEL)
        {
            ++validCount;
        }
    }

    return validCount;
}

u16 Rogue_PostRunRewardLvls()
{
    u16 lvlCount = 2;
    u16 targettedMons = CalculateRewardLvlMonCount();

    if(targettedMons > 1)
    {
        // Only give 1 lvl per mon
        lvlCount = 1;
    }

    lvlCount *= (Rogue_GetCurrentDifficulty() + gRogueRun.victoryLapTotalWins);

    if(lvlCount != 0)
    {
        u8 i, j;
        u32 exp;
        u16 daycareLvls = lvlCount;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            // Award levels
            for(j = 0; j < lvlCount; ++j)
            {
                if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(&gPlayerParty[i], MON_DATA_LEVEL) != MAX_LEVEL)
                {
                    exp = Rogue_ModifyExperienceTables(gRogueSpeciesInfo[GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL)].growthRate, GetMonData(&gPlayerParty[i], MON_DATA_LEVEL, NULL) + 1);
                    SetMonData(&gPlayerParty[i], MON_DATA_EXP, &exp);
                    CalculateMonStats(&gPlayerParty[i]);
                }

                // Increase friendship from these levels
                AdjustFriendship(&gPlayerParty[i], FRIENDSHIP_EVENT_GROW_LEVEL);
            }

            // Increase hub tutor move access for party Pokémon after winning a run.
            if(Rogue_GetCurrentDifficulty() >= ROGUE_MAX_BOSS_COUNT)
            {
                u8 tutorMoveLvl = GetMonData(&gPlayerParty[i], MON_DATA_TUTOR_MOVE_LVL);

                if(tutorMoveLvl + 1 < TUTOR_MOVE_LVL_COUNT_HUB)
                {
                    ++tutorMoveLvl;
                    SetMonData(&gPlayerParty[i], MON_DATA_TUTOR_MOVE_LVL, &tutorMoveLvl);
                }
            }
        }

        // Daycare
        if(RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_EXP_SHARE2))
        {
            daycareLvls = max(1, daycareLvls);
        }
        else if(RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_EXP_SHARE1))
        {
            daycareLvls = max(1, daycareLvls / 2);
        }
        else if(RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_EXP_SHARE0))
        {
            daycareLvls = max(1, daycareLvls / 4);
        }
        else
        {
            daycareLvls = 0;
        }

        if(daycareLvls != 0)
        {
            u8 maxSlots = Rogue_GetCurrentDaycareSlotCount();

            for(i = 0; i < maxSlots; ++i)
            {
                struct BoxPokemon* boxMon = Rogue_GetDaycareBoxMon(i);
                struct Pokemon* tempMon = &gEnemyParty[PARTY_SIZE - 1];

                BoxMonToMon(boxMon, tempMon);

                // Award levels
                for(j = 0; j < daycareLvls; ++j)
                {
                    if(GetMonData(tempMon, MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(tempMon, MON_DATA_LEVEL) != MAX_LEVEL)
                    {
                        exp = Rogue_ModifyExperienceTables(gRogueSpeciesInfo[GetMonData(tempMon, MON_DATA_SPECIES, NULL)].growthRate, GetMonData(tempMon, MON_DATA_LEVEL, NULL) + 1);
                        SetMonData(tempMon, MON_DATA_EXP, &exp);
                        CalculateMonStats(tempMon);
                    }

                    // don't give friendship for daycare mons
                }

                CopyMon(boxMon, &tempMon->box, sizeof(struct BoxPokemon));
            }
        }
    }

    return lvlCount;
}

u16 Rogue_PostRunRewardMoney()
{
    u32 amount = 0;

    if(gRogueRun.enteredRoomCounter > 1)
    {
        u16 i = gRogueRun.victoryLapTotalWins + gRogueRun.enteredRoomCounter - 1;

        switch (Rogue_GetDifficultyRewardLevel())
        {
        case DIFFICULTY_LEVEL_EASY:
            amount = i * 200;
            break;

        case DIFFICULTY_LEVEL_AVERAGE:
            amount = i * 250;
            break;

        case DIFFICULTY_LEVEL_HARD:
            amount = i * 300;
            break;

        case DIFFICULTY_LEVEL_BRUTAL:
            amount = i * 350;
            break;
        }
    }

    AddMoney(&gSaveBlock1Ptr->money, amount);
    return amount;
}

static u16 ChooseRunRewardPokeblock(void);

#define POKEBLOCKS_PER_SMALL_BUNDLE 2
#define POKEBLOCKS_PER_BIG_BUNDLE 5

u16 Rogue_PostRunRewardPokeblocks()
{
    u16 i;
    u16 count = 0;
    u16 bundleRewardCount = gRogueRun.pendingPokeblockBundleRewardCount;

    for(i = 0; i < ARRAY_COUNT(gRogueRun.completedBadges); ++i)
    {
        if(gRogueRun.completedBadges[i] != TYPE_NONE && AddBagItem(ChooseRunRewardPokeblock(), 1))
            ++count;
    }

    gRogueRun.pendingPokeblockBundleRewardCount = 0;

    for(i = 0; i < bundleRewardCount; ++i)
    {
        if(AddBagItem(ChooseRunRewardPokeblock(), 1))
            ++count;
    }

    return count;
}

static struct Pokemon* GetLabMon(u8 slot)
{
    AGB_ASSERT(slot < LAB_MON_COUNT);
    return (struct Pokemon*)&gRogueRun.labParty[slot];
}

static void ResetFaintedLabMonAtSlot(u16 slot)
{
    u16 species;

    struct Pokemon* mon = GetLabMon(slot);

    species = VarGet(VAR_ROGUE_STARTER0 + slot);

    if(species == VarGet(VAR_STARTER_SWAP_SPECIES))
    {
        species = SPECIES_SUNKERN;
    }

    CreateMonWithNature(mon, species, STARTER_MON_LEVEL, USE_RANDOM_IVS, Random() % NUM_NATURES);
}

static void InitialiseFaintedLabMons(void)
{
    u16 i;
    for(i = 0; i < LAB_MON_COUNT; ++i)
    {
        ResetFaintedLabMonAtSlot(i);
    }
}

static u16 GetActiveWeakLegendary(bool8* fromDaycare)
{
    u16 i;
    *fromDaycare = FALSE;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        if(species != SPECIES_NONE && RoguePokedex_IsSpeciesLegendary(species))
        {
            return species;
        }
    }

    for(i = 0; i < Rogue_GetCurrentDaycareSlotCount(); ++i)
    {
        u16 species = GetBoxMonData(Rogue_GetDaycareBoxMon(i), MON_DATA_SPECIES);
        if(species != SPECIES_NONE && RoguePokedex_IsSpeciesLegendary(species))
        {
            *fromDaycare = TRUE;
            return species;
        }
    }

    return SPECIES_NONE;
}

static u16 GetActiveStrongLegendary(bool8* fromDaycare)
{
    u16 i;
    *fromDaycare = FALSE;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if(species != SPECIES_NONE && RoguePokedex_IsSpeciesLegendary(species))
        {
            if(Rogue_CheckMonFlags(species, MON_FLAG_STRONG_WILD))
            {
                return species;
            }
        }
    }

    for(i = 0; i < Rogue_GetCurrentDaycareSlotCount(); ++i)
    {
        u16 species = GetBoxMonData(Rogue_GetDaycareBoxMon(i), MON_DATA_SPECIES);
        if(species != SPECIES_NONE && RoguePokedex_IsSpeciesLegendary(species))
        {
            if(Rogue_CheckMonFlags(species, MON_FLAG_STRONG_WILD))
            {
                *fromDaycare = TRUE;
                return species;
            }
        }
    }

    return SPECIES_NONE;
}

static bool8 CanBringInHeldItem(u16 itemId)
{
    switch(itemId)
    {
        case ITEM_SMALL_COIN_CASE:
        case ITEM_LARGE_COIN_CASE:
            return FALSE;
    }

    return TRUE;
}

static void BeginRogueRun_ModifyParty(void)
{
    u16 starterSpecies = VarGet(VAR_STARTER_SWAP_SPECIES);
    u32 startLevel = STARTER_MON_LEVEL;
    bool8 hasFixedStartingParty;

    if(Rogue_GetModeRules()->initialLevelOverride != 0)
        startLevel = Rogue_GetModeRules()->initialLevelOverride;

    FlagClear(FLAG_ROGUE_HAS_RANDOM_STARTER);
    hasFixedStartingParty = RogueTrial_ApplyFixedStartingParty(startLevel);

    if(!hasFixedStartingParty && starterSpecies != SPECIES_NONE)
    {
        FlagSet(FLAG_ROGUE_HAS_RANDOM_STARTER);
        ClearPlayerTeam();

        CreateMon(&gEnemyParty[0], starterSpecies, STARTER_MON_LEVEL, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

        GiveMonToPlayer(&gEnemyParty[0]);
        CalculatePlayerPartyCount();

        if(!Rogue_ShouldSkipAssignNickname(&gPlayerParty[0]))
            gRogueLocal.hasNicknameMonMsgPending = TRUE;
    }

    // Always clear out EVs as we shouldn't have them in the HUB anymore
    {
        u16 i;
        u16 temp = 0;
        u32 exp;
        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
            if(species != SPECIES_NONE)
            {
                temp = 0;
                SetMonData(&gPlayerParty[i], MON_DATA_HP_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_ATK_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_DEF_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_SPEED_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_SPATK_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_SPDEF_EV, &temp);
                gPlayerParty[i].rogueExtraData.runTutorMoveLvl = 0;
                gPlayerParty[i].rogueExtraData.abilityOverride = ABILITY_NONE;

                // Force to starter lvl
                exp = Rogue_ModifyExperienceTables(gRogueSpeciesInfo[GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL)].growthRate, startLevel);
                SetMonData(&gPlayerParty[i], MON_DATA_EXP, &exp);

                if(hasFixedStartingParty || starterSpecies != SPECIES_NONE)
                {
                    // This mon was just added so it can appear in the safari
                }
                else
                {
                    // Partner's can't reappear in safari
                    gPlayerParty[i].rogueExtraData.isSafariIllegal = TRUE;
                }

                // Adjust item
                temp = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
                if(!CanBringInHeldItem(temp))
                {
                    temp = ITEM_NONE;
                    SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &temp);
                }
                else if(!Rogue_TryRemoveDuplicateHeldItemForParty(&gPlayerParty[i], i, PARTY_SIZE))
                {
                    temp = ITEM_NONE;
                    SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &temp);
                }

                CalculateMonStats(&gPlayerParty[i]);
            }
        }

        // Update daycare mons
        for(i = 0; i < DAYCARE_SLOT_COUNT; ++i)
        {
            struct BoxPokemon* boxMon = Rogue_GetDaycareBoxMon(i);
            u16 species = GetBoxMonData(boxMon, MON_DATA_SPECIES);

            if(species != SPECIES_NONE)
            {
                u32 exp = Rogue_ModifyExperienceTables(gRogueSpeciesInfo[species].growthRate, startLevel);
                SetBoxMonData(boxMon, MON_DATA_EXP, &exp);

                temp = 0;
                SetBoxMonData(boxMon, MON_DATA_HP_EV, &temp);
                SetBoxMonData(boxMon, MON_DATA_ATK_EV, &temp);
                SetBoxMonData(boxMon, MON_DATA_DEF_EV, &temp);
                SetBoxMonData(boxMon, MON_DATA_SPEED_EV, &temp);
                SetBoxMonData(boxMon, MON_DATA_SPATK_EV, &temp);
                SetBoxMonData(boxMon, MON_DATA_SPDEF_EV, &temp);

                // Adjust item
                temp = GetBoxMonData(boxMon, MON_DATA_HELD_ITEM);
                if(!CanBringInHeldItem(temp))
                {
                    temp = ITEM_NONE;
                    SetBoxMonData(boxMon, MON_DATA_HELD_ITEM, &temp);
                }
            }

            gRogueSaveBlock->daycarePokemon[i].isSafariIllegal = TRUE;
        }
    }
}

static void BeginRogueRun_ConsiderItems(void)
{
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 startClock = RogueDebug_SampleClock();
#endif
    DebugPrintf("Evos: required bits %d, avaliable bits %d", gRogueBake_EvoItems_Count, ARRAY_COUNT(gRogueRun.activeEvoItemFlags) * 8);
    AGB_ASSERT(gRogueBake_EvoItems_Count <= ARRAY_COUNT(gRogueRun.activeEvoItemFlags) * 8);

    memset(&gRogueRun.activeEvoItemFlags, 0, sizeof(gRogueRun.activeEvoItemFlags));

#ifdef ROGUE_EXPANSION
    DebugPrintf("Forms: required bits %d, avaliable bits %d", gRogueBake_FormItems_Count, ARRAY_COUNT(gRogueRun.activeFormItemFlags) * 8);
    AGB_ASSERT(gRogueBake_FormItems_Count <= ARRAY_COUNT(gRogueRun.activeFormItemFlags) * 8);

    memset(&gRogueRun.activeFormItemFlags, 0, sizeof(gRogueRun.activeFormItemFlags));
#endif

    // Go through all active evo/forms and figure out what items we want to enable
    {
        struct Evolution evo;
        u16 e, evoCount, species;

        // Always enable these as they have a non mon specific held effect
        SetEvolutionItemFlag(ITEM_KINGS_ROCK, TRUE);
        SetEvolutionItemFlag(ITEM_METAL_COAT, TRUE);
#ifdef ROGUE_EXPANSION
        SetEvolutionItemFlag(ITEM_RAZOR_CLAW, TRUE);
#endif

        // Always enable? as we may get an evo curse
        //SetEvoFormItemFlag(ITEM_LINK_CABLE, TRUE);

        for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
        {
            if(Query_IsSpeciesEnabledForceDexChecking(species))
            {
                evoCount = Rogue_GetMaxEvolutionCount(species);

                for(e = 0; e < evoCount; ++e)
                {
                    Rogue_ModifyEvolution(species, e, &evo);

                    switch (evo.method)
                    {
                    case EVO_ITEM:
#ifdef ROGUE_EXPANSION
                    case EVO_ITEM_MALE:
                    case EVO_ITEM_FEMALE:
#endif
                        SetEvolutionItemFlag(evo.param, TRUE);
                        break;
                    }
                }
            }
        }
    }

#ifdef ROGUE_EXPANSION
    {
        struct FormChange form;
        u16 species, e;

        for (species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
        {
            if(Query_IsSpeciesEnabledForceDexChecking(species))
            {
                for (e = 0; TRUE; ++e)
                {
                    Rogue_ModifyFormChange(species, e, &form);

                    if(form.method == FORM_CHANGE_TERMINATOR)
                        break;

                    switch (form.method)
                    {
                    case FORM_CHANGE_ITEM_HOLD:
                    case FORM_CHANGE_ITEM_USE:
                    case FORM_CHANGE_BEGIN_BATTLE:
                    case FORM_CHANGE_END_BATTLE:
                    case FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM:
                    case FORM_CHANGE_BATTLE_PRIMAL_REVERSION:
                        if(form.param1 != ITEM_NONE)
                            SetFormItemFlag(form.param1, TRUE);
                        break;
                    }
                }
            }
        }
    }
#endif

#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Run Load] Evolution item setup: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
}

static bool8 CanEnterWithItem(u16 itemId, bool8 isBasicBagEnabled)
{
    u8 pocket;
    if(!isBasicBagEnabled)
        return TRUE;

    pocket = GetPocketByItemId(itemId);
    if(pocket == POCKET_KEY_ITEMS)
        return TRUE;

    return FALSE;
}

static u32 GetAdventureFundStartMoney(void)
{
    switch(VarGet(VAR_ROGUE_ADVENTURE_MONEY_TIER))
    {
        case 1:
            return 10000;

        case 2:
            return 20000;

        case 3:
            return 30000;

        case 4:
            return 40000;

        case 5:
            return 50000;

        default:
            return 0;
    }
}

static void SetupRogueRunBag()
{
    u16 i;
    u16 itemId;
    u32 quantity;
    bool8 isBasicBagEnabled = Rogue_GetConfigToggle(CONFIG_TOGGLE_BAG_WIPE);
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 startClock = RogueDebug_SampleClock();
#endif

    SetMoney(&gSaveBlock1Ptr->money, 0);
    ClearBag();

    // Re-add items
    for(i = 0; i < BAG_ITEM_CAPACITY; ++i)
    {
        itemId = RogueSave_GetHubBagItemIdAt(i);
        quantity = RogueSave_GetHubBagItemQuantityAt(i);

        if(itemId != ITEM_NONE && CanEnterWithItem(itemId, isBasicBagEnabled))
        {
            AddBagItem(itemId, quantity);
        }
    }

    if(!isBasicBagEnabled)
        SetMoney(&gSaveBlock1Ptr->money, GetAdventureFundStartMoney());

    // Give basic inventory
    if(isBasicBagEnabled)
    {
        AddBagItem(ITEM_POKE_BALL, 5);
        AddBagItem(ITEM_POTION, 1);
    }

    RecalcCharmCurseValues();
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Run Load] Bag setup: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
}

enum
{
    BEGIN_RUN_PHASE_RESET,
    BEGIN_RUN_PHASE_WORLD_STATE,
    BEGIN_RUN_PHASE_PARTY_AND_BAG,
    BEGIN_RUN_PHASE_SPECIAL_CLAUSES,
    BEGIN_RUN_PHASE_LEGENDS,
    BEGIN_RUN_PHASE_TEAM_ENCOUNTERS,
    BEGIN_RUN_PHASE_TRAINERS,
    BEGIN_RUN_PHASE_ADVENTURE_PATH,
    BEGIN_RUN_PHASE_MINIBOSS_PREVIEWS,
    BEGIN_RUN_PHASE_FINALIZE,
    BEGIN_RUN_PHASE_COUNT,
};

static void BeginRogueRunPhase_Reset(void)
{
    DebugPrint("BeginRogueRun");

    ClearRogueLocalData();
    memset(&gRogueRun, 0, sizeof(gRogueRun));
    gRogueRun.routeSceneRoomId = ADVPATH_INVALID_ROOM_ID;
    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
    ClearHoneyTreePokeblock();
    ResetHotTracking();

    RogueGift_EnsureDynamicCustomMonsAreValid();
    // Save before applying temporary Trial items so the exact hub inventory is restored after the run.
    RogueSave_SaveHubStates();
    RogueTrial_ApplyPendingSelection();
    RogueTrial_ApplyRunBagItems();
    RogueMonQuery_InvalidateSpeciesActiveCache();

#ifdef ROGUE_EXPANSION
    // Cache the results for the run (Must do before ActiveRun flag is set)
    gRogueRun.megasEnabled = IsMegaEvolutionEnabled();
    gRogueRun.zMovesEnabled = IsZMovesEnabled();
    gRogueRun.dynamaxEnabled = IsDynamaxEnabled();
    gRogueRun.terastallizeEnabled = IsTerastallizeEnabled();
    RogueTrial_ApplyBattleGimmickOverride();
    // CheckBagHasItem(ITEM_DYNAMAX_BAND, 1)
#endif

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_ROGUE_IS_VICTORY_LAP);
    FlagClear(FLAG_ROGUE_MYSTERIOUS_SIGN_KNOWN);
    FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    FlagClear(FLAG_ROGUE_ITEM_ROOM_CLAIMED_0);
    FlagClear(FLAG_ROGUE_ITEM_ROOM_CLAIMED_1);
    FlagClear(FLAG_ROGUE_ITEM_ROOM_CLAIMED_2);

    SetLastHealLocationWarp(HEAL_LOCATION_ROGUE_HUB);
    if (RogueTrial_IsActive())
    {
        gSaveBlock1Ptr->lastHealLocation.mapGroup = MAP_GROUP(ROGUE_AREA_ADVENTURE_ENTRANCE);
        gSaveBlock1Ptr->lastHealLocation.mapNum = MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE);
        gSaveBlock1Ptr->lastHealLocation.warpId = WARP_ID_NONE;
        gSaveBlock1Ptr->lastHealLocation.x = 10;
        gSaveBlock1Ptr->lastHealLocation.y = 7;
    }

    VarSet(VAR_ROGUE_COURIER_ITEM, ITEM_NONE);
    VarSet(VAR_ROGUE_COURIER_COUNT, 0);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, 0);
    FlagClear(FLAG_ROGUE_COURIER_READY);

    gRogueRun.victoryLapTotalWins = 0;
    Rogue_RefillDayCareCharges(FALSE);

    Rogue_PreActivateDesiredCampaign();

    if(RogueMP_IsActive() && RogueMP_IsClient())
    {
        AGB_ASSERT(gRogueMultiplayer != NULL);
        AGB_ASSERT(gRogueMultiplayer->gameState.adventure.isRunActive);

        gRogueRun.baseSeed = gRogueMultiplayer->gameState.adventure.baseSeed;
    }
    else
    {
        struct AdventureReplay const* replay = &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED];

        if(RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_ADVENTURE_REPLAY) && FlagGet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE) && replay->isValid)
        {
            gRogueRun.baseSeed = replay->baseSeed;
            memcpy(&gRogueSaveBlock->difficultyConfig, &replay->difficultyConfig, sizeof(gRogueSaveBlock->difficultyConfig));

            Rogue_PushPopup_AdventureReplay();
        }
        else
        {
            gRogueRun.baseSeed = Random();
            FlagClear(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
        }
    }

    // From this point on, the committed run configuration is authoritative.
    // Drop the temporary review override before deriving run rules from it.
    RogueRunStart_Clear();

    if (Rogue_GetModeRules()->disableMainQuests || AnyCharmsActive())
        FlagSet(FLAG_ROGUE_RUN_MAIN_QUESTS_DISABLED);
    else
        FlagClear(FLAG_ROGUE_RUN_MAIN_QUESTS_DISABLED);

    if (Rogue_GetModeRules()->disableTrialQuests || AnyCharmsActive())
        FlagSet(FLAG_ROGUE_RUN_TRIAL_QUESTS_DISABLED);
    else
        FlagClear(FLAG_ROGUE_RUN_TRIAL_QUESTS_DISABLED);

    Rogue_SetCurrentDifficulty(GetStartDifficulty());
    gRogueRun.currentLevelOffset = Rogue_GetModeRules()->initialLevelOffset;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;

#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Run Gen] reset complete baseSeed=%u mode=%d generator=%d difficulty=%d levelOffset=%d",
        gRogueRun.baseSeed,
        Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM),
        Rogue_GetModeRules()->adventureGenerator,
        Rogue_GetCurrentDifficulty(),
        gRogueRun.currentLevelOffset);
#endif

    if(gRogueRun.currentLevelOffset == 0)
    {
        // Apply default
        gRogueRun.currentLevelOffset = 3; // assume STARTER_MON_LEVEL == 5 and first boss level is 10
    }

    // Apply some base seed for anything which needs to be randomly setup
    SeedRogueRng(gRogueRun.baseSeed * 23151 + 29867);

    {
        u32 i;
        for(i = 0; i < ROGUE_SUBSEED_COUNT; ++i)
            gRogueRun.subSeeds[i] = RogueRandom();
    }

    memset(&gRogueRun.completedBadges[0], TYPE_NONE, sizeof(gRogueRun.completedBadges));
    memset(&gRogueRun.lastShopVisitDifficulty[0], ROGUE_MAX_BOSS_COUNT, sizeof(gRogueRun.lastShopVisitDifficulty));

    VarSet(VAR_ROGUE_DIFFICULTY, Rogue_GetCurrentDifficulty());
    VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, 0);
    VarSet(VAR_ROGUE_DESIRED_WEATHER, WEATHER_NONE);
    VarSet(VAR_ROGUE_TOTAL_RUN_CATCHES, 0);

    VarSet(VAR_ROGUE_FLASK_HEALS_USED, 0);
    VarSet(VAR_ROGUE_FLASK_HEALS_MAX, 3);
}

static void BeginRogueRunPhase_WorldState(void)
{
    ClearBerryTreeRange(BERRY_TREE_ROUTE_FIRST, BERRY_TREE_ROUTE_LAST);
    ClearBerryTreeRange(BERRY_TREE_DAYCARE_FIRST, BERRY_TREE_DAYCARE_LAST);

    if(Rogue_GetConfigToggle(CONFIG_TOGGLE_BAG_WIPE))
    {
        // Clear daycare mons
        u8 i;

        for(i = 0; i < DAYCARE_SLOT_COUNT; ++i)
        {
            struct BoxPokemon* boxMon = Rogue_GetDaycareBoxMon(i);
            ZeroBoxMonData(boxMon);
        }
    }

    RandomiseFishingEncounters();
    RandomiseTRMoves();
    InitialiseFaintedLabMons();
    PlayTimeCounter_Reset();
    PlayTimeCounter_Start();

    BeginRogueRun_ConsiderItems();

    // After we've decided what items are active, cache the rare shop state
    gRogueRun.rareShopEnabled = IsRareShopActiveInternal();
}

static void BeginRogueRunPhase_PartyAndBag(void)
{
    BeginRogueRun_ModifyParty();
    SetupRogueRunBag();
    RogueTrial_ApplyRunBagItems();

    FlagClear(FLAG_ROGUE_FREE_HEAL_USED);
    FlagClear(FLAG_ROGUE_RUN_COMPLETED);
    FlagClear(FLAG_ROGUE_FINAL_QUEST_MET_FAKE_CHAMP);
    FlagClear(FLAG_ROGUE_DYNAMAX_BATTLE);
    FlagClear(FLAG_ROGUE_TERASTALLIZE_BATTLE);
    FlagClear(FLAG_ROGUE_IN_SNAG_BATTLE);

    FlagSet(FLAG_ROGUE_TERA_ORB_CHARGED);

    Rogue_PostActivateDesiredCampaign();

    FlagClear(FLAG_ROGUE_TRAINERS_WEAK_LEGENDARIES);
    FlagClear(FLAG_ROGUE_TRAINERS_STRONG_LEGENDARIES);
}

static void BeginRogueRunPhase_SpecialClauses(void)
{
    if(Rogue_GetConfigRange(CONFIG_RANGE_LEGENDARY) == DIFFICULTY_LEVEL_BRUTAL)
    {
        bool8 weakSpeciesInDaycare;
        bool8 strongSpeciesInDaycare;
        u16 weakSpecies = GetActiveWeakLegendary(&weakSpeciesInDaycare);
        u16 strongSpecies = GetActiveStrongLegendary(&strongSpeciesInDaycare);

        if(weakSpecies != SPECIES_NONE)
            FlagSet(FLAG_ROGUE_TRAINERS_WEAK_LEGENDARIES);

        if(strongSpecies != SPECIES_NONE)
            FlagSet(FLAG_ROGUE_TRAINERS_STRONG_LEGENDARIES);

        if(strongSpecies != SPECIES_NONE)
            Rogue_PushPopup_StrongPokemonClause(strongSpecies, strongSpeciesInDaycare);
        else if(weakSpecies != SPECIES_NONE)
            Rogue_PushPopup_WeakPokemonClause(weakSpecies, weakSpeciesInDaycare);
    }

    GiveMonPartnerRibbon();
}

static void BeginRogueRunPhase_Legends(void)
{
    // Choose legendaries before trainers so rival can avoid these legends
    ChooseLegendarysForNewAdventure();
}

static void BeginRogueRunPhase_TeamEncounters(void)
{
    ChooseTeamEncountersForNewAdventure();
}

static void BeginRogueRunPhase_Trainers(void)
{
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 startClock = RogueDebug_SampleClock();
#endif
    // Choose bosses last
    Rogue_ChooseRivalTrainerForNewAdventure();
    Rogue_EnsureRivalBaseTeamForNewAdventure();
    Rogue_ChooseBossTrainersForNewAdventure();
    Rogue_ChooseFrontierBrainTrainersForNewAdventure();
    EnableRivalEncounterIfRequired();

    gRogueRun.shrineSpawnDifficulty = 1 + RogueRandomRange(ROGUE_MAX_BOSS_COUNT - 1, 0);
    ChooseUniqueDenForNewAdventure();

    RogueSafari_CompactEmptyEntries();
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Run Load] Trainer setup: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
}

static void BeginRogueRunPhase_AdventurePath(void)
{
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Run Gen] adventure path begin difficulty=%d room=%d roomCount=%d",
        Rogue_GetCurrentDifficulty(),
        gRogueRun.adventureRoomId,
        gRogueAdvPath.roomCount);
#endif
    // Build the first path while the portal loading screen is already black so
    // map entry never inherits the remaining generation pause.
    RogueAdv_GenerateAdventurePathsIfRequired();
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Run Gen] adventure path end room=%d roomCount=%d currentType=%d",
        gRogueRun.adventureRoomId,
        gRogueAdvPath.roomCount,
        gRogueAdvPath.currentRoomType);
#endif
}

static void BeginRogueRunPhase_MiniBossPreviews(void)
{
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 startClock = RogueDebug_SampleClock();
#endif

    // Keep full Frontier Brain setup behind the run loading screen so entering
    // a previewed node does not inherit the generation cost.
    RogueAdv_CacheMiniBossPreviews();

#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Run Load] Mini-boss previews: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
}

static void BeginRogueRunPhase_Finalize(void)
{
    IncrementGameStat(GAME_STAT_TOTAL_RUNS);

    // Trigger before and after as we may have hub/run only quests which are interested in this trigger
    RogueQuest_OnTrigger(QUEST_TRIGGER_RUN_START);
    RogueQuest_ActivateQuestsFor(QUEST_CONST_ACTIVE_IN_RUN);
    RogueQuest_OnTrigger(QUEST_TRIGGER_RUN_START);

    Rogue_AddPartySnapshot(-1);

    if(Rogue_ShouldDisableMainQuests())
        Rogue_PushPopup_MainQuestsDisabled();

    if(Rogue_ShouldDisableTrialQuests())
        Rogue_PushPopup_TrialQuestsDisabled();

    gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_MOST_RECENT].isValid = TRUE;
    gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_MOST_RECENT].baseSeed = gRogueRun.baseSeed;

    if(RogueMP_IsActive() && RogueMP_IsClient())
    {
        AGB_ASSERT(gRogueMultiplayer != NULL);
        AGB_ASSERT(gRogueMultiplayer->gameState.adventure.isRunActive);
        memcpy(&gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_MOST_RECENT].difficultyConfig, &gRogueMultiplayer->gameState.hub.difficultyConfig, sizeof(gRogueSaveBlock->difficultyConfig));
    }
    else
    {
        memcpy(&gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_MOST_RECENT].difficultyConfig, &gRogueSaveBlock->difficultyConfig, sizeof(gRogueSaveBlock->difficultyConfig));
    }

    RogueQuest_CheckQuestRequirements();
}

static void BeginRogueRunPhase(u8 phase)
{
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    static u32 sRunLoadStartClock;
    static u32 sRunLoadPhaseClocks[BEGIN_RUN_PHASE_COUNT];
    u32 phaseStartClock;

    if(phase == BEGIN_RUN_PHASE_RESET)
        sRunLoadStartClock = RogueDebug_SampleClock();
    phaseStartClock = RogueDebug_SampleClock();
    DebugPrintf("[Run Load] phase start: %d", phase);
#endif

    switch(phase)
    {
    case BEGIN_RUN_PHASE_RESET:
        BeginRogueRunPhase_Reset();
        break;

    case BEGIN_RUN_PHASE_WORLD_STATE:
        BeginRogueRunPhase_WorldState();
        break;

    case BEGIN_RUN_PHASE_PARTY_AND_BAG:
        BeginRogueRunPhase_PartyAndBag();
        break;

    case BEGIN_RUN_PHASE_SPECIAL_CLAUSES:
        BeginRogueRunPhase_SpecialClauses();
        break;

    case BEGIN_RUN_PHASE_LEGENDS:
        BeginRogueRunPhase_Legends();
        break;

    case BEGIN_RUN_PHASE_TEAM_ENCOUNTERS:
        BeginRogueRunPhase_TeamEncounters();
        break;

    case BEGIN_RUN_PHASE_TRAINERS:
        BeginRogueRunPhase_Trainers();
        break;

    case BEGIN_RUN_PHASE_ADVENTURE_PATH:
        BeginRogueRunPhase_AdventurePath();
        break;

    case BEGIN_RUN_PHASE_MINIBOSS_PREVIEWS:
        BeginRogueRunPhase_MiniBossPreviews();
        break;

    case BEGIN_RUN_PHASE_FINALIZE:
        BeginRogueRunPhase_Finalize();
        break;
    }

#ifdef DEBUG_FEATURE_FRAME_TIMERS
    sRunLoadPhaseClocks[phase] = RogueDebug_SampleClock() - phaseStartClock;
    DebugPrintf("[Run Load] phase end: %d (%d us)", phase, RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[phase]));
    if(phase == BEGIN_RUN_PHASE_FINALIZE)
    {
        DebugPrintf("[Run Load] phases us: reset=%d world=%d bag=%d clauses=%d legends=%d teams=%d trainers=%d path=%d previews=%d final=%d",
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_RESET]),
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_WORLD_STATE]),
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_PARTY_AND_BAG]),
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_SPECIAL_CLAUSES]),
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_LEGENDS]),
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_TEAM_ENCOUNTERS]),
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_TRAINERS]),
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_ADVENTURE_PATH]),
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_MINIBOSS_PREVIEWS]),
            RogueDebug_ClockToDisplayUnits(sRunLoadPhaseClocks[BEGIN_RUN_PHASE_FINALIZE]));
        DebugPrintf("[Run Load] setup total: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - sRunLoadStartClock));
    }
#endif
}

static void BeginRogueRun(void)
{
    u8 phase;

    for(phase = 0; phase < BEGIN_RUN_PHASE_COUNT; ++phase)
        BeginRogueRunPhase(phase);
}

static u16 GetRequiredBadgesForEggToHatch(u16 species)
{
    // Equiv to egg cyles of 210
    if(RoguePokedex_IsSpeciesLegendary(species))
        return ROGUE_MAX_BOSS_COUNT + (ROGUE_MAX_BOSS_COUNT / 2);

    // Badge = 10 egg cyles
    return gRogueSpeciesInfo[species].eggCycles / 10;
}

static void EndRogueRun(void)
{
    u8 i;

    HandleForfeitingInCatchingContest();

    // Generated quests and their temporary cargo never survive the run boundary.
    RogueAdventureQuests_Clear();
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY, 0);
    VarSet(VAR_ROGUE_ROUTE_EVENT_HISTORY_2, 0);

    for(i = 0; i < gPlayerPartyCount; ++i)
        gPlayerParty[i].rogueExtraData.abilityOverride = ABILITY_NONE;

    if(Rogue_IsCampaignActive())
        Rogue_DeactivateActiveCampaign();

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    FlagClear(FLAG_ROGUE_IS_VICTORY_LAP);

    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_NONE;
    gRogueRun.wildEncounters.roamer.species = SPECIES_NONE;


    // We're back from adventure, so any mon we finished or retired with add to the safari
    {
        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
            if(species != SPECIES_NONE)
            {
                RogueSafari_PushMon(&gPlayerParty[i]);
            }
        }
    }

    gRogueRun.pendingPokeblockBundleRewardCount =
        POKEBLOCKS_PER_SMALL_BUNDLE * CountTotalItemQuantityInBag(ITEM_POKEBLOCK_BUNDLE) +
        POKEBLOCKS_PER_BIG_BUNDLE * CountTotalItemQuantityInBag(ITEM_BIG_POKEBLOCK_BUNDLE);

    RogueSave_LoadHubStates();
    RogueGift_EnsureDynamicCustomMonsAreValid();

    // Trigger before and after as we may have hub/run only quests which are interested in this trigger
    RogueQuest_OnTrigger(QUEST_TRIGGER_RUN_END);
    RogueQuest_OnTrigger(QUEST_TRIGGER_MISC_UPDATE);
    RogueQuest_ActivateQuestsFor(QUEST_CONST_ACTIVE_IN_HUB);
    RogueQuest_OnTrigger(QUEST_TRIGGER_RUN_END);
    RogueQuest_OnTrigger(QUEST_TRIGGER_MISC_UPDATE);

    if(Rogue_GetCurrentDifficulty() > 0)
    {
        u16 eggSpecies = VarGet(VAR_ROGUE_DAYCARE_EGG_SPECIES);

        // Update the egg we're looking for in the daycare
        if(eggSpecies != SPECIES_NONE)
        {
            u16 eggCounter = VarGet(VAR_ROGUE_DAYCARE_EGG_CYCLES) + Rogue_GetCurrentDifficulty();
            VarSet(VAR_ROGUE_DAYCARE_EGG_CYCLES, eggCounter);

            if(eggCounter >= GetRequiredBadgesForEggToHatch(eggSpecies))
            {
                // Egg has now been cought
                FlagSet(FLAG_ROGUE_DAYCARE_EGG_READY);
            }
        }

        // Give ball guy a random ball
        ChooseRandomPokeballReward();
    }
    else if(Rogue_GetCurrentDifficulty() != ROGUE_MAX_BOSS_COUNT)
    {
        // Increment stats
        IncrementGameStat(GAME_STAT_RUN_LOSSES);
        IncrementGameStat(GAME_STAT_CURRENT_RUN_LOSS_STREAK);
        SetGameStat(GAME_STAT_CURRENT_RUN_WIN_STREAK, 0);

        if(GetGameStat(GAME_STAT_CURRENT_RUN_LOSS_STREAK) > GetGameStat(GAME_STAT_LONGEST_RUN_LOSS_STREAK))
            SetGameStat(GAME_STAT_LONGEST_RUN_LOSS_STREAK, GetGameStat(GAME_STAT_CURRENT_RUN_LOSS_STREAK));
    }

    RogueQuest_CheckQuestRequirements();
    RogueHub_UpdateWanderMons();
}

static u16 SelectLegendarySpecies(u8 legendId)
{
    u16 i;
    u16 species;
    RogueMonQuery_Begin();
    RogueMonQuery_Reset(QUERY_FUNC_EXCLUDE);

    // Only include legends which we have valid encounter maps for
    for(i = 0; i < gRogueLegendaryEncounterInfo.mapCount; ++i)
    {
        species = gRogueLegendaryEncounterInfo.mapTable[i].encounterId;

        if(Query_IsSpeciesEnabledForceDexChecking(species))
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, species);
    }

    for(i = 0; i < ADVPATH_LEGEND_COUNT; ++i)
    {
        if(gRogueRun.legendarySpecies[i] != SPECIES_NONE)
            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, gRogueRun.legendarySpecies[i]);
    }

    switch (legendId)
    {
    case ADVPATH_LEGEND_BOX:
        RogueMonQuery_IsBoxLegendary(QUERY_FUNC_INCLUDE);
        break;

    case ADVPATH_LEGEND_MINOR:
        RogueMonQuery_IsBoxLegendary(QUERY_FUNC_EXCLUDE);
        break;

    case ADVPATH_LEGEND_ROAMER:
        RogueMonQuery_IsRoamerLegendary(QUERY_FUNC_INCLUDE);
        break;

    default:
        AGB_ASSERT(FALSE);
        break;
    }

    RogueWeightQuery_Begin();
    {
        RogueWeightQuery_FillWeights(1);
        if(RogueWeightQuery_HasAnyWeights())
        {
            species = RogueWeightQuery_SelectRandomFromWeights(RogueRandom());
        }
        else
        {
            DebugPrintf("No legendary species candidates for slot %d", legendId);
            species = SPECIES_NONE;
        }
    }
    RogueWeightQuery_End();

    RogueMonQuery_End();

#ifdef ROGUE_DEBUG
    // Call this to throw asserts early
    if(species != SPECIES_NONE)
        Rogue_GetLegendaryRoomForSpecies(species);
#endif

    return species;
}

static void ChooseLegendarysForNewAdventure()
{
    bool8 spawnRoamer = RogueRandomChance(50, 0);
    bool8 spawnMinor = RogueRandomChance(75, 0);
    bool8 spawnBox = TRUE;

    // Always have 1
    if(!spawnRoamer && !spawnMinor)
    {
        if(RogueRandom() % 2)
            spawnRoamer = TRUE;
        else
            spawnMinor = TRUE;
    }

    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
    {
        // Gauntlet always generates a minor legendary only
        spawnRoamer = FALSE;
        spawnMinor = TRUE;
        spawnBox = FALSE;

    }

    // DEBUG - Force all legends to spawn
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_LEGENDS))
    {
        spawnRoamer = TRUE;
        spawnMinor = TRUE;
    }

    // Reset
    memset(&gRogueRun.legendarySpecies, 0, sizeof(gRogueRun.legendarySpecies));
    memset(&gRogueRun.legendaryDifficulties, ROGUE_MAX_BOSS_COUNT, sizeof(gRogueRun.legendaryDifficulties));


    // Prioritise box legend first, then roamer, then finally minor

    if(spawnBox)
    {
        gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX] = (Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET) ? 0 : ROGUE_ELITE_START_DIFFICULTY - 1 + RogueRandomRange(3, 0);
        gRogueRun.legendarySpecies[ADVPATH_LEGEND_BOX] = SelectLegendarySpecies(ADVPATH_LEGEND_BOX);

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD)
        {
            // Standard hides routes every other path, so place special encounters on reset paths.
            gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX] = (gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX] / 2) * 2;
        }
    }

    if(spawnRoamer)
    {
        gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_ROAMER] = (Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET) ? 0 : 1 + RogueRandomRange(5, 0);
        gRogueRun.legendarySpecies[ADVPATH_LEGEND_ROAMER] = SelectLegendarySpecies(ADVPATH_LEGEND_ROAMER);

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD)
        {
            gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_ROAMER] = (gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_ROAMER] / 2) * 2;
        }
    }

    if(spawnMinor)
    {
        gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR] = (Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET) ? 0 : 4 + RogueRandomRange(4, 0);
        gRogueRun.legendarySpecies[ADVPATH_LEGEND_MINOR] = SelectLegendarySpecies(ADVPATH_LEGEND_MINOR);

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD)
        {
            gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR] = (gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR] / 2) * 2;
        }
    }

    // DEBUG - Force all legends to spawn at specific difficulties
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_LEGENDS))
    {
        gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_ROAMER] = 0;
        gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR] = 1;
        gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX] = 2;
    }

    {
        u8 collisionStep = (Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD) ? 2 : 1;

        if(gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_ROAMER] == gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR])
            gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR] += collisionStep;

        if(gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_ROAMER] == gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX])
            gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX] += collisionStep;

        if(gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR] == gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX])
            gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX] += collisionStep;
    }
}

static bool8 IsUniqueDenDifficultyAvailable(u8 difficulty, u8 selectedDenCount)
{
    u8 i;

    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD && (difficulty % 2) != 0)
        return FALSE;

    for(i = 0; i < ADVPATH_LEGEND_COUNT; ++i)
    {
        if(difficulty == gRogueRun.legendaryDifficulties[i])
            return FALSE;
    }

    if(difficulty == gRogueRun.shrineSpawnDifficulty)
        return FALSE;

    for(i = 0; i < ADVPATH_TEAM_ENCOUNTER_COUNT; ++i)
    {
        if(difficulty == gRogueRun.teamEncounterDifficulties[i])
            return FALSE;
    }

    for(i = 0; i < selectedDenCount; ++i)
    {
        if(difficulty == gRogueRun.uniqueDenDifficulties[i])
            return FALSE;
    }

    return TRUE;
}

static u8 ChooseUniqueDenDifficulty(u8 minDifficulty, u8 maxDifficulty, u8 selectedDenCount)
{
    u8 i;
    u8 candidateCount = 0;
    u8 candidates[ROGUE_MAX_BOSS_COUNT];

    for(i = minDifficulty; i <= maxDifficulty; ++i)
    {
        if(IsUniqueDenDifficultyAvailable(i, selectedDenCount))
            candidates[candidateCount++] = i;
    }

    if(candidateCount == 0)
        return ROGUE_MAX_BOSS_COUNT;

    return candidates[RogueRandomRange(candidateCount, 0)];
}

static void ChooseUniqueDenForNewAdventure()
{
    memset(gRogueRun.uniqueDenDifficulties, ROGUE_MAX_BOSS_COUNT, sizeof(gRogueRun.uniqueDenDifficulties));

    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
    {
        // Preserve the existing gauntlet scheduling behavior.
        gRogueRun.uniqueDenDifficulties[0] = 0;
        return;
    }

    // Keep one guaranteed den in the early run. If every early slot is reserved,
    // fall back to any available pre-Champion difficulty rather than colliding.
    gRogueRun.uniqueDenDifficulties[0] = ChooseUniqueDenDifficulty(
        Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD ? 0 : 1,
        ROGUE_ELITE_START_DIFFICULTY - 1,
        0
    );

    if(gRogueRun.uniqueDenDifficulties[0] == ROGUE_MAX_BOSS_COUNT)
    {
        gRogueRun.uniqueDenDifficulties[0] = ChooseUniqueDenDifficulty(
            0,
            ROGUE_CHAMP_START_DIFFICULTY - 1,
            0
        );
    }

    // Occasionally schedule one additional den in the mid or late run. This is
    // decided once up front so regenerating an adventure path cannot reroll it.
    if(RogueRandomChance(30, 0))
    {
        gRogueRun.uniqueDenDifficulties[1] = ChooseUniqueDenDifficulty(
            ROGUE_GYM_MID_DIFFICULTY + 2,
            ROGUE_CHAMP_START_DIFFICULTY - 1,
            1
        );
    }
}

static u16 ChooseTeamEncounterNum()
{
    u16 i;
    RogueCustomQuery_Begin();

    if(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO))
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, TEAM_NUM_KANTO_ROCKET);

    if(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO))
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, TEAM_NUM_JOHTO_ROCKET);

    if(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN))
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, TEAM_NUM_AQUA);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, TEAM_NUM_MAGMA);
    }

#ifdef ROGUE_EXPANSION
    if(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_SINNOH))
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, TEAM_NUM_GALACTIC);

    if(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_UNOVA))
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, TEAM_NUM_PLASMA);
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, TEAM_NUM_NEOPLASMA);
    }

    if(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS))
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, TEAM_NUM_FLARE);

    //if(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_ALOLA))
    //    filter->trainerFlagsInclude |= TRAINER_FLAG_REGION_ALOLA;
//
    //if(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_GALAR))
    //    filter->trainerFlagsInclude |= TRAINER_FLAG_REGION_GALAR;
//
    //if(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_PALDEA))
    //    filter->trainerFlagsInclude |= TRAINER_FLAG_REGION_PALDEA;
#endif

    if(!RogueMiscQuery_AnyActiveElements())
    {
        // Enable all teams if we wouldn't have one active now
        for(i = 0; i < TEAM_NUM_COUNT; ++i)
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, i);
    }

    RogueWeightQuery_Begin();
    {
        RogueWeightQuery_FillWeights(1);

        i = RogueWeightQuery_SelectRandomFromWeights(RogueRandom());
    }
    RogueWeightQuery_End();

    RogueCustomQuery_End();

    if(RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_EVIL_TEAM) != 0)
    {
        return RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_EVIL_TEAM) - 1;
    }

    return i;
}

static void ChooseTeamEncountersForNewAdventure()
{
    u16 i;

    // Reset
    memset(&gRogueRun.teamEncounterRooms, 0, sizeof(gRogueRun.teamEncounterRooms));
    memset(&gRogueRun.teamEncounterDifficulties, ROGUE_MAX_BOSS_COUNT, sizeof(gRogueRun.teamEncounterDifficulties));

    // Select a random active team to encounter this run
    gRogueRun.teamEncounterNum = ChooseTeamEncounterNum();

    Rogue_ChooseTeamBossTrainerForNewAdventure();

    // Don't place any of these encounters
    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
        return;

    RogueQuest_UnlockCurrentEvilTeamQuest();

    // Setup maps (There's only 1 per each currently)
    for(i = 0; i < gRogueTeamEncounterInfo.mapCount; ++i)
    {
        if(gRogueTeamEncounterInfo.mapTable[i].encounterId == gRogueRun.teamEncounterNum)
            gRogueRun.teamEncounterRooms[ADVPATH_TEAM_ENCOUNTER_EARLY] = i;
        else if(gRogueTeamEncounterInfo.mapTable[i].encounterId == gRogueRun.teamEncounterNum + TEAM_PRE_LEGEND_MAP_OFFSET)
            gRogueRun.teamEncounterRooms[ADVPATH_TEAM_ENCOUNTER_PRE_LEGEND] = i;
    }

    // Pre legend matches the difficulty
    gRogueRun.teamEncounterDifficulties[ADVPATH_TEAM_ENCOUNTER_PRE_LEGEND] = gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX];

    // Early can be anytime from badge 2 to badge 5 (provided there is no legend at that time)
    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD)
    {
        u8 candidates[] = {2, 4, 6};
        u8 validCandidates[ARRAY_COUNT(candidates)];
        u8 validCount = 0;

        for(i = 0; i < ARRAY_COUNT(candidates); ++i)
        {
            if(candidates[i] == gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR])
                continue;
            if(candidates[i] == gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_ROAMER])
                continue;
            if(candidates[i] == gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX])
                continue;

            validCandidates[validCount++] = candidates[i];
        }

        if(validCount == 0)
        {
            // Prefer not to stack with the box legend, because pre-legend team
            // already uses that path. Minor/roamer overlap is better than no early team encounter.
            for(i = 0; i < ARRAY_COUNT(candidates); ++i)
            {
                if(candidates[i] == gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX])
                    continue;

                validCandidates[validCount++] = candidates[i];
            }
        }

        if(validCount != 0)
            gRogueRun.teamEncounterDifficulties[ADVPATH_TEAM_ENCOUNTER_EARLY] = validCandidates[RogueRandomRange(validCount, 0)];
    }
    else
    {
        while(TRUE)
        {
            gRogueRun.teamEncounterDifficulties[ADVPATH_TEAM_ENCOUNTER_EARLY] = 2 + RogueRandomRange(3, 0);

            if(gRogueRun.teamEncounterDifficulties[ADVPATH_TEAM_ENCOUNTER_EARLY] == gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR])
                continue;
            if(gRogueRun.teamEncounterDifficulties[ADVPATH_TEAM_ENCOUNTER_EARLY] == gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_ROAMER])
                continue;

            break;
        };
    }
}

u8 Rogue_GetCurrentLegendaryEncounterId()
{
    u8 i;

    for(i = 0; i < ADVPATH_LEGEND_COUNT; ++i)
    {
        if(gRogueRun.legendaryDifficulties[i] == Rogue_GetCurrentDifficulty())
            return i;
    }

    AGB_ASSERT(FALSE);
    return 0;
}

u16 Rogue_GetLegendaryRoomForSpecies(u16 species)
{
    u16 i;

    for(i = 0; i < gRogueLegendaryEncounterInfo.mapCount; ++i)
    {
        if(gRogueLegendaryEncounterInfo.mapTable[i].encounterId == species)
            return i;
    }

    AGB_ASSERT(FALSE);
    return 0;
}

u8 Rogue_GetCurrentTeamHideoutEncounterId(void)
{
    u8 i;

    for(i = 0; i < ADVPATH_TEAM_ENCOUNTER_COUNT; ++i)
    {
        if(gRogueRun.teamEncounterDifficulties[i] == Rogue_GetCurrentDifficulty())
            return i;
    }

    AGB_ASSERT(FALSE);
    return 0;
}

bool8 Rogue_IsBattleAlphaMon(u16 species)
{
    // The shrine guardian uses a custom-mon appearance, but Sacred Ash provides
    // its second phase instead of the catchable Alpha encounter flow.
    if(Rogue_IsShrineChallengeActive())
        return FALSE;

    // Roamer legend fight is not an alpha fight
    if(gRogueRun.legendarySpecies[ADVPATH_LEGEND_MINOR] == species && gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_MINOR] == Rogue_GetCurrentDifficulty())
        return TRUE;

    if(gRogueRun.legendarySpecies[ADVPATH_LEGEND_BOX] == species && gRogueRun.legendaryDifficulties[ADVPATH_LEGEND_BOX] == Rogue_GetCurrentDifficulty())
        return TRUE;

    if(Rogue_IsRunActive())
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_GAMESHOW && species == SPECIES_ELECTRODE)
            return TRUE;

        if(gRogueLocal.wildBattleCustomMonId != 0)
            return TRUE;
    }

    return FALSE;
}

bool8 Rogue_IsShrineChallengeActive(void)
{
    return gRogueLocal.isShrineChallengeActive;
}

void Rogue_ActivateUncatchableWildBattle(void)
{
    // Route-event bosses share the shrine guardian's transient battle rules:
    // smart wild AI, no capture, and no Alpha encounter treatment.
    gRogueLocal.isShrineChallengeActive = TRUE;
}

bool8 Rogue_HasChallengedShrine(void)
{
    return gRogueRun.hasChallengedShrine;
}

static void SanitizeShrineGuardianMoves(struct Pokemon *mon)
{
    u8 i;
    u32 move;

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        move = GetMonData(mon, MON_DATA_MOVE1 + i);

        // Phazing ends a solo wild battle immediately, bypassing the shrine trial.
        if(move == MOVE_WHIRLWIND)
        {
            move = MOVE_TAILWIND;
            SetMonData(mon, MON_DATA_MOVE1 + i, &move);
            SetMonData(mon, MON_DATA_PP1 + i, &gBattleMoves[move].pp);
        }
    }
}

static bool8 ShrineGuardianMoveListContains(const u16 *moves, u8 moveCount, u16 move)
{
    u8 i;

    for(i = 0; i < moveCount; ++i)
    {
        if(moves[i] == move)
            return TRUE;
    }

    return FALSE;
}

static void SelectShrineGuardianMoves(const u16 *customMoves, u8 customMoveCount, const struct RoguePokemonCompetitiveSet *preset, u16 *outMoves)
{
    u8 i;
    u8 candidateCount = 0;
    u8 damagingCandidateCount = 0;
    u8 outMoveCount = 0;
    u8 selectedIndex;
    u16 candidateMoves[MAX_MON_MOVES];
    u16 damagingCandidateIndices[MAX_MON_MOVES];
    bool8 hasDamagingMove = FALSE;

    for(i = 0; i < customMoveCount; ++i)
    {
        if(customMoves[i] != MOVE_NONE && !ShrineGuardianMoveListContains(outMoves, outMoveCount, customMoves[i]))
        {
            outMoves[outMoveCount++] = customMoves[i];
            if(!IS_MOVE_STATUS(customMoves[i]))
                hasDamagingMove = TRUE;
        }
    }

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        u16 move = preset->moves[i];

        if(move != MOVE_NONE
            && !ShrineGuardianMoveListContains(outMoves, outMoveCount, move)
            && !ShrineGuardianMoveListContains(candidateMoves, candidateCount, move))
        {
            candidateMoves[candidateCount++] = move;
        }
    }

    // If both generated moves are utility moves, retain one damaging preset
    // move before filling the final slot freely.
    if(!hasDamagingMove && outMoveCount < MAX_MON_MOVES)
    {
        for(i = 0; i < candidateCount; ++i)
        {
            if(!IS_MOVE_STATUS(candidateMoves[i]))
                damagingCandidateIndices[damagingCandidateCount++] = i;
        }

        AGB_ASSERT(damagingCandidateCount != 0);
        if(damagingCandidateCount != 0)
        {
            selectedIndex = damagingCandidateIndices[Random() % damagingCandidateCount];
            outMoves[outMoveCount++] = candidateMoves[selectedIndex];
            candidateMoves[selectedIndex] = candidateMoves[--candidateCount];
        }
    }

    while(outMoveCount < MAX_MON_MOVES && candidateCount != 0)
    {
        selectedIndex = Random() % candidateCount;
        outMoves[outMoveCount++] = candidateMoves[selectedIndex];
        candidateMoves[selectedIndex] = candidateMoves[--candidateCount];
    }

    AGB_ASSERT(outMoveCount == MAX_MON_MOVES);
}

#define SHRINE_GUARDIAN_TYPING_CHANCE 67

void Rogue_PrepareShrineChallenge(void)
{
    u8 i;
    u8 level = Rogue_CalculateBossMonLvl();
    u16 species = SPECIES_HO_OH;
    u16 customMoves[MAX_MON_MOVES];
    u16 customMoveCount;
    u16 presetCount = gRoguePokemonProfiles[species].competitiveSetCount;
    u32 customMonId = RogueGift_CreateDynamicMonIdRawWithTypingChance(UNIQUE_RARITY_EPIC, species, SHRINE_GUARDIAN_TYPING_CHANCE);
    u32 temp;

    gRogueRun.hasChallengedShrine = TRUE;
    gRogueLocal.isShrineChallengeActive = TRUE;

    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], species, level, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    ModifyExistingMonToCustomMon(customMonId, &gEnemyParty[0]);
    gRogueLocal.wildBattleCustomMonId = customMonId;

    // Preserve the unique payload, then use a normal competitive set to make
    // the remaining move slots and nature coherent for Ho-Oh.
    customMoveCount = min(RogueGift_GetCustomMonMoveCount(customMonId), MAX_MON_MOVES);
    for(i = 0; i < customMoveCount; ++i)
        customMoves[i] = RogueGift_GetCustomMonMove(customMonId, i);

    if(presetCount != 0)
    {
        struct RoguePokemonCompetitiveSetRules rules = {0};
        u16 presetIndex = Random() % presetCount;
        const struct RoguePokemonCompetitiveSet *preset = &gRoguePokemonProfiles[species].competitiveSets[presetIndex];
        u16 finalMoves[MAX_MON_MOVES] = {0};

        rules.skipAbility = TRUE;
        rules.skipHeldItem = TRUE;
        Rogue_ApplyMonCompetitiveSet(&gEnemyParty[0], level, preset, &rules);
        SelectShrineGuardianMoves(customMoves, customMoveCount, preset, finalMoves);

        for(i = 0; i < MAX_MON_MOVES; ++i)
        {
            temp = finalMoves[i];
            SetMonData(&gEnemyParty[0], MON_DATA_MOVE1 + i, &temp);
            SetMonData(&gEnemyParty[0], MON_DATA_PP1 + i, &gBattleMoves[temp].pp);
        }
    }

    SanitizeShrineGuardianMoves(&gEnemyParty[0]);

    // Match unique-den encounter quality, while never presenting an
    // uncatchable shiny or Ho-Oh's normal held Sacred Ash.
    temp = 31;
    SetMonData(&gEnemyParty[0], MON_DATA_HP_IV + (Random() % NUM_STATS), &temp);
    temp = 0;
    SetMonData(&gEnemyParty[0], MON_DATA_FRIENDSHIP, &temp);
    SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &temp);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &temp);
    CalculateMonStats(&gEnemyParty[0]);
    temp = GetMonData(&gEnemyParty[0], MON_DATA_MAX_HP);
    SetMonData(&gEnemyParty[0], MON_DATA_HP, &temp);

}

bool8 Rogue_IsBattleRoamerMon(u16 species)
{
    if(gRogueRun.legendarySpecies[ADVPATH_LEGEND_ROAMER] != species)
        return FALSE;

    // A Legendary node that rolled a custom Pokémon is converted into an
    // Alpha encounter.
    if(Rogue_IsRunActive()
        && gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY
        && gRogueAdvPath.currentRoomParams.perType.legendary.customMonId != 0)
        return FALSE;

    return TRUE;
}

static bool8 IsMiniBossRewardEligible(struct Pokemon* party, u8 index, bool8 allowLegendary)
{
    u16 species = GetMonData(&party[index], MON_DATA_SPECIES);
    u32 otId = GetMonData(&party[index], MON_DATA_OT_ID);

    if(species == SPECIES_NONE)
        return FALSE;

    if(!allowLegendary && RoguePokedex_IsSpeciesLegendary(species))
        return FALSE;

    return RogueTrial_IsSpeciesLegal(species, otId);
}

static u8 FindMiniBossRewardIndex(struct Pokemon* party, u8 partySize, u8 startIndex, u8 excludedIndex, bool8 allowLegendary)
{
    u8 i;

    for(i = 0; i < partySize; ++i)
    {
        u8 index = (startIndex + i) % partySize;

        if(index != excludedIndex && IsMiniBossRewardEligible(party, index, allowLegendary))
            return index;
    }

    return PARTY_SIZE;
}

static u8 GetMiniBossRewardOverrideMode(void)
{
    if(VarGet(VAR_ROGUE_ACTIVE_CAMPAIGN) == ROGUE_CAMPAIGN_POKEBALL_LIMIT
        || RogueTrial_IsActiveTrial(ROGUE_TRIAL_LIMITED_CAPTURE))
        return MINIBOSS_REWARD_MODE_NONE;

    if(RogueTrial_IsActiveTrial(ROGUE_TRIAL_ORRE_STYLE))
        return MINIBOSS_REWARD_MODE_SNAG;

    return 0xFF;
}

static u8 SelectMiniBossRewardIndices(u16 trainerNum, struct Pokemon* party, u8 partySize, u16 roomSeed, u8* indexA, u8* indexB)
{
    const struct RogueTrainer* trainer;
    bool8 isBrandon;
    u8 overrideMode = GetMiniBossRewardOverrideMode();
    u8 startA;
    u8 startB;

    *indexA = PARTY_SIZE;
    *indexB = PARTY_SIZE;

    if(partySize == 0 || trainerNum >= gRogueTrainerCount)
        return MINIBOSS_REWARD_MODE_NO_LEGAL;

    if(overrideMode != 0xFF)
        return overrideMode;

    AGB_ASSERT(trainerNum < gRogueTrainerCount);
    trainer = &gRogueTrainers[trainerNum];
    isBrandon = (trainer->classFlags & CLASS_FLAG_MINIBOSS_BRANDON) != 0;
    startA = (roomSeed * 13u + trainerNum * 7u) % partySize;
    startB = (roomSeed * 29u + trainerNum * 11u + 1u) % partySize;

    if(isBrandon)
    {
        u8 i;
        *indexA = PARTY_SIZE;

        for(i = 0; i < partySize; ++i)
        {
            u8 index = (startA + i) % partySize;
            u16 species = GetMonData(&party[index], MON_DATA_SPECIES);

            if(species != SPECIES_NONE
                && RoguePokedex_IsSpeciesLegendary(species)
                && IsMiniBossRewardEligible(party, index, TRUE))
            {
                *indexA = index;
                break;
            }
        }

        if(*indexA == PARTY_SIZE)
            *indexA = FindMiniBossRewardIndex(party, partySize, startA, PARTY_SIZE, TRUE);

        *indexB = FindMiniBossRewardIndex(party, partySize, startB, *indexA, TRUE);
    }
    else
    {
        *indexA = FindMiniBossRewardIndex(party, partySize, startA, PARTY_SIZE, FALSE);
        *indexB = FindMiniBossRewardIndex(party, partySize, startB, *indexA, FALSE);
    }

    if(*indexA == PARTY_SIZE)
        return MINIBOSS_REWARD_MODE_NO_LEGAL;
    if(*indexB == PARTY_SIZE)
        return MINIBOSS_REWARD_MODE_SINGLE;
    return MINIBOSS_REWARD_MODE_DOUBLE;
}

static u8 BufferMiniBossRewardSelection(u16 trainerNum, struct Pokemon* party, u8 partySize, u16 roomSeed)
{
    u8 indexA;
    u8 indexB;
    u8 rewardMode = SelectMiniBossRewardIndices(trainerNum, party, partySize, roomSeed, &indexA, &indexB);

    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1,
        indexA == PARTY_SIZE ? SPECIES_NONE : GetMonData(&party[indexA], MON_DATA_SPECIES));
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2,
        indexB == PARTY_SIZE ? SPECIES_NONE : GetMonData(&party[indexB], MON_DATA_SPECIES));

    return rewardMode;
}

void Rogue_SelectMiniBossRewardMons()
{
    u8 partySize = CalculateEnemyPartyCount();
    u16 trainerNum = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA);
    u16 roomSeed = gRogueAdvPath.rooms[gRogueRun.adventureRoomId].rngSeed;

    AGB_ASSERT(partySize != 0);
    if(partySize == 0)
    {
        VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, SPECIES_NONE);
        VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, SPECIES_NONE);
        gSpecialVar_Result = MINIBOSS_REWARD_MODE_NO_LEGAL;
        return;
    }

    gSpecialVar_Result = BufferMiniBossRewardSelection(trainerNum, gEnemyParty, partySize, roomSeed);
}

static bool8 GenerateMiniBossPreview(u8 roomIdx)
{
    struct RogueAdvPathRoom* room = &gRogueAdvPath.rooms[roomIdx];
    struct Pokemon* enemyParty = Alloc(sizeof(gEnemyParty));
    u16 trainerNum = room->roomParams.perType.miniboss.trainerNum;
    RAND_TYPE rogueRng = gRngRogueValue;
    RAND_TYPE rng = gRngValue;
    RAND_TYPE rng2 = gRng2Value;
    RAND_TYPE localSeedToRestore = gRogueLocal.rngSeedToRestore;
    RAND_TYPE localRngToRestore = gRogueLocal.rngToRestore;
    RAND_TYPE localRng2ToRestore = gRogueLocal.rng2ToRestore;
    struct RogueAdvPathRoomParams currentRoomParams = gRogueAdvPath.currentRoomParams;
    u32 battleTypeFlags = gBattleTypeFlags;
    u8 currentRoomType = gRogueAdvPath.currentRoomType;
    bool8 hadDynamax = FlagGet(FLAG_ROGUE_DYNAMAX_BATTLE);
    bool8 hadTera = FlagGet(FLAG_ROGUE_TERASTALLIZE_BATTLE);
    bool8 hadBattleEvent = gRogueLocal.hasBattleEventOccurred;
    bool8 itemFlags[ROGUE_ITEM_COUNT];
    u16 itemVars[ROGUE_ITEM_COUNT];
    RAND_TYPE miniBossSeed;
    u8 partySize;
    u8 i;

    AGB_ASSERT(room->roomType == ADVPATH_ROOM_MINIBOSS);
    AGB_ASSERT(enemyParty != NULL);
    if(enemyParty == NULL)
        return FALSE;

    memcpy(enemyParty, gEnemyParty, sizeof(gEnemyParty));
    ZeroEnemyPartyMons();
    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_MINIBOSS;
    gRogueAdvPath.currentRoomParams = room->roomParams;

    // Preview the item availability that will be generated on room entry, as
    // it influences competitive preset selection. Keep it entirely temporary.
    for(i = 0; i < ROGUE_ITEM_COUNT; ++i)
    {
        itemFlags[i] = FlagGet(FLAG_ROGUE_ITEM_START + i);
        itemVars[i] = VarGet(VAR_ROGUE_ITEM_START + i);
    }

    SeedRogueRng(room->rngSeed);
    miniBossSeed = gRngRogueValue;
    RandomiseMiniBossItemsAndRestoreSeed();
    AGB_ASSERT(gRngRogueValue == miniBossSeed);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    SetupTrainerBattleInternal(trainerNum);
    partySize = Rogue_CreateTrainerParty(trainerNum, gEnemyParty, PARTY_SIZE, TRUE);
    AGB_ASSERT(partySize >= 2);

    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, trainerNum);
    if(partySize != 0)
        gSpecialVar_Result = BufferMiniBossRewardSelection(trainerNum, gEnemyParty, partySize, room->rngSeed);
    else
    {
        VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, SPECIES_NONE);
        VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, SPECIES_NONE);
        gSpecialVar_Result = MINIBOSS_REWARD_MODE_NO_LEGAL;
    }

    memcpy(gEnemyParty, enemyParty, sizeof(gEnemyParty));
    Free(enemyParty);
    CalculateEnemyPartyCount();

    for(i = 0; i < ROGUE_ITEM_COUNT; ++i)
    {
        if(itemFlags[i])
            FlagSet(FLAG_ROGUE_ITEM_START + i);
        else
            FlagClear(FLAG_ROGUE_ITEM_START + i);
        VarSet(VAR_ROGUE_ITEM_START + i, itemVars[i]);
    }

    if(hadDynamax)
        FlagSet(FLAG_ROGUE_DYNAMAX_BATTLE);
    else
        FlagClear(FLAG_ROGUE_DYNAMAX_BATTLE);

    if(hadTera)
        FlagSet(FLAG_ROGUE_TERASTALLIZE_BATTLE);
    else
        FlagClear(FLAG_ROGUE_TERASTALLIZE_BATTLE);

    gBattleTypeFlags = battleTypeFlags;
    gRogueAdvPath.currentRoomType = currentRoomType;
    gRogueAdvPath.currentRoomParams = currentRoomParams;
    gRogueLocal.hasBattleEventOccurred = hadBattleEvent;
    gRogueLocal.rngSeedToRestore = localSeedToRestore;
    gRogueLocal.rngToRestore = localRngToRestore;
    gRogueLocal.rng2ToRestore = localRng2ToRestore;
    gRngRogueValue = rogueRng;
    gRngValue = rng;
    gRng2Value = rng2;

    return TRUE;
}

void Rogue_CacheMiniBossPreview(u8 roomIdx)
{
    struct RogueAdvPathRoom* room = &gRogueAdvPath.rooms[roomIdx];
    u16 previousTrainerNum = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA);
    u16 previousSpeciesA = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1);
    u16 previousSpeciesB = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2);
    u16 previousResult = gSpecialVar_Result;

    if(!GenerateMiniBossPreview(roomIdx))
        return;

    room->roomParams.perType.miniboss.rewardSpeciesA = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1);
    room->roomParams.perType.miniboss.rewardSpeciesB = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2);
    room->roomParams.perType.miniboss.rewardMode = gSpecialVar_Result;
    room->roomParams.perType.miniboss.hasRewardPreview = TRUE;

    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, previousTrainerNum);
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, previousSpeciesA);
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, previousSpeciesB);
    gSpecialVar_Result = previousResult;
}

void Rogue_BufferMiniBossPreview(u8 roomIdx)
{
    struct RogueAdvPathRoom* room = &gRogueAdvPath.rooms[roomIdx];

    AGB_ASSERT(room->roomType == ADVPATH_ROOM_MINIBOSS);

    // Keep a fallback for debug-created rooms and older in-memory path state.
    if(!room->roomParams.perType.miniboss.hasRewardPreview)
        Rogue_CacheMiniBossPreview(roomIdx);

    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, room->roomParams.perType.miniboss.trainerNum);
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, room->roomParams.perType.miniboss.rewardSpeciesA);
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, room->roomParams.perType.miniboss.rewardSpeciesB);
    gSpecialVar_Result = room->roomParams.perType.miniboss.rewardMode;
}

static u8 UNUSED RandomMonType(u16 seedFlag)
{
    u8 type;

    do
    {
        type = RogueRandomRange(NUMBER_OF_MON_TYPES, seedFlag);
    }
    while(!IS_STANDARD_TYPE(type));

    return type;
}

#ifdef ROGUE_EXPANSION
#define WILD_FORM_QUERY_NUM_SPECIES (PLACEHOLDER_START + 1)

enum
{
    WILD_FORM_FAMILY_UNOWN = NUM_SPECIES + 1,
    WILD_FORM_FAMILY_PIKACHU,
    WILD_FORM_FAMILY_ROTOM,
    WILD_FORM_FAMILY_SQUAWKABILLY,
    WILD_FORM_FAMILY_VIVILLON,
    WILD_FORM_FAMILY_FURFROU,
    WILD_FORM_FAMILY_ALCREMIE,
    WILD_FORM_FAMILY_TATSUGIRI,
    WILD_FORM_FAMILY_MAUSHOLD,
    WILD_FORM_FAMILY_DUDUNSPARCE,
    WILD_FORM_FAMILY_MINIOR,
    WILD_FORM_FAMILY_BURMY,
    WILD_FORM_FAMILY_WORMADAM,
    WILD_FORM_FAMILY_SHELLOS,
    WILD_FORM_FAMILY_GASTRODON,
    WILD_FORM_FAMILY_BASCULIN,
    WILD_FORM_FAMILY_BASCULEGION,
    WILD_FORM_FAMILY_DEERLING,
    WILD_FORM_FAMILY_SAWSBUCK,
    WILD_FORM_FAMILY_ORICORIO,
    WILD_FORM_FAMILY_LYCANROC,
    WILD_FORM_FAMILY_TOXTRICITY,
    WILD_FORM_FAMILY_PUMPKABOO,
    WILD_FORM_FAMILY_GOURGEIST,
    WILD_FORM_FAMILY_FLABEBE,
    WILD_FORM_FAMILY_FLOETTE,
    WILD_FORM_FAMILY_FLORGES,
    WILD_FORM_FAMILY_URSHIFU,
    WILD_FORM_FAMILY_PALDEAN_TAUROS,
    WILD_FORM_FAMILY_OINKOLOGNE,
    WILD_FORM_FAMILY_MEOWSTIC,
    WILD_FORM_FAMILY_INDEEDEE,
    WILD_FORM_FAMILY_SINISTEA,
    WILD_FORM_FAMILY_POLTEAGEIST,
    WILD_FORM_FAMILY_GIMMIGHOUL,
};

static const u16 sWildUnownForms[] = {
    SPECIES_UNOWN,
    SPECIES_UNOWN_B,
    SPECIES_UNOWN_C,
    SPECIES_UNOWN_D,
    SPECIES_UNOWN_E,
    SPECIES_UNOWN_F,
    SPECIES_UNOWN_G,
    SPECIES_UNOWN_H,
    SPECIES_UNOWN_I,
    SPECIES_UNOWN_J,
    SPECIES_UNOWN_K,
    SPECIES_UNOWN_L,
    SPECIES_UNOWN_M,
    SPECIES_UNOWN_N,
    SPECIES_UNOWN_O,
    SPECIES_UNOWN_P,
    SPECIES_UNOWN_Q,
    SPECIES_UNOWN_R,
    SPECIES_UNOWN_S,
    SPECIES_UNOWN_T,
    SPECIES_UNOWN_U,
    SPECIES_UNOWN_V,
    SPECIES_UNOWN_W,
    SPECIES_UNOWN_X,
    SPECIES_UNOWN_Y,
    SPECIES_UNOWN_Z,
    SPECIES_UNOWN_EMARK,
    SPECIES_UNOWN_QMARK,
};

static const u16 sWildSquawkabillyForms[] = {
    SPECIES_SQUAWKABILLY_GREEN_PLUMAGE,
    SPECIES_SQUAWKABILLY_BLUE_PLUMAGE,
    SPECIES_SQUAWKABILLY_YELLOW_PLUMAGE,
    SPECIES_SQUAWKABILLY_WHITE_PLUMAGE,
};

static const u16 sWildVivillonForms[] = {
    SPECIES_VIVILLON_ICY_SNOW,
    SPECIES_VIVILLON_POLAR,
    SPECIES_VIVILLON_TUNDRA,
    SPECIES_VIVILLON_CONTINENTAL,
    SPECIES_VIVILLON_GARDEN,
    SPECIES_VIVILLON_ELEGANT,
    SPECIES_VIVILLON_MEADOW,
    SPECIES_VIVILLON_MODERN,
    SPECIES_VIVILLON_MARINE,
    SPECIES_VIVILLON_ARCHIPELAGO,
    SPECIES_VIVILLON_HIGH_PLAINS,
    SPECIES_VIVILLON_SANDSTORM,
    SPECIES_VIVILLON_RIVER,
    SPECIES_VIVILLON_MONSOON,
    SPECIES_VIVILLON_SAVANNA,
    SPECIES_VIVILLON_SUN,
    SPECIES_VIVILLON_OCEAN,
    SPECIES_VIVILLON_JUNGLE,
    SPECIES_VIVILLON_FANCY,
    SPECIES_VIVILLON_POKE_BALL,
};

static const u16 sWildFurfrouForms[] = {
    SPECIES_FURFROU_NATURAL,
    SPECIES_FURFROU_HEART_TRIM,
    SPECIES_FURFROU_STAR_TRIM,
    SPECIES_FURFROU_DIAMOND_TRIM,
    SPECIES_FURFROU_DEBUTANTE_TRIM,
    SPECIES_FURFROU_MATRON_TRIM,
    SPECIES_FURFROU_DANDY_TRIM,
    SPECIES_FURFROU_LA_REINE_TRIM,
    SPECIES_FURFROU_KABUKI_TRIM,
    SPECIES_FURFROU_PHARAOH_TRIM,
};

// Original Cap Pikachu remains exclusive to the Can't Pick!? quest reward.
// Gigantamax Pikachu and the unimplemented Let's Go Partner Pikachu are battle
// and placeholder forms respectively, so neither belongs in wild generation.
static const u16 sWildPikachuSpecialForms[] = {
    SPECIES_PIKACHU_COSPLAY,
    SPECIES_PIKACHU_ROCK_STAR,
    SPECIES_PIKACHU_BELLE,
    SPECIES_PIKACHU_POP_STAR,
    SPECIES_PIKACHU_PH_D,
    SPECIES_PIKACHU_LIBRE,
    SPECIES_PIKACHU_HOENN_CAP,
    SPECIES_PIKACHU_SINNOH_CAP,
    SPECIES_PIKACHU_UNOVA_CAP,
    SPECIES_PIKACHU_KALOS_CAP,
    SPECIES_PIKACHU_ALOLA_CAP,
    SPECIES_PIKACHU_PARTNER_CAP,
    SPECIES_PIKACHU_WORLD_CAP,
};

static const u16 sWildMiniorForms[] = {
    SPECIES_MINIOR_METEOR_RED,
    SPECIES_MINIOR_METEOR_ORANGE,
    SPECIES_MINIOR_METEOR_YELLOW,
    SPECIES_MINIOR_METEOR_GREEN,
    SPECIES_MINIOR_METEOR_BLUE,
    SPECIES_MINIOR_METEOR_INDIGO,
    SPECIES_MINIOR_METEOR_VIOLET,
};

static const u16 sWildAlcremieForms[] = {
    SPECIES_ALCREMIE_STRAWBERRY_VANILLA_CREAM,
    SPECIES_ALCREMIE_STRAWBERRY_RUBY_CREAM,
    SPECIES_ALCREMIE_STRAWBERRY_MATCHA_CREAM,
    SPECIES_ALCREMIE_STRAWBERRY_MINT_CREAM,
    SPECIES_ALCREMIE_STRAWBERRY_LEMON_CREAM,
    SPECIES_ALCREMIE_STRAWBERRY_SALTED_CREAM,
    SPECIES_ALCREMIE_STRAWBERRY_RUBY_SWIRL,
    SPECIES_ALCREMIE_STRAWBERRY_CARAMEL_SWIRL,
    SPECIES_ALCREMIE_STRAWBERRY_RAINBOW_SWIRL,
    SPECIES_ALCREMIE_BERRY_VANILLA_CREAM,
    SPECIES_ALCREMIE_BERRY_RUBY_CREAM,
    SPECIES_ALCREMIE_BERRY_MATCHA_CREAM,
    SPECIES_ALCREMIE_BERRY_MINT_CREAM,
    SPECIES_ALCREMIE_BERRY_LEMON_CREAM,
    SPECIES_ALCREMIE_BERRY_SALTED_CREAM,
    SPECIES_ALCREMIE_BERRY_RUBY_SWIRL,
    SPECIES_ALCREMIE_BERRY_CARAMEL_SWIRL,
    SPECIES_ALCREMIE_BERRY_RAINBOW_SWIRL,
    SPECIES_ALCREMIE_LOVE_VANILLA_CREAM,
    SPECIES_ALCREMIE_LOVE_RUBY_CREAM,
    SPECIES_ALCREMIE_LOVE_MATCHA_CREAM,
    SPECIES_ALCREMIE_LOVE_MINT_CREAM,
    SPECIES_ALCREMIE_LOVE_LEMON_CREAM,
    SPECIES_ALCREMIE_LOVE_SALTED_CREAM,
    SPECIES_ALCREMIE_LOVE_RUBY_SWIRL,
    SPECIES_ALCREMIE_LOVE_CARAMEL_SWIRL,
    SPECIES_ALCREMIE_LOVE_RAINBOW_SWIRL,
    SPECIES_ALCREMIE_STAR_VANILLA_CREAM,
    SPECIES_ALCREMIE_STAR_RUBY_CREAM,
    SPECIES_ALCREMIE_STAR_MATCHA_CREAM,
    SPECIES_ALCREMIE_STAR_MINT_CREAM,
    SPECIES_ALCREMIE_STAR_LEMON_CREAM,
    SPECIES_ALCREMIE_STAR_SALTED_CREAM,
    SPECIES_ALCREMIE_STAR_RUBY_SWIRL,
    SPECIES_ALCREMIE_STAR_CARAMEL_SWIRL,
    SPECIES_ALCREMIE_STAR_RAINBOW_SWIRL,
    SPECIES_ALCREMIE_CLOVER_VANILLA_CREAM,
    SPECIES_ALCREMIE_CLOVER_RUBY_CREAM,
    SPECIES_ALCREMIE_CLOVER_MATCHA_CREAM,
    SPECIES_ALCREMIE_CLOVER_MINT_CREAM,
    SPECIES_ALCREMIE_CLOVER_LEMON_CREAM,
    SPECIES_ALCREMIE_CLOVER_SALTED_CREAM,
    SPECIES_ALCREMIE_CLOVER_RUBY_SWIRL,
    SPECIES_ALCREMIE_CLOVER_CARAMEL_SWIRL,
    SPECIES_ALCREMIE_CLOVER_RAINBOW_SWIRL,
    SPECIES_ALCREMIE_FLOWER_VANILLA_CREAM,
    SPECIES_ALCREMIE_FLOWER_RUBY_CREAM,
    SPECIES_ALCREMIE_FLOWER_MATCHA_CREAM,
    SPECIES_ALCREMIE_FLOWER_MINT_CREAM,
    SPECIES_ALCREMIE_FLOWER_LEMON_CREAM,
    SPECIES_ALCREMIE_FLOWER_SALTED_CREAM,
    SPECIES_ALCREMIE_FLOWER_RUBY_SWIRL,
    SPECIES_ALCREMIE_FLOWER_CARAMEL_SWIRL,
    SPECIES_ALCREMIE_FLOWER_RAINBOW_SWIRL,
    SPECIES_ALCREMIE_RIBBON_VANILLA_CREAM,
    SPECIES_ALCREMIE_RIBBON_RUBY_CREAM,
    SPECIES_ALCREMIE_RIBBON_MATCHA_CREAM,
    SPECIES_ALCREMIE_RIBBON_MINT_CREAM,
    SPECIES_ALCREMIE_RIBBON_LEMON_CREAM,
    SPECIES_ALCREMIE_RIBBON_SALTED_CREAM,
    SPECIES_ALCREMIE_RIBBON_RUBY_SWIRL,
    SPECIES_ALCREMIE_RIBBON_CARAMEL_SWIRL,
    SPECIES_ALCREMIE_RIBBON_RAINBOW_SWIRL,
};

static const u16 sWildTatsugiriForms[] = {
    SPECIES_TATSUGIRI_CURLY,
    SPECIES_TATSUGIRI_DROOPY,
    SPECIES_TATSUGIRI_STRETCHY,
};

static const u16 sWildGimmighoulForms[] = {
    SPECIES_GIMMIGHOUL_CHEST,
};

static bool8 IsSpeciesInRange(u16 species, u16 fromSpecies, u16 toSpecies)
{
    return species >= fromSpecies && species <= toSpecies;
}

static u16 GetWildFormFamilyKey(u16 species)
{
    if(species == SPECIES_UNOWN || IsSpeciesInRange(species, SPECIES_UNOWN_B, SPECIES_UNOWN_QMARK))
        return WILD_FORM_FAMILY_UNOWN;

    switch(species)
    {
    case SPECIES_PIKACHU:
    case SPECIES_PIKACHU_COSPLAY:
    case SPECIES_PIKACHU_ROCK_STAR:
    case SPECIES_PIKACHU_BELLE:
    case SPECIES_PIKACHU_POP_STAR:
    case SPECIES_PIKACHU_PH_D:
    case SPECIES_PIKACHU_LIBRE:
    case SPECIES_PIKACHU_ORIGINAL_CAP:
    case SPECIES_PIKACHU_HOENN_CAP:
    case SPECIES_PIKACHU_SINNOH_CAP:
    case SPECIES_PIKACHU_UNOVA_CAP:
    case SPECIES_PIKACHU_KALOS_CAP:
    case SPECIES_PIKACHU_ALOLA_CAP:
    case SPECIES_PIKACHU_PARTNER_CAP:
    case SPECIES_PIKACHU_WORLD_CAP:
        return WILD_FORM_FAMILY_PIKACHU;

    case SPECIES_ROTOM:
    case SPECIES_ROTOM_HEAT:
    case SPECIES_ROTOM_WASH:
    case SPECIES_ROTOM_FROST:
    case SPECIES_ROTOM_FAN:
    case SPECIES_ROTOM_MOW:
        return WILD_FORM_FAMILY_ROTOM;

    case SPECIES_SQUAWKABILLY_GREEN_PLUMAGE:
    case SPECIES_SQUAWKABILLY_BLUE_PLUMAGE:
    case SPECIES_SQUAWKABILLY_YELLOW_PLUMAGE:
    case SPECIES_SQUAWKABILLY_WHITE_PLUMAGE:
        return WILD_FORM_FAMILY_SQUAWKABILLY;

    case SPECIES_FURFROU_NATURAL:
    case SPECIES_FURFROU_HEART_TRIM:
    case SPECIES_FURFROU_STAR_TRIM:
    case SPECIES_FURFROU_DIAMOND_TRIM:
    case SPECIES_FURFROU_DEBUTANTE_TRIM:
    case SPECIES_FURFROU_MATRON_TRIM:
    case SPECIES_FURFROU_DANDY_TRIM:
    case SPECIES_FURFROU_LA_REINE_TRIM:
    case SPECIES_FURFROU_KABUKI_TRIM:
    case SPECIES_FURFROU_PHARAOH_TRIM:
        return WILD_FORM_FAMILY_FURFROU;

    case SPECIES_TATSUGIRI_CURLY:
    case SPECIES_TATSUGIRI_DROOPY:
    case SPECIES_TATSUGIRI_STRETCHY:
        return WILD_FORM_FAMILY_TATSUGIRI;

    case SPECIES_MAUSHOLD_FAMILY_OF_THREE:
    case SPECIES_MAUSHOLD_FAMILY_OF_FOUR:
        return WILD_FORM_FAMILY_MAUSHOLD;

    case SPECIES_DUDUNSPARCE_TWO_SEGMENT:
    case SPECIES_DUDUNSPARCE_THREE_SEGMENT:
        return WILD_FORM_FAMILY_DUDUNSPARCE;

    case SPECIES_BURMY_PLANT_CLOAK:
    case SPECIES_BURMY_SANDY_CLOAK:
    case SPECIES_BURMY_TRASH_CLOAK:
        return WILD_FORM_FAMILY_BURMY;

    case SPECIES_WORMADAM_PLANT_CLOAK:
    case SPECIES_WORMADAM_SANDY_CLOAK:
    case SPECIES_WORMADAM_TRASH_CLOAK:
        return WILD_FORM_FAMILY_WORMADAM;

    case SPECIES_SHELLOS_WEST_SEA:
    case SPECIES_SHELLOS_EAST_SEA:
        return WILD_FORM_FAMILY_SHELLOS;

    case SPECIES_GASTRODON_WEST_SEA:
    case SPECIES_GASTRODON_EAST_SEA:
        return WILD_FORM_FAMILY_GASTRODON;

    case SPECIES_BASCULIN_RED_STRIPED:
    case SPECIES_BASCULIN_BLUE_STRIPED:
    case SPECIES_BASCULIN_WHITE_STRIPED:
        return WILD_FORM_FAMILY_BASCULIN;

    case SPECIES_BASCULEGION_MALE:
    case SPECIES_BASCULEGION_FEMALE:
        return WILD_FORM_FAMILY_BASCULEGION;

    case SPECIES_DEERLING_SPRING:
    case SPECIES_DEERLING_SUMMER:
    case SPECIES_DEERLING_AUTUMN:
    case SPECIES_DEERLING_WINTER:
        return WILD_FORM_FAMILY_DEERLING;

    case SPECIES_SAWSBUCK_SPRING:
    case SPECIES_SAWSBUCK_SUMMER:
    case SPECIES_SAWSBUCK_AUTUMN:
    case SPECIES_SAWSBUCK_WINTER:
        return WILD_FORM_FAMILY_SAWSBUCK;

    case SPECIES_ORICORIO_BAILE:
    case SPECIES_ORICORIO_POM_POM:
    case SPECIES_ORICORIO_PAU:
    case SPECIES_ORICORIO_SENSU:
        return WILD_FORM_FAMILY_ORICORIO;

    case SPECIES_LYCANROC_MIDDAY:
    case SPECIES_LYCANROC_MIDNIGHT:
    case SPECIES_LYCANROC_DUSK:
        return WILD_FORM_FAMILY_LYCANROC;

    case SPECIES_TOXTRICITY_AMPED:
    case SPECIES_TOXTRICITY_LOW_KEY:
        return WILD_FORM_FAMILY_TOXTRICITY;

    case SPECIES_PUMPKABOO_AVERAGE:
    case SPECIES_PUMPKABOO_SMALL:
    case SPECIES_PUMPKABOO_LARGE:
    case SPECIES_PUMPKABOO_SUPER:
        return WILD_FORM_FAMILY_PUMPKABOO;

    case SPECIES_GOURGEIST_AVERAGE:
    case SPECIES_GOURGEIST_SMALL:
    case SPECIES_GOURGEIST_LARGE:
    case SPECIES_GOURGEIST_SUPER:
        return WILD_FORM_FAMILY_GOURGEIST;

    case SPECIES_FLABEBE_RED_FLOWER:
    case SPECIES_FLABEBE_YELLOW_FLOWER:
    case SPECIES_FLABEBE_ORANGE_FLOWER:
    case SPECIES_FLABEBE_BLUE_FLOWER:
    case SPECIES_FLABEBE_WHITE_FLOWER:
        return WILD_FORM_FAMILY_FLABEBE;

    case SPECIES_FLOETTE_RED_FLOWER:
    case SPECIES_FLOETTE_YELLOW_FLOWER:
    case SPECIES_FLOETTE_ORANGE_FLOWER:
    case SPECIES_FLOETTE_BLUE_FLOWER:
    case SPECIES_FLOETTE_WHITE_FLOWER:
        return WILD_FORM_FAMILY_FLOETTE;

    case SPECIES_FLORGES_RED_FLOWER:
    case SPECIES_FLORGES_YELLOW_FLOWER:
    case SPECIES_FLORGES_ORANGE_FLOWER:
    case SPECIES_FLORGES_BLUE_FLOWER:
    case SPECIES_FLORGES_WHITE_FLOWER:
        return WILD_FORM_FAMILY_FLORGES;

    case SPECIES_URSHIFU_SINGLE_STRIKE_STYLE:
    case SPECIES_URSHIFU_RAPID_STRIKE_STYLE:
        return WILD_FORM_FAMILY_URSHIFU;

    case SPECIES_TAUROS_PALDEAN_COMBAT_BREED:
    case SPECIES_TAUROS_PALDEAN_BLAZE_BREED:
    case SPECIES_TAUROS_PALDEAN_AQUA_BREED:
        return WILD_FORM_FAMILY_PALDEAN_TAUROS;

    case SPECIES_OINKOLOGNE_MALE:
    case SPECIES_OINKOLOGNE_FEMALE:
        return WILD_FORM_FAMILY_OINKOLOGNE;

    case SPECIES_MEOWSTIC_MALE:
    case SPECIES_MEOWSTIC_FEMALE:
        return WILD_FORM_FAMILY_MEOWSTIC;

    case SPECIES_INDEEDEE_MALE:
    case SPECIES_INDEEDEE_FEMALE:
        return WILD_FORM_FAMILY_INDEEDEE;

    case SPECIES_SINISTEA_PHONY:
    case SPECIES_SINISTEA_ANTIQUE:
        return WILD_FORM_FAMILY_SINISTEA;

    case SPECIES_POLTEAGEIST_PHONY:
    case SPECIES_POLTEAGEIST_ANTIQUE:
        return WILD_FORM_FAMILY_POLTEAGEIST;

    case SPECIES_GIMMIGHOUL_CHEST:
    case SPECIES_GIMMIGHOUL_ROAMING:
        return WILD_FORM_FAMILY_GIMMIGHOUL;
    }

    if(species == SPECIES_VIVILLON_ICY_SNOW
        || IsSpeciesInRange(species, SPECIES_VIVILLON_POLAR, SPECIES_VIVILLON_POKE_BALL))
        return WILD_FORM_FAMILY_VIVILLON;
    if(species == SPECIES_MINIOR_METEOR_RED
        || IsSpeciesInRange(species, SPECIES_MINIOR_METEOR_ORANGE, SPECIES_MINIOR_CORE_VIOLET))
        return WILD_FORM_FAMILY_MINIOR;
    if(species == SPECIES_ALCREMIE_STRAWBERRY_VANILLA_CREAM
        || IsSpeciesInRange(species, SPECIES_ALCREMIE_STRAWBERRY_RUBY_CREAM, SPECIES_ALCREMIE_STRAWBERRY_RAINBOW_SWIRL)
        || IsSpeciesInRange(species, SPECIES_ALCREMIE_BERRY_VANILLA_CREAM, SPECIES_ALCREMIE_RIBBON_RAINBOW_SWIRL))
        return WILD_FORM_FAMILY_ALCREMIE;

    return species;
}

static bool8 GetWildApprovedFormList(u16 familyKey, const u16 **speciesList, u16 *speciesCount)
{
    *speciesList = NULL;
    *speciesCount = 0;

    switch(familyKey)
    {
    case WILD_FORM_FAMILY_UNOWN:
        *speciesList = sWildUnownForms;
        *speciesCount = ARRAY_COUNT(sWildUnownForms);
        return TRUE;
    case WILD_FORM_FAMILY_SQUAWKABILLY:
        *speciesList = sWildSquawkabillyForms;
        *speciesCount = ARRAY_COUNT(sWildSquawkabillyForms);
        return TRUE;
    case WILD_FORM_FAMILY_VIVILLON:
        *speciesList = sWildVivillonForms;
        *speciesCount = ARRAY_COUNT(sWildVivillonForms);
        return TRUE;
    case WILD_FORM_FAMILY_FURFROU:
        *speciesList = sWildFurfrouForms;
        *speciesCount = ARRAY_COUNT(sWildFurfrouForms);
        return TRUE;
    case WILD_FORM_FAMILY_MINIOR:
        *speciesList = sWildMiniorForms;
        *speciesCount = ARRAY_COUNT(sWildMiniorForms);
        return TRUE;
    case WILD_FORM_FAMILY_ALCREMIE:
        *speciesList = sWildAlcremieForms;
        *speciesCount = ARRAY_COUNT(sWildAlcremieForms);
        return TRUE;
    case WILD_FORM_FAMILY_TATSUGIRI:
        *speciesList = sWildTatsugiriForms;
        *speciesCount = ARRAY_COUNT(sWildTatsugiriForms);
        return TRUE;
    case WILD_FORM_FAMILY_GIMMIGHOUL:
        *speciesList = sWildGimmighoulForms;
        *speciesCount = ARRAY_COUNT(sWildGimmighoulForms);
        return TRUE;
    }

    return FALSE;
}

#else
#define WILD_FORM_QUERY_NUM_SPECIES NUM_SPECIES

static u16 GetWildFormFamilyKey(u16 species)
{
    return species;
}

static bool8 GetWildApprovedFormList(u16 familyKey, const u16 **speciesList, u16 *speciesCount)
{
    *speciesList = NULL;
    *speciesCount = 0;
    return FALSE;
}
#endif

#ifdef ROGUE_EXPANSION
#define WILD_FORM_FAMILY_CAPACITY (WILD_FORM_FAMILY_GIMMIGHOUL + 1)
#else
#define WILD_FORM_FAMILY_CAPACITY WILD_FORM_QUERY_NUM_SPECIES
#endif

struct WildFormFamilyScratch
{
    u16 familyKeys[WILD_FORM_FAMILY_CAPACITY];
    u8 familyWeightsByKey[WILD_FORM_FAMILY_CAPACITY];
    u16 familyCount;
    u16 totalWeight;
    u16 lastSelectedFamilyKey;
};

static u16 SelectWildSpeciesFromApprovedForms(u16 familyKey, u16 randValue);
static u16 SelectWildSpeciesFromFormFamilies(u16 familyRand, u16 formRand, WeightCallback weightFunc, void *data, bool8 excludeSelectedFamily);

static u8 WildFormFlatWeight(u16 weightIndex UNUSED, u16 species UNUSED, void* data UNUSED)
{
    return 1;
}

u16 RogueDebug_GetWildFormFamilyKey(u16 species)
{
    return GetWildFormFamilyKey(species);
}

u16 RogueDebug_GetWildApprovedFamilyForm(u16 species, u16 randValue)
{
    return SelectWildSpeciesFromApprovedForms(GetWildFormFamilyKey(species), randValue);
}

u16 RogueDebug_SelectWildSpeciesFromCurrentQuery(u16 familyRand, u16 formRand, bool8 excludeSelectedFamily)
{
    return SelectWildSpeciesFromFormFamilies(familyRand, formRand, WildFormFlatWeight, NULL, excludeSelectedFamily);
}

static void BuildWildFormFamilyWeights(struct WildFormFamilyScratch *scratch, WeightCallback weightFunc, void *data)
{
    u16 species;
    u16 weightIndex = 0;
#ifdef ROGUE_EXPANSION
    bool8 furfrouAnchorEligible = FALSE;
#endif

    scratch->familyCount = 0;
    scratch->totalWeight = 0;
    scratch->lastSelectedFamilyKey = SPECIES_NONE;
    memset(scratch->familyWeightsByKey, 0, sizeof(scratch->familyWeightsByKey));

    for(species = SPECIES_NONE + 1; species < WILD_FORM_QUERY_NUM_SPECIES; ++species)
    {
        if(RogueMiscQuery_CheckState(species))
        {
            u8 weight = weightFunc(weightIndex, species, data);

            ++weightIndex;

#ifdef ROGUE_EXPANSION
            if(species == SPECIES_FURFROU_NATURAL)
                furfrouAnchorEligible = weight != 0;
#endif

            if(weight != 0)
            {
                u16 familyKey = GetWildFormFamilyKey(species);

#ifdef ROGUE_EXPANSION
                // Every Furfrou trim has Normal as its primary type. Requiring
                // Natural Form to survive the query makes the family occupy one
                // Normal-family slot instead of qualifying through nine extra
                // secondary types. A trim is chosen after family selection.
                if(familyKey == WILD_FORM_FAMILY_FURFROU
                    && !furfrouAnchorEligible)
                    continue;
#endif

                AGB_ASSERT(familyKey < WILD_FORM_FAMILY_CAPACITY);
                if(familyKey >= WILD_FORM_FAMILY_CAPACITY)
                    continue;

                if(scratch->familyWeightsByKey[familyKey] == 0)
                {
                    AGB_ASSERT(scratch->familyCount < WILD_FORM_FAMILY_CAPACITY);

                    if(scratch->familyCount < WILD_FORM_FAMILY_CAPACITY)
                    {
                        scratch->familyKeys[scratch->familyCount] = familyKey;
                        scratch->familyWeightsByKey[familyKey] = weight;
                        ++scratch->familyCount;
                        scratch->totalWeight += weight;
                    }
                }
                else if(weight > scratch->familyWeightsByKey[familyKey])
                {
                    scratch->totalWeight -= scratch->familyWeightsByKey[familyKey];
                    scratch->familyWeightsByKey[familyKey] = weight;
                    scratch->totalWeight += weight;
                }
            }
        }
    }
}

static void RemoveWildFormFamilyWeight(struct WildFormFamilyScratch *scratch, u16 familyKey)
{
    u8 familyWeight;

    if(familyKey >= WILD_FORM_FAMILY_CAPACITY)
        return;

    familyWeight = scratch->familyWeightsByKey[familyKey];
    scratch->familyWeightsByKey[familyKey] = 0;
    scratch->totalWeight -= familyWeight;
}

static u16 SelectWildFormFamilyFromWeights(struct WildFormFamilyScratch *scratch, u16 randValue)
{
    u16 targetWeight;
    u16 i;

    if(scratch->totalWeight == 0)
        return SPECIES_NONE;

    targetWeight = randValue % scratch->totalWeight;
    for(i = 0; i < scratch->familyCount; ++i)
    {
        u16 familyKey = scratch->familyKeys[i];
        u8 familyWeight = scratch->familyWeightsByKey[familyKey];

        if(targetWeight < familyWeight)
        {
            RemoveWildFormFamilyWeight(scratch, familyKey);
            scratch->lastSelectedFamilyKey = familyKey;
            return familyKey;
        }

        targetWeight -= familyWeight;
    }

    AGB_ASSERT(FALSE);
    return SPECIES_NONE;
}

static u16 SelectWildSpeciesFromApprovedForms(u16 familyKey, u16 randValue)
{
    const u16 *speciesList;
    u16 speciesCount;

#ifdef ROGUE_EXPANSION
    if(familyKey == WILD_FORM_FAMILY_PIKACHU)
    {
        // Preserve ordinary Pikachu, and therefore access to Raichu, on two
        // thirds of family rolls. The remaining third exposes a special form.
        if(randValue % 3 != 0)
            return SPECIES_PIKACHU;

        return sWildPikachuSpecialForms[(randValue / 3) % ARRAY_COUNT(sWildPikachuSpecialForms)];
    }
#endif

    if(GetWildApprovedFormList(familyKey, &speciesList, &speciesCount) && speciesCount != 0)
        return speciesList[randValue % speciesCount];

    return SPECIES_NONE;
}

static u16 SelectWildSpeciesFromEligibleFamily(u16 familyKey, u16 randValue, WeightCallback weightFunc, void *data)
{
    u16 species;
    u16 weightIndex = 0;
    u16 eligibleCount = 0;
    u16 target;

    for(species = SPECIES_NONE + 1; species < WILD_FORM_QUERY_NUM_SPECIES; ++species)
    {
        if(RogueMiscQuery_CheckState(species))
        {
            if(GetWildFormFamilyKey(species) == familyKey
                && weightFunc(weightIndex, species, data) != 0)
                ++eligibleCount;

            ++weightIndex;
        }
    }

    if(eligibleCount == 0)
        return SPECIES_NONE;

    target = randValue % eligibleCount;
    weightIndex = 0;

    for(species = SPECIES_NONE + 1; species < WILD_FORM_QUERY_NUM_SPECIES; ++species)
    {
        if(RogueMiscQuery_CheckState(species))
        {
            if(GetWildFormFamilyKey(species) == familyKey
                && weightFunc(weightIndex, species, data) != 0)
            {
                if(target == 0)
                    return species;

                --target;
            }

            ++weightIndex;
        }
    }

    AGB_ASSERT(FALSE);
    return SPECIES_NONE;
}

static void ExcludeWildFormFamilyFromQuery(u16 familyKey)
{
    u16 species;

    for(species = SPECIES_NONE + 1; species < WILD_FORM_QUERY_NUM_SPECIES; ++species)
    {
        if(RogueMiscQuery_CheckState(species) && GetWildFormFamilyKey(species) == familyKey)
            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, species);
    }
}

static u16 SelectWildSpeciesFromBuiltFormFamilies(struct WildFormFamilyScratch *scratch, u16 familyRand, u16 formRand, WeightCallback weightFunc, void *data, bool8 excludeSelectedFamily)
{
    u16 selectedFamilyKey = SelectWildFormFamilyFromWeights(scratch, familyRand);
    u16 species;

    if(selectedFamilyKey == SPECIES_NONE)
        return SPECIES_NONE;

    // Furfrou trims have distinct typings, so typed queries must select from
    // the eligible trims instead of leaking an unrelated approved form.
#ifdef ROGUE_EXPANSION
    if(selectedFamilyKey == WILD_FORM_FAMILY_FURFROU)
        species = SelectWildSpeciesFromEligibleFamily(selectedFamilyKey, formRand, weightFunc, data);
    else
#endif
        species = SelectWildSpeciesFromApprovedForms(selectedFamilyKey, formRand);

    // Families without curated form behavior use their species ID as the key.
    // The family was only added if that species was active with nonzero weight.
    if(species == SPECIES_NONE && selectedFamilyKey < WILD_FORM_QUERY_NUM_SPECIES)
        species = selectedFamilyKey;

    if(species == SPECIES_NONE)
        species = SelectWildSpeciesFromEligibleFamily(selectedFamilyKey, formRand, weightFunc, data);

    if(excludeSelectedFamily)
        ExcludeWildFormFamilyFromQuery(selectedFamilyKey);

    return species;
}

static u16 SelectWildSpeciesFromFormFamilies(u16 familyRand, u16 formRand, WeightCallback weightFunc, void *data, bool8 excludeSelectedFamily)
{
    struct WildFormFamilyScratch *scratch = Alloc(sizeof(*scratch));
    u16 species;

    if(scratch == NULL)
        return SPECIES_NONE;

    BuildWildFormFamilyWeights(scratch, weightFunc, data);
    species = SelectWildSpeciesFromBuiltFormFamilies(scratch, familyRand, formRand, weightFunc, data, excludeSelectedFamily);
    Free(scratch);
    return species;
}

static u8 WildDenEncounter_CalculateWeight(u16 index, u16 species, void* data)
{
    if(RoguePokedex_IsSpeciesParadox(species))
    {
        if(Rogue_GetCurrentDifficulty() < ROGUE_GYM_START_DIFFICULTY + 2)
            return 0;
    }

    if(IsRareWeightedSpecies(species))
    {
        // Rare species become more common into late game
        if(Rogue_GetCurrentDifficulty() >= ROGUE_GYM_MID_DIFFICULTY + 1)
            return 3;
        else if(Rogue_GetCurrentDifficulty() >= ROGUE_GYM_MID_DIFFICULTY - 1)
            return 2;
        else
            return 1;
    }

    return 3;
}

u16 Rogue_SelectWildDenEncounterRoom(void)
{
    u16 species;

    RogueMonQuery_Begin();

    RogueMonQuery_IsSpeciesActive();
    RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);
    RogueMonQuery_TransformIntoEggSpecies();
    RogueMonQuery_TransformIntoEvos(Rogue_CalculatePlayerMonLvl(), TRUE, FALSE);

    // Now transform back into egg species, so the spawning should still be deteministic
    // (although the type hints could be invalid)
    if(IsCurseActive(EFFECT_WILD_EGG_SPECIES))
        RogueMonQuery_TransformIntoEggSpecies();

    species = SelectWildSpeciesFromFormFamilies(RogueRandom(), RogueRandom(), WildDenEncounter_CalculateWeight, NULL, FALSE);

    RogueMonQuery_End();

    return species;
}

u16 Rogue_SelectUniqueDenEncounterRoom(void)
{
    return Rogue_SelectWildDenEncounterRoom();
}

//static u8 HoneyTree_CalculateWeight(u16 weightIndex, u16 species, void* data)
//{
//    u32 weight;
//    u8 type1 = RoguePokedex_GetSpeciesType(species, 0);
//    u8 type2 = RoguePokedex_GetSpeciesType(species, 1);
//
//    u8 matchingTypes = 0;
//    if(IS_STANDARD_TYPE(type1) && (gRogueRun.honeyTreePokeblockTypeFlags & MON_TYPE_VAL_TO_FLAGS(type1)))
//        ++matchingTypes;
//
//    if(type1 != type2 && IS_STANDARD_TYPE(type2) && (gRogueRun.honeyTreePokeblockTypeFlags & MON_TYPE_VAL_TO_FLAGS(type2)))
//        ++matchingTypes;
//
//    if(matchingTypes == 2)
//        weight = 32;
//    else
//        weight = 1;
//
//    if(IsRareWeightedSpecies(species))
//    {
//        weight /= 2;
//
//        if(weight == 0)
//            weight = 1;
//    }
//
//    return weight;
//}

u16 Rogue_SelectHoneyTreeEncounterRoom(void)
{
    // Intentionally use Random instead of RogueRandom as this may be conditionally rerolled per player
    // so could break with sacred ash
    //
    u16 species;
    u32 typeFlags;

    if(!HasHoneyTreeEncounterPending())
        return SPECIES_NONE;

    species = SPECIES_NONE;
    typeFlags = gRogueRun.honeyTreePokeblockTypeFlags;

    while(species == SPECIES_NONE)
    {
        RogueMonQuery_Begin();

        RogueMonQuery_IsSpeciesActive();

        // Prefilter to mons of types we're interested in
        RogueMonQuery_EvosContainType(QUERY_FUNC_INCLUDE, typeFlags);
        RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);

        RogueMonQuery_TransformIntoEggSpecies();
        RogueMonQuery_TransformIntoEvos(Rogue_CalculatePlayerMonLvl(), TRUE, FALSE);

        // Now we've evolved we're only caring about mons of this type
        //RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, typeFlags);
        {
            // Go through all types 1 by 1 and filter on top, so we only get matching dual types
            u8 type;

            for(type = 0; type < NUMBER_OF_MON_TYPES; ++type)
            {
                u32 currTypeFlag = MON_TYPE_VAL_TO_FLAGS(type);

                if(IS_STANDARD_TYPE(type) && (typeFlags & currTypeFlag))
                    RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, currTypeFlag);
            }
        }

        // Now transform back into egg species, so the spawning should still be deteministic
        // (although the type hints could be invalid)
        if(IsCurseActive(EFFECT_WILD_EGG_SPECIES))
            RogueMonQuery_TransformIntoEggSpecies();

        // Evolution can turn a species from the active regional Dex into one
        // that the current Trial does not allow (for example, Basculin into
        // Basculegion during a Paldea Trial). Filter the final encounter form.
        RogueTrial_FilterMonQuery();

        species = SelectWildSpeciesFromFormFamilies(Random(), Random(), WildFormFlatWeight, NULL, FALSE);

        if(species == SPECIES_NONE)
        {
            // Randomly remove one of the type flags for the next attempt
            u8 randType = Random() % NUMBER_OF_MON_TYPES;

            while(!IS_STANDARD_TYPE(randType) || (typeFlags & MON_TYPE_VAL_TO_FLAGS(randType)) == 0)
            {
                randType = Random() % NUMBER_OF_MON_TYPES;
            }

            typeFlags &= ~MON_TYPE_VAL_TO_FLAGS(randType);
        }

        RogueMonQuery_End();
    }

    return species;
}

void Rogue_ResetAdventurePathBuffers()
{
    memset(&gRogueAdvPath.routeHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueAdvPath.routeHistoryBuffer));
}

static u8 SelectRouteRoom_CalculateWeight(u16 index, u16 routeId, void* data, bool8 applyDelaySeeds)
{
    u8 const roomDelay = 3; // maybe can increase this once added more routes
    u8 difficulty = *((u8*)data);
    u16 currentSeed = difficulty % roomDelay;
    u16 roomSeed = (gRogueRun.baseSeed * 2135 ^ (13890 * routeId)) % roomDelay;

    if(HistoryBufferContains(&gRogueAdvPath.routeHistoryBuffer[0], ARRAY_COUNT(gRogueAdvPath.routeHistoryBuffer), routeId))
    {
        // Don't repeat routes on same screen
        return 0;
    }

    if(applyDelaySeeds)
    {
        if(roomSeed == currentSeed)
            return 255;
        else
            // Don't place routes we've recently seen
            return 0;
    }
    else
    {
        return 255;
    }
}

static u8 SelectRouteRoom_CalculateWeightDefault(u16 index, u16 routeId, void* data)
{
    return SelectRouteRoom_CalculateWeight(index, routeId, data, TRUE);
}

static u8 SelectRouteRoom_CalculateWeightFallback(u16 index, u16 routeId, void* data)
{
    return SelectRouteRoom_CalculateWeight(index, routeId, data, FALSE);
}

u8 Rogue_SelectRouteRoom(u8 difficulty)
{
    u16 routeId;

    RogueCustomQuery_Begin();
    {
        u16 i;

        for(i = 0; i < gRogueRouteTable.routeCount; ++i)
        {
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, i);
        }

        RogueWeightQuery_Begin();
        {
            RogueWeightQuery_CalculateWeights(SelectRouteRoom_CalculateWeightDefault, &difficulty);

            if(!RogueWeightQuery_HasAnyWeights())
            {
                // Fallback and include all routes
                RogueWeightQuery_CalculateWeights(SelectRouteRoom_CalculateWeightFallback, &difficulty);
            }

            routeId = RogueWeightQuery_SelectRandomFromWeights(RogueRandom());
        }
        RogueWeightQuery_End();
    }
    RogueCustomQuery_End();

    // Sanity check that we haven't already placed this route on this path
    AGB_ASSERT(!HistoryBufferContains(&gRogueAdvPath.routeHistoryBuffer[0], ARRAY_COUNT(gRogueAdvPath.routeHistoryBuffer), routeId));

    HistoryBufferPush(&gRogueAdvPath.routeHistoryBuffer[0], ARRAY_COUNT(gRogueAdvPath.routeHistoryBuffer), routeId);

    if(RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_ROUTE) != 0)
    {
        routeId = RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_ROUTE) - 1;
    }

    return routeId;
}

static void ResetSpecialEncounterStates(void)
{
    // Special states
    // Rayquaza
    VarSet(VAR_SKY_PILLAR_STATE, 2); // Keep in clean layout, but act as is R is has left for G/K cutscene
    //VarSet(VAR_SKY_PILLAR_RAQUAZA_CRY_DONE, 1); // Hide cutscene R
    FlagClear(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA_STILL); // Show battle
    FlagSet(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA); // Hide cutscene R

    // Mew
    FlagClear(FLAG_HIDE_MEW);
    FlagClear(FLAG_DEFEATED_MEW);
    FlagClear(FLAG_CAUGHT_MEW);

    // Deoxys
    FlagClear(FLAG_BATTLED_DEOXYS);
    FlagClear(FLAG_DEFEATED_DEOXYS);

    // Ho-oh + Lugia
    FlagClear(FLAG_CAUGHT_HO_OH);
    FlagClear(FLAG_CAUGHT_LUGIA);

    // Latis
    //FlagClear(FLAG_DEFEATED_LATIAS_OR_LATIOS);
    //FlagClear(FLAG_CAUGHT_LATIAS_OR_LATIOS);

    // Reset ledgendary encounters
    //FlagSet(FLAG_HIDE_SOUTHERN_ISLAND_EON_STONE);
}

enum
{
    RUN_PORTAL_STATE_WAIT_TRANSITION,
    RUN_PORTAL_STATE_RUN_PHASE,
};

enum
{
    RUN_PORTAL_LOADING_WINDOW_WIDTH = 12,
    RUN_PORTAL_LOADING_WINDOW_HEIGHT = 2,
    RUN_PORTAL_LOADING_WINDOW_PALETTE = 15,
    RUN_PORTAL_LOADING_WINDOW_TILE_START = 0x1E0,
};

static const u16 sRunPortalLoadingPalette[16] =
{
    RGB_BLACK,
    RGB_WHITE,
    RGB(8, 8, 8),
};

static const u8 sText_RunPortalLoading0[] = _("Loading");
static const u8 sText_RunPortalLoading1[] = _("Loading.");
static const u8 sText_RunPortalLoading2[] = _("Loading..");
static const u8 sText_RunPortalLoading3[] = _("Loading...");
static const u8 *const sRunPortalLoadingTexts[] =
{
    sText_RunPortalLoading0,
    sText_RunPortalLoading1,
    sText_RunPortalLoading2,
    sText_RunPortalLoading3,
};
static const u8 sRunPortalLoadingTextColors[] = {0, 1, 2};
static const struct WindowTemplate sRunPortalLoadingWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = (30 - RUN_PORTAL_LOADING_WINDOW_WIDTH) / 2,
    .tilemapTop = 9,
    .width = RUN_PORTAL_LOADING_WINDOW_WIDTH,
    .height = RUN_PORTAL_LOADING_WINDOW_HEIGHT,
    .paletteNum = RUN_PORTAL_LOADING_WINDOW_PALETTE,
    .baseBlock = RUN_PORTAL_LOADING_WINDOW_TILE_START,
};

#define tRunPortalState data[0]
#define tSavedFadeSpeed data[1]
#define tLoadingWindowId data[2]
#define tDotCount data[4]
#define tSetupPhase data[6]

static void RunPortal_PrintLoadingText(u8 windowId, u8 dotCount)
{
    const u8 *text = sRunPortalLoadingTexts[dotCount % ARRAY_COUNT(sRunPortalLoadingTexts)];
    u8 x = GetStringCenterAlignXOffset(FONT_NORMAL, text, RUN_PORTAL_LOADING_WINDOW_WIDTH * 8);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(0));
    AddTextPrinterParameterized3(windowId, FONT_NORMAL, x, 1, sRunPortalLoadingTextColors, TEXT_SKIP_DRAW, text);
    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static u8 RunPortal_CreateLoadingTextWindow(void)
{
    u8 windowId = AddWindow(&sRunPortalLoadingWindowTemplate);

    if(windowId != WINDOW_NONE)
    {
        LoadPalette(sRunPortalLoadingPalette, BG_PLTT_ID(RUN_PORTAL_LOADING_WINDOW_PALETTE), sizeof(sRunPortalLoadingPalette));
        RunPortal_PrintLoadingText(windowId, 0);
    }

    return windowId;
}

static void RunPortal_FinishTransitionTask(u8 taskId)
{
    u8 windowId = gTasks[taskId].tLoadingWindowId;

    if(windowId != WINDOW_NONE)
    {
        ClearWindowTilemap(windowId);
        RemoveWindow(windowId);
    }

    gSaveBlock2Ptr->optionsFadeSpeed = gTasks[taskId].tSavedFadeSpeed;
    ScriptContext_Enable();
    DestroyTask(taskId);
}

static void Task_RogueRunPortalTransition(u8 taskId)
{
    switch(gTasks[taskId].tRunPortalState)
    {
    case RUN_PORTAL_STATE_WAIT_TRANSITION:
        if(IsBattleTransitionDone())
        {
            BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
            gTasks[taskId].tDotCount = 0;
            gTasks[taskId].tSetupPhase = 0;
            gTasks[taskId].tLoadingWindowId = WINDOW_NONE;
            gTasks[taskId].tRunPortalState = RUN_PORTAL_STATE_RUN_PHASE;
        }
        break;

    case RUN_PORTAL_STATE_RUN_PHASE:
        if(gTasks[taskId].tSetupPhase >= BEGIN_RUN_PHASE_COUNT)
        {
            RunPortal_FinishTransitionTask(taskId);
        }
        else
        {
            if(gTasks[taskId].tLoadingWindowId == WINDOW_NONE)
                gTasks[taskId].tLoadingWindowId = RunPortal_CreateLoadingTextWindow();

            if(gTasks[taskId].tLoadingWindowId != WINDOW_NONE)
                RunPortal_PrintLoadingText(gTasks[taskId].tLoadingWindowId, gTasks[taskId].tDotCount++);

            BeginRogueRunPhase(gTasks[taskId].tSetupPhase);
            gTasks[taskId].tSetupPhase++;
        }
        break;
    }
}

void Rogue_StartRunPortalTransition(void)
{
    if(!FuncIsActiveTask(Task_RogueRunPortalTransition))
    {
        u8 taskId = CreateTask(Task_RogueRunPortalTransition, 1);
        gTasks[taskId].tSavedFadeSpeed = gSaveBlock2Ptr->optionsFadeSpeed;

        // The transition task owns the black-screen handoff into the warp. A
        // normal palette fade can finish before the field script warps and
        // briefly expose the hub again, so keep this on the proven transition
        // path even when the player's general fade option is Fast.
        gSaveBlock2Ptr->optionsFadeSpeed = OPTIONS_TEXT_SPEED_MID;
        PlaySE(SE_WARP_OUT);
        BattleTransition_StartOnField(B_TRANSITION_BLUR);
    }
}

#undef tRunPortalState
#undef tSavedFadeSpeed
#undef tLoadingWindowId
#undef tDotCount
#undef tSetupPhase

void Rogue_OnWarpIntoMap(void)
{
    if(sPendingRoomEntrySetup)
    {
        struct WarpData deferredWarp = {};

        // Room setup used to run while selecting the node, before DoWarp had
        // a chance to start its fade. Run the same work after the warp task
        // has reached the black-screen handoff instead.
        sPendingRoomEntrySetup = FALSE;
        sExecutingDeferredRoomEntry = TRUE;
        Rogue_OnSetWarpData(&deferredWarp);
        sExecutingDeferredRoomEntry = FALSE;

        // Rogue_OnSetWarpData normally performs these after room setup. The
        // deferred path suppresses that tail so the same work is completed
        // after the map has reached the transition handoff.
        gRogueLocal.totalMoneySpentOnMap = 0;
        FollowMon_OnWarp();
    }

    gRogueAdvPath.isOverviewActive = FALSE;

    VarSet(VAR_ROGUE_ACTIVE_POKEBLOCK, ITEM_NONE);
    FlagSet(FLAG_ROGUE_REWARD_ITEM_MART_DISABLED);
    FlagSet(FLAG_ROGUE_RARE_ITEM_MART_DISABLED);

    if(IsRareShopActive())
        FlagClear(FLAG_ROGUE_RARE_ITEM_MART_DISABLED);


    // Set new safari flag on entering area
    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE || gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL || gMapHeader.mapLayoutId == LAYOUT_ROGUE_INTERIOR_SAFARI_CAVE)
    {
        FlagSet(FLAG_ROGUE_WILD_SAFARI);
        RogueSafari_ResetSpawns();
    }
    else
        FlagClear(FLAG_ROGUE_WILD_SAFARI);


    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB_TRANSITION)
    {
        if(!Rogue_IsRunActive())
        {
            BeginRogueRun();
        }
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_ADVENTURE_PATHS)
    {
#ifdef DEBUG_FEATURE_FRAME_TIMERS
        u32 previewStartClock = RogueDebug_SampleClock();
#endif

        gRogueAdvPath.isOverviewActive = TRUE;

        // Paths generated after a boss do not use the run-start portal phases.
        // Finish their preview setup while the overview transition still
        // covers the map, rather than deferring it to node interaction.
        RogueAdv_CacheMiniBossPreviews();

#ifdef DEBUG_FEATURE_FRAME_TIMERS
        DebugPrintf("[Adventure Path] Overview previews: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - previewStartClock));
#endif
    }
    else if((gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_ADVENTURE_ENTRANCE || gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB_ADVENTURE_ENTERANCE) && Rogue_IsRunActive())
    {
        EndRogueRun();
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL)
    {
        // Generate starters now (Do it now, so config/pokedex settings can be used to limit starters moreso)
        struct StarterSelectionData starters = SelectStarterMons(FALSE);
        VarSet(VAR_ROGUE_STARTER0, starters.species[0]);
        VarSet(VAR_ROGUE_STARTER1, starters.species[1]);
        VarSet(VAR_ROGUE_STARTER2, starters.species[2]);

        FollowMon_SetGraphics(0, starters.species[0], starters.shinyState[0], 0);
        FollowMon_SetGraphics(1, starters.species[1], starters.shinyState[1], 0);
        FollowMon_SetGraphics(2, starters.species[2], starters.shinyState[2], 0);
    }

    if(Rogue_IsRunActive())
    {
        RogueToD_AddMinutes(60);
    }
    else
    {
        u16 specialState = VarGet(VAR_ROGUE_SPECIAL_MODE);

        if(specialState != ROGUE_SPECIAL_MODE_NONE)
        {
            if(specialState == ROGUE_SPECIAL_MODE_DECORATING)
            {
                // Maintain special state provided we're still in any player home area
                bool8 inHomeMap = RogueHub_IsPlayerBaseLayout(gMapHeader.mapLayoutId);

                if(!inHomeMap)
                    VarSet(VAR_ROGUE_SPECIAL_MODE, ROGUE_SPECIAL_MODE_NONE);
            }
            else
            {
                // Clear map state
                VarSet(VAR_ROGUE_SPECIAL_MODE, ROGUE_SPECIAL_MODE_NONE);
            }
        }


        // In the hub, clients just copy host state
        if(RogueMP_IsActive() && RogueMP_IsClient())
        {
            AGB_ASSERT(gRogueMultiplayer != NULL);
            RogueToD_SetTime(gRogueMultiplayer->gameState.hub.timeOfDay);
            RogueToD_SetSeason(gRogueMultiplayer->gameState.hub.season);
        }
        else
            RogueToD_AddMinutes(15);
    }
}

static void TryRandomanSpawn(u8 chance)
{
    if(Rogue_GetModeRules()->forceRandomanAlwaysActive || IsCurseActive(EFFECT_RANDOMAN_ALWAYS_SPAWN) || RogueRandomChance(chance, OVERWORLD_FLAG))
    {
        // Enable random trader
        FlagClear(FLAG_ROGUE_RANDOM_TRADE_DISABLED);

        // Update tracking flags
        FlagSet(FLAG_ROGUE_RANDOM_TRADE_WAS_ACTIVE);
    }
    else
    {
        FlagSet(FLAG_ROGUE_RANDOM_TRADE_DISABLED);

        FlagClear(FLAG_ROGUE_RANDOM_TRADE_WAS_ACTIVE);
    }
}

static void TryOptionalRandomanSpawn()
{
    if(IsCurseActive(EFFECT_RANDOMAN_ROUTE_SPAWN))
    {
        if(gRogueRun.hasPendingRivalBattle)
        {
            // Force off in scenario where rival would be here
            FlagSet(FLAG_ROGUE_RANDOM_TRADE_DISABLED);

            FlagClear(FLAG_ROGUE_RANDOM_TRADE_WAS_ACTIVE);
        }
        else
        {
            TryRandomanSpawn(20);
        }
    }
}

void Rogue_OnSetWarpData(struct WarpData *warp)
{
    if(!sExecutingDeferredRoomEntry)
        sPendingRoomEntrySetup = FALSE;

    if(!sExecutingDeferredRoomEntry && warp->mapGroup == MAP_GROUP(ROGUE_HUB) && warp->mapNum == MAP_NUM(ROGUE_HUB))
    {
        // Warping back to hub must be intentional
        return;
    }
    else if(!sExecutingDeferredRoomEntry && warp->mapGroup == MAP_GROUP(ROGUE_BOSS_VICTORY_LAP) && warp->mapNum == MAP_NUM(ROGUE_BOSS_VICTORY_LAP))
    {
        // Never override this warp
        return;
    }
    else if(!sExecutingDeferredRoomEntry && warp->mapGroup == MAP_GROUP(ROGUE_BOSS_FINAL) && warp->mapNum == MAP_NUM(ROGUE_BOSS_FINAL))
    {
        // Never override this warp
        return;
    }
    else if(!sExecutingDeferredRoomEntry && warp->mapGroup == MAP_GROUP(ROGUE_AREA_ADVENTURE_ENTRANCE) && warp->mapNum == MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE))
    {
        // Warping back to hub must be intentional
        return;
    }
    else if(!sExecutingDeferredRoomEntry && warp->mapGroup == MAP_GROUP(ROGUE_HUB_ADVENTURE_ENTERANCE) && warp->mapNum == MAP_NUM(ROGUE_HUB_ADVENTURE_ENTERANCE))
    {
        // Warping back to hub must be intentional
        return;
    }
    else if(!sExecutingDeferredRoomEntry && warp->mapGroup == MAP_GROUP(ROGUE_ADVENTURE_PATHS) && warp->mapNum == MAP_NUM(ROGUE_ADVENTURE_PATHS))
    {
        // Ensure the run has started if we're trying to directly warp into the paths screen
        if(!Rogue_IsRunActive())
        {
            BeginRogueRun();
        }
    }
    else if(!sExecutingDeferredRoomEntry && warp->warpId != 0 && warp->mapGroup == gSaveBlock1Ptr->location.mapGroup && warp->mapNum == gSaveBlock1Ptr->location.mapNum)
    {
        // Allow warping to non-0 warps within the same ID
        if(warp->warpId == WARP_ID_MAP_START)
        {
            // Warp back to start of map
            warp->warpId = 0;
        }
        return;
    }

    FlagClear(FLAG_ROGUE_MAP_EVENT);

    // Weird edge case fix for gyms
    VarSet(VAR_ROGUE_ALWAYS_ZERO, 0);

    // Reset preview data
    memset(&gRogueLocal.encounterPreview[0], 0, sizeof(gRogueLocal.encounterPreview));

    if(Rogue_IsRunActive())
    {
        u8 warpType = sExecutingDeferredRoomEntry ? ROGUE_WARP_TO_ROOM : RogueAdv_OverrideNextWarp(warp);

        if(warpType == ROGUE_WARP_TO_ADVPATH)
        {
        }
        else if(warpType == ROGUE_WARP_TO_ROOM)
        {
            if(!sExecutingDeferredRoomEntry)
            {
                sPendingRoomEntrySetup = TRUE;
            }
            else
            {
            ++gRogueRun.enteredRoomCounter;

            Rogue_ResetFlightCharges();
            FlagSet(FLAG_ROGUE_TERA_ORB_CHARGED);

            // Grow berries based on progress in runs (This will grow in run berries and hub berries)
            BerryTreeTimeUpdate(120);

            VarSet(VAR_ROGUE_DESIRED_WEATHER, WEATHER_NONE);

            // We're warping into a valid map
            // We've already set the next room type so adjust the scaling now
            switch(gRogueAdvPath.currentRoomType)
            {
                case ADVPATH_ROOM_RESTSTOP:
                {
                    FlagSet(FLAG_ROGUE_COURIER_READY);
                    FlagClear(FLAG_ROGUE_VENDING_MACHINE_USED);
                    TryRandomanSpawn(33);
                    break;
                }

                case ADVPATH_ROOM_ROUTE:
                {
                    u8 weatherChance = 5 + 20 * gRogueAdvPath.currentRoomParams.perType.route.difficulty;
#ifdef DEBUG_FEATURE_FRAME_TIMERS
                    u32 routeEntryStartClock = RogueDebug_SampleClock();
#endif

                    gRogueRun.currentRouteIndex = gRogueAdvPath.currentRoomParams.roomIdx;
                    RandomiseWildEncounters();
                    RogueRouteScenes_OnEnterRoute();
                    ResetTrainerBattles();
                    RandomiseBerryTrees();
                    RandomiseEnabledTrainers();
                    RogueRouteScenes_PrepareRouteTrainers();
                    RandomiseEnabledItems();
                    TryOptionalRandomanSpawn();

                    if(Rogue_GetCurrentDifficulty() != 0 && RogueRandomChance(weatherChance, OVERWORLD_FLAG))
                    {
                        u8 randIdx = RogueRandomRange(ARRAY_COUNT(gRogueRouteTable.routes[gRogueRun.currentRouteIndex].wildTypeTable), OVERWORLD_FLAG);
                        u16 chosenType = gRogueRouteTable.routes[gRogueRun.currentRouteIndex].wildTypeTable[randIdx];
                        u16 weatherType = gRogueTypeWeatherTable[chosenType];

                        VarSet(VAR_ROGUE_DESIRED_WEATHER, weatherType);
                    }

                    // Push notification for unique mon
                    {
                        u8 i;
                        u32 customMonId;
                        u16 species;
                        u16 count = GetCurrentWildEncounterCount();

                        for(i = 0; i < count; ++i)
                        {
                            species = GetWildGrassEncounter(i);
                            customMonId = RogueGift_TryFindEnabledDynamicCustomMonForSpecies(species);

                            if(customMonId)
                            {
                                Rogue_PushPopup_UniquePokemonDetected(species);
                            }
                        }
                    }
#ifdef DEBUG_FEATURE_FRAME_TIMERS
                    DebugPrintf("[Room Load] Route entry setup: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - routeEntryStartClock));
#endif
                    break;
                }

                case ADVPATH_ROOM_TEAM_HIDEOUT:
                {
                    gRogueRun.currentRouteIndex = gRogueAdvPath.currentRoomParams.roomIdx;

                    ResetTrainerBattles();
                    RandomiseEnabledTrainers();
                    RandomiseEnabledItems();
                    TryOptionalRandomanSpawn();

                    FlagClear(FLAG_ROGUE_TEAM_BOSS_DISABLED);

                    VarSet(VAR_ROGUE_DESIRED_WEATHER, Rogue_GetTrainerWeather(gRogueRun.teamBossTrainerNum));
                    break;
                }

                case ADVPATH_ROOM_BOSS:
                {
                    u16 trainerNum;
                    trainerNum = gRogueAdvPath.currentRoomParams.perType.boss.trainerNum;
                    gRogueLocal.rngSeedToRestore = gRngRogueValue;

                    gRogueRun.currentLevelOffset = 0;
                    RandomiseEnabledTrainers();
                    RandomiseEnabledItems();

                    VarSet(VAR_OBJ_GFX_ID_0, Rogue_GetTrainerObjectEventGfx(trainerNum));
                    VarSet(VAR_ROGUE_DESIRED_WEATHER, Rogue_GetTrainerWeather(trainerNum));

                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, trainerNum);
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, Rogue_GetTrainerTypeAssignment(trainerNum));

                    // Restore seed so it's predictable and we can reliably generate gym team in other encounters
                    gRngRogueValue = gRogueLocal.rngSeedToRestore;
                    break;
                }

                case ADVPATH_ROOM_MINIBOSS:
                {
                    u16 trainerNum;
                    trainerNum = gRogueAdvPath.currentRoomParams.perType.miniboss.trainerNum;
                    gRogueLocal.rngSeedToRestore = gRngRogueValue;

                    RandomiseMiniBossItemsAndRestoreSeed();

                    VarSet(VAR_OBJ_GFX_ID_0, Rogue_GetTrainerObjectEventGfx(trainerNum));
                    VarSet(VAR_ROGUE_DESIRED_WEATHER, Rogue_GetTrainerWeather(trainerNum));

                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, trainerNum);
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, Rogue_GetTrainerTypeAssignment(trainerNum));

                    break;
                }

                case ADVPATH_ROOM_LEGENDARY:
                {
                    u16 species = gRogueLegendaryEncounterInfo.mapTable[gRogueAdvPath.currentRoomParams.roomIdx].encounterId;
                    u32 customMonId = gRogueAdvPath.currentRoomParams.perType.legendary.customMonId;
                    ResetSpecialEncounterStates();
                    ResetTrainerBattles();
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, species);
                    FollowMon_SetGraphics(
                        0,
                        species,
                        gRogueAdvPath.currentRoomParams.perType.legendary.shinyState,
                        customMonId
                    );
                    break;
                }

                case ADVPATH_ROOM_WILD_DEN:
                {
                    ResetSpecialEncounterStates();
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, gRogueAdvPath.currentRoomParams.perType.wildDen.species);

                    FollowMon_SetGraphics(
                        0,
                        gRogueAdvPath.currentRoomParams.perType.wildDen.species,
                        gRogueAdvPath.currentRoomParams.perType.wildDen.shinyState,
                        0
                    );
                    break;
                }

                case ADVPATH_ROOM_UNIQUE_DEN:
                {
                    ResetSpecialEncounterStates();
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, gRogueAdvPath.currentRoomParams.perType.uniqueDen.species);

                    FollowMon_SetGraphics(
                        0,
                        gRogueAdvPath.currentRoomParams.perType.uniqueDen.species,
                        gRogueAdvPath.currentRoomParams.perType.uniqueDen.shinyState,
                        gRogueAdvPath.currentRoomParams.perType.uniqueDen.customMonId
                    );
                    break;
                }

                case ADVPATH_ROOM_HONEY_TREE:
                {
                    ResetSpecialEncounterStates();
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, gRogueAdvPath.currentRoomParams.perType.honeyTree.species);

                    RandomiseWildEncounters();

                    FollowMon_SetGraphics(
                        0,
                        gRogueAdvPath.currentRoomParams.perType.honeyTree.species,
                        gRogueAdvPath.currentRoomParams.perType.honeyTree.shinyState,
                        0
                    );

                    // Only clear the last scattered Pokeblock once we've actually entered the encounter
                    ClearHoneyTreePokeblock();
                    break;
                }

                case ADVPATH_ROOM_LAB:
                {
                    u8 i;
                    RandomiseCharmItems();

                    // Copy lab mons into enemy party, so we can send mons to the lab whilst being inside the lab
                    for(i = 0; i < LAB_MON_COUNT; ++i)
                    {
                        struct Pokemon* labMon = GetLabMon(i);
                        CopyMon(&gEnemyParty[i], labMon, sizeof(struct Pokemon));
                    }

                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, GetMonData(&gEnemyParty[0], MON_DATA_SPECIES));
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, GetMonData(&gEnemyParty[1], MON_DATA_SPECIES));
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, GetMonData(&gEnemyParty[2], MON_DATA_SPECIES));
                    break;
                }

                case ADVPATH_ROOM_ITEM:
                {
                    u8 scheduleSlot = gRogueAdvPath.currentRoomParams.perType.itemRoom.scheduleSlot;

                    FollowMon_SetGraphics(0, SPECIES_SABLEYE, FALSE, 0);
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, gRogueAdvPath.currentRoomParams.perType.itemRoom.itemId);
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, scheduleSlot);

                    if(scheduleSlot >= ITEM_ROOM_REWARD_COUNT)
                    {
                        AGB_ASSERT(FALSE);
                        FlagSet(FLAG_TEMP_1);
                    }
                    else if(RogueAdv_IsItemRoomRewardClaimed(scheduleSlot))
                        FlagSet(FLAG_TEMP_1);
                    else
                        FlagClear(FLAG_TEMP_1);
                    break;
                }

                case ADVPATH_ROOM_DARK_DEAL:
                {
                    RandomiseCharmItems();
                    break;
                }

                case ADVPATH_ROOM_SHRINE:
                {
                    break;
                }

                case ADVPATH_ROOM_CATCHING_CONTEST:
                {
                    break;
                }

                case ADVPATH_ROOM_BATTLE_SIM:
                {
                    break;
                }

                case ADVPATH_ROOM_BATTLE_TOWER:
                {
                    u8 const battleWeather[] =
                    {
                        WEATHER_NONE,
                        WEATHER_SUNNY,
                        WEATHER_SANDSTORM,
                        WEATHER_DROUGHT,
                        WEATHER_DOWNPOUR,
                        WEATHER_LEAVES,
                        WEATHER_RAIN_THUNDERSTORM,
                        WEATHER_PSYCHIC_FOG,
                        WEATHER_SNOW,
                        WEATHER_MISTY_FOG,
                        WEATHER_ACID_RAIN,
                        WEATHER_PLAIN_TERRAIN,
                        WEATHER_INFESTED_TERRAIN,
                        WEATHER_ECLIPSE,
                    };

                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, RogueRandom());

                    if(Rogue_GetConfigRange(CONFIG_RANGE_TRAINER) != DIFFICULTY_LEVEL_EASY)
                        VarSet(VAR_ROGUE_DESIRED_WEATHER, battleWeather[RogueRandomRange(ARRAY_COUNT(battleWeather), 0)]);

                    ResetTrainerBattles();
                    RandomiseEnabledTrainers();
                    break;
                }

                case ADVPATH_ROOM_GAMESHOW:
                {
                    FlagClear(FLAG_ROGUE_HIDE_GAMESHOW_REWARD);
                    break;
                }

                case ADVPATH_ROOM_SIGN:
                {
                    u8 i;
                    u16 strongestSpecies = SPECIES_NONE;
                    u16 trainerNum = gRogueRun.bossTrainerNums[Rogue_GetCurrentDifficulty()];

                    SetupTrainerBattleInternal(trainerNum);
                    Rogue_CreateTrainerParty(trainerNum, gEnemyParty, PARTY_SIZE, TRUE);
                    gRngRogueValue = gRogueLocal.rngSeedToRestore;

                    for(i = 0; i < PARTY_SIZE; ++i)
                    {
                        u16 species = GetMonData(&gEnemyParty[i], MON_DATA_SPECIES);
                        if(species != SPECIES_NONE)
                        {
                            if(strongestSpecies == SPECIES_NONE || RoguePokedex_GetSpeciesBST(species) > RoguePokedex_GetSpeciesBST(strongestSpecies))
                                strongestSpecies = species;
                        }
                    }

                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, GetMonData(&gEnemyParty[0], MON_DATA_SPECIES));
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, Rogue_GetTrainerTypeAssignment(trainerNum));
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, strongestSpecies);
                    break;
                }
            };

            // Update VARs
            VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, gRogueRun.enteredRoomCounter);
            VarSet(VAR_ROGUE_CURRENT_LEVEL_CAP, Rogue_CalculatePlayerMaxLvl());

#ifdef DEBUG_FEATURE_FRAME_TIMERS
            {
                u32 routeEntryHooksStartClock = RogueDebug_SampleClock();
#endif
                RogueQuest_OnTrigger(QUEST_TRIGGER_ENTER_ENCOUNTER);
                RogueTrial_OnEnterEncounter();
#ifdef DEBUG_FEATURE_FRAME_TIMERS
                if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
                    DebugPrintf("[Room Load] Route entry hooks: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - routeEntryHooksStartClock));
            }
#endif
            }
        }
    }

    if(!sExecutingDeferredRoomEntry)
    {
        gRogueLocal.totalMoneySpentOnMap = 0;
        FollowMon_OnWarp();
    }
}

void Rogue_ModifyMapHeader(struct MapHeader *mapHeader)
{
    // NOTE: This method shouldn't be used
    // For some reason editing the map header and repointing stuff messes with other collections in the header
    // e.g. repointing object events messes with the warps for some reason
}

void Rogue_ModifyMapWarpEvent(struct MapHeader *mapHeader, u8 warpId, struct WarpEvent *warp)
{
    RogueHub_ModifyMapWarpEvent(mapHeader, warpId, warp);
}

bool8 Rogue_AcceptMapConnection(struct MapHeader *mapHeader, const struct MapConnection *connection)
{
    if(!RogueHub_AcceptMapConnection(mapHeader, connection))
    {
        return FALSE;
    }

    return TRUE;
}

static bool8 IsHubMapGroup()
{
    return gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_HUB) || gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_AREA_HOME) || gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_INTERIOR_HOME);
}

static bool8 ShouldAdjustRouteObjectEvents()
{
    return gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_TEAM_HIDEOUT || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BATTLE_TOWER;
}

void Rogue_ModifyObjectEvents(struct MapHeader *mapHeader, bool8 loadingFromSave, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity)
{
    bool8 isLoadingSameMap = (gRogueLocal.recentObjectEventLoadedLayout == mapHeader->mapLayoutId);
    gRogueLocal.recentObjectEventLoadedLayout = mapHeader->mapLayoutId;

    // If we're in run and not trying to exit (gRogueAdvPath.currentRoomType isn't wiped at this point)
    if(Rogue_IsRunActive() && !IsHubMapGroup())
    {
        u8 originalObjectCount = *objectEventCount;

        if(mapHeader->mapLayoutId == LAYOUT_ROGUE_ADVENTURE_PATHS)
        {
            RogueAdv_ModifyObjectEvents(mapHeader, objectEvents, objectEventCount, objectEventCapacity);
        }
        else if(ShouldAdjustRouteObjectEvents() && !loadingFromSave)
        {
            u8 write, read;
            u16 trainerCounter;
            bool8 shouldGuaranteePokeblockBundle = gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE
                && Rogue_GetConfigToggle(CONFIG_TOGGLE_BAG_CLAUSE);
            bool8 hasVisiblePokeblockBundle = FALSE;
            u8 bundleCandidateObjects[OBJECT_EVENT_TEMPLATES_COUNT];
            u8 bundleCandidateCount = 0;

            trainerCounter = 0;
            write = 0;
            read = 0;

            for(;read < originalObjectCount; ++read)
            {
                if(write != read)
                {
                    memcpy(&objectEvents[write], &objectEvents[read], sizeof(struct ObjectEventTemplate));
                }

                // Adjust trainers
                //
                if(objectEvents[write].trainerType == TRAINER_TYPE_NORMAL && objectEvents[write].trainerRange_berryTreeId != 0)
                {
                    u16 trainerIndex = trainerCounter++;
                    u16 trainerNum = Rogue_GetDynamicTrainer(trainerIndex);

                    // Don't increment write, if we're not accepting the trainer // RogueRandomChanceTrainer
                    if(trainerNum != TRAINER_NONE)
                    {
                        objectEvents[write].graphicsId = OBJ_EVENT_GFX_DYNAMIC_TRAINER_FIRST + trainerIndex;
                        objectEvents[write].flagId = 0;//FLAG_ROGUE_TRAINER0 + ;

                        // Preserve Ninja Boys' signature hidden encounter without requiring
                        // route-specific object templates. Buried trainers remain stationary
                        // until they spot the player, then reveal themselves before approaching.
                        if(Rogue_GetTrainer(trainerNum)->trainerPic == TRAINER_PIC_NINJA_BOY)
                        {
                            objectEvents[write].movementType = MOVEMENT_TYPE_BURIED;
                            objectEvents[write].trainerType = TRAINER_TYPE_BURIED;
                        }

                        // Accept this trainer
                        write++;
                    }
                }
                // Adjust items
                //
                else if(objectEvents[write].flagId >= FLAG_ROGUE_ITEM_START && objectEvents[write].flagId <= FLAG_ROGUE_ITEM_END)
                {
                    // Already decided this earlier
                    if(!FlagGet(objectEvents[write].flagId))
                    {
                        u16 idx = objectEvents[write].flagId - FLAG_ROGUE_ITEM_START;
                        u16 itemId = SanitizeRouteRewardItem(VarGet(VAR_ROGUE_ITEM_START + idx));

                        VarSet(VAR_ROGUE_ITEM_START + idx, itemId);

                        // Default to a greyed out pokeball
                        objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_POKE_BALL;

                        if(itemId == ITEM_RARE_CANDY)
                        {
                            objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_RARE_CANDY;
                        }
                        else if(itemId == ITEM_ESCAPE_ROPE)
                        {
                            objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_ESCAPE_ROPE;
                        }
#ifdef ROGUE_EXPANSION
                        else if(itemId >= ITEM_LONELY_MINT && itemId <= ITEM_SERIOUS_MINT)
                        {
                            objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_MINT;
                        }
                        else if((itemId >= ITEM_BUG_TERA_SHARD && itemId <= ITEM_WATER_TERA_SHARD) || itemId == ITEM_STELLAR_TERA_SHARD)
                        {
                            objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_TERA_SHARD;
                        }
#endif
                        else if(Rogue_IsEvolutionItem(itemId))
                        {
                            objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_EVO_STONE;
                        }
                        else if(Rogue_IsTreasureItem(itemId))
                        {
                            objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_TREASURE;
                        }
                        else
                        {

                            switch (ItemId_GetPocket(itemId))
                            {
                            case POCKET_HELD_ITEMS:
                                objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_HOLD_ITEM;
                                break;

                            case POCKET_MEDICINE:
                                objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_MEDICINE;
                                break;

                            case POCKET_POKE_BALLS:
                                objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_BALL;
                                break;

                            case POCKET_TM_HM:
                                if(itemId >= ITEM_TR01 && itemId <= ITEM_TR50)
                                    objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_SILVER_TM;
                                else
                                    objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_GOLD_TM;
                                break;
#ifdef ROGUE_EXPANSION
                            case POCKET_STONES:
                                if((itemId >= ITEM_RED_ORB && itemId <= ITEM_BLUE_ORB) || IS_MEGA_STONE_ITEM(itemId))
                                    objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_MEGA_STONE;
                                else if(itemId >= ITEM_NORMALIUM_Z && itemId <= ITEM_ULTRANECROZIUM_Z)
                                    objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_Z_CRYSTAL;
                                else
                                    objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_HOLD_ITEM;
                                break;
#endif
                            }
                        }

                        if(shouldGuaranteePokeblockBundle)
                        {
                            if(itemId == ITEM_POKEBLOCK_BUNDLE)
                                hasVisiblePokeblockBundle = TRUE;
                            else if(CanReplaceRouteRewardWithPokeblockBundle(itemId))
                                bundleCandidateObjects[bundleCandidateCount++] = write;
                        }

                        // Accept this item
                        write++;
                    }
                }
                else
                {
                    // Accept all other types of object
                    write++;
                }
            }

            if(shouldGuaranteePokeblockBundle && !hasVisiblePokeblockBundle && bundleCandidateCount != 0)
            {
                u8 i;
                u8 selectedObject = bundleCandidateObjects[RogueRandomRange(bundleCandidateCount, FLAG_SET_SEED_ITEMS)];
                u16 selectedFlag = objectEvents[selectedObject].flagId;
                u16 selectedIdx = selectedFlag - FLAG_ROGUE_ITEM_START;

                VarSet(VAR_ROGUE_ITEM_START + selectedIdx, ITEM_POKEBLOCK_BUNDLE);

                for(i = 0; i < write; ++i)
                {
                    if(objectEvents[i].flagId == selectedFlag)
                        objectEvents[i].graphicsId = OBJ_EVENT_GFX_ITEM_POKE_BALL;
                }
            }

            *objectEventCount = write;

            if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
            {
#ifdef DEBUG_FEATURE_FRAME_TIMERS
                u32 sceneObjectStartClock = RogueDebug_SampleClock();
#endif
                RogueRouteScenes_ModifyObjectEvents(objectEvents, objectEventCount, objectEventCapacity);
#ifdef DEBUG_FEATURE_FRAME_TIMERS
                DebugPrintf("[Room Load] Scene object events: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - sceneObjectStartClock));
#endif
            }
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE && loadingFromSave)
        {
            // Authored-script restoration uses local IDs, so generated scene
            // objects must be rebound after it has run. The main NPC reuses an
            // anchor ID and props may reuse IDs removed by route generation.
            RogueRouteScenes_RestoreObjectEvents(
                objectEvents,
                *objectEventCount,
                mapHeader->events->objectEvents,
                mapHeader->events->objectEventCount);
        }

        // We need to reapply this as pending when loading from a save, as we would've already consumed it here
        if(loadingFromSave || isLoadingSameMap)
        {
            if(!FlagGet(FLAG_ROGUE_RIVAL_DISABLED))
                gRogueRun.hasPendingRivalBattle = TRUE;
        }

        // Attempt to find and activate the rival object
        FlagSet(FLAG_ROGUE_RIVAL_DISABLED);

        // Don't place rival battle on first encounter for first fight, otherwise place at earliest convenience :3
        if(Rogue_GetCurrentDifficulty() >= 1 || gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.x < gRogueAdvPath.pathLength - 1)
        {
            if(gRogueRun.hasPendingRivalBattle)
            {
                u8 i;

                for(i = 0; i < originalObjectCount; ++i)
                {
                    // Found rival, so make visible and clear pending
                    if(objectEvents[i].flagId == FLAG_ROGUE_RIVAL_DISABLED)
                    {
                        const struct RogueTrainer* trainer = Rogue_GetTrainer(gRogueRun.rivalTrainerNum);

                        FlagClear(FLAG_ROGUE_RIVAL_DISABLED);
                        gRogueRun.hasPendingRivalBattle = FALSE; // TODO - Need to make sure this works when warping within a map e.g. rocket base?

                        if(trainer != NULL)
                        {
                            objectEvents[i].graphicsId = trainer->objectEventGfx;
                        }
                        break;
                    }
                }
            }
            // Place randoman where rival would be
            else if(IsCurseActive(EFFECT_RANDOMAN_ROUTE_SPAWN) && gRogueAdvPath.currentRoomType != ADVPATH_ROOM_RESTSTOP)
            {
                u8 i;

                for(i = 0; i < originalObjectCount; ++i)
                {
                    // Found rival, so make visible and clear pending
                    if(objectEvents[i].flagId == FLAG_ROGUE_RIVAL_DISABLED)
                    {
                        objectEvents[i].flagId = FLAG_ROGUE_RANDOM_TRADE_DISABLED;
                        objectEvents[i].graphicsId = OBJ_EVENT_GFX_CONTEST_JUDGE;
                        objectEvents[i].script = Rogue_Encounter_RestStop_RandomMan;
                        break;
                    }
                }
            }
        }

        // Look for team boss NPC and update
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_TEAM_HIDEOUT)
        {
            u8 i;

            for(i = 0; i < originalObjectCount; ++i)
            {
                // Found rival, so make visible and clear pending
                if(objectEvents[i].flagId == FLAG_ROGUE_TEAM_BOSS_DISABLED)
                {
                    const struct RogueTrainer* trainer = Rogue_GetTrainer(gRogueRun.teamBossTrainerNum);

                    if(trainer != NULL)
                    {
                        objectEvents[i].graphicsId = trainer->objectEventGfx;
                    }
                    break;
                }
            }
        }
    }
    else
    {
        if(RogueHub_IsPlayerBaseLayout(mapHeader->mapLayoutId))
        {
            RogueHub_ModifyPlayerBaseObjectEvents(mapHeader->mapLayoutId, loadingFromSave, objectEvents, objectEventCount, objectEventCapacity);
        }
    }
}

static void PushFaintedMonToLab(struct Pokemon* srcMon)
{
    u16 temp;
    struct Pokemon* destMon;
    u16 i = Random() % (LAB_MON_COUNT + 1);

    if(Rogue_IsCatchingContestActive())
    {
        // Don't send temp catching contest mons to the lab
        return;
    }

    if(i >= LAB_MON_COUNT)
    {
        // Ignore this fainted mon
        return;
    }

    destMon = GetLabMon(i);
    CopyMon(destMon, srcMon, sizeof(*destMon));

    temp = max(1, GetMonData(destMon, MON_DATA_MAX_HP) / 2);
    SetMonData(destMon, MON_DATA_HP, &temp);

    // Wipe EVs
    temp = 0;
    SetMonData(destMon, MON_DATA_HP_EV, &temp);
    SetMonData(destMon, MON_DATA_ATK_EV, &temp);
    SetMonData(destMon, MON_DATA_DEF_EV, &temp);
    SetMonData(destMon, MON_DATA_SPEED_EV, &temp);
    SetMonData(destMon, MON_DATA_SPATK_EV, &temp);
    SetMonData(destMon, MON_DATA_SPDEF_EV, &temp);
    CalculateMonStats(destMon);
}

void Rogue_CopyLabEncounterMonNickname(u16 index, u8* dst)
{
    AGB_ASSERT(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LAB);

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LAB)
    {
        if(index < LAB_MON_COUNT)
        {
            GetMonData(&gEnemyParty[index], MON_DATA_NICKNAME, dst);
            StringGet_Nickname(dst);
        }
    }
}

bool8 Rogue_GiveLabEncounterMon(u16 index)
{
    AGB_ASSERT(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LAB);

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LAB)
    {
        if(gPlayerPartyCount < PARTY_SIZE && index < LAB_MON_COUNT)
        {
            struct Pokemon mon;
            bool8 gaveMon = FALSE;

            CopyMon(&mon, &gEnemyParty[index], sizeof(mon));

            // Already in safari from? (Maybe should track index and then wipe here, as we could have higher priority)
            mon.rogueExtraData.isSafariIllegal = TRUE;

            if(Rogue_PartyHasDuplicateSpecies(&mon, PARTY_SIZE, PARTY_SIZE))
            {
                gaveMon = CopyMonToPC(&mon) != MON_CANT_GIVE;
            }
            else if(!Rogue_TryRemoveDuplicateHeldItemForParty(&mon, PARTY_SIZE, PARTY_SIZE))
            {
                gaveMon = CopyMonToPC(&mon) != MON_CANT_GIVE;
            }
            else
            {
                CopyMon(&gPlayerParty[gPlayerPartyCount], &mon, sizeof(gPlayerParty[gPlayerPartyCount]));
                gaveMon = TRUE;
            }

            if(gaveMon)
            {
                gPlayerPartyCount = CalculatePlayerPartyCount();
                ResetFaintedLabMonAtSlot(index);
                return TRUE;
            }
        }
    }

    return FALSE;
}

void RemoveMonAtSlot(u8 slot, bool8 keepItems, bool8 compactPartySlots)
{
    if(slot < gPlayerPartyCount)
    {
        if(GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES) != SPECIES_NONE)
        {
            if(keepItems)
            {
                u16 heldItem = GetMonData(&gPlayerParty[slot], MON_DATA_HELD_ITEM);

                // Try to put held item back in bag
                if(heldItem != ITEM_NONE && AddBagItem(heldItem, 1))
                {
                    Rogue_PushPopup_AddItem(heldItem, 1);
                    heldItem = ITEM_NONE;
                    SetMonData(&gPlayerParty[slot], MON_DATA_HELD_ITEM, &heldItem);
                }
            }

            // Forget about re-equipping the held item
            gRogueRun.partyHeldItems[slot] = ITEM_NONE;

            PushFaintedMonToLab(&gPlayerParty[slot]);

            ZeroMonData(&gPlayerParty[slot]);

#ifdef ROGUE_EXPANSION
            if (gBattleStruct != NULL)
                gBattleStruct->changedSpecies[B_SIDE_PLAYER][slot] = SPECIES_NONE;
#endif

            if(compactPartySlots)
            {
                CompactPartySlots();
                CalculatePlayerPartyCount();
            }

            if(Rogue_IsRunActive())
                IncrementGameStat(GAME_STAT_POKEMON_RELEASED);
        }
    }
}

static void CheckAndNotifyForFaintedMons()
{
    if(Rogue_IsRunActive())
    {
        u8 i;
        u8 faintedCount = 0;

        for(i = 0; i < PARTY_SIZE; ++i)
        {
            bool8 hasValidSpecies = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE;

            if(hasValidSpecies && GetMonData(&gPlayerParty[i], MON_DATA_HP, NULL) != 0)
            {
                // This mon is alive
            }
            else if(hasValidSpecies)
            {
                ++faintedCount;

                if(gRogueLocal.partyRememberedHealth[i] != 0) // todo - check this still works
                {
                    IncrementGameStat(GAME_STAT_POKEMON_FAINTED);
                }
            }
        }

        if(faintedCount != 0)
        {
            RogueQuest_OnTrigger(QUEST_TRIGGER_MON_FAINTED);
        }
    }
}

static bool8 ShouldReleaseFaintedMonsNow(void)
{
    if(Rogue_IsRunActive())
    {
        // If we're finished, we don't want to release any mons, just check if anything has fainted or not
        if(Rogue_GetCurrentDifficulty() >= ROGUE_MAX_BOSS_COUNT)
            return FALSE;

        // Don't release any fainted mons for final champ fight
        if(Rogue_UseFinalQuestEffects() && Rogue_GetCurrentDifficulty() >= ROGUE_FINAL_CHAMP_DIFFICULTY)
            return FALSE;

        // Leave all mons in party
        if(Rogue_IsVictoryLapActive())
            return FALSE;
    }

    if(!Rogue_ShouldReleaseFaintedMons())
        return FALSE;

    return TRUE;
}

void RemoveAnyFaintedMons(bool8 keepItems)
{
    bool8 hasValidSpecies;
    u8 read;
    u8 write = 0;

    if(ShouldReleaseFaintedMonsNow())
    {
        for(read = 0; read < PARTY_SIZE; ++read)
        {
            hasValidSpecies = GetMonData(&gPlayerParty[read], MON_DATA_SPECIES) != SPECIES_NONE;

            if(hasValidSpecies && GetMonData(&gPlayerParty[read], MON_DATA_HP, NULL) != 0)
            {
                // This mon is alive

                // No need to do anything as this mon is in the correct slot
                if(write != read)
                {
                    CopyMon(&gPlayerParty[write], &gPlayerParty[read], sizeof(struct Pokemon));
                    ZeroMonData(&gPlayerParty[read]);
                }

                ++write;
            }
            else if(hasValidSpecies)
            {
                if(keepItems)
                {
                    // Dead so give back held item
                    u16 heldItem = GetMonData(&gPlayerParty[read], MON_DATA_HELD_ITEM);
                    if(heldItem != ITEM_NONE && AddBagItem(heldItem, 1))
                        Rogue_PushPopup_AddItem(heldItem, 1);
                }

                // Only push mons if run is active
                if(Rogue_IsRunActive() && !Rogue_IsVictoryLapActive())
                {
                    RogueSafari_PushMon(&gPlayerParty[read]);
                }

                if(Rogue_IsRunActive())
                    IncrementGameStat(GAME_STAT_POKEMON_RELEASED);

                PushFaintedMonToLab(&gPlayerParty[read]);

                ZeroMonData(&gPlayerParty[read]);

#ifdef ROGUE_EXPANSION
                if (gBattleStruct != NULL)
                    gBattleStruct->changedSpecies[B_SIDE_PLAYER][read] = SPECIES_NONE;
#endif
            }
        }
    }

    gPlayerPartyCount = CalculatePlayerPartyCount();
}

static bool8 PlayerPartyHasFaintedMon(void)
{
    u8 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE
        && GetMonData(&gPlayerParty[i], MON_DATA_HP) == 0)
            return TRUE;
    }

    return FALSE;
}

static bool8 CanOfferSacredAshRecovery(void)
{
    return ShouldReleaseFaintedMonsNow()
        && CheckBagHasItem(ITEM_SACRED_ASH, 1)
        && PlayerPartyHasFaintedMon();
}

static void DeferFaintedReleaseForSacredAsh(void)
{
    // Defer based on the party loss alone. Item availability is checked when
    // the field script actually offers recovery, after battle teardown has
    // finished and the live bag state is available.
    if(ShouldReleaseFaintedMonsNow() && PlayerPartyHasFaintedMon())
        sSacredAshRecoveryPending = TRUE;
    else
        RemoveAnyFaintedMons(FALSE);
}

static u8 BufferSacredAshLosses(void)
{
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u8 lossCount = 0;
    u8 i;

    gStringVar2[0] = EOS;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == SPECIES_NONE
        || GetMonData(&gPlayerParty[i], MON_DATA_HP) != 0)
            continue;

        if(lossCount != 0)
        {
            // Keep two nicknames on each line. Further lines scroll so every
            // loss remains visible in the standard two-line field message box.
            if((lossCount % 2) == 0)
                StringAppend(gStringVar2, gText_LineBreak);
            else
                StringAppend(gStringVar2, gText_CommaSpace);
        }

        GetMonData(&gPlayerParty[i], MON_DATA_NICKNAME, nickname);
        StringAppend(gStringVar2, nickname);
        lossCount++;
    }

    ConvertIntToDecimalStringN(gStringVar3, lossCount, STR_CONV_MODE_LEFT_ALIGN, 1);
    return lossCount;
}

u16 Rogue_BufferSacredAshRecovery(void)
{
    bool8 canRecover = sSacredAshRecoveryPending
        && CheckBagHasItem(ITEM_SACRED_ASH, 1)
        && PlayerPartyHasFaintedMon();

    // This value is returned directly because the event command `specialvar`
    // writes the special function's return value into its destination var.
    if(canRecover)
        BufferSacredAshLosses();

    return canRecover;
}

bool8 Rogue_HasPendingSacredAshRecovery(void)
{
    return sSacredAshRecoveryPending;
}

void Rogue_AcceptSacredAshRecovery(void)
{
    bool8 wasPending = sSacredAshRecoveryPending;

    if(wasPending
    && PlayerPartyHasFaintedMon()
    && RemoveBagItem(ITEM_SACRED_ASH, 1))
    {
        HealPlayerParty();
    }

    sSacredAshRecoveryPending = FALSE;
    if(wasPending)
        RemoveAnyFaintedMons(FALSE);
}

void Rogue_DeclineSacredAshRecovery(void)
{
    bool8 wasPending = sSacredAshRecoveryPending;

    sSacredAshRecoveryPending = FALSE;
    if(wasPending)
        RemoveAnyFaintedMons(FALSE);
}

#ifdef ROGUE_DEBUG
void RogueDebug_SetSacredAshRecoveryPending(bool8 pending)
{
    sSacredAshRecoveryPending = pending;
}

bool8 RogueDebug_CanOfferSacredAshRecovery(void)
{
    return CanOfferSacredAshRecovery();
}
#endif

struct Pokemon* GetRecordedPlayerPartyPtr();
struct Pokemon* GetRecordedEnemyPartyPtr();

static void TempSavePlayerTeam()
{
    u8 i;
    struct Pokemon* tempParty = GetRecordedPlayerPartyPtr();
    AGB_ASSERT(!gRogueLocal.hasUsePlayerTeamTempSave);

    for(i = 0; i < PARTY_SIZE; ++i)
        CopyMon(&tempParty[i], &gPlayerParty[i], sizeof(struct Pokemon));

    gRogueLocal.hasUsePlayerTeamTempSave = TRUE;
}

static void TempRestorePlayerTeam()
{
    u8 i;
    struct Pokemon* tempParty = GetRecordedPlayerPartyPtr();
    AGB_ASSERT(gRogueLocal.hasUsePlayerTeamTempSave);

    for(i = 0; i < PARTY_SIZE; ++i)
        CopyMon(&gPlayerParty[i], &tempParty[i], sizeof(struct Pokemon));

    CalculatePlayerPartyCount();
    gRogueLocal.hasUsePlayerTeamTempSave = FALSE;
}

static void ClearPlayerTeam()
{
    u8 i;

    for(i = 0; i < PARTY_SIZE; ++i)
        ZeroMonData(&gPlayerParty[i]);

    CalculatePlayerPartyCount();
}

static void TempSaveEnemyTeam()
{
    u8 i;
    struct Pokemon* tempParty = GetRecordedEnemyPartyPtr();
    AGB_ASSERT(!gRogueLocal.hasUseEnemyTeamTempSave);

    for(i = 0; i < PARTY_SIZE; ++i)
        CopyMon(&tempParty[i], &gEnemyParty[i], sizeof(struct Pokemon));

    gRogueLocal.hasUseEnemyTeamTempSave = TRUE;
}

static void UNUSED TempRestoreEnemyTeam()
{
    u8 i;
    struct Pokemon* tempParty = GetRecordedEnemyPartyPtr();
    AGB_ASSERT(gRogueLocal.hasUseEnemyTeamTempSave);

    for(i = 0; i < PARTY_SIZE; ++i)
        CopyMon(&gEnemyParty[i], &tempParty[i], sizeof(struct Pokemon));

    CalculateEnemyPartyCount();
    gRogueLocal.hasUseEnemyTeamTempSave = FALSE;
}

static bool8 AllowBattleGimics(u16 trainerNum, bool8 keyBattlesOnly)
{
    if(Rogue_IsRunActive())
    {
        // Always enable for key trainer
        if(Rogue_IsKeyTrainer(trainerNum))
            return TRUE;
        else if(!keyBattlesOnly)
            return RogueRandomChance(33, 0); // Only a chance?
        else
            return FALSE;
    }
    else
        return TRUE;
}

static void SetupTrainerBattleInternal(u16 trainerNum)
{
    bool8 shouldDoubleBattle = FALSE;
    gRogueLocal.hasBattleEventOccurred = FALSE;

    // Apply trainer specific seed
    // (seed regular RNG so the internal mon stats are always the same)
    gRogueLocal.rngSeedToRestore = gRngRogueValue;
    gRogueLocal.rngToRestore = gRngValue;
    gRogueLocal.rng2ToRestore = gRng2Value;

    SeedRogueRng(RogueRandom() + (trainerNum ^ RogueRandom()));
    SeedRng(RogueRandom() + (trainerNum ^ RogueRandom()));
    SeedRng2(RogueRandom() + (trainerNum ^ RogueRandom()));

    // enable dyanmax for this fight
    if(IsDynamaxEnabled() && AllowBattleGimics(trainerNum, TRUE))
        FlagSet(FLAG_ROGUE_DYNAMAX_BATTLE);
    else
        FlagClear(FLAG_ROGUE_DYNAMAX_BATTLE);

    // enable tera for this fight
    if(IsTerastallizeEnabled() && !Rogue_IsExpTrainer(trainerNum) && AllowBattleGimics(trainerNum, FALSE))
        FlagSet(FLAG_ROGUE_TERASTALLIZE_BATTLE);
    else
        FlagClear(FLAG_ROGUE_TERASTALLIZE_BATTLE);

    // Only allow one to be active at once, but make it random
    if(FlagGet(FLAG_ROGUE_DYNAMAX_BATTLE) && FlagGet(FLAG_ROGUE_TERASTALLIZE_BATTLE))
    {
        if(RogueRandom() % 2)
            FlagClear(FLAG_ROGUE_DYNAMAX_BATTLE);
        else
            FlagClear(FLAG_ROGUE_TERASTALLIZE_BATTLE);
    }

    switch(Rogue_GetConfigRange(CONFIG_RANGE_BATTLE_FORMAT))
    {
        case BATTLE_FORMAT_SINGLES:
            shouldDoubleBattle = FALSE;
            break;

        case BATTLE_FORMAT_DOUBLES:
            shouldDoubleBattle = TRUE;
            break;

        case BATTLE_FORMAT_MIXED:
            shouldDoubleBattle = (RogueRandom() % 2);
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
    }

    if(Rogue_IsExpTrainer(trainerNum))
        shouldDoubleBattle = FALSE;

    if(shouldDoubleBattle) //NoOfApproachingTrainers != 2
    {
        // No need to check opponent party as we force it to 2 below
        if(gPlayerPartyCount >= 2) // gEnemyPartyCount >= 2
        {
             // Force double?
            gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;
        }
    }
}

void Rogue_Battle_StartTrainerBattle(void)
{
    gRogueLocal.hasBattleInputStarted = FALSE;

    RogueTrial_OnTrainerBattleStart();

    // Remove soft level cap
    if(Rogue_IsExpTrainer(gTrainerBattleOpponent_A))
        gRogueRun.currentLevelOffset = 0;

    SetupTrainerBattleInternal(gTrainerBattleOpponent_A);

    RememberPartyHeldItems();
    RememberPartyHealth();

    if(Rogue_IsBattleSimTrainer(gTrainerBattleOpponent_A))
    {
        TempSavePlayerTeam();
        ClearPlayerTeam();
    }

    if(Rogue_IsRunActive())
    {
        IncrementGameStat(GAME_STAT_TOTAL_BATTLES);
        IncrementGameStat(GAME_STAT_TRAINER_BATTLES);

        if(Rogue_IsRivalTrainer(gTrainerBattleOpponent_A))
            IncrementGameStat(GAME_STAT_RIVAL_BATTLES);
    }

    RogueQuest_OnTrigger(QUEST_TRIGGER_TRAINER_BATTLE_START);
}

void Rogue_Battle_TrainerTeamReady(void)
{
    if(!Rogue_IsVictoryLapActive() && (Rogue_IsBossTrainer(gTrainerBattleOpponent_A) || Rogue_IsRivalTrainer(gTrainerBattleOpponent_A)))
    {
        Rogue_AddPartySnapshot(gTrainerBattleOpponent_A);
    }

    if(Rogue_IsFinalQuestFinalBoss())
    {
        u32 temp;
        u8 i, j;
        struct Pokemon* tempPlayerParty = GetRecordedPlayerPartyPtr();
        struct Pokemon* tempEnemyParty = GetRecordedEnemyPartyPtr();

        // Now swap teams but the player's team keeps it's IVs/EVs and the EnemyTeam keeps it's IVs/EVs
        TempSavePlayerTeam();
        TempSaveEnemyTeam();

        // Heal player's mons before swapping them over
        HealPlayerParty();

        for(i = 0; i < PARTY_SIZE; ++i)
        {
            CopyMon(&gEnemyParty[i], &gPlayerParty[i], sizeof(struct Pokemon));
            CopyMon(&gPlayerParty[i], &tempEnemyParty[i], sizeof(struct Pokemon));

            // Maintain IVs/EVs on swapped mons
            for(j = 0; j < NUM_STATS; ++j)
            {
                temp = GetMonData(&tempPlayerParty[i], MON_DATA_HP_IV + j);
                SetMonData(&gPlayerParty[i], MON_DATA_HP_IV + j, &temp);
                temp = GetMonData(&tempPlayerParty[i], MON_DATA_HP_EV + j);
                SetMonData(&gPlayerParty[i], MON_DATA_HP_EV + j, &temp);

                temp = GetMonData(&tempEnemyParty[i], MON_DATA_HP_IV + j);
                SetMonData(&gEnemyParty[i], MON_DATA_HP_IV + j, &temp);
                temp = GetMonData(&tempEnemyParty[i], MON_DATA_HP_EV + j);
                SetMonData(&gEnemyParty[i], MON_DATA_HP_EV + j, &temp);
            }

            // Make sure everything is lvl 100
            temp = Rogue_ModifyExperienceTables(gRogueSpeciesInfo[GetMonData(&gPlayerParty[i], MON_DATA_SPECIES)].growthRate, MAX_LEVEL);
            SetMonData(&gPlayerParty[i], MON_DATA_EXP, &temp);

            temp = Rogue_ModifyExperienceTables(gRogueSpeciesInfo[GetMonData(&gEnemyParty[i], MON_DATA_SPECIES)].growthRate, MAX_LEVEL);
            SetMonData(&gEnemyParty[i], MON_DATA_EXP, &temp);

            CalculateMonStats(&gPlayerParty[i]);
            CalculateMonStats(&gEnemyParty[i]);
        }

        CalculatePlayerPartyCount();
        CalculateEnemyPartyCount();
    }

    RogueTrial_OnTrainerTeamReady();

    // Can restore the seed now
    gRngRogueValue = gRogueLocal.rngSeedToRestore;
    gRngValue = gRogueLocal.rngToRestore;
    gRng2Value = gRogueLocal.rng2ToRestore;
}

static bool32 IsPlayerDefeated(u32 battleOutcome)
{
    switch (battleOutcome)
    {
    case B_OUTCOME_LOST:
    case B_OUTCOME_FORFEITED:
    case B_OUTCOME_DREW:
        return TRUE;
    case B_OUTCOME_WON:
    case B_OUTCOME_RAN:
    case B_OUTCOME_PLAYER_TELEPORTED:
    case B_OUTCOME_MON_FLED:
    case B_OUTCOME_CAUGHT:
        return FALSE;
    default:
        return FALSE;
    }
}

static bool32 DidPlayerRun(u32 battleOutcome)
{
    switch (battleOutcome)
    {
    case B_OUTCOME_RAN:
    case B_OUTCOME_PLAYER_TELEPORTED:
    case B_OUTCOME_MON_FLED:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 DidPlayerCatch(u32 battleOutcome)
{
    switch (battleOutcome)
    {
    case B_OUTCOME_CAUGHT:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 DidDefeatLegendaryDenMon(u32 battleOutcome, u16 species)
{
    return battleOutcome == B_OUTCOME_WON
        && gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY
        && RoguePokedex_IsSpeciesLegendary(species);
}

static bool32 DidCompleteWildChain(u32 battleOutcome)
{
    switch (battleOutcome)
    {
    case B_OUTCOME_WON:
        return TRUE;
    }

    return FALSE;
}

static bool32 DidFailWildChain(u32 battleOutcome, u16 species)
{
    switch (battleOutcome)
    {
    // If you catch anything, you end the chain
    case B_OUTCOME_CAUGHT:
        return TRUE;
    }

    // If we fail a battle against the mon we're chaining, end the chain
    if(GetWildChainSpecies() == species)
    {
        switch (battleOutcome)
        {
        case B_OUTCOME_LOST:
        case B_OUTCOME_DREW:
        case B_OUTCOME_RAN:
        case B_OUTCOME_PLAYER_TELEPORTED:
        case B_OUTCOME_MON_FLED:
        case B_OUTCOME_FORFEITED:
        case B_OUTCOME_MON_TELEPORTED:
            return TRUE;
        }
    }

    return FALSE;
}

const u16 gNatureEvRewardStatTable[NUM_NATURES][NUM_STATS] =
{
                       // Hp  Atk Def Spd Sp.Atk Sp.Def
    [NATURE_HARDY]   = {  6,  2,  2,  4,  4,     4},
    [NATURE_LONELY]  = {  4,  6,  0,  4,  4,     4},
    [NATURE_BRAVE]   = {  4,  6,  4,  0,  4,     4},
    [NATURE_ADAMANT] = {  4,  6,  4,  4,  0,     4},
    [NATURE_NAUGHTY] = {  4,  6,  4,  4,  4,     0},
    [NATURE_BOLD]    = {  4,  0,  6,  4,  4,     4},
    [NATURE_DOCILE]  = {  6,  4,  4,  4,  2,     2},
    [NATURE_RELAXED] = {  4,  4,  6,  0,  4,     4},
    [NATURE_IMPISH]  = {  4,  4,  6,  4,  0,     4},
    [NATURE_LAX]     = {  4,  4,  6,  4,  4,     0},
    [NATURE_TIMID]   = {  4,  0,  4,  6,  4,     4},
    [NATURE_HASTY]   = {  4,  4,  0,  6,  4,     4},
    [NATURE_SERIOUS] = {  6,  2,  4,  4,  2,     4},
    [NATURE_JOLLY]   = {  4,  4,  4,  6,  0,     4},
    [NATURE_NAIVE]   = {  4,  4,  4,  6,  4,     0},
    [NATURE_MODEST]  = {  4,  0,  4,  4,  6,     4},
    [NATURE_MILD]    = {  4,  4,  0,  4,  6,     4},
    [NATURE_QUIET]   = {  4,  4,  4,  0,  6,     4},
    [NATURE_BASHFUL] = {  6,  4,  2,  4,  4,     2},
    [NATURE_RASH]    = {  4,  4,  4,  4,  6,     0},
    [NATURE_CALM]    = {  4,  0,  4,  4,  4,     6},
    [NATURE_GENTLE]  = {  4,  4,  0,  4,  4,     6},
    [NATURE_SASSY]   = {  4,  4,  4,  0,  4,     6},
    [NATURE_CAREFUL] = {  4,  4,  4,  4,  0,     6},
    [NATURE_QUIRKY]  = {  2,  4,  4,  4,  4,     4},
};

// Modified version of MonGainEVs
// Award EVs based on nature
static void MonGainRewardEVs(struct Pokemon *mon)
{
    u8 evs[NUM_STATS];
    u16 evIncrease;
    u16 totalEVs;
    u16 heldItem;
    u8 holdEffect;
    int i, multiplier;
    u8 stat;
    u8 bonus;
    u8 nature;

    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, 0);
    nature = GetNature(mon);

    if (heldItem == ITEM_ENIGMA_BERRY)
    {
        if (gMain.inBattle)
            holdEffect = gEnigmaBerries[0].holdEffect;
        else
            holdEffect = gSaveBlock1Ptr->enigmaBerry.holdEffect;
    }
    else
    {
        holdEffect = ItemId_GetHoldEffect(heldItem);
    }

    stat = ItemId_GetSecondaryId(heldItem);
    bonus = ItemId_GetHoldEffectParam(heldItem);
    totalEVs = 0;

    for (i = 0; i < NUM_STATS; i++)
    {
        evs[i] = GetMonData(mon, MON_DATA_HP_EV + i, 0);
        totalEVs += evs[i];
    }

    for (i = 0; i < NUM_STATS; i++)
    {
        if (totalEVs >= MAX_TOTAL_EVS)
            break;

        if (CheckPartyHasHadPokerus(mon, 0))
            multiplier = 2;
        else
            multiplier = 1;

        if(multiplier == 0)
            continue;

        evIncrease = gNatureEvRewardStatTable[nature][i];

#ifdef ROGUE_EXPANSION
        if (holdEffect == HOLD_EFFECT_POWER_ITEM && stat == i)
            evIncrease += bonus;
#endif

        if (holdEffect == HOLD_EFFECT_MACHO_BRACE)
            multiplier *= 2;

        evIncrease *= multiplier;

        if (totalEVs + (s16)evIncrease > MAX_TOTAL_EVS)
            evIncrease = ((s16)evIncrease + MAX_TOTAL_EVS) - (totalEVs + evIncrease);

        if (evs[i] + (s16)evIncrease > MAX_PER_STAT_EVS)
        {
            int val1 = (s16)evIncrease + MAX_PER_STAT_EVS;
            int val2 = evs[i] + evIncrease;
            evIncrease = val1 - val2;
        }

        evs[i] += evIncrease;
        totalEVs += evIncrease;
        SetMonData(mon, MON_DATA_HP_EV + i, &evs[i]);
    }
}

void EnableRivalEncounterIfRequired()
{
    u8 i;

    gRogueRun.hasPendingRivalBattle = FALSE;

    for(i = 0; i < ROGUE_RIVAL_MAX_ROUTE_ENCOUNTERS; ++i)
    {
        if(gRogueRun.rivalEncounterDifficulties[i] == Rogue_GetCurrentDifficulty())
        {
            gRogueRun.hasPendingRivalBattle = TRUE;
            return;
        }
    }
}

static const u16 sRunRewardTypedPokeblocks[] =
{
    ITEM_POKEBLOCK_NORMAL,
    ITEM_POKEBLOCK_FIGHTING,
    ITEM_POKEBLOCK_FLYING,
    ITEM_POKEBLOCK_POISON,
    ITEM_POKEBLOCK_GROUND,
    ITEM_POKEBLOCK_ROCK,
    ITEM_POKEBLOCK_BUG,
    ITEM_POKEBLOCK_GHOST,
    ITEM_POKEBLOCK_STEEL,
    ITEM_POKEBLOCK_FIRE,
    ITEM_POKEBLOCK_WATER,
    ITEM_POKEBLOCK_GRASS,
    ITEM_POKEBLOCK_ELECTRIC,
    ITEM_POKEBLOCK_PSYCHIC,
    ITEM_POKEBLOCK_ICE,
    ITEM_POKEBLOCK_DRAGON,
    ITEM_POKEBLOCK_DARK,
#ifdef ROGUE_EXPANSION
    ITEM_POKEBLOCK_FAIRY,
#endif
};

static const u16 sRunRewardStatPokeblocks[] =
{
    ITEM_POKEBLOCK_HP,
    ITEM_POKEBLOCK_ATK,
    ITEM_POKEBLOCK_DEF,
    ITEM_POKEBLOCK_SPEED,
    ITEM_POKEBLOCK_SPATK,
    ITEM_POKEBLOCK_SPDEF,
};

static u16 ChooseRunRewardPokeblock(void)
{
    u16 roll = Random() % 100;

    if(roll < 3)
        return ITEM_POKEBLOCK_SHINY;

    if(roll < 15)
        return sRunRewardStatPokeblocks[Random() % ARRAY_COUNT(sRunRewardStatPokeblocks)];

    return sRunRewardTypedPokeblocks[Random() % ARRAY_COUNT(sRunRewardTypedPokeblocks)];
}

struct PotentialEvolutionStone
{
    u16 item;
    u16 targetSpecies;
};

static const struct PotentialEvolutionStone sPotentialEvolutionStones[] =
{
    { ITEM_FIRE_STONE, SPECIES_FLAREON },
    { ITEM_WATER_STONE, SPECIES_VAPOREON },
    { ITEM_THUNDER_STONE, SPECIES_JOLTEON },
    { ITEM_LEAF_STONE, SPECIES_LEAFEON },
    { ITEM_ICE_STONE, SPECIES_GLACEON },
    { ITEM_SUN_STONE, SPECIES_ESPEON },
    { ITEM_MOON_STONE, SPECIES_UMBREON },
};

static void TryRewardPotentialEvolutionStone(u16 trainerNum)
{
    u16 i;
    u16 validStoneCount = 0;
    u16 validStones[ARRAY_COUNT(sPotentialEvolutionStones)];
    bool8 hasPotential = FALSE;

    if (gBattleOutcome != B_OUTCOME_WON
     || Rogue_IsExpTrainer(trainerNum)
     || Rogue_IsBattleSimTrainer(trainerNum)
     || Rogue_IsVictoryLapActive()
     || FlagGet(FLAG_ROGUE_RUN_COMPLETED))
        return;

    for (i = 0; i < gPlayerPartyCount; ++i)
    {
        if (GetMonUniqueAbility(&gPlayerParty[i]) == ABILITY_POTENTIAL)
        {
            hasPotential = TRUE;
            break;
        }
    }

    if (!hasPotential || !RandomPercentage(RNG_ROGUE_POTENTIAL, 25))
        return;

    for (i = 0; i < ARRAY_COUNT(sPotentialEvolutionStones); ++i)
    {
        if (RoguePokedex_IsSpeciesEnabled(sPotentialEvolutionStones[i].targetSpecies))
            validStones[validStoneCount++] = sPotentialEvolutionStones[i].item;
    }

    if (validStoneCount != 0)
    {
        u16 item = validStones[RandomUniform(RNG_ROGUE_POTENTIAL_STONE, 0, validStoneCount - 1)];

        if (AddBagItem(item, 1))
            Rogue_PushPopup_AddItem(item, 1);
    }
}

static void TryRewardHiddenStashCoins(u16 trainerNum)
{
    u16 amount = gBattleResults.hiddenStashKOs;

    if (gBattleOutcome != B_OUTCOME_WON
     || amount == 0
     || Rogue_IsExpTrainer(trainerNum)
     || Rogue_IsBattleSimTrainer(trainerNum)
     || Rogue_IsVictoryLapActive())
        return;

    if (AddBagItem(ITEM_GIMMIGHOUL_COIN, amount))
        Rogue_PushPopup_AddItem(ITEM_GIMMIGHOUL_COIN, amount);
    else
        Rogue_PushPopup_CannotTakeItem(ITEM_GIMMIGHOUL_COIN, amount);
}

void Rogue_Battle_EndTrainerBattle(u16 trainerNum)
{
    sSacredAshRecoveryPending = FALSE;
    RogueTrial_OnTrainerBattleEnd();

    if(Rogue_IsFinalQuestFinalBoss())
    {
        TempRestorePlayerTeam();
    }

    if(Rogue_IsBattleSimTrainer(trainerNum))
    {
        TempRestorePlayerTeam();
    }

    TryRestorePartyHeldItems(FALSE);
    FlagClear(FLAG_ROGUE_DYNAMAX_BATTLE);
    FlagClear(FLAG_ROGUE_TERASTALLIZE_BATTLE);
    CheckAndNotifyForFaintedMons();
    RogueQuest_OnTrigger(QUEST_TRIGGER_TRAINER_BATTLE_END);
    if(Rogue_IsRunActive() && gBattleOutcome == B_OUTCOME_WON)
        RogueAdventureQuests_EmitSignal(ROGUE_ADVENTURE_QUEST_SIGNAL_TRAINER_DEFEATED, 1);
    TryRewardHiddenStashCoins(trainerNum);

    if(Rogue_IsRunActive())
    {
        bool8 isBossTrainer = Rogue_IsBossTrainer(trainerNum);

        if(Rogue_IsRivalTrainer(trainerNum) && Rogue_GetCurrentDifficulty() >= ROGUE_CHAMP_START_DIFFICULTY)
        {
            // If we fight rival in champ phase, it must've been a champ fight
            isBossTrainer = TRUE;
        }

        if(Rogue_IsVictoryLapActive() && IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            ++gRogueRun.victoryLapTotalWins;
            Rogue_PushPopup_VictoryLapProgress(Rogue_GetTrainerTypeAssignment(trainerNum), gRogueRun.victoryLapTotalWins);

            VarSet(VAR_TEMP_1, gRogueRun.victoryLapTotalWins);

            if(IsTerastallizeEnabled())
                FlagSet(FLAG_ROGUE_TERA_ORB_CHARGED);

            if(gRogueRun.victoryLapTotalWins == 15)
            {
                if(!CheckBagHasItem(ITEM_HEALING_FLASK, 1))
                {
                    AddBagItem(ITEM_HEALING_FLASK, 1);
                    Rogue_PushPopup_AddItem(ITEM_HEALING_FLASK, 1);
                }
            }
        }
        else
        {
            if (IsPlayerDefeated(gBattleOutcome) != TRUE && isBossTrainer)
            {
                u8 nextLevel;
                u8 prevLevel = Rogue_CalculateBossMonLvl();

                // Update badge for trainer card
                gRogueRun.completedBadges[Rogue_GetCurrentDifficulty()] = Rogue_GetTrainerTypeAssignment(trainerNum);

                if(gRogueRun.completedBadges[Rogue_GetCurrentDifficulty()] == TYPE_NONE)
                    gRogueRun.completedBadges[Rogue_GetCurrentDifficulty()] = TYPE_MYSTERY;

                // Increment difficulty
                Rogue_SetCurrentDifficulty(Rogue_GetCurrentDifficulty() + 1);
                nextLevel = Rogue_CalculateBossMonLvl();

                RogueQuest_OnTrigger(QUEST_TRIGGER_EARN_BADGE);

                gRogueRun.currentLevelOffset = nextLevel - prevLevel;
                gRogueRun.wildEncounters.roamerActiveThisPath = TRUE;

                // Increase run tutor move access for the Pokémon that earned this badge.
                {
                    u16 i;

                    for(i = 0; i < gPlayerPartyCount; ++i)
                    {
                        if(gPlayerParty[i].rogueExtraData.runTutorMoveLvl + 1 < TUTOR_MOVE_LVL_COUNT_RUN)
                            ++gPlayerParty[i].rogueExtraData.runTutorMoveLvl;
                    }
                }

                if(Rogue_GetCurrentDifficulty() >= ROGUE_MAX_BOSS_COUNT)
                {
                    // Snapshot HoF team
                    Rogue_AddPartySnapshot(-1);
                    UpdateTrainerCardMonIconsFromParty();

                    FlagSet(FLAG_IS_CHAMPION);
                    FlagSet(FLAG_ROGUE_RUN_COMPLETED);
                    RogueQuest_UnlockAllEvilTeamQuests();

                    if(!Rogue_ShouldDisableMainQuests())
                        RogueQuest_SetMonMasteryFlagFromParty();

                    RogueQuest_OnTrigger(QUEST_TRIGGER_ENTER_HALL_OF_FAME);
                    RogueQuest_OnTrigger(QUEST_TRIGGER_MISC_UPDATE);

                    // Increment stats
                    IncrementGameStat(GAME_STAT_RUN_WINS);
                    IncrementGameStat(GAME_STAT_CURRENT_RUN_WIN_STREAK);
                    SetGameStat(GAME_STAT_CURRENT_RUN_LOSS_STREAK, 0);

                    if(GetGameStat(GAME_STAT_CURRENT_RUN_WIN_STREAK) > GetGameStat(GAME_STAT_LONGEST_RUN_WIN_STREAK))
                        SetGameStat(GAME_STAT_LONGEST_RUN_WIN_STREAK, GetGameStat(GAME_STAT_CURRENT_RUN_WIN_STREAK));
                }
                else
                {
                    IncrementGameStat(GAME_STAT_TOTAL_BADGES);

                    // Increment stats
                    if(Rogue_GetCurrentDifficulty() <= ROGUE_ELITE_START_DIFFICULTY)
                        IncrementGameStat(GAME_STAT_GYM_BADGES);
                    else if(Rogue_GetCurrentDifficulty() <= ROGUE_CHAMP_START_DIFFICULTY)
                        IncrementGameStat(GAME_STAT_ELITE_BADGES);
                    else
                        IncrementGameStat(GAME_STAT_CHAMPION_BADGES);
                }

                FlagClear(FLAG_ROGUE_MYSTERIOUS_SIGN_KNOWN);
                VarSet(VAR_ROGUE_DIFFICULTY, Rogue_GetCurrentDifficulty());
                VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, max(Rogue_GetCurrentDifficulty(), VarGet(VAR_ROGUE_FURTHEST_DIFFICULTY)));

                EnableRivalEncounterIfRequired();
            }

            // Don't adjust soft cap for battle sim
            if(!Rogue_IsBattleSimTrainer(trainerNum))
            {
                // Adjust this after the boss reset
                if(gRogueRun.currentLevelOffset)
                {
                    u8 levelOffsetDelta = Rogue_GetModeRules()->levelOffsetInterval;

                    if(levelOffsetDelta == 0)
                    {
                        // Apply default
                        levelOffsetDelta = 4;
                    }

                    // Every trainer battle drops level cap slightly
                    if(gRogueRun.currentLevelOffset < levelOffsetDelta)
                        gRogueRun.currentLevelOffset = 0;
                    else
                        gRogueRun.currentLevelOffset -= levelOffsetDelta;
                }
            }
        }

        if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            TryRewardPotentialEvolutionStone(trainerNum);
            DeferFaintedReleaseForSacredAsh();

            if(isBossTrainer)
            {
                Rogue_ClearTemporaryDarkDealCurse();

                if(IsCurseActive(EFFECT_SNOWBALL_CURSES) && Rogue_GetCurrentDifficulty() < ROGUE_MAX_BOSS_COUNT)
                {
                    // Add new curse
                    u16 tempBuffer[5];
                    u16 tempBufferCount = 0;
                    u16 itemId = Rogue_NextCurseItem(tempBuffer, tempBufferCount++);

                    AddBagItem(itemId, 1);
                    Rogue_PushPopup_AddItem(itemId, 1);
                }
            }

            if(IsCurseActive(EFFECT_SNAG_TRAINER_MON) && !Rogue_IsBattleSimTrainer(trainerNum) && !Rogue_IsExpTrainer(trainerNum))
                gRogueLocal.hasPendingSnagBattle = TRUE;

            // Reward EVs based on nature
            if(Rogue_GetConfigToggle(CONFIG_TOGGLE_EV_GAIN))
            {
                u16 i;

                for(i = 0; i < gPlayerPartyCount; ++i)
                {
                    if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE
                    && GetMonData(&gPlayerParty[i], MON_DATA_HP) != 0)
                    {
                        MonGainRewardEVs(&gPlayerParty[i]);
                        CalculateMonStats(&gPlayerParty[i]);
                    }
                }
            }
        }
    }
}

void Rogue_Battle_StartWildBattle(void)
{
    gRogueLocal.hasBattleInputStarted = FALSE;
    gRogueLocal.hasBattleEventOccurred = FALSE;
    RememberPartyHeldItems();
    RememberPartyHealth();

    // enable tera for this fight
    if(IsTerastallizeEnabled())
        FlagSet(FLAG_ROGUE_TERASTALLIZE_BATTLE);

    RogueQuest_OnTrigger(QUEST_TRIGGER_WILD_BATTLE_START);

    if(Rogue_IsRunActive())
    {
        IncrementGameStat(GAME_STAT_TOTAL_BATTLES);
        IncrementGameStat(GAME_STAT_WILD_BATTLES);
    }
}

void Rogue_Battle_EndWildBattle(void)
{
    u16 wildSpecies = GetMonData(&gEnemyParty[0], MON_DATA_SPECIES);

    sSacredAshRecoveryPending = FALSE;

    TryRestorePartyHeldItems(TRUE);
    CheckAndNotifyForFaintedMons();
    RogueQuest_OnTrigger(QUEST_TRIGGER_WILD_BATTLE_END);

    FlagClear(FLAG_ROGUE_IN_SNAG_BATTLE);

    if(DidCompleteWildChain(gBattleOutcome))
    {
        UpdateWildEncounterChain(wildSpecies);
        DebugPrintf("ShinyOdds n:%d odds:%d", GetWildChainCount(), GetEncounterChainShinyOdds(GetWildChainCount()));
    }
    else if(DidFailWildChain(gBattleOutcome, wildSpecies))
        UpdateWildEncounterChain(SPECIES_NONE);

    if(Rogue_IsRunActive())
    {
        if(DidPlayerCatch(gBattleOutcome))
        {
            IncrementGameStat(GAME_STAT_POKEMON_CAUGHT);

            if(IsMonShiny(&gEnemyParty[0]))
                IncrementGameStat(GAME_STAT_SHINIES_CAUGHT);

            if(RoguePokedex_IsSpeciesLegendary(wildSpecies))
                IncrementGameStat(GAME_STAT_LEGENDS_CAUGHT);

            if(Rogue_IsBattleRoamerMon(wildSpecies))
                IncrementGameStat(GAME_STAT_ROAMERS_CAUGHT);
        }

        if(DidDefeatLegendaryDenMon(gBattleOutcome, wildSpecies))
        {
            u32 customMonId = RogueGift_GetCustomMonId(&gEnemyParty[0]);

            RogueSafari_PushMon(&gEnemyParty[0]);

            if(customMonId != CUSTOM_MON_NONE)
            {
                RogueGift_RemoveDynamicCustomMon(customMonId);
                gRogueLocal.wildBattleCustomMonId = 0;
            }
        }

        if(gRogueRun.currentLevelOffset && !DidPlayerRun(gBattleOutcome))
        {
            u8 levelOffsetDelta = Rogue_GetModeRules()->levelOffsetInterval;

            if(levelOffsetDelta == 0)
            {
                // Apply default
                levelOffsetDelta = 4;
            }

            // Don't increase the level caps if we only caught the mon
            if(!DidPlayerCatch(gBattleOutcome))
            {
                // Every trainer battle drops level cap slightly
                if(gRogueRun.currentLevelOffset < levelOffsetDelta)
                    gRogueRun.currentLevelOffset = 0;
                else
                    gRogueRun.currentLevelOffset -= levelOffsetDelta;
            }
        }

        if(Rogue_IsBattleRoamerMon(wildSpecies))
        {
            if(gBattleOutcome == B_OUTCOME_CAUGHT || gBattleOutcome == B_OUTCOME_WON)
            {
                // Roamer is gone
                gRogueRun.wildEncounters.roamer.species = SPECIES_NONE;
            }
            else
            {
                if(gRogueRun.wildEncounters.roamer.species != wildSpecies)
                {
                    gRogueRun.wildEncounters.roamer.encounerCount = 0;
                    Rogue_PushPopup_RoamerPokemonActivated(wildSpecies);
                }
                else if(gRogueRun.wildEncounters.roamer.encounerCount < 10)
                    ++gRogueRun.wildEncounters.roamer.encounerCount;

                // Keep track of roamer
                gRogueRun.wildEncounters.roamer.species = wildSpecies;
                gRogueRun.wildEncounters.roamer.hpIV = GetMonData(&gEnemyParty[0], MON_DATA_HP_IV);
                gRogueRun.wildEncounters.roamer.attackIV = GetMonData(&gEnemyParty[0], MON_DATA_ATK_IV);
                gRogueRun.wildEncounters.roamer.defenseIV = GetMonData(&gEnemyParty[0], MON_DATA_DEF_IV);
                gRogueRun.wildEncounters.roamer.shinyFlag = GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY);
                gRogueRun.wildEncounters.roamer.speedIV = GetMonData(&gEnemyParty[0], MON_DATA_SPEED_IV);
                gRogueRun.wildEncounters.roamer.spAttackIV = GetMonData(&gEnemyParty[0], MON_DATA_SPATK_IV);
                gRogueRun.wildEncounters.roamer.spDefenseIV = GetMonData(&gEnemyParty[0], MON_DATA_SPDEF_IV);
                gRogueRun.wildEncounters.roamer.abilityNum = GetMonData(&gEnemyParty[0], MON_DATA_ABILITY_NUM);
                gRogueRun.wildEncounters.roamer.genderFlag = GetMonData(&gEnemyParty[0], MON_DATA_GENDER_FLAG);
                gRogueRun.wildEncounters.roamer.status = GetMonData(&gEnemyParty[0], MON_DATA_STATUS);
                gRogueRun.wildEncounters.roamer.hpPerc = (GetMonData(&gEnemyParty[0], MON_DATA_HP) * 100) / GetMonData(&gEnemyParty[0], MON_DATA_MAX_HP);
            }
        }

        if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            DeferFaintedReleaseForSacredAsh();
        }
    }

    if(gRogueLocal.isShrineChallengeActive)
    {
        gRogueLocal.isShrineChallengeActive = FALSE;
        gRogueLocal.wildBattleCustomMonId = 0;
    }
}

void Rogue_Safari_EndWildBattle(void)
{
    if(VarGet(VAR_ROGUE_INTRO_STATE) == ROGUE_INTRO_STATE_CATCH_MON)
    {
        if(gBattleOutcome == B_OUTCOME_CAUGHT)
        {
            u8 i;

            for(i = 0; i < gSaveBlock1Ptr->objectEventTemplatesCount; ++i)
            {
                // Hide all the mons and the NPC
                if(gSaveBlock1Ptr->objectEventTemplates[i].graphicsId == OBJ_EVENT_GFX_MISC_RUIN_MANIAC || (gSaveBlock1Ptr->objectEventTemplates[i].graphicsId >= OBJ_EVENT_GFX_FOLLOW_MON_FIRST && gSaveBlock1Ptr->objectEventTemplates[i].graphicsId <= OBJ_EVENT_GFX_FOLLOW_MON_LAST))
                {
                    RemoveObjectEventByLocalIdAndMap(gSaveBlock1Ptr->objectEventTemplates[i].localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
                    FlagSet(gSaveBlock1Ptr->objectEventTemplates[i].flagId);
                }

                // Move prof just above the player
                if(gSaveBlock1Ptr->objectEventTemplates[i].graphicsId == OBJ_EVENT_GFX_PROF_BIRCH)
                {
                    SetObjEventTemplateCoords(gSaveBlock1Ptr->objectEventTemplates[i].localId, gSaveBlock1Ptr->pos.x, gSaveBlock1Ptr->pos.y - 2);
                    TryMoveObjectEventToMapCoords(gSaveBlock1Ptr->objectEventTemplates[i].localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->pos.x, gSaveBlock1Ptr->pos.y - 2);
                }
            }

            // Prof may not have been in view, so force it to spawn
            TrySpawnObjectEvents(gSaveBlock1Ptr->pos.x, gSaveBlock1Ptr->pos.y);

            VarSet(VAR_ROGUE_INTRO_STATE, VarGet(VAR_ROGUE_INTRO_STATE) + 1);
        }
    }
    else if (gBattleOutcome == B_OUTCOME_CAUGHT)
    {
        u8 safariIndex = RogueSafari_GetPendingBattleMonIdx();
        RogueSafari_ClearSafariMonAtIdx(safariIndex);
    }
}

static void RememberPartyHeldItems()
{
    if(Rogue_IsRunActive())
    {
        u8 i;

        for(i = 0; i < PARTY_SIZE; ++i)
        {
            if(i >= gPlayerPartyCount)
                gRogueRun.partyHeldItems[i] = ITEM_NONE; // account for if we catch a mon
            else
                gRogueRun.partyHeldItems[i] = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
        }
    }
}

static void RememberPartyHealth()
{
    if(Rogue_IsRunActive())
    {
        u8 i;

        for(i = 0; i < PARTY_SIZE; ++i)
        {
            if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE)
                gRogueLocal.partyRememberedHealth[i] = GetMonData(&gPlayerParty[i], MON_DATA_HP);
            else
                gRogueLocal.partyRememberedHealth[i] = 0;
        }
    }
}

static bool8 TryConsumeHeldItemFromBuffer(u16* heldItems, u16 itemId)
{
    u8 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(heldItems[i] == itemId)
        {
            heldItems[i] = ITEM_NONE;
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 IsItemInRememberedHeldItems(u16 itemId)
{
    u8 i;

    if(itemId == ITEM_NONE)
        return FALSE;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(gRogueRun.partyHeldItems[i] == itemId)
            return TRUE;
    }

    return FALSE;
}

static void TryRestorePartyHeldItems(bool8 allowThief)
{
    if(Rogue_IsRunActive())
    {
        u8 i;
        u32 item;
        u16 successBerryIcon = ITEM_NONE;
        u16 failBerryIcon = ITEM_NONE;

        if(IsCurseActive(EFFECT_ITEM_SHUFFLE))
        {
            u16 currentHeldItems[PARTY_SIZE];

            for(i = 0; i < PARTY_SIZE; ++i)
            {
                if(i < gPlayerPartyCount)
                {
                    if(GetMonData(&gPlayerParty[i], MON_DATA_HP) == 0)
                    {
                        currentHeldItems[i] = ITEM_NONE;
                        continue;
                    }

                    currentHeldItems[i] = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);

                    item = ITEM_NONE;
                    SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &item);
                }
                else
                {
                    currentHeldItems[i] = ITEM_NONE;
                }
            }

            for(i = 0; i < gPlayerPartyCount; ++i)
            {
                item = gRogueRun.partyHeldItems[i];

                // Ignore fainted mons
                if(GetMonData(&gPlayerParty[i], MON_DATA_HP) == 0)
                    continue;

                if(item == ITEM_NONE)
                {
                    if(allowThief)
                    {
                        u16 currentItem = currentHeldItems[i];

                        if(currentItem != ITEM_NONE && !IsItemInRememberedHeldItems(currentItem))
                            SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &currentItem);
                    }
                    continue;
                }

                if(item >= FIRST_BERRY_INDEX && item <= LAST_BERRY_INDEX)
                {
                    if(TryConsumeHeldItemFromBuffer(currentHeldItems, item))
                    {
                        // Use the still-held shuffled berry without consuming a spare.
                    }
                    else if(RemoveBagItem(item, 1))
                    {
                        successBerryIcon = item;
                    }
                    else
                    {
                        failBerryIcon = item;
                        item = ITEM_NONE;
                    }
                }

                SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &item);
            }

            // Make a popup to indicate the berries have or haven't been requiped
            if(failBerryIcon != ITEM_NONE)
                Rogue_PushPopup_RequipBerryFail(failBerryIcon);
            else if(successBerryIcon != ITEM_NONE)
                Rogue_PushPopup_RequipBerrySuccess(successBerryIcon);

            return;
        }

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            item = gRogueRun.partyHeldItems[i];

            // Ignore fainted mons
            if(GetMonData(&gPlayerParty[i], MON_DATA_HP) == 0)
                continue;

            // We're still holding the same item no need to continue
            if(GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM) == item)
                continue;

            if(item == ITEM_NONE)
            {
                // We previously weren't holding anything but if we're allowed to steal then don't stomp over current held item
                if(allowThief)
                {
                    if(!Rogue_TryRemoveDuplicateHeldItemForParty(&gPlayerParty[i], i, PARTY_SIZE))
                    {
                        item = ITEM_NONE;
                        SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &item);
                    }
                    continue;
                }
            }

            if(Rogue_PartyHasHeldItem(item, i, PARTY_SIZE))
            {
                item = ITEM_NONE;
            }
            // Consume berries but attempt to auto re-equip from bag
            else if(item >= FIRST_BERRY_INDEX && item <= LAST_BERRY_INDEX)
            {
                if(RemoveBagItem(item, 1))
                    successBerryIcon = item;
                else
                {
                    failBerryIcon = item;
                    item = ITEM_NONE;
                }
            }

            SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &item);
        }

        // Make a popup to indicate the berries have or haven't been requiped
        if(failBerryIcon != ITEM_NONE)
            Rogue_PushPopup_RequipBerryFail(failBerryIcon);
        else if(successBerryIcon != ITEM_NONE)
            Rogue_PushPopup_RequipBerrySuccess(successBerryIcon);
    }
}

bool8 Rogue_AllowItemUse(u16 itemId)
{
    //if (gMain.inBattle)
    //{
    //    return FALSE;
    //}

    return TRUE;
}

void Rogue_OnItemUse(u16 itemId)
{
    // Expected to be called in such a way that we can check the var
    AGB_ASSERT(itemId == gSpecialVar_ItemId);

    if (gMain.inBattle)
    {
        RogueQuest_OnTrigger(QUEST_TRIGGER_BATTLE_ITEM_USED);
    }
    else
    {
        RogueQuest_OnTrigger(QUEST_TRIGGER_FIELD_ITEM_USED);
    }
}

void Rogue_OnSpendMoney(u32 money)
{
    gRogueLocal.totalMoneySpentOnMap += money;
}

u32 Rogue_GetTotalSpentOnActiveMap()
{
    return gRogueLocal.totalMoneySpentOnMap;
}

u16 Rogue_GetBagCapacity()
{
    if(gSaveBlock1Ptr->bagCapacityUpgrades >= ITEM_BAG_MAX_CAPACITY_UPGRADE)
        return BAG_ITEM_CAPACITY;
    else
    {
        // Takes into account BAG_ITEM_RESERVED_SLOTS
        u16 startingSlots = BAG_ITEM_CAPACITY - ITEM_BAG_MAX_CAPACITY_UPGRADE * ITEM_BAG_SLOTS_PER_UPGRADE;
        u16 slotCount = startingSlots + gSaveBlock1Ptr->bagCapacityUpgrades * ITEM_BAG_SLOTS_PER_UPGRADE;
        return min(slotCount, BAG_ITEM_CAPACITY);
    }
}

u16 Rogue_GetBagPocketAmountPerItem(u8 pocket)
{
    return MAX_BAG_ITEM_CAPACITY;
}

u32 Rogue_CalcBagUpgradeCost()
{
    return 500 + 250 * (u32)(gSaveBlock1Ptr->bagCapacityUpgrades);
}

void Rogue_AddPartySnapshot(u16 trainerId)
{
    AGB_ASSERT(gRogueRun.partySnapshotCount < ARRAY_COUNT(gRogueRun.partySnapshots));

    if(gRogueRun.partySnapshotCount < ARRAY_COUNT(gRogueRun.partySnapshots))
    {
        u8 i, s;
        u8 index = gRogueRun.partySnapshotCount++;
        memset(&gRogueRun.partySnapshots[index], 0, sizeof(gRogueRun.partySnapshots[index]));

        gRogueRun.partySnapshots[index].trainerId = trainerId;

        // Player party
        s = 0;

        for(i = 0; i < PARTY_SIZE; ++i)
        {
            if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(&gPlayerParty[i], MON_DATA_HP) != 0)
            {
                gRogueRun.partySnapshots[index].partySpeciesGfx[s] = FollowMon_GetMonGraphics(&gPlayerParty[i]);
                gRogueRun.partySnapshots[index].partyPersonalities[s] = (GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY) & ~24); // remove nature part as that might change
                ++s;
            }
        }

        // Opponent party
        s = 0;

        for(i = 0; i < PARTY_SIZE; ++i)
        {
            if(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(&gEnemyParty[i], MON_DATA_HP) != 0)
            {
                gRogueRun.partySnapshots[index].enemySpeciesGfx[s] = FollowMon_GetMonGraphics(&gEnemyParty[i]);
                ++s;
            }
        }
    }
}

#ifdef ROGUE_DEBUG
static u16 Debug_RandomActiveSpecies()
{
    u16 species;
    RogueMonQuery_Begin();
    RogueMonQuery_IsSpeciesActive();

    species = RogueMiscQuery_SelectRandomElement(Random());

    RogueMonQuery_End();

    return species;
}
#endif

void Rogue_DebugFillPartySnapshots()
{
#ifdef ROGUE_DEBUG
    u8 i, j, snapshotIndex;
    gRogueRun.partySnapshotCount = 0;

    for(i = 0; i < ROGUE_MAX_BOSS_COUNT; ++i)
    {
        snapshotIndex = gRogueRun.partySnapshotCount++;

        if(i == 0)
        {
            gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[0] = SPECIES_CHARMANDER;
            gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[0] = 123;

            for(j = 1; j < PARTY_SIZE; ++j)
            {
                gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[0] = SPECIES_NONE;
                gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[0] = 0;
            }
        }
        else if(i == 1)
        {
            gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[0] = SPECIES_CHARMELEON;
            gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[0] = 123;
        }
        else
        {
            gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[0] = SPECIES_CHARIZARD;
            gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[0] = 123;
        }

        if(i != 0)
        {
            for(j = 1; j < PARTY_SIZE; ++j)
            {
                switch (Random() % 6)
                {
                case 0:
                    gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[j] = Debug_RandomActiveSpecies() + ((Random() % 5) ? 0 : FOLLOWMON_SHINY_OFFSET);
                    gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[j] = Random();
                    break;

                case 1:
                    gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[j] = 0;
                    gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[j] = 0;
                    break;

                default:
                    if(gRogueRun.partySnapshots[snapshotIndex - 1].partySpeciesGfx[j] != 0)
                    {
                        RogueMonQuery_Begin();
                        if(gRogueRun.partySnapshots[snapshotIndex - 1].partySpeciesGfx[j] >= FOLLOWMON_SHINY_OFFSET)
                            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, gRogueRun.partySnapshots[snapshotIndex - 1].partySpeciesGfx[j] - FOLLOWMON_SHINY_OFFSET);
                        else
                            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, gRogueRun.partySnapshots[snapshotIndex - 1].partySpeciesGfx[j]);

                        RogueMonQuery_TransformIntoEvos(10 * i, TRUE, FALSE);

                        gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[j] = RogueMiscQuery_SelectRandomElement(Random());
                        gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[j] = gRogueRun.partySnapshots[snapshotIndex - 1].partyPersonalities[j];

                        RogueMonQuery_End();
                    }
                    else
                    {
                        gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[j] = Debug_RandomActiveSpecies() + ((Random() % 5) ? 0 : FOLLOWMON_SHINY_OFFSET);
                        gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[j] = Random();
                    }
                    break;
                }
            }
        }

        // Force spawn 2 large mons
        if(i == 4)
        {
            u8 randIdx = 1 + Random() % 5;
            gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[randIdx] = SPECIES_RAYQUAZA + ((Random() % 5) ? 0 : FOLLOWMON_SHINY_OFFSET);
            gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[randIdx] = Random();

            randIdx = 1 + Random() % 5;
            gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[randIdx] = SPECIES_KYOGRE + ((Random() % 5) ? 0 : FOLLOWMON_SHINY_OFFSET);
            gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[randIdx] = Random();
        }
    }
#endif
}

void Rogue_PreBattleSetup(void)
{
    if(IsCurseActive(EFFECT_ITEM_SHUFFLE))
    {
        u16 i;
        u8 size = CalculatePlayerPartyCount();

        for(i = 0; i < size; ++i)
            SwapMonItems(i, Random() % size, gPlayerParty);
    }
}

bool8 Rogue_OverrideTrainerItems(u16* items)
{
    u8 i;

    for (i = 0; i < MAX_TRAINER_ITEMS; i++)
    {
        items[i] = ITEM_NONE;
    }

    return TRUE;
}

static void SwapMonItems(u8 aIdx, u8 bIdx, struct Pokemon *party)
{
    if(aIdx != bIdx)
    {
        u16 itemA = GetMonData(&party[aIdx], MON_DATA_HELD_ITEM);
        u16 itemB = GetMonData(&party[bIdx], MON_DATA_HELD_ITEM);

        SetMonData(&party[aIdx], MON_DATA_HELD_ITEM, &itemB);
        SetMonData(&party[bIdx], MON_DATA_HELD_ITEM, &itemA);
    }
}

static bool8 CanLearnMoveByLvl(u16 species, u16 move, s32 level)
{
    u16 eggSpecies;
    s32 i;

    for (i = 0; gRoguePokemonProfiles[species].levelUpMoves[i].move != MOVE_NONE; i++)
    {
        u16 moveLevel;

        if(move == gRoguePokemonProfiles[species].levelUpMoves[i].move)
        {
            moveLevel = gRoguePokemonProfiles[species].levelUpMoves[i].level;

            if (moveLevel > level)
                return FALSE;

            return TRUE;
        }
    }

    eggSpecies = Rogue_GetEggSpecies(species);

    if(eggSpecies == species)
    {
        // Must be taught by some other means
        return TRUE;
    }
    else
    {
        // Check if we would've learnt this before evolving (not 100% as can skip middle exclusive learn moves)
        return CanLearnMoveByLvl(eggSpecies, move, level);
    }
}

void Rogue_ApplyMonCompetitiveSet(struct Pokemon* mon, u8 level, struct RoguePokemonCompetitiveSet const* preset, struct RoguePokemonCompetitiveSetRules const* rules)
{
#ifdef ROGUE_EXPANSION
    u16 const abilityCount = 3;
#else
    u16 const abilityCount = 2;
#endif
    u16 i;
    u16 move;
    u8 writeMoveIdx;
    u16 initialMonMoves[MAX_MON_MOVES];
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    bool8 useMaxHappiness = TRUE;

    if(!rules->skipMoves)
    {
        // We want to start writing the move from the first free slot and loop back around
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            initialMonMoves[i] = GetMonData(mon, MON_DATA_MOVE1 + i);

            move = MOVE_NONE;
            SetMonData(mon, MON_DATA_MOVE1 + i, &move);
        }
    }

    if(!rules->skipAbility)
    {
        if(preset->ability != ABILITY_NONE)
        {
            // We need to set the ability index
            for(i = 0; i < abilityCount; ++i)
            {
                SetMonData(mon, MON_DATA_ABILITY_NUM, &i);

                if(GetMonAbility(mon) == preset->ability)
                {
                    // Ability is set
                    break;
                }
            }

            if(i >= abilityCount)
            {
                // We failed to set it, so fall back to default
                i = 0;
                SetMonData(mon, MON_DATA_ABILITY_NUM, &i);
            }
        }
    }

    if(!rules->skipNature)
    {
        SetNature(mon, preset->nature);
    }

    if(!rules->skipHeldItem)
    {
        if(preset->heldItem != ITEM_NONE)
        {
            SetMonData(mon, MON_DATA_HELD_ITEM, &preset->heldItem);
        }
    }

    // Teach moves from set that we can learn at this lvl
    writeMoveIdx = 0;

    if(!rules->skipMoves)
    {
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            move = preset->moves[i];

            if(move != MOVE_NONE && CanLearnMoveByLvl(species, move, level))
            {
                if(move == MOVE_FRUSTRATION)
                    useMaxHappiness = FALSE;

                SetMonData(mon, MON_DATA_MOVE1 + writeMoveIdx, &move);
                SetMonData(mon, MON_DATA_PP1 + writeMoveIdx, &gBattleMoves[move].pp);
                ++writeMoveIdx;
            }
        }

        if(rules->allowMissingMoves)
        {
            // Fill the remainer slots with empty moves
            for (i = writeMoveIdx; i < MAX_MON_MOVES; i++)
            {
                move = MOVE_NONE;
                SetMonData(mon, MON_DATA_MOVE1 + i, &move);
                SetMonData(mon, MON_DATA_PP1 + i, &gBattleMoves[move].pp);
            }
        }
        else
        {
            // Try to re-teach initial moves to fill out last slots
            for(i = 0; i < MAX_MON_MOVES && writeMoveIdx < MAX_MON_MOVES; ++i)
            {
                move = initialMonMoves[i];

                if(move != MOVE_NONE && !MonKnowsMove(mon, move))
                {
                    SetMonData(mon, MON_DATA_MOVE1 + writeMoveIdx, &move);
                    SetMonData(mon, MON_DATA_PP1 + writeMoveIdx, &gBattleMoves[move].pp);
                    ++writeMoveIdx;
                }
            }
        }
    }

    move = useMaxHappiness ? MAX_FRIENDSHIP : 0;
    SetMonData(mon, MON_DATA_FRIENDSHIP, &move);

#ifdef ROGUE_EXPANSION
    if(!rules->skipTeraType)
    {
        u32 teraType = preset->teraType;
        if(teraType != TYPE_NONE)
        {
            SetMonData(mon, MON_DATA_TERA_TYPE, &teraType);
        }
    }
#endif

    if(!rules->skipHiddenPowerType)
    {
        u8 hiddenPowerType = preset->hiddenPowerType;

        if(hiddenPowerType != TYPE_NONE)
        {
            u16 value;
            bool8 ivStatsOdd[6];

            if(hiddenPowerType == TYPE_MYSTERY)
            {
                // Source data didn't provide type so just default to primary type
                hiddenPowerType = RoguePokedex_GetSpeciesType(species, 0);
            }

            #define oddHP ivStatsOdd[0]
            #define oddAtk ivStatsOdd[1]
            #define oddDef ivStatsOdd[2]
            #define oddSpeed ivStatsOdd[3]
            #define oddSpAtk ivStatsOdd[4]
            #define oddSpDef ivStatsOdd[5]

            oddHP = TRUE;
            oddAtk = TRUE;
            oddDef = TRUE;
            oddSpeed = TRUE;
            oddSpAtk = TRUE;
            oddSpDef = TRUE;

            switch(hiddenPowerType)
            {
                case TYPE_FIGHTING:
                    oddDef = FALSE;
                    oddSpeed = FALSE;
                    oddSpAtk = FALSE;
                    oddSpDef = FALSE;
                    break;

                case TYPE_FLYING:
                    oddSpeed = FALSE;
                    oddSpAtk = FALSE;
                    oddSpDef = FALSE;
                    break;

                case TYPE_POISON:
                    oddDef = FALSE;
                    oddSpAtk = FALSE;
                    oddSpDef = FALSE;
                    break;

                case TYPE_GROUND:
                    oddSpAtk = FALSE;
                    oddSpDef = FALSE;
                    break;

                case TYPE_ROCK:
                    oddDef = FALSE;
                    oddSpeed = FALSE;
                    oddSpDef = FALSE;
                    break;

                case TYPE_BUG:
                    oddSpeed = FALSE;
                    oddSpDef = FALSE;
                    break;

                case TYPE_GHOST:
                    oddAtk = FALSE;
                    oddSpDef = FALSE;
                    break;

                case TYPE_STEEL:
                    oddSpDef = FALSE;
                    break;

                case TYPE_FIRE:
                    oddAtk = FALSE;
                    oddSpeed = FALSE;
                    oddSpAtk = FALSE;
                    break;

                case TYPE_WATER:
                    oddSpeed = FALSE;
                    oddSpAtk = FALSE;
                    break;

                case TYPE_GRASS:
                    oddHP = FALSE;
                    oddSpAtk = FALSE;
                    break;

                case TYPE_ELECTRIC:
                    oddSpAtk = FALSE;
                    break;

                case TYPE_PSYCHIC:
                    oddHP = FALSE;
                    oddSpeed = FALSE;
                    break;

                case TYPE_ICE:
                    oddSpeed = FALSE;
                    break;

                case TYPE_DRAGON:
                    oddHP = FALSE;
                    break;

                case TYPE_DARK:
                    break;
            }

            #undef oddHP
            #undef oddAtk
            #undef oddDef
            #undef oddSpeed
            #undef oddSpAtk
            #undef oddSpDef

            for(i = 0; i < 6; ++i)
            {
                value = GetMonData(mon, MON_DATA_HP_IV + i);

                if((value % 2) == 0) // Current IV is even
                {
                    if(ivStatsOdd[i]) // Wants to be odd
                    {
                        if(value == 0)
                            value = 1;
                        else
                            --value;
                    }
                }
                else // Current IV is odd
                {
                    if(!ivStatsOdd[i]) // Wants to be even
                        --value;
                }

                SetMonData(mon, MON_DATA_HP_IV + i, &value);
            }
        }
    }
}

static u8 GetCurrentWildEncounterCount()
{
    u16 count = 0;

    if(Rogue_IsRunActive())
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_HONEY_TREE)
        {
            count = 1;
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
        {
            u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();
            count = 5;

            if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_TOUGH) // Hard route
            {
                // Less encounters
                count = 2;
            }
            else if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_AVERAGE) // Avg route
            {
                // Slightly less encounters
                count = 3;
            }
        }
        else
        {
            return 0;
        }

        // Apply charms
        {
            u16 decValue = GetCurseValue(EFFECT_WILD_ENCOUNTER_COUNT);

            count += GetCharmValue(EFFECT_WILD_ENCOUNTER_COUNT);

            if(decValue > count)
                count = 0;
            else
                count -= decValue;
        }

        // Clamp
        count = max(count, 1);
        count = min(count, WILD_ENCOUNTER_GRASS_CAPACITY);

        // Move count down if we have haven't actually managed to spawn in enough unique encounters
        while(count != 0 && GetWildGrassEncounter(count - 1) == SPECIES_NONE)
        {
            count--;
        }
    }
    else if(GetSafariZoneFlag())
    {
        count = 6;
    }

    return count;
}

static u8 GetCurrentWaterEncounterCount(void)
{
    u16 count = 0;

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
    {
        u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();
        count = 2;

        if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_TOUGH) // Hard route
        {
            // Less encounters
            count = 1;
        }
        else if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_AVERAGE) // Avg route
        {
            // Slightly less encounters
            count = 1;
        }

        // Apply charms
        {
            u16 decValue = GetCurseValue(EFFECT_WILD_ENCOUNTER_COUNT);

            count += GetCharmValue(EFFECT_WILD_ENCOUNTER_COUNT);

            if(decValue > count)
                count = 0;
            else
                count -= decValue;
        }

        // Clamp
        count = max(count, 1);
        count = min(count, WILD_ENCOUNTER_WATER_CAPACITY);

        // Move count down if we have haven't actually managed to spawn in enough unique encounters
        while(count != 0 && GetWildWaterEncounter(count - 1) == SPECIES_NONE)
        {
            count--;
        }
    }

    return count;
}

static u16 GetWildGrassEncounter(u8 index)
{
    AGB_ASSERT(index < WILD_ENCOUNTER_GRASS_CAPACITY);

    if(index < WILD_ENCOUNTER_GRASS_CAPACITY)
    {
        return gRogueRun.wildEncounters.species[index];
    }

    return SPECIES_NONE;
}

u8 Rogue_GetCurrentWildEncounterCount(void)
{
    return GetCurrentWildEncounterCount();
}

u16 Rogue_GetCurrentWildEncounterSpecies(u8 index)
{
    if(index >= GetCurrentWildEncounterCount())
        return SPECIES_NONE;

    return GetWildGrassEncounter(index);
}

static u16 GetWildWaterEncounter(u8 index)
{
    AGB_ASSERT(index < WILD_ENCOUNTER_WATER_CAPACITY);

    if(index < WILD_ENCOUNTER_WATER_CAPACITY)
    {
        return gRogueRun.wildEncounters.species[WILD_ENCOUNTER_GRASS_CAPACITY + index];
    }

    return SPECIES_NONE;
}

static u16 GetWildEncounterIndexFor(u16 species)
{
    u8 i;
    u16 checkSpecies;

#ifdef ROGUE_EXPANSION
    species = GET_BASE_SPECIES_ID(species);
#endif

    for(i = 0; i < WILD_ENCOUNTER_TOTAL_CAPACITY; ++i)
    {
        checkSpecies = gRogueRun.wildEncounters.species[i];

#ifdef ROGUE_EXPANSION
        checkSpecies = GET_BASE_SPECIES_ID(checkSpecies);
#endif

        if(species == checkSpecies)
        {
            return i;
        }
    }

    return WILD_ENCOUNTER_TOTAL_CAPACITY;
}

void Rogue_ModifyWildMonHeldItem(u16* itemId)
{
    if(Rogue_IsRunActive())
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_WILD_DEN || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_UNIQUE_DEN || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_CATCHING_CONTEST)
        {
            *itemId = 0;
            return;
        }

        if(Rogue_IsItemRoomReward(*itemId) || !Rogue_IsItemEnabled(*itemId))
        {
            *itemId = 0;
        }
    }
    else if(GetSafariZoneFlag() || Rogue_InWildSafari())
    {
        *itemId = 0;
    }

}

static bool8 IsChainSpeciesValidForSpawning(u8 area, u16 species)
{
    u8 i;

    if(species == SPECIES_NONE)
        return FALSE;

    if(area == 1) //WILD_AREA_WATER)
    {
        u16 count = GetCurrentWaterEncounterCount();

        for(i = 0; i < count; ++i)
        {
            if(GetWildWaterEncounter(i) == species)
                return TRUE;
        }
    }
    else
    {
        u16 count = GetCurrentWildEncounterCount();

        for(i = 0; i < count; ++i)
        {
            if(GetWildGrassEncounter(i) == species)
                return TRUE;
        }
    }

    return FALSE;
}

static u16 GetChainSpawnOdds(u8 encounterCount)
{
    return 10 - min(encounterCount, 9);
}

static bool8 ForceChainSpeciesSpawn(u8 area)
{
    if(GetWildChainCount() > 1 && IsChainSpeciesValidForSpawning(area, GetWildChainSpecies()))
    {
        // We're allow to spawn the chain species in for this area
        return ((Random() % GetChainSpawnOdds(GetWildChainCount())) == 0);
    }

    return FALSE;
}

static bool8 ForceRoamerMonSpawn()
{
    if(gRogueRun.wildEncounters.roamer.species != SPECIES_NONE && gRogueRun.wildEncounters.roamerActiveThisPath)
    {
        // % chance to force roamer to spawn, but it's only once per path at most
        if(Random() % 100 < 7)
            return TRUE;
    }

    return FALSE;
}

void Rogue_CreateWildMon(u8 area, u16* species, u8* level, bool8* forceShiny)
{
    if(Rogue_InWildSafari())
    {
        *species = SPECIES_ABRA;
        *level = STARTER_MON_LEVEL;
        *forceShiny = FALSE;
        return;
    }

    // Note: Don't seed individual encounters
    else if(Rogue_IsRunActive() || GetSafariZoneFlag())
    {
        u16 shinyOdds = Rogue_GetShinyOdds(SHINY_ROLL_DYNAMIC);

        if(GetSafariZoneFlag())
            *level  = CalculateWildLevel(3);
        else
            *level  = CalculateWildLevel(6);

        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_CATCHING_CONTEST)
        {
            // Speculative check
            if(gRogueLocal.catchingContest.isActive && gRogueLocal.catchingContest.spawnsRemaining != 0)
            {
                if(!RogueMiscQuery_AnyActiveElements())
                    gRogueLocal.catchingContest.spawnsRemaining = 0;
            }

            if(gRogueLocal.catchingContest.isActive && gRogueLocal.catchingContest.spawnsRemaining != 0)
            {
                u8 stats[NUM_STATS];

                *species = SelectWildSpeciesFromFormFamilies(RogueRandom(), RogueRandom(), WildFormFlatWeight, NULL, FALSE);
                *level = Rogue_CalculatePlayerMonLvl();
                --gRogueLocal.catchingContest.spawnsRemaining;

                // Update score
                RoguePokedex_GetSpeciesStatArray(*species, stats, ARRAY_COUNT(stats));

                if(stats[gRogueLocal.catchingContest.activeStat] > gRogueLocal.catchingContest.winningScore)
                {
                    gRogueLocal.catchingContest.winningSpecies = *species;
                    gRogueLocal.catchingContest.winningScore = stats[gRogueLocal.catchingContest.activeStat];
                }
            }
            else
            {
                *species = SPECIES_NONE;
            }
        }
        else if(ForceChainSpeciesSpawn(area))
        {
            *species = GetWildChainSpecies();
            shinyOdds = GetEncounterChainShinyOdds(GetWildChainCount());
        }
        else if(area == 1) //WILD_AREA_WATER)
        {
            u16 randIdx = Random() % GetCurrentWaterEncounterCount();

            *species = GetWildWaterEncounter(randIdx);
        }
        else
        {
            u16 count = GetCurrentWildEncounterCount();
            u16 historyBufferCount = ARRAY_COUNT(gRogueLocal.wildEncounterHistoryBuffer);
            u16 randIdx;

            if(ForceRoamerMonSpawn())
            {
                *species = gRogueRun.wildEncounters.roamer.species;
                *forceShiny = gRogueRun.wildEncounters.roamer.shinyFlag;
                gRogueRun.wildEncounters.roamerActiveThisPath = FALSE;
                return;
            }

            do
            {
                // Prevent recent duplicates when on a run (Don't use this in safari mode though)
                randIdx = Random() % count;
                *species = GetWildGrassEncounter(randIdx);

                if(Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_LATERMANNER)
                    break;
            }
            while(!GetSafariZoneFlag() && (count > historyBufferCount) && HistoryBufferContains(&gRogueLocal.wildEncounterHistoryBuffer[0], historyBufferCount, *species));

            HistoryBufferPush(&gRogueLocal.wildEncounterHistoryBuffer[0], historyBufferCount, *species);
        }

        *forceShiny = (shinyOdds == 0) ? FALSE : (Random() % shinyOdds) == 0;
    }
}

u16 Rogue_SelectRandomWildMon(void)
{
    if(Rogue_IsRunActive() || GetSafariZoneFlag())
    {
        u16 count = GetCurrentWildEncounterCount();
        if(count != 0)
            return GetWildGrassEncounter(Random() % count);
    }

    return SPECIES_NONE;
}

bool8 Rogue_PreferTraditionalWildMons(void)
{
    if(Rogue_IsRunActive())
    {
        return !Rogue_GetConfigToggle(CONFIG_TOGGLE_OVERWORLD_MONS);
    }

    return FALSE;
}

bool8 Rogue_AreWildMonEnabled(void)
{
    if(Rogue_PreferTraditionalWildMons())
    {
        return FALSE;
    }

    if(Rogue_IsRunActive())
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_CATCHING_CONTEST)
        {
            return gRogueLocal.catchingContest.isActive ? TRUE : FALSE;
        }

        return GetCurrentWildEncounterCount() > 0;
    }

    if(GetSafariZoneFlag())
    {
        return GetCurrentWildEncounterCount() > 0;
    }

    if(Rogue_InWildSafari() && gMapHeader.mapLayoutId != LAYOUT_ROGUE_INTERIOR_SAFARI_CAVE)
    {
        return TRUE;
    }

    return FALSE;
}

void Rogue_CreateEventMon(u16* species, u8* level, u16* itemId)
{
    if(Rogue_InWildSafari())
    {
        // Thrown away later, so doesn't matter too much
        *level = STARTER_MON_LEVEL;
    }
    else
    {
        *level = CalculateWildLevel(3);
    }
}

static void FillWithRoamerState(struct Pokemon* mon, u8 level)
{
    u32 temp;

    ZeroMonData(mon);
    CreateMon(mon, gRogueRun.wildEncounters.roamer.species, level, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

    temp = gRogueRun.wildEncounters.roamer.hpIV;
    SetMonData(mon, MON_DATA_HP_IV, &temp);

    temp = gRogueRun.wildEncounters.roamer.attackIV;
    SetMonData(mon, MON_DATA_ATK_IV, &temp);

    temp = gRogueRun.wildEncounters.roamer.defenseIV;
    SetMonData(mon, MON_DATA_DEF_IV, &temp);

    temp = gRogueRun.wildEncounters.roamer.shinyFlag;
    SetMonData(mon, MON_DATA_IS_SHINY, &temp);

    temp = gRogueRun.wildEncounters.roamer.speedIV;
    SetMonData(mon, MON_DATA_SPEED_IV, &temp);

    temp = gRogueRun.wildEncounters.roamer.spAttackIV;
    SetMonData(mon, MON_DATA_SPATK_IV, &temp);

    temp = gRogueRun.wildEncounters.roamer.spDefenseIV;
    SetMonData(mon, MON_DATA_SPDEF_IV, &temp);

    temp = gRogueRun.wildEncounters.roamer.abilityNum;
    SetMonData(mon, MON_DATA_ABILITY_NUM, &temp);

    temp = gRogueRun.wildEncounters.roamer.genderFlag;
    SetMonData(mon, MON_DATA_GENDER_FLAG, &temp);

    temp = gRogueRun.wildEncounters.roamer.status;
    SetMonData(mon, MON_DATA_STATUS, &temp);

    temp = (((u32)gRogueRun.wildEncounters.roamer.hpPerc) * GetMonData(mon, MON_DATA_MAX_HP)) / 100;
    if(temp <= 0)
        temp = 1;
    SetMonData(mon, MON_DATA_HP, &temp);
}

static void ApplyCustomMonToWildBattle(u32 customMonId, struct Pokemon* mon)
{
    if(customMonId != 0)
    {
        // Make sure shiny state isn't changed
        u32 shinyState = GetMonData(mon, MON_DATA_IS_SHINY);

        ModifyExistingMonToCustomMon(customMonId, mon);

        SetMonData(mon, MON_DATA_IS_SHINY, &shinyState);

        gRogueLocal.wildBattleCustomMonId = customMonId;
    }
}

static void TryApplyCustomMon(u16 species, struct Pokemon* mon)
{
    if(Rogue_IsRunActive())
    {
        switch (gRogueAdvPath.currentRoomType)
        {
        case ADVPATH_ROOM_ROUTE:
        case ADVPATH_ROOM_WILD_DEN:
        case ADVPATH_ROOM_LEGENDARY:
            // Allow these
            break;

        default:
            // Don't allow any
            return;
        }

        // If we're here, we're allowed to apply unique species
        {
            u32 customMonId = RogueGift_TryFindEnabledDynamicCustomMonForSpecies(species);

            if(customMonId != 0)
            {
                ApplyCustomMonToWildBattle(customMonId, mon);
            }
        }
    }
}

static void ApplyDenEncounterQuality(struct Pokemon* mon, u16 species, bool8 isUniqueDen)
{
    u16 presetIndex;
    u16 presetCount = gRoguePokemonProfiles[species].competitiveSetCount;
    u16 statA = (Random() % 6);
    //u16 statB = (statA + 1 + (Random() % 5)) % 6;
    u16 temp = 31;

    if(presetCount != 0)
    {
        struct RoguePokemonCompetitiveSetRules rules;
        memset(&rules, 0, sizeof(rules));

        presetIndex = Random() % presetCount;
        Rogue_ApplyMonCompetitiveSet(mon, GetMonData(mon, MON_DATA_LEVEL), &gRoguePokemonProfiles[species].competitiveSets[presetIndex], &rules);
    }

    // Clear friendship
    temp = 0;
    SetMonData(mon, MON_DATA_FRIENDSHIP, &temp);

    // Bump 2 of the IVs to max
    temp = 31;
    SetMonData(mon, MON_DATA_HP_IV + statA, &temp);
    //SetMonData(mon, MON_DATA_HP_IV + statB, &temp);

    // Clear held item
    temp = 0;
    SetMonData(mon, MON_DATA_HELD_ITEM, &temp);

    if(isUniqueDen)
        ApplyCustomMonToWildBattle(gRogueAdvPath.currentRoomParams.perType.uniqueDen.customMonId, mon);
    else
        TryApplyCustomMon(species, mon);
}

void Rogue_ModifyWildMon(struct Pokemon* mon)
{
    gRogueLocal.wildBattleCustomMonId = 0;

    if(Rogue_IsRunActive() && gRogueAdvPath.currentRoomType == ADVPATH_ROOM_UNIQUE_DEN)
    {
        ApplyDenEncounterQuality(mon, GetMonData(mon, MON_DATA_SPECIES), TRUE);
    }
    else if(Rogue_InWildSafari())
    {
        if(VarGet(VAR_ROGUE_INTRO_STATE) == ROGUE_INTRO_STATE_CATCH_MON)
        {
            // Do nothing in intro i.e. generate IVs moves etc normally

            // If player has tried to be smart and thrown away pokeball, silently give them another ;)
            if(!CheckBagHasItem(ITEM_POKE_BALL, 1))
            {
                AddBagItem(ITEM_POKE_BALL, 1);
                Rogue_ClearPopupQueue();
            }
            return;
        }
        else
        {
            struct RogueSafariMon* safariMon = RogueSafari_GetPendingBattleMon();

            AGB_ASSERT(safariMon != NULL);
            if(safariMon != NULL)
            {
                u8 text[POKEMON_NAME_LENGTH + 1];
                u16 eggSpecies = Rogue_GetEggSpecies(safariMon->species);

                RogueSafari_CopyFromSafariMon(safariMon, &mon->box);

                // Make baby form
                if(eggSpecies != safariMon->species)
                {
                    SetMonData(mon, MON_DATA_SPECIES, &eggSpecies);
                    GetMonData(mon, MON_DATA_NICKNAME, text);

                    if(StringCompareN(text, RoguePokedex_GetSpeciesName(safariMon->species), POKEMON_NAME_LENGTH) == 0)
                    {
                        // Doesn't have a nickname so update to match species name
                        StringCopy_Nickname(text, RoguePokedex_GetSpeciesName(eggSpecies));
                        SetMonData(mon, MON_DATA_NICKNAME, text);
                    }
                }

                if(safariMon->customMonLookup != 0)
                {
                    u8 idx = safariMon->customMonLookup - 1;
                    u32 customMonId = gRogueSaveBlock->safariMonCustomIds[idx];

                    if(customMonId & OTID_FLAG_DYNAMIC_CUSTOM_MON)
                    {
                        AGB_ASSERT(customMonId & OTID_FLAG_CUSTOM_MON);
                        AGB_ASSERT(customMonId & OTID_FLAG_DYNAMIC_CUSTOM_MON);
                        ModifyExistingMonToCustomMon(customMonId, mon);
                    }
                    else
                    {
                        // This must be exotic
                        ModifyExistingMonToCustomMon(customMonId, mon);
                    }
                }

                CalculateMonStats(mon);
            }
        }
    }
    else if(Rogue_IsRunActive())
    {
        u16 species = GetMonData(mon, MON_DATA_SPECIES);

        if(IsCurseActive(EFFECT_SNAG_TRAINER_MON) && FlagGet(FLAG_ROGUE_IN_SNAG_BATTLE))
        {
            // Save values to restore
            u8 slot = 0;
            u8 genderFlag = GetMonData(&gEnemyParty[gSpecialVar_0x800A], MON_DATA_GENDER_FLAG);
            u8 shinyFlag = GetMonData(&gEnemyParty[gSpecialVar_0x800A], MON_DATA_IS_SHINY);
            u8 abilityNum = GetMonData(&gEnemyParty[gSpecialVar_0x800A], MON_DATA_ABILITY_NUM);
            u8 nature = GetNature(&gEnemyParty[gSpecialVar_0x800A]);
            u16 move1 = GetMonData(&gEnemyParty[gSpecialVar_0x800A], MON_DATA_MOVE1);
            u16 move2 = GetMonData(&gEnemyParty[gSpecialVar_0x800A], MON_DATA_MOVE2);
            u16 move3 = GetMonData(&gEnemyParty[gSpecialVar_0x800A], MON_DATA_MOVE3);
            u16 move4 = GetMonData(&gEnemyParty[gSpecialVar_0x800A], MON_DATA_MOVE4);

            // Recreate mon to use a custom OtID
            CreateMon(
                mon,
                GetMonData(mon, MON_DATA_SPECIES),
                GetMonData(mon, MON_DATA_LEVEL),
                USE_RANDOM_IVS,
                0,
                0,
                OT_ID_RANDOM_NO_SHINY,
                0
            );

            // Copy over some of the data from the mon the trainer actually used
            SetMonData(mon, MON_DATA_GENDER_FLAG, &genderFlag);
            SetMonData(mon, MON_DATA_IS_SHINY, &shinyFlag);
            SetMonData(mon, MON_DATA_ABILITY_NUM, &abilityNum);
            SetNature(mon, nature);

            slot = 0;
            if(move1 != MOVE_NONE)
                SetMonMoveSlot(mon, move1, slot++);
            if(move2 != MOVE_NONE)
                SetMonMoveSlot(mon, move2, slot++);
            if(move3 != MOVE_NONE)
                SetMonMoveSlot(mon, move3, slot++);
            if(move4 != MOVE_NONE)
                SetMonMoveSlot(mon, move4, slot++);
        }
        else if(species == gRogueRun.wildEncounters.roamer.species)
        {
            FillWithRoamerState(mon, GetMonData(mon, MON_DATA_LEVEL));

            // TODO - Consider interaction for roamer
            //TryApplyCustomMon(species, mon);
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_GAMESHOW)
        {
            RogueGift_CreateMon(CUSTOM_MON_WAHEY_ELECTRODE, mon, SPECIES_ELECTRODE, GetMonData(mon, MON_DATA_LEVEL), 31);
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_WILD_DEN || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_UNIQUE_DEN)
        {
            ApplyDenEncounterQuality(mon, species, gRogueAdvPath.currentRoomType == ADVPATH_ROOM_UNIQUE_DEN);
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY)
        {
            u8 i;
            u16 moveId;

            if(gRogueAdvPath.currentRoomParams.perType.legendary.customMonId != 0)
                ApplyCustomMonToWildBattle(gRogueAdvPath.currentRoomParams.perType.legendary.customMonId, mon);

            // Replace roar with hidden power to avoid pokemon roaring itself out of battle
            for (i = 0; i < MAX_MON_MOVES; i++)
            {
                moveId = GetMonData(mon, MON_DATA_MOVE1 + i);
                if( moveId == MOVE_ROAR ||
                    moveId == MOVE_WHIRLWIND ||
                    moveId == MOVE_EXPLOSION ||
                    moveId == MOVE_SELF_DESTRUCT ||
                    moveId == MOVE_TELEPORT)
                {
                    moveId = MOVE_HIDDEN_POWER;
                    SetMonData(mon, MON_DATA_MOVE1 + i, &moveId);
                    SetMonData(mon, MON_DATA_PP1 + i, &gBattleMoves[moveId].pp);
                }
            }
        }
        else
        {
            TryApplyCustomMon(species, mon);
        }
    }
}

void Rogue_ModifyScriptMon(struct Pokemon* mon)
{
    if(Rogue_IsRunActive())
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_MINIBOSS)
        {
            u32 temp;
            u16 species = GetMonData(mon, MON_DATA_SPECIES);
            u8 perfectIvCandidates[NUM_STATS];
            u8 perfectIvCandidateCount = 0;
            u8 statA;
            u8 statB;

            // Apply the miniboss preset for this mon
            {
                u8 i;
                u8 target;
                u8 partySize = CalculateEnemyPartyCount();

                // Find the matching species
                for(i = 0; i < partySize; ++i)
                {
                    if(species == GetMonData(&gEnemyParty[i], MON_DATA_SPECIES))
                        break;
                }

                target = i;

                if(target != partySize)
                {
                    struct RoguePokemonCompetitiveSetRules rules;
                    struct RoguePokemonCompetitiveSet customPreset;

                    memset(&rules, 0, sizeof(rules));
                    memset(&customPreset, 0, sizeof(customPreset));
                    rules.allowMissingMoves = TRUE;
#ifdef ROGUE_EXPANSION
                    rules.skipTeraType = TRUE;
#endif
                    customPreset.heldItem = ITEM_NONE;
                    customPreset.ability = GetMonAbility(&gEnemyParty[target]);
                    customPreset.nature = GetNature(&gEnemyParty[target]);
                    customPreset.hiddenPowerType = CalcMonHiddenPowerType(&gEnemyParty[target]);

                    for(i = 0; i < MAX_MON_MOVES; ++i)
                        customPreset.moves[i] = GetMonData(&gEnemyParty[target], MON_DATA_MOVE1 + i);

                    Rogue_ApplyMonCompetitiveSet(mon, Rogue_CalculatePlayerMonLvl(), &customPreset, &rules);
                }
            }

            // Normalize IVs around the copied Hidden Power parity, then select
            // exactly two contributing stats to perfect without changing it.
            for(temp = 0; temp < NUM_STATS; ++temp)
            {
                u32 iv;

                if((GetMonData(mon, MON_DATA_HP_IV + temp) & 1) != 0)
                {
                    perfectIvCandidates[perfectIvCandidateCount++] = temp;
                    iv = 29;
                }
                else
                {
                    iv = 30;
                }

                SetMonData(mon, MON_DATA_HP_IV + temp, &iv);
            }

            AGB_ASSERT(perfectIvCandidateCount >= 2);
            statB = Random() % perfectIvCandidateCount;
            statA = perfectIvCandidates[statB];
            perfectIvCandidates[statB] = perfectIvCandidates[--perfectIvCandidateCount];
            statB = perfectIvCandidates[Random() % perfectIvCandidateCount];

            temp = 31;
            SetMonData(mon, MON_DATA_HP_IV + statA, &temp);
            SetMonData(mon, MON_DATA_HP_IV + statB, &temp);

            // The trophy is a clean, player-owned echo rather than the
            // battle-worn opponent object.
            temp = 0;
            SetMonData(mon, MON_DATA_HELD_ITEM, &temp);
            SetMonData(mon, MON_DATA_IS_SHINY, &temp);
            SetMonData(mon, MON_DATA_STATUS, &temp);

            temp = gSpeciesInfo[species].friendship;
            SetMonData(mon, MON_DATA_FRIENDSHIP, &temp);

            for(temp = 0; temp < NUM_STATS; ++temp)
            {
                u32 ev = 0;
                SetMonData(mon, MON_DATA_HP_EV + temp, &ev);
            }

            // Set to the correct level
            temp = Rogue_ModifyExperienceTables(gRogueSpeciesInfo[species].growthRate, Rogue_CalculatePlayerMonLvl());
            SetMonData(mon, MON_DATA_EXP, &temp);
            CalculateMonStats(mon);

            temp = GetMonData(mon, MON_DATA_LEVEL);
            SetMonData(mon, MON_DATA_MET_LEVEL, &temp);
        }
    }
}

void Rogue_ModifyGiveMon(struct Pokemon* mon)
{
    if(!Rogue_IsRunActive())
    {
        if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_RIDE_TRAINING)
        {
            if(GetMonData(mon, MON_DATA_SPECIES) == SPECIES_STANTLER)
            {
                // The stantler you are given by the clown
                u32 temp;

                ZeroMonData(mon);
                RogueGift_CreateMon(CUSTOM_MON_CLOWN_STANTLER, mon, SPECIES_STANTLER, STARTER_MON_LEVEL, USE_RANDOM_IVS);

                temp = 0;
                SetMonData(mon, MON_DATA_GENDER_FLAG, &temp);
            }
        }
    }

    if(Rogue_IsRunActive() && !RogueTrial_TransformMonIfIllegal(mon))
        RogueTrial_Invalidate();

    // Clear popup data on catch
    mon->rogueExtraData.lastPopupLevel = GetMonData(mon, MON_DATA_LEVEL) - 1;
    mon->rogueExtraData.hasPendingEvo = FALSE;
}

struct BoxPokemon* Rogue_GetDaycareBoxMon(u8 slot)
{
    AGB_ASSERT(slot < DAYCARE_SLOT_COUNT);
    return (struct BoxPokemon*)&gRogueSaveBlock->daycarePokemon[slot].boxMonFacade;
}

u8 Rogue_GetCurrentDaycareSlotCount()
{
    if(RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_CAPACITY1))
        return 3;
    else if(RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_CAPACITY0))
        return 2;
    else
        return 1;
}

void Rogue_SwapMonInDaycare(struct Pokemon* partyMon, u8 daycareSlot)
{
    u16 species;
    u8 wasSafariIllegal = (GetMonData(partyMon, MON_DATA_SPECIES) == SPECIES_NONE) ? FALSE : partyMon->rogueExtraData.isSafariIllegal;
    struct BoxPokemon* daycareMon = (struct BoxPokemon*)&gRogueSaveBlock->daycarePokemon[daycareSlot].boxMonFacade;
    struct BoxPokemon temp = *daycareMon;

    AGB_ASSERT(daycareSlot < DAYCARE_SLOT_COUNT);

    *daycareMon = partyMon->box;
    BoxMonRestorePP(daycareMon);

    ZeroMonData(partyMon);
    BoxMonToMon(&temp, partyMon);

    if(Rogue_IsRunActive())
    {
        partyMon->rogueExtraData.isSafariIllegal = gRogueSaveBlock->daycarePokemon[daycareSlot].isSafariIllegal;
        gRogueSaveBlock->daycarePokemon[daycareSlot].isSafariIllegal = wasSafariIllegal;
    }

    species = GetMonData(partyMon, MON_DATA_SPECIES);

    if(Rogue_IsRunActive() && species != SPECIES_NONE)
    {
        // Scale level so it just lags behind the player
        u8 level = GetMonData(partyMon, MON_DATA_LEVEL);
        u8 targetLevel = Rogue_CalculatePlayerMonLvl();

        if(targetLevel < 5)
            targetLevel = 1;
        else
            targetLevel -= 5;

        if(level < targetLevel)
        {
            u32 exp = Rogue_ModifyExperienceTables(gRogueSpeciesInfo[species].growthRate, targetLevel);
            SetMonData(partyMon, MON_DATA_EXP, &exp);
        }

        // Always recalc for safety
        CalculateMonStats(partyMon);

        if(!RogueTrial_TransformMonIfIllegal(partyMon))
            RogueTrial_Invalidate();
    }

    CompactPartySlots();
    CalculatePlayerPartyCount();
}

static const u8 sText_EmptyDaycareSlot[] = _("Empty / -");
static const u8 sText_DaycareSlotSpacing[] = _(" / ");

void Rogue_DaycareMultichoiceCallback(struct MenuAction* outList, u8* outCount, u8 listCapacity)
{
    u8 i;
    u16 species;
    struct BoxPokemon* mon;
    u8* str;
    u8* stringVars[] =
    {
        gStringVar1,
        gStringVar2,
        gStringVar3
    };

    for(i = 0; i < Rogue_GetCurrentDaycareSlotCount(); ++i)
    {
        mon = Rogue_GetDaycareBoxMon(i);
        species = GetBoxMonData(mon, MON_DATA_SPECIES);

        if(species == SPECIES_NONE)
        {
            outList[i].text = sText_EmptyDaycareSlot;
        }
        else
        {
            GetBoxMonData(mon, MON_DATA_NICKNAME, stringVars[i]);

            str = StringAppend(stringVars[i], sText_DaycareSlotSpacing);
            str = StringAppend(str, RoguePokedex_GetSpeciesName(species));

            outList[i].text = stringVars[i];
        }
    }

    *outCount = i;
}

static u32 GetMaxDayCareCharges(void)
{
    if(RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_PHONE2))
        return 3;

    if(RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_PHONE1))
        return 2;

    return 1;
}

void Rogue_RefillDayCareCharges(bool8 createPopup)
{
    if(VarGet(VAR_ROGUE_DAYCARE_PHONE_REMAINING_CHARGES) != GetMaxDayCareCharges())
    {
        VarSet(VAR_ROGUE_DAYCARE_PHONE_REMAINING_CHARGES, GetMaxDayCareCharges());

        if(createPopup)
            Rogue_PushPopup_DaycareChargeRefilled(GetMaxDayCareCharges());
    }
}

void Rogue_OnDayCareChargeUsed(void)
{
    if(Rogue_IsRunActive())
        Rogue_PushPopup_DaycareChargeUsed(VarGet(VAR_ROGUE_DAYCARE_PHONE_REMAINING_CHARGES), GetMaxDayCareCharges());
}

void Rogue_BeginCatchingContest(u8 type, u8 stat)
{
    gRogueLocal.catchingContest.activeType = type;
    gRogueLocal.catchingContest.activeStat = stat;
    gRogueLocal.catchingContest.winningSpecies = SPECIES_NONE;
    gRogueLocal.catchingContest.winningScore = 0;
    gRogueLocal.catchingContest.spawnsRemaining = 10 + Random() % 20;
    gRogueLocal.catchingContest.isActive = TRUE;

    // Take player team
    TempSavePlayerTeam();
    ClearPlayerTeam();

    // Temporarily take pokeballs away
    gRogueLocal.catchingContest.itemCountToRestore = CountTotalItemQuantityInBag(ITEM_CATCHING_CONTEST_POKEBALL);

    RogueMonQuery_Begin();

    RogueMonQuery_IsSpeciesActive();

    // Prefilter to mons of types we're interested in
    RogueMonQuery_EvosContainType(QUERY_FUNC_INCLUDE, MON_TYPE_VAL_TO_FLAGS(type));
    RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);

    RogueMonQuery_TransformIntoEggSpecies();
    RogueMonQuery_TransformIntoEvos(Rogue_CalculatePlayerMonLvl(), TRUE, FALSE);

    // Now we've evolved we're only caring about mons of this type
    RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, MON_TYPE_VAL_TO_FLAGS(type));

    // Now transform back into egg species, so the spawning should still be deteministic
    // (although the type hints could be invalid)
    if(IsCurseActive(EFFECT_WILD_EGG_SPECIES))
        RogueMonQuery_TransformIntoEggSpecies();

    SetupFollowParterMonObjectEvent();
}

void Rogue_EndCatchingContest()
{
    while(gRogueLocal.catchingContest.spawnsRemaining != 0)
    {
        // Force spawn remaining mons
        u8 area = 0;
        u16 species = 0;
        u8 level = 0;
        bool8 forceShiny = FALSE;
        Rogue_CreateWildMon(area, &species, &level, &forceShiny);
    }

    // Re-add pokeballs we had when entering
    while(TRUE)
    {
        if(!RemoveBagItem(ITEM_CATCHING_CONTEST_POKEBALL, 1))
            break;
    }
    if(gRogueLocal.catchingContest.itemCountToRestore)
        AddBagItem(ITEM_CATCHING_CONTEST_POKEBALL, gRogueLocal.catchingContest.itemCountToRestore);

    gRogueLocal.catchingContest.isActive = FALSE;

    // Store caught mon for later
    CopyMon(&gEnemyParty[0], &gPlayerParty[0], sizeof(struct Pokemon));
    {
        u32 item = ITEM_NONE;
        SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &item);
    }

    // Hack to hide follower
    ZeroMonData(&gPlayerParty[0]);
    SetupFollowParterMonObjectEvent();

    // Give back player team
    TempRestorePlayerTeam();

    RogueMonQuery_End();

    SetupFollowParterMonObjectEvent();
}

static void HandleForfeitingInCatchingContest()
{
    if(gRogueLocal.catchingContest.isActive)
    {
        // Handle forfeiting mid catching contest
        TempRestorePlayerTeam();
        RogueMonQuery_End();
        gRogueLocal.catchingContest.isActive = FALSE;
    }
}

bool8 Rogue_IsCatchingContestActive()
{
    return Rogue_IsRunActive() && gRogueLocal.catchingContest.isActive;
}

void Rogue_GetCatchingContestResults(u16* caughtSpecies, bool8* didWin, u16* winningSpecies)
{
    u8 stats[NUM_STATS];

    *caughtSpecies = GetMonData(&gEnemyParty[0], MON_DATA_SPECIES);
    *winningSpecies = gRogueLocal.catchingContest.winningSpecies;
    *didWin = FALSE;

    if(*caughtSpecies != SPECIES_NONE)
    {
        RoguePokedex_GetSpeciesStatArray(*caughtSpecies, stats, ARRAY_COUNT(stats));

        if(stats[gRogueLocal.catchingContest.activeStat] >= gRogueLocal.catchingContest.winningScore)
        {
            *didWin = TRUE;
        }
    }
}

static void ApplyMartSeed(u16 itemCategory)
{
    if(!Rogue_IsRunActive())
        return;

    if(ROGUE_SHOP_IS_TRAVELING_MERCHANT(itemCategory))
    {
        SeedRogueRng(
            gRogueAdvPath.rooms[gRogueRun.adventureRoomId].rngSeed
            ^ (ROGUE_SHOP_GET_CATEGORY(itemCategory) * 0x45D9));
        return;
    }

    switch (ROGUE_SHOP_GET_CATEGORY(itemCategory))
    {
    case ROGUE_SHOP_GENERAL:
        SeedRogueRng(gRogueRun.subSeeds[ROGUE_SUBSEED_SHOP_GENERAL]);
        break;
    case ROGUE_SHOP_BALLS:
        SeedRogueRng(gRogueRun.subSeeds[ROGUE_SUBSEED_SHOP_BALLS]);
        break;
    case ROGUE_SHOP_TMS:
        SeedRogueRng(gRogueRun.subSeeds[ROGUE_SUBSEED_SHOP_TMS]);
        break;
    case ROGUE_SHOP_BATTLE_ENHANCERS:
        SeedRogueRng(gRogueRun.subSeeds[ROGUE_SUBSEED_SHOP_BATTLE_ENHANCERS]);
        break;
    case ROGUE_SHOP_HELD_ITEMS:
        SeedRogueRng(gRogueRun.subSeeds[ROGUE_SUBSEED_SHOP_HELD_ITEMS]);
        break;
    case ROGUE_SHOP_RARE_HELD_ITEMS:
        SeedRogueRng(gRogueRun.subSeeds[ROGUE_SUBSEED_SHOP_RARE_HELD_ITEMS]);
        break;
    case ROGUE_SHOP_BERRIES:
        SeedRogueRng(gRogueRun.subSeeds[ROGUE_SUBSEED_SHOP_BERRIES]);
        break;
    case ROGUE_SHOP_TREATS:
        SeedRogueRng(gRogueRun.subSeeds[ROGUE_SUBSEED_SHOP_TREATS]);
        break;
    case ROGUE_SHOP_COURIER:
        SeedRogueRng(gRogueAdvPath.rooms[gRogueRun.adventureRoomId].rngSeed);
        break;

    default:
        // Use whatever the active seed is.
        break;
    }
}

struct BattleEnhancerShopChance
{
    u16 difficulty;
    u8 defaultChance;
};

static u8 GetBattleEnhancerShopChance(u16 itemId, void* usrData)
{
    struct BattleEnhancerShopChance const* data = usrData;
    u16 chance;

    if(Rogue_IsEvolutionItem(itemId))
    {
        chance = 40 + 15 * data->difficulty;
        return min(100, chance);
    }

#ifdef ROGUE_EXPANSION
    if(itemId == ITEM_ABILITY_CAPSULE)
    {
        chance = 35 + 12 * data->difficulty;
        return min(100, chance);
    }

    if(itemId == ITEM_ABILITY_PATCH)
    {
        chance = 25 + 10 * data->difficulty;
        return min(100, chance);
    }
#endif

    return data->defaultChance;
}

void Rogue_OpenMartQuery(u16 difficulty, u16 itemCategory, u16* minSalePrice)
{
    bool8 applyRandomChance = FALSE;
    bool8 applyBattleEnhancerChance = FALSE;
    bool8 applyPriceRange = TRUE;
    u16 randomChanceMinimum = 10;
    u16 randomChanceGymRate = 5;
    u16 maxPriceRange = 65000;
    u16 originalItemCategory = itemCategory;
    u16 randomSeed;

    if(Rogue_GetModeRules()->forceFullShopInventory)
        difficulty = ROGUE_FINAL_CHAMP_DIFFICULTY;

    gRogueLocal.rngSeedToRestore = gRngRogueValue;
    ApplyMartSeed(itemCategory);
    randomSeed = RogueRandom();
    itemCategory = ROGUE_SHOP_GET_CATEGORY(itemCategory);

    if(itemCategory == ROGUE_SHOP_COURIER)
    {
        u16 range = IsRareShopActive() ? 5 : 4;

        switch (RogueRandomRange(range, 0))
        {
        case 0:
            itemCategory = ROGUE_SHOP_TMS;
            break;
        case 1:
            itemCategory = ROGUE_SHOP_BALLS;
            break;
        case 2:
            itemCategory = ROGUE_SHOP_BATTLE_ENHANCERS;
            break;
        case 3:
            itemCategory = ROGUE_SHOP_HELD_ITEMS;
            break;
        case 4:
            itemCategory = ROGUE_SHOP_RARE_HELD_ITEMS;
            break;

        default:
            AGB_ASSERT(FALSE);
            itemCategory = ROGUE_SHOP_TMS;
            break;
        }

        difficulty = ROGUE_FINAL_CHAMP_DIFFICULTY;
    }

    RogueItemQuery_IsItemActive();

    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_KEY_ITEMS);

    switch (itemCategory)
    {
    case ROGUE_SHOP_GENERAL:
        RogueItemQuery_IsGeneralShopItem(QUERY_FUNC_INCLUDE);
        {
            u8 pocket;
            for(pocket = POCKET_NONE + 1; pocket <= POCKET_KEY_ITEMS; ++pocket)
            {
                switch (pocket)
                {
                // Allow these pockets
                case POCKET_ITEMS:
                case POCKET_MEDICINE:
                    break;

                default:
                    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, pocket);
                    break;
                }
            }
        }

        if(Rogue_IsRunActive())
        {
            maxPriceRange =  300 + difficulty * 400;
        }
        else
        {
            if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_GENERAL_STOCK))
            {
                maxPriceRange = 1200;
                RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_FULL_HEAL);
            }
        }
        break;

    case ROGUE_SHOP_BALLS:
        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_INCLUDE, POCKET_POKE_BALLS);
        *minSalePrice = 100;

        if(Rogue_IsRunActive())
        {
            maxPriceRange =  300 + difficulty * 400;

            if(difficulty <= 0)
                maxPriceRange = 200;
            else if(difficulty <= 1)
                maxPriceRange = 600;
            else if(difficulty <= 2)
                maxPriceRange = 1000;
            else if(difficulty < 11)
                maxPriceRange = 2000;
        }
        else
        {
            if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_POKE_BALLS_STOCK))
            {
                RogueItemQuery_InPriceRange(QUERY_FUNC_INCLUDE, 10, 600);
                RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_ULTRA_BALL);
            }
        }
        break;

    case ROGUE_SHOP_TMS:
        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_INCLUDE, POCKET_TM_HM);
        *minSalePrice = 0;
        applyRandomChance = TRUE;

        if(!Rogue_IsRunActive())
        {
            if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_TMS_STOCK))
            {
#ifdef ROGUE_EXPANSION
                maxPriceRange = 15000;
#else
                maxPriceRange = 8000;
#endif
            }
        }
        break;

    case ROGUE_SHOP_BATTLE_ENHANCERS:
        RogueItemQuery_IsGeneralShopItem(QUERY_FUNC_EXCLUDE);

#ifdef ROGUE_EXPANSION
        // Mints are in treat shop
        RogueMiscQuery_EditRange(QUERY_FUNC_EXCLUDE, ITEM_LONELY_MINT, ITEM_SERIOUS_MINT);
#endif
        {
            u8 pocket;
            for(pocket = POCKET_NONE + 1; pocket <= POCKET_KEY_ITEMS; ++pocket)
            {
                switch (pocket)
                {
                // Allow these pockets
                case POCKET_ITEMS:
                case POCKET_MEDICINE:
                    break;

                default:
                    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, pocket);
                    break;
                }
            }
        }
        applyRandomChance = TRUE;
        applyBattleEnhancerChance = TRUE;
        randomChanceMinimum = 20;
        randomChanceGymRate = 3;
        break;

    case ROGUE_SHOP_HELD_ITEMS:
        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_INCLUDE, POCKET_HELD_ITEMS);
        *minSalePrice = 0;
        applyRandomChance = TRUE;
        break;

    case ROGUE_SHOP_RARE_HELD_ITEMS:
#ifdef ROGUE_EXPANSION
        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_INCLUDE, POCKET_STONES);
        applyRandomChance = TRUE;
#else
        AGB_ASSERT(FALSE);
#endif
        break;

    case ROGUE_SHOP_QUEST_REWARDS:
        AGB_ASSERT(FALSE);
        break;

    case ROGUE_SHOP_BERRIES:
        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_INCLUDE, POCKET_BERRIES);
        if(Rogue_IsRunActive())
            *minSalePrice = 1000;
        else
            *minSalePrice = 2500;
        applyRandomChance = TRUE;
        break;

    case ROGUE_SHOP_TREATS:
        // Pokeblock and mints
        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_INCLUDE, POCKET_POKEBLOCK);

        if(Rogue_IsRunActive())
        {
            // No need to sell these in run
            RogueMiscQuery_EditRange(QUERY_FUNC_EXCLUDE, ITEM_POKEBLOCK_HP, ITEM_POKEBLOCK_SPDEF);
        }
        else
        {
            // In Vanilla this shop only sells pokeblock
#ifdef ROGUE_EXPANSION
            if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARKET_TREAT_SHOP_STOCK))
                RogueMiscQuery_EditRange(QUERY_FUNC_EXCLUDE, FIRST_ITEM_POKEBLOCK, LAST_ITEM_POKEBLOCK);
#endif
        }

        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_POKEBLOCK_SHINY);

#ifdef ROGUE_EXPANSION
        RogueMiscQuery_EditRange(QUERY_FUNC_INCLUDE, ITEM_LONELY_MINT, ITEM_SERIOUS_MINT);
#endif

        *minSalePrice = 1500;
        maxPriceRange = 10000;
        applyRandomChance = TRUE;
        break;

    case ROGUE_SHOP_CURSES:
        RogueItemQuery_Reset(QUERY_FUNC_EXCLUDE);
        RogueMiscQuery_EditRange(QUERY_FUNC_INCLUDE, FIRST_ITEM_CURSE, LAST_ITEM_CURSE);
        applyPriceRange = FALSE;
        break;

    case ROGUE_SHOP_CHARMS:
        RogueItemQuery_Reset(QUERY_FUNC_EXCLUDE);
        RogueMiscQuery_EditRange(QUERY_FUNC_INCLUDE, FIRST_ITEM_CHARM, LAST_ITEM_CHARM);
        applyPriceRange = FALSE;
        break;

    default:
        AGB_ASSERT(FALSE);
        break;
    }

    if(Rogue_GetModeRules()->forceFullShopInventory)
    {
        applyRandomChance = FALSE;
    }

    if(originalItemCategory == ROGUE_SHOP_COURIER)
    {
        applyRandomChance = FALSE;
    }

    // Run only items
    //if(!Rogue_IsRunActive())
    //{
    //    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_ESCAPE_ROPE);
    //}

    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_TINY_MUSHROOM);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_BIG_MUSHROOM);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_PEARL);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_BIG_PEARL);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_STARDUST);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_STAR_PIECE);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_NUGGET);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_ENERGY_POWDER);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_ENERGY_ROOT);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_HEAL_POWDER);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_BERRY_JUICE);

    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_MOOMOO_MILK);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_SODA_POP);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_FRESH_WATER);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_LEMONADE);

    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_LAVA_COOKIE);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_PREMIER_BALL);

#ifdef ROGUE_EXPANSION
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_PEWTER_CRUNCHIES);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_RAGE_CANDY_BAR);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_LAVA_COOKIE);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_OLD_GATEAU);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_CASTELIACONE);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_LUMIOSE_GALETTE);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_SHALOUR_SABLE);
    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_BIG_MALASADA);
#endif

    // Remove quests unlocks
    if(!Rogue_IsRunActive() && !RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_SHOPS))
    {
        u16 questId, i, rewardCount;
        struct RogueQuestReward const* reward;

        for(questId = 0; questId < QUEST_ID_COUNT; ++questId)
        {
            if(!RogueQuest_HasCollectedRewards(questId))
            {
                // TODO - If this is slow probably want to break this out to only check items or quests which definately have shop items as rewards
                rewardCount = RogueQuest_GetRewardCount(questId);

                for(i = 0; i < rewardCount; ++i)
                {
                    reward = RogueQuest_GetReward(questId, i);
                    if(reward->type == QUEST_REWARD_SHOP_ITEM)
                    {
                        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, reward->perType.shopItem.item);
                    }
                }
            }
        }
    }

    // Remove EV items
    if(!Rogue_IsRunActive() || !Rogue_GetConfigToggle(CONFIG_TOGGLE_EV_GAIN))
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_HP_UP);
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_PROTEIN);
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_IRON);
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_CALCIUM);
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_ZINC);
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_CARBOS);
#ifdef ROGUE_EXPANSION
        RogueMiscQuery_EditRange(QUERY_FUNC_EXCLUDE, ITEM_HP_UP, ITEM_CARBOS);
#endif
    }

#ifdef ROGUE_EXPANSION
    //Always exclude feathers from shop
    RogueMiscQuery_EditRange(QUERY_FUNC_EXCLUDE, ITEM_HEALTH_FEATHER, ITEM_SWIFT_FEATHER);
#endif

    // Only show items with a valid price
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_SHOPS))
    {
        *minSalePrice = 0;
    }
    else
    {
        if(applyPriceRange)
            RogueItemQuery_InPriceRange(QUERY_FUNC_INCLUDE, 10, maxPriceRange);

        if(Rogue_IsRunActive())
        {
            // Always allow to always buy revives if players have this on
            if(itemCategory == ROGUE_SHOP_GENERAL && !Rogue_ShouldReleaseFaintedMons())
                RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_REVIVE);

            if(applyRandomChance)
            {
                u32 chance = 100;

                if(difficulty < ROGUE_ELITE_START_DIFFICULTY)
                {
                    chance = randomChanceMinimum + randomChanceGymRate * difficulty;
                }
                else if(difficulty < ROGUE_CHAMP_START_DIFFICULTY)
                {
                    chance = 60 + 10 * (difficulty - ROGUE_ELITE_START_DIFFICULTY);
                }

                if(chance < 100)
                {
                    if(applyBattleEnhancerChance)
                    {
                        struct BattleEnhancerShopChance shopChance =
                        {
                            .difficulty = difficulty,
                            .defaultChance = chance,
                        };
                        RogueMiscQuery_FilterByChanceCallback(randomSeed, QUERY_FUNC_INCLUDE, GetBattleEnhancerShopChance, &shopChance, 1);
                    }
                    else
                    {
                        RogueMiscQuery_FilterByChance(randomSeed, QUERY_FUNC_INCLUDE, chance, 1);
                    }
                }
            }
        }
    }
}

void Rogue_CloseMartQuery()
{
    gRngRogueValue = gRogueLocal.rngSeedToRestore;
}

void Rogue_ModifyTutorMoves(struct Pokemon* mon, u8 tutorType, u8* count, u8* hiddenCount, u16* moves)
{
    *hiddenCount = 0;

    if(tutorType != 0) // TEACH_STATE_RELEARN
    {
        // Remove moves we already know.
        {
            u16 readIdx;
            u16 writeIdx = 0;

            for(readIdx = 0; readIdx < *count; ++readIdx)
            {
                if(!MonKnowsMove(mon, moves[readIdx]))
                {
                    moves[writeIdx++] = moves[readIdx];
                }
            }

            *count = writeIdx;
        }
    }
}

void Rogue_CorrectMonDetails(struct Pokemon* party, u8 count)
{
    u8 i;

    for(i = 0; i < count; ++i)
    {
        Rogue_CorrectBoxMonDetails(&party[i].box);
    }
}

void Rogue_CorrectBoxMonDetails(struct BoxPokemon* mon)
{
    // Ensure OT details are updated for all slots, as they can change
    u32 otId = GetBoxMonData(mon, MON_DATA_OT_ID, NULL);

    if(!IsOtherTrainer(otId))
    {
        u16 gender = RoguePlayer_GetTextVariantId();

        SetBoxMonData(mon, MON_DATA_OT_NAME, gSaveBlock2Ptr->playerName);
        SetBoxMonData(mon, MON_DATA_OT_GENDER, &gender);
    }
}

static bool8 IsRareWeightedSpecies(u16 species)
{
    if(RoguePokedex_GetSpeciesBST(species) >= 570)
    {
        if(Rogue_GetMaxEvolutionCount(species) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 RandomiseWildEncounters_CalculateWeight(u16 index, u16 species, void* data)
{
#ifdef ROGUE_EXPANSION
    switch (species)
    {
    case SPECIES_DEERLING:
    case SPECIES_SAWSBUCK:
        if(RogueToD_GetSeason() != SEASON_SPRING)
            return 0;
        break;

    case SPECIES_DEERLING_SUMMER:
    case SPECIES_SAWSBUCK_SUMMER:
        if(RogueToD_GetSeason() != SEASON_SUMMER)
            return 0;
        break;

    case SPECIES_DEERLING_AUTUMN:
    case SPECIES_SAWSBUCK_AUTUMN:
        if(RogueToD_GetSeason() != SEASON_AUTUMN)
            return 0;
        break;

    case SPECIES_DEERLING_WINTER:
    case SPECIES_SAWSBUCK_WINTER:
        if(RogueToD_GetSeason() != SEASON_WINTER)
            return 0;
        break;

    case SPECIES_LYCANROC:
        if(RogueToD_IsNight() || RogueToD_IsDusk())
            return 0;
        break;

    case SPECIES_LYCANROC_MIDNIGHT:
        if(!RogueToD_IsNight())
            return 0;
        break;

    case SPECIES_LYCANROC_DUSK:
        if(!RogueToD_IsDusk())
            return 0;
        break;

    case SPECIES_ROCKRUFF:
        if(RogueToD_IsDusk())
            return 0;
        break;

    case SPECIES_ROCKRUFF_OWN_TEMPO:
        if(!RogueToD_IsDusk())
            return 0;
        break;

    default:
        break;
    }

#endif

    if(RoguePokedex_IsSpeciesParadox(species))
    {
        if(Rogue_GetCurrentDifficulty() < ROGUE_GYM_MID_DIFFICULTY)
            return 0;
    }

    if(IsRareWeightedSpecies(species))
    {
        // Rare species become more common into late game
        if(Rogue_GetCurrentDifficulty() >= ROGUE_GYM_MID_DIFFICULTY + 1)
            return 3;
        else if(Rogue_GetCurrentDifficulty() >= ROGUE_GYM_MID_DIFFICULTY - 1)
            return 2;
        else
            return 1;
    }

    return 3;
}

static u8 RandomiseWildEncounters_CalculateInitialWeight(u16 index, u16 species, void* data)
{
    // For the 1st encounter, we ensure we will have a mon of that typ
    u8 typeHint = *((u8*)data);

    if(RoguePokedex_GetSpeciesType(species, 0) == typeHint || RoguePokedex_GetSpeciesType(species, 1) == typeHint)
        return RandomiseWildEncounters_CalculateWeight(index, species, NULL);
    else
        return 0;
}

static void BeginWildEncounterQuery()
{
    u8 maxlevel = CalculateWildLevel(0);
    u32 typeFlags = 0;

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
    {
        typeFlags = Rogue_GetTypeFlagsFromArray(
            &gRogueRouteTable.routes[gRogueRun.currentRouteIndex].wildTypeTable[0],
            ARRAY_COUNT(gRogueRouteTable.routes[gRogueRun.currentRouteIndex].wildTypeTable)
        );
    }
    else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_HONEY_TREE)
    {
        u8 type;

        do
        {
            type = RogueRandom() % NUMBER_OF_MON_TYPES;
        }
        while(!IS_STANDARD_TYPE(type));

        typeFlags = MON_TYPE_VAL_TO_FLAGS(type);
    }
    else
    {
        AGB_ASSERT(FALSE);
    }

    RogueMonQuery_Begin();

    RogueMonQuery_IsSpeciesActive();

    // Prefilter to mons of types we're interested in
    RogueMonQuery_EvosContainType(QUERY_FUNC_INCLUDE, typeFlags);
    RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);

    RogueMonQuery_TransformIntoEggSpecies();
    RogueMonQuery_TransformIntoEvos(maxlevel - min(6, maxlevel - 1), FALSE, FALSE);

    // Now we've evolved we're only caring about mons of this type
    RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, typeFlags);

    // Now transform back into egg species, so the spawning should still be deteministic
    // (although the type hints could be invalid)
    if(IsCurseActive(EFFECT_WILD_EGG_SPECIES))
        RogueMonQuery_TransformIntoEggSpecies();

}

static void EndWildEncounterQuery()
{
    RogueMonQuery_End();
}

static void RandomiseWildEncounters(void)
{
    struct WildFormFamilyScratch *scratch;
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 startClock = RogueDebug_SampleClock();
#endif

    BeginWildEncounterQuery();
    scratch = Alloc(sizeof(*scratch));
    if(scratch == NULL)
    {
        u8 i;

        for(i = 0; i < WILD_ENCOUNTER_GRASS_CAPACITY; ++i)
        {
            gRogueRun.wildEncounters.species[i] = SPECIES_NONE;
            gRogueRun.wildEncounters.catchCounts[i] = 0;
        }
        EndWildEncounterQuery();
#ifdef DEBUG_FEATURE_FRAME_TIMERS
        DebugPrintf("[Room Load] Wild grass roster: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
        return;
    }

    {
        u8 i;
        u8 typeHint = Rogue_GetTypeForHintForRoom(&gRogueAdvPath.rooms[gRogueRun.adventureRoomId]);
        bool8 hasHintEncounter = FALSE;
        u16 hintFamilyKey;

        BuildWildFormFamilyWeights(scratch, RandomiseWildEncounters_CalculateInitialWeight, &typeHint);
        gRogueRun.wildEncounters.species[0] = SelectWildSpeciesFromBuiltFormFamilies(
            scratch,
            RogueRandom(),
            RogueRandom(),
            RandomiseWildEncounters_CalculateInitialWeight,
            &typeHint,
            FALSE);
        hasHintEncounter = gRogueRun.wildEncounters.species[0] != SPECIES_NONE;
        hintFamilyKey = scratch->lastSelectedFamilyKey;

        // The normal weights do not change between draws. Build them once, drop
        // the hint family if one was selected, then remove subsequent families
        // directly from the scratch table.
        BuildWildFormFamilyWeights(scratch, RandomiseWildEncounters_CalculateWeight, NULL);
        if(hasHintEncounter)
            RemoveWildFormFamilyWeight(scratch, hintFamilyKey);

        for(i = hasHintEncounter ? 1 : 0; i < WILD_ENCOUNTER_GRASS_CAPACITY; ++i)
        {
            gRogueRun.wildEncounters.species[i] = SelectWildSpeciesFromBuiltFormFamilies(
                scratch,
                RogueRandom(),
                RogueRandom(),
                RandomiseWildEncounters_CalculateWeight,
                NULL,
                FALSE);
            gRogueRun.wildEncounters.catchCounts[i] = 0;
        }

        if(hasHintEncounter)
            gRogueRun.wildEncounters.catchCounts[0] = 0;
    }
    Free(scratch);
    EndWildEncounterQuery();
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Room Load] Wild grass roster: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
}

bool8 Rogue_CanScatterPokeblock(u16 itemId)
{
    if(Rogue_IsRunActive())
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_HONEY_TREE)
        {
            if(itemId != ITEM_POKEBLOCK_SHINY)
                return TRUE;
        }
        else
            return VarGet(VAR_ROGUE_ACTIVE_POKEBLOCK) == ITEM_NONE && GetCurrentWildEncounterCount() > 0;
    }

    return FALSE;
}

static bool8 IsNewWildGrassEncounterSpecies(u16 species, void* usrData UNUSED)
{
    u8 i;
    u8 count = GetCurrentWildEncounterCount();
    u16 speciesFamilyKey = GetWildFormFamilyKey(species);

    for(i = 0; i < count; ++i)
    {
        u16 encounterSpecies = GetWildGrassEncounter(i);

        if(encounterSpecies == SPECIES_NONE)
            continue;

        if(GetWildFormFamilyKey(encounterSpecies) == speciesFamilyKey)
            return FALSE;
    }

    return TRUE;
}

bool8 Rogue_RerollSingleWildSpecies(u8 type)
{
    bool8 success = FALSE;
    u8 encounterCount = GetCurrentWildEncounterCount();

    if(encounterCount == 0)
        return FALSE;

    BeginWildEncounterQuery();
    RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, MON_TYPE_VAL_TO_FLAGS(type));
    RogueMonQuery_CustomFilter(IsNewWildGrassEncounterSpecies, NULL);

    {
        u16 species = SelectWildSpeciesFromFormFamilies(Random(), Random(), WildFormFlatWeight, NULL, FALSE);

        if(species != SPECIES_NONE)
        {
            u8 index = Random() % encounterCount;

            gRogueRun.wildEncounters.species[index] = species;
            success = TRUE;
        }
    }

    EndWildEncounterQuery();

    return success;
}

static bool8 HasHoneyTreeEncounterPending()
{
    return gRogueRun.honeyTreePokeblockTypeFlags != 0;
}

static void ClearHoneyTreePokeblock()
{
    gRogueRun.honeyTreePokeblockTypeFlags = 0;
}

bool8 Rogue_TryAddHoneyTreePokeblock(u16 itemId)
{
    // Shiny pokeblock isn't supported by honey tree atm for balance concerns
    if(itemId >= FIRST_ITEM_POKEBLOCK && itemId <= LAST_ITEM_POKEBLOCK && itemId != ITEM_POKEBLOCK_SHINY)
    {
        //u16 itemId = FIRST_ITEM_POKEBLOCK + i;
        u8 type = ItemId_GetSecondaryId(itemId);
        u32 mask = MON_TYPE_VAL_TO_FLAGS(type);

        if(mask != 0)
        {
            if((gRogueRun.honeyTreePokeblockTypeFlags & mask) == 0)
            {
                gRogueRun.honeyTreePokeblockTypeFlags |= mask;
                return 1;
            }

            return 2;
        }
    }

    return 0;
}

static u8 RandomiseFishingEncounters_CalculateWeight(u16 index, u16 species, void* data)
{
    if(IsRareWeightedSpecies(species))
        return 1;

    return 10;
}

static void RandomiseFishingEncounters(void)
{
    struct WildFormFamilyScratch *scratch;
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 startClock = RogueDebug_SampleClock();
#endif

    RogueMonQuery_Begin();

    RogueMonQuery_IsSpeciesActive();

    // Prefilter to mons of types we're interested in
    RogueMonQuery_EvosContainType(QUERY_FUNC_INCLUDE, MON_TYPE_VAL_TO_FLAGS(TYPE_WATER));
    RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);

    RogueMonQuery_TransformIntoEggSpecies();

    // Now we've evolved we're only caring about mons of this type
    RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, MON_TYPE_VAL_TO_FLAGS(TYPE_WATER));

    scratch = Alloc(sizeof(*scratch));
    {
        u8 i;

        if(scratch != NULL)
            BuildWildFormFamilyWeights(scratch, RandomiseFishingEncounters_CalculateWeight, NULL);

        for(i = 0; i < WILD_ENCOUNTER_WATER_CAPACITY; ++i)
        {
            if(scratch == NULL)
            {
                gRogueRun.wildEncounters.species[WILD_ENCOUNTER_GRASS_CAPACITY + i] = SPECIES_NONE;
            }
            else
            {
                gRogueRun.wildEncounters.species[WILD_ENCOUNTER_GRASS_CAPACITY + i] = SelectWildSpeciesFromBuiltFormFamilies(
                    scratch,
                    RogueRandom(),
                    RogueRandom(),
                    RandomiseFishingEncounters_CalculateWeight,
                    NULL,
                    FALSE);
            }
            gRogueRun.wildEncounters.catchCounts[WILD_ENCOUNTER_GRASS_CAPACITY + i] = 0;
        }
    }
    Free(scratch);

    RogueMonQuery_End();
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Run Load] Wild fishing roster: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
}

void Rogue_SafariTypeForMap(u8* outArray, u8 arraySize)
{
    AGB_ASSERT(arraySize == 3);

    outArray[0] = TYPE_NONE;
    outArray[1] = TYPE_NONE;
    outArray[2] = TYPE_NONE;

    if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_SOUTH)
    {
        outArray[0] = TYPE_NORMAL;
        outArray[1] = TYPE_FIGHTING;
#ifdef ROGUE_EXPANSION
        outArray[2] = TYPE_FAIRY;
#endif
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_SOUTHWEST)
    {
        outArray[0] = TYPE_GRASS;
        outArray[1] = TYPE_POISON;
        outArray[2] = TYPE_DARK;
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_NORTHWEST)
    {
        outArray[0] = TYPE_DRAGON;
        outArray[1] = TYPE_STEEL;
        outArray[2] = TYPE_PSYCHIC;
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_NORTH)
    {
        outArray[0] = TYPE_FLYING;
        outArray[1] = TYPE_GHOST;
        outArray[2] = TYPE_FIRE;
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_NORTHEAST)
    {
        outArray[0] = TYPE_ROCK;
        outArray[1] = TYPE_GROUND;
        outArray[2] = TYPE_ELECTRIC;
    }
    else // SAFARI_ZONE_SOUTHEAST
    {
        outArray[0] = TYPE_WATER;
        outArray[1] = TYPE_BUG;
        outArray[2] = TYPE_ICE;
    }
}

static void RandomiseSafariWildEncounters(void)
{
    // No longer supported code path
    AGB_ASSERT(FALSE);
}

static void ResetTrainerBattles(void)
{
    // Reset trainer encounters
    s16 i;
    for(i = 0; i < TRAINERS_COUNT; ++i)
    {
        ClearTrainerFlag(i);
    }
}

static bool8 RogueRandomChanceTrainer();

static void RandomiseEnabledTrainers()
{
    u16 i;
    u16 activeTrainers = 0;
    u16 trainerBuffer[ROGUE_MAX_ACTIVE_TRAINER_COUNT];
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 startClock = RogueDebug_SampleClock();
#endif

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_TEAM_HIDEOUT)
        Rogue_ChooseTeamHideoutTrainers(trainerBuffer, ARRAY_COUNT(trainerBuffer));
    else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BATTLE_TOWER)
        Rogue_ChooseSpectatorTrainers(trainerBuffer, ARRAY_COUNT(trainerBuffer));
    else
        Rogue_ChooseRouteTrainers(trainerBuffer, ARRAY_COUNT(trainerBuffer));

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BATTLE_TOWER)
    {
        u16 enabledCount = 2 + RogueRandomRange(3, 0);

        if(RogueRandomChance(20, 0))
            enabledCount = 4 + RogueRandomRange(3, 0);

        for(i = 0; i < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++i)
            Rogue_SetDynamicTrainer(i, TRAINER_NONE);

        for(i = 0; i < enabledCount; ++i)
            Rogue_SetDynamicTrainer(i, trainerBuffer[i]);

#ifdef DEBUG_FEATURE_FRAME_TIMERS
        DebugPrintf("[Room Load] Dynamic trainers: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
        return;
    }

    for(i = 0; i < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++i)
    {
        if(RogueRandomChanceTrainer())
        {
            Rogue_SetDynamicTrainer(i, trainerBuffer[i]);
            ++activeTrainers;
        }
        else
            Rogue_SetDynamicTrainer(i, TRAINER_NONE);
    }

    // May only limited number of trainers active
    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS)
    {
        while(activeTrainers > 12)
        {
            i = RogueRandom() % ROGUE_MAX_ACTIVE_TRAINER_COUNT;

            if(Rogue_GetDynamicTrainer(i) != TRAINER_NONE)
            {
                Rogue_SetDynamicTrainer(i, TRAINER_NONE);
                --activeTrainers;
            }
        }
    }
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Room Load] Dynamic trainers: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
}

u8 GetLeadMonLevel(void);

static u8 CalculateWildLevel(u8 variation)
{
    u8 wildLevel;
    u8 playerLevel = Rogue_CalculatePlayerMonLvl();

    if(GetSafariZoneFlag())
    {
        if((Random() % 6) == 0)
        {
            // Occasionally throw in starter level mons
            wildLevel = 7;
        }
        else
        {
            wildLevel = GetLeadMonLevel();
        }
    }
    else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_WILD_DEN || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_UNIQUE_DEN)
    {
        wildLevel = playerLevel - 5;
    }
    else if(playerLevel < 10)
    {
        wildLevel = 4;
    }
    else
    {
        wildLevel = playerLevel - 7;
    }

    variation = min(variation, wildLevel);

    if(variation != 0)
        return wildLevel - (Random() % variation);
    else
        return wildLevel;
}

u8 Rogue_GetEncounterDifficultyModifier()
{
    switch (gRogueAdvPath.currentRoomType)
    {
    case ADVPATH_ROOM_ROUTE:
        return gRogueAdvPath.currentRoomParams.perType.route.difficulty;

    case ADVPATH_ROOM_TEAM_HIDEOUT:
        return ADVPATH_SUBROOM_ROUTE_TOUGH;
    }

    return ADVPATH_SUBROOM_ROUTE_AVERAGE;
}

u16 Rogue_GetTRMove(u16 trNumber)
{
    if(trNumber < NUM_TECHNICAL_RECORDS && Rogue_IsRunActive())
        return gRogueRun.dynamicTRMoves[trNumber];

    // Return dud moves for item pricing calcs etc.
    return MOVE_SPLASH;
}

static u8 TRMove_CalculateWeight(u16 index, u16 move, void* data)
{
    // We're specifically going to use moves which would be Tutor moves i.e. ignore moves like growl or splash
    u16 usage = gRoguePokemonSpecialMoveUsages[move];

    // If we only have little usage on mons we're not going to allow it to be a tm
#ifdef ROGUE_EXPANSION
    if(usage <= 2)
#else
    if(usage <= 1)
#endif
        return 0;


    if(usage >= 300)
        return 5;
    if(usage >= 200)
        return 4;
    if(usage >= 100)
        return 3;
    if(usage >= 50)
        return 2;

    return 1;
}

static void RandomiseTRMoves()
{
    RogueMoveQuery_Begin();
    RogueMoveQuery_Reset(QUERY_FUNC_INCLUDE);

    RogueMoveQuery_IsTM(QUERY_FUNC_EXCLUDE);
    RogueMoveQuery_IsHM(QUERY_FUNC_EXCLUDE);

    RogueWeightQuery_Begin();
    {
        u8 i;
        RogueWeightQuery_CalculateWeights(TRMove_CalculateWeight, NULL);

        for(i = 0; i < NUM_TECHNICAL_RECORDS; ++i)
        {
            AGB_ASSERT(RogueWeightQuery_HasAnyWeights());
            gRogueRun.dynamicTRMoves[i] = RogueWeightQuery_SelectRandomFromWeightsWithUpdate(RogueRandom(), 0);
            AGB_ASSERT(gRogueRun.dynamicTRMoves[i] != MOVE_NONE);
        }
    }
    RogueWeightQuery_End();

    RogueMoveQuery_End();
}

static bool8 RogueRandomChanceTrainer()
{
    u8 difficultyLevel = Rogue_GetCurrentDifficulty();
    u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();
    s32 chance = 4 * (difficultyLevel + 1);

    if(Rogue_GetModeRules()->disableRouteTrainers)
    {
        return FALSE;
    }

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS)
    {
        // Scale with badge count
        switch (difficultyLevel)
        {
        case 0:
            chance = 1;
            break;

        case 1:
            chance = 5;
            break;

        case 2:
            chance = 10;
            break;

        case 3:
            chance = 20;
            break;

        case 4:
            chance = 35;
            break;

        case 5:
            chance = 55;
            break;

        case 6:
            chance = 80;
            break;

        default:
            chance = 100;
            break;
        }
    }
    else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_TEAM_HIDEOUT)
    {
        // We want a good number of trainers in the hideout
        chance = max(66, chance);
    }
    else
    {
        if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_CALM)
            chance = max(5, chance - 20); // Trainers are fewer
        else
            chance = max(15, chance); // Trainers are harder on tough routes
    }

    return RogueRandomChance(chance, FLAG_SET_SEED_TRAINERS);
}

static bool8 RogueRandomChanceItem()
{
    s32 chance;
    u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();

    if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_CALM)
        chance = 65;
    else if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_TOUGH)
        chance = 95;
    else
        chance = 80;

    return RogueRandomChance(chance, FLAG_SET_SEED_ITEMS);
}

static bool8 RogueRandomChanceBerry()
{
    u8 difficultyLevel = Rogue_GetCurrentDifficulty();
    u8 chance = max(10, 70 - min(65, 5 * difficultyLevel));

    return RogueRandomChance(chance, FLAG_SET_SEED_ITEMS);
}

struct RouteItemWeightContext
{
    u16 selectedItems[ROGUE_ITEM_COUNT];
    u16 partySpecialItems[ROUTE_SPECIAL_ITEM_CAPACITY];
    u16 otherSpecialItems[ROUTE_SPECIAL_ITEM_CAPACITY];
    u16 partyBoostItems[ROUTE_SPECIAL_ITEM_CAPACITY];
    u16 selectedItemCount;
    u16 partySpecialItemCount;
    u16 otherSpecialItemCount;
    u16 partyBoostItemCount;
    bool8 isTeamHideout;
};

static bool8 IsRouteSpecialDropFormChangeMethod(u16 method)
{
    switch (method)
    {
    case FORM_CHANGE_ITEM_HOLD:
    case FORM_CHANGE_ITEM_USE:
    case FORM_CHANGE_ITEM_USE_MULTICHOICE:
    case FORM_CHANGE_BEGIN_BATTLE:
    case FORM_CHANGE_END_BATTLE:
    case FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM:
    case FORM_CHANGE_BATTLE_PRIMAL_REVERSION:
    case FORM_CHANGE_BATTLE_ULTRA_BURST:
        return TRUE;

    default:
        return FALSE;
    }
}

static bool8 IsRouteBoostedEvolutionMethod(u16 method)
{
    switch (method)
    {
    case EVO_ITEM:
#ifdef ROGUE_EXPANSION
    case EVO_ITEM_HOLD_DAY:
    case EVO_ITEM_HOLD_NIGHT:
    case EVO_ITEM_MALE:
    case EVO_ITEM_FEMALE:
    case EVO_ITEM_NIGHT:
    case EVO_ITEM_DAY:
    case EVO_ITEM_HOLD:
#endif
        return TRUE;

    default:
        return FALSE;
    }
}

static bool8 IsRouteSpecialDropItemAllowed(u16 itemId)
{
    if(itemId == ITEM_NONE || itemId == ITEM_LIST_END)
        return FALSE;

    if(Rogue_IsReusableItem(itemId) && CheckBagHasItem(itemId, 1))
        return FALSE;

    switch (ItemId_GetPocket(itemId))
    {
    case POCKET_KEY_ITEMS:
    case POCKET_BERRIES:
    case POCKET_POKEBLOCK:
        return FALSE;

    default:
        return itemId != ITEM_NONE && itemId != ITEM_LIST_END && Rogue_IsItemEnabled(itemId);
    }
}

static bool8 RouteItemContextContainsItem(const u16* items, u16 count, u16 itemId)
{
    u16 i;

    for(i = 0; i < count; ++i)
    {
        if(items[i] == itemId)
            return TRUE;
    }

    return FALSE;
}

static void RouteItemContextAppendUnique(u16* items, u16* count, u16 capacity, u16 itemId)
{
    if(!RouteItemContextContainsItem(items, *count, itemId))
    {
        AGB_ASSERT(*count < capacity);

        if(*count < capacity)
            items[(*count)++] = itemId;
    }
}

static void AppendPartyFamilyRouteItems(struct RouteItemWeightContext* context, u16 species)
{
    u8 i;
    struct FormChange formChange;
    u8 formChangeCount = Rogue_GetActiveFormChangeCount(species);
    struct Evolution evolution;
    u8 evolutionCount = Rogue_GetMaxEvolutionCount(species);

    for(i = 0; i < formChangeCount; ++i)
    {
        Rogue_ModifyFormChange(species, i, &formChange);

        if(IsRouteSpecialDropFormChangeMethod(formChange.method) && IsRouteSpecialDropItemAllowed(formChange.param1))
        {
            RouteItemContextAppendUnique(context->partySpecialItems, &context->partySpecialItemCount, ARRAY_COUNT(context->partySpecialItems), formChange.param1);
            RouteItemContextAppendUnique(context->partyBoostItems, &context->partyBoostItemCount, ARRAY_COUNT(context->partyBoostItems), formChange.param1);
        }
    }

    for(i = 0; i < evolutionCount; ++i)
    {
        Rogue_ModifyEvolution(species, i, &evolution);

        if(evolution.method != 0 && evolution.targetSpecies != SPECIES_NONE)
        {
            if(IsRouteBoostedEvolutionMethod(evolution.method) && IsRouteSpecialDropItemAllowed(evolution.param))
                RouteItemContextAppendUnique(context->partyBoostItems, &context->partyBoostItemCount, ARRAY_COUNT(context->partyBoostItems), evolution.param);

            AppendPartyFamilyRouteItems(context, evolution.targetSpecies);
        }
    }
}

static void AppendActiveRouteSpecialItems(struct RouteItemWeightContext* context)
{
    u16 i;

    for(i = 0; i < gRogueBake_FormItems_Count; ++i)
    {
        u16 itemId = gRogueBake_FormItems[i];

        if(GetFormItemFlag(itemId)
            && IsRouteSpecialDropItemAllowed(itemId)
            && !RouteItemContextContainsItem(context->partySpecialItems, context->partySpecialItemCount, itemId))
            RouteItemContextAppendUnique(context->otherSpecialItems, &context->otherSpecialItemCount, ARRAY_COUNT(context->otherSpecialItems), itemId);
    }
}

static void InitRouteItemWeightContext(struct RouteItemWeightContext* context)
{
    u8 i;

    memset(context, 0, sizeof(*context));
    context->isTeamHideout = gRogueAdvPath.currentRoomType == ADVPATH_ROOM_TEAM_HIDEOUT;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        u16 partySpecies = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if(partySpecies != SPECIES_NONE)
        {
            u16 eggSpecies = Rogue_GetEggSpecies(partySpecies);

            if(eggSpecies != SPECIES_NONE)
                AppendPartyFamilyRouteItems(context, eggSpecies);
        }
    }

    AppendActiveRouteSpecialItems(context);
}

static bool8 RouteItemContextIsPartySpecialItem(struct RouteItemWeightContext* context, u16 itemId)
{
    return context != NULL && RouteItemContextContainsItem(context->partySpecialItems, context->partySpecialItemCount, itemId);
}

static bool8 RouteItemContextIsAnySpecialItem(struct RouteItemWeightContext* context, u16 itemId)
{
    return context != NULL
        && (RouteItemContextContainsItem(context->partySpecialItems, context->partySpecialItemCount, itemId)
            || RouteItemContextContainsItem(context->otherSpecialItems, context->otherSpecialItemCount, itemId));
}

static bool8 RouteItemContextHasAnySpecialItem(struct RouteItemWeightContext* context)
{
    return context != NULL && (context->partySpecialItemCount != 0 || context->otherSpecialItemCount != 0);
}

static bool8 RouteItemContextWasSelected(struct RouteItemWeightContext* context, u16 itemId)
{
    return context != NULL && RouteItemContextContainsItem(context->selectedItems, context->selectedItemCount, itemId);
}

static bool8 RouteItemContextHasSelectedSpecialItem(struct RouteItemWeightContext* context)
{
    u16 i;

    if(context == NULL)
        return FALSE;

    for(i = 0; i < context->selectedItemCount; ++i)
    {
        if(RouteItemContextIsAnySpecialItem(context, context->selectedItems[i]))
            return TRUE;
    }

    return FALSE;
}

static bool8 RouteItemContextIsPartyBoostItem(struct RouteItemWeightContext* context, u16 itemId)
{
    return context != NULL && RouteItemContextContainsItem(context->partyBoostItems, context->partyBoostItemCount, itemId);
}

static void RouteItemContextRecordSelection(struct RouteItemWeightContext* context, u16 itemId)
{
    if(context != NULL)
        RouteItemContextAppendUnique(context->selectedItems, &context->selectedItemCount, ARRAY_COUNT(context->selectedItems), itemId);
}

static void ExcludeRouteSpecialItems(struct RouteItemWeightContext* context)
{
    u16 i;

    for(i = 0; i < context->partySpecialItemCount; ++i)
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, context->partySpecialItems[i]);

    for(i = 0; i < context->otherSpecialItemCount; ++i)
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, context->otherSpecialItems[i]);
}

static u16 GetRouteSpecialItemDropChancePerMille(struct RouteItemWeightContext* context)
{
    if(context != NULL && context->isTeamHideout)
        return HIDEOUT_SPECIAL_ITEM_DROP_CHANCE_PER_MILLE;

    return ROUTE_SPECIAL_ITEM_DROP_CHANCE_PER_MILLE;
}

static bool8 RollRouteSpecialItemDrop(struct RouteItemWeightContext* context)
{
    return RogueRandomRange(1000, FLAG_SET_SEED_ITEMS) < GetRouteSpecialItemDropChancePerMille(context);
}

static void AccumulateRouteSpecialItemWeight(struct RouteItemWeightContext* context, u16 itemId, u8 weight, u16* totalWeight)
{
    if(!RouteItemContextWasSelected(context, itemId))
        *totalWeight += weight;
}

static bool8 TrySelectRouteSpecialItem(struct RouteItemWeightContext* context, u16* itemId)
{
    u16 i;
    u16 totalWeight = 0;
    u16 targetWeight;

    if(context == NULL || !RouteItemContextHasAnySpecialItem(context) || RouteItemContextHasSelectedSpecialItem(context))
        return FALSE;

    for(i = 0; i < context->partySpecialItemCount; ++i)
        AccumulateRouteSpecialItemWeight(context, context->partySpecialItems[i], PARTY_SPECIAL_ITEM_WEIGHT, &totalWeight);

    for(i = 0; i < context->otherSpecialItemCount; ++i)
        AccumulateRouteSpecialItemWeight(context, context->otherSpecialItems[i], OTHER_SPECIAL_ITEM_WEIGHT, &totalWeight);

    if(totalWeight == 0)
        return FALSE;

    targetWeight = RogueRandom() % totalWeight;

    for(i = 0; i < context->partySpecialItemCount; ++i)
    {
        if(!RouteItemContextWasSelected(context, context->partySpecialItems[i]))
        {
            if(targetWeight < PARTY_SPECIAL_ITEM_WEIGHT)
            {
                *itemId = context->partySpecialItems[i];
                return TRUE;
            }

            targetWeight -= PARTY_SPECIAL_ITEM_WEIGHT;
        }
    }

    for(i = 0; i < context->otherSpecialItemCount; ++i)
    {
        if(!RouteItemContextWasSelected(context, context->otherSpecialItems[i]))
        {
            if(targetWeight < OTHER_SPECIAL_ITEM_WEIGHT)
            {
                *itemId = context->otherSpecialItems[i];
                return TRUE;
            }

            targetWeight -= OTHER_SPECIAL_ITEM_WEIGHT;
        }
    }

    AGB_ASSERT(FALSE);
    return FALSE;
}

static u8 RouteItems_CalculateBaseWeight(u16 itemId)
{
    u8 pocket = ItemId_GetPocket(itemId);

    if(itemId == ITEM_POKEBLOCK_BUNDLE)
        return ROUTE_POKEBLOCK_BUNDLE_WEIGHT;

    switch (pocket)
    {
    case POCKET_TM_HM:
        return 3;

    case POCKET_HELD_ITEMS:
        return 3;

    case POCKET_STONES:
        return 6;

    case POCKET_MEDICINE:
    case POCKET_ITEMS:
        return 20;

    default:
        return 10;
    }
}

static u8 RouteItems_CalculateWeight(u16 index, u16 itemId, void* data)
{
    struct RouteItemWeightContext* context = data;
    u8 weight = RouteItems_CalculateBaseWeight(itemId);
    bool8 isPartySpecialItem = RouteItemContextIsPartySpecialItem(context, itemId);
    bool8 isSpecialItem = isPartySpecialItem || RouteItemContextIsAnySpecialItem(context, itemId);
    bool8 isPartyBoostItem = isPartySpecialItem || RouteItemContextIsPartyBoostItem(context, itemId);

    if(RouteItemContextWasSelected(context, itemId))
        return (isSpecialItem || Rogue_IsReusableItem(itemId)) ? 0 : 1;

    if(isSpecialItem)
        return 0;

    if(isPartyBoostItem)
        weight = min(80, weight + 15);

    return weight;
}

u8 GetCurrentDropRarity()
{
    u8 rarityBoost = Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD ? 1 : 0;

    switch (gRogueAdvPath.currentRoomType)
    {
    case ADVPATH_ROOM_ROUTE:
        return rarityBoost + gRogueRouteTable.routes[gRogueRun.currentRouteIndex].dropRarity;

    case ADVPATH_ROOM_TEAM_HIDEOUT:
        return rarityBoost + 3;
    }

    return rarityBoost;
}

static void RandomiseItemContent(u8 difficultyLevel)
{
    u16 itemId;
    u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();
    u8 dropRarity = GetCurrentDropRarity();
    struct RouteItemWeightContext *routeItemContext = NULL;
    bool8 allowSpecialItemDrops = gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE
        || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_TEAM_HIDEOUT;

    if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_CALM) // Easy
    {
        if(dropRarity != 0)
            --dropRarity;
    }
    else if(difficultyModifier == ADVPATH_SUBROOM_ROUTE_TOUGH) // Hard
    {
        if(dropRarity != 0)
            ++dropRarity;
    }

    if(allowSpecialItemDrops)
    {
        routeItemContext = Alloc(sizeof(*routeItemContext));
        if(routeItemContext != NULL)
            InitRouteItemWeightContext(routeItemContext);
        else
            allowSpecialItemDrops = FALSE;
    }

    RogueItemQuery_Begin();
    {
        RogueItemQuery_IsItemActive();

        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_KEY_ITEMS);
        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_BERRIES);
        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_POKEBLOCK);

        for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; ++itemId)
        {
            if(Rogue_IsReusableItem(itemId) && CheckBagHasItem(itemId, 1))
                RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, itemId);
        }

        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_PREMIER_BALL);
        RogueMiscQuery_EditRange(QUERY_FUNC_EXCLUDE, ITEM_X_ATTACK, ITEM_GUARD_SPEC);
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_POKE_DOLL);
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_FLUFFY_TAIL);
#ifdef ROGUE_EXPANSION
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, ITEM_POKE_TOY);
#endif

        RogueItemQuery_InPriceRange(QUERY_FUNC_INCLUDE, 50 + 100 * (difficultyLevel + dropRarity), 300 + 800 * (difficultyLevel + dropRarity));

        if(difficultyLevel <= 1)
        {
            //RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_BERRIES);
        }

        if(difficultyLevel <= 3)
        {
            RogueItemQuery_IsHeldItem(QUERY_FUNC_EXCLUDE);
        }

        if(allowSpecialItemDrops && Rogue_GetConfigToggle(CONFIG_TOGGLE_BAG_CLAUSE))
        {
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_POKEBLOCK_BUNDLE);
        }

        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_TEAM_HIDEOUT)
        {
            // Allow rare candies and escape ropes in hideouts
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_RARE_CANDY);
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_ESCAPE_ROPE);
        }

        if(allowSpecialItemDrops)
            ExcludeRouteSpecialItems(routeItemContext);

        RogueWeightQuery_Begin();
        {
            u8 i;
            u16 itemId;

            for(i = 0; i < ROGUE_ITEM_COUNT; ++i)
            {
                bool8 selectedSpecialItem = allowSpecialItemDrops
                    && !FlagGet(FLAG_ROGUE_ITEM_START + i)
                    && RollRouteSpecialItemDrop(routeItemContext)
                    && TrySelectRouteSpecialItem(routeItemContext, &itemId);

                if(!selectedSpecialItem)
                {
                    RogueWeightQuery_CalculateWeights(RouteItems_CalculateWeight, routeItemContext);

                    AGB_ASSERT(RogueWeightQuery_HasAnyWeights());
                    itemId = RogueWeightQuery_SelectRandomFromWeights(RogueRandom());
                }

                if(allowSpecialItemDrops)
                    RouteItemContextRecordSelection(routeItemContext, itemId);
                VarSet(VAR_ROGUE_ITEM_START + i, itemId);
            }
        }
        RogueWeightQuery_End();

    }
    RogueItemQuery_End();
    Free(routeItemContext);
}

static void RandomiseEnabledItems(void)
{
    s32 i;
    u8 difficultyLevel = Rogue_GetCurrentDifficulty();
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 startClock = RogueDebug_SampleClock();
#endif

    if(Rogue_GetModeRules()->forceEndGameRouteItems)
    {
        difficultyLevel = ROGUE_MAX_BOSS_COUNT - 1;
    }

    for(i = 0; i < ROGUE_ITEM_COUNT; ++i)
    {
        if(RogueRandomChanceItem())
        {
            // Clear flag to show
            FlagClear(FLAG_ROGUE_ITEM_START + i);
        }
        else
        {
            // Set flag to hide
            FlagSet(FLAG_ROGUE_ITEM_START + i);
        }
    }

    RandomiseItemContent(difficultyLevel);
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Room Load] Item setup: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - startClock));
#endif
}

static void RandomiseMiniBossItemsAndRestoreSeed(void)
{
    RAND_TYPE seed = gRngRogueValue;

    RandomiseEnabledItems();

    // Item availability affects preset scoring, while team generation must
    // still begin from the room seed used by the path preview.
    gRngRogueValue = seed;
}

static void RandomiseCharmItems(void)
{
    u16 tempBuffer[5];
    u16 tempBufferCount = 0;

    // Charm Items
    VarSet(VAR_ROGUE_ITEM0, Rogue_NextCharmItem(tempBuffer, tempBufferCount++));
    VarSet(VAR_ROGUE_ITEM1, Rogue_NextCharmItem(tempBuffer, tempBufferCount++));
    VarSet(VAR_ROGUE_ITEM2, Rogue_NextCharmItem(tempBuffer, tempBufferCount++));

    // Curse Items
    VarSet(VAR_ROGUE_ITEM10, Rogue_NextDarkDealCurseItem(tempBuffer, tempBufferCount++));
    //VarSet(VAR_ROGUE_ITEM11, Rogue_NextCurseItem(tempBuffer, 4));
    //VarSet(VAR_ROGUE_ITEM12, Rogue_NextCurseItem(tempBuffer, 5));
}

#define FIRST_USELESS_BERRY_INDEX ITEM_RAZZ_BERRY
#define LAST_USELESS_BERRY_INDEX  ITEM_BELUE_BERRY

// Ignore enigma berry as it's useless in gen 3
#define BERRY_COUNT (LAST_BERRY_INDEX - FIRST_BERRY_INDEX)

static void RandomiseBerryTrees(void)
{
    RogueItemQuery_Begin();
    {
        RogueItemQuery_IsItemActive();
        RogueItemQuery_IsStoredInPocket(QUERY_FUNC_INCLUDE, POCKET_BERRIES);

        RogueWeightQuery_Begin();
        {
            u8 i, berryNum;
            u16 itemId;

            // The higher this number the less likely a berry repeats
            RogueWeightQuery_FillWeights(4);

            for(i = BERRY_TREE_ROUTE_FIRST; i <= BERRY_TREE_ROUTE_LAST; ++i)
            {
                if(RogueRandomChanceBerry())
                {
                    // Make unlikely to get this item again, but not impossible
                    itemId = RogueWeightQuery_SelectRandomFromWeightsWithUpdate(RogueRandom(), 1);
                    berryNum = ItemIdToBerryType(itemId);

                    PlantBerryTree(i, berryNum, BERRY_STAGE_BERRIES, FALSE);
                }
                else
                {
                    RemoveBerryTree(i);
                }
            }
        }
        RogueWeightQuery_End();

    }
    RogueItemQuery_End();
}
