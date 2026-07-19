#include "global.h"
#include "battle.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_quest.h"
#include "rogue_settings.h"
#include "test/test.h"

static const u8 sTrainerConfigToggles[] =
{
    CONFIG_TOGGLE_TRAINER_ROGUE,
    CONFIG_TOGGLE_TRAINER_KANTO,
    CONFIG_TOGGLE_TRAINER_JOHTO,
    CONFIG_TOGGLE_TRAINER_HOENN,
#ifdef ROGUE_EXPANSION
    CONFIG_TOGGLE_TRAINER_SINNOH,
    CONFIG_TOGGLE_TRAINER_UNOVA,
    CONFIG_TOGGLE_TRAINER_KALOS,
    CONFIG_TOGGLE_TRAINER_ALOLA,
    CONFIG_TOGGLE_TRAINER_GALAR,
    CONFIG_TOGGLE_TRAINER_PALDEA,
#endif
};

static void ClearRecoveryTestState(void)
{
    u8 i;

    RogueDebug_SetSacredAshRecoveryPending(FALSE);
    while(CheckBagHasItem(ITEM_SACRED_ASH, 1))
        RemoveBagItem(ITEM_SACRED_ASH, 1);

    for(i = 0; i < PARTY_SIZE; ++i)
        ZeroMonData(&gPlayerParty[i]);
    CalculatePlayerPartyCount();
}

static void CreateRecoveryTestParty(void)
{
    u32 value;

    CreateMon(&gPlayerParty[0], SPECIES_BULBASAUR, 50, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CreateMon(&gPlayerParty[1], SPECIES_CHARMANDER, 50, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);

    value = 0;
    SetMonData(&gPlayerParty[0], MON_DATA_HP, &value);
    value = 1;
    SetMonData(&gPlayerParty[1], MON_DATA_HP, &value);
    value = STATUS1_POISON;
    SetMonData(&gPlayerParty[1], MON_DATA_STATUS, &value);
    CalculatePlayerPartyCount();
}

TEST("Sacred Ash can restore the full party after battle losses")
{
    bool8 releaseFaintedMons = Rogue_GetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS);

    ClearRecoveryTestState();
    CreateRecoveryTestParty();
    AddBagItem(ITEM_SACRED_ASH, 1);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, TRUE);
    RogueDebug_SetSacredAshRecoveryPending(TRUE);

    EXPECT(Rogue_BufferSacredAshRecovery());

    Rogue_AcceptSacredAshRecovery();

    EXPECT(!CheckBagHasItem(ITEM_SACRED_ASH, 1));
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP));
    EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_HP), GetMonData(&gPlayerParty[1], MON_DATA_MAX_HP));
    EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_STATUS), 0);
    EXPECT_EQ(gPlayerPartyCount, 2);

    EXPECT(!Rogue_BufferSacredAshRecovery());
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, releaseFaintedMons);
    ClearRecoveryTestState();
}

TEST("Each Sacred Ash copy can recover a separate battle")
{
    bool8 releaseFaintedMons = Rogue_GetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS);
    u32 value;

    ClearRecoveryTestState();
    CreateRecoveryTestParty();
    EXPECT(AddBagItem(ITEM_SACRED_ASH, 2));
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, TRUE);

    RogueDebug_SetSacredAshRecoveryPending(TRUE);
    EXPECT(Rogue_BufferSacredAshRecovery());
    Rogue_AcceptSacredAshRecovery();

    EXPECT_EQ(GetItemCountInBag(ITEM_SACRED_ASH), 1);
    EXPECT_EQ(gPlayerPartyCount, 2);

    value = 0;
    SetMonData(&gPlayerParty[0], MON_DATA_HP, &value);
    RogueDebug_SetSacredAshRecoveryPending(TRUE);
    EXPECT(Rogue_BufferSacredAshRecovery());
    Rogue_AcceptSacredAshRecovery();

    EXPECT_EQ(GetItemCountInBag(ITEM_SACRED_ASH), 0);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP));
    EXPECT_EQ(gPlayerPartyCount, 2);

    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, releaseFaintedMons);
    ClearRecoveryTestState();
}

TEST("Declining Sacred Ash preserves the item and releases fainted Pokemon normally")
{
    bool8 releaseFaintedMons = Rogue_GetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS);

    ClearRecoveryTestState();
    CreateRecoveryTestParty();
    AddBagItem(ITEM_SACRED_ASH, 1);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, TRUE);
    RogueDebug_SetSacredAshRecoveryPending(TRUE);

    Rogue_DeclineSacredAshRecovery();

    EXPECT(CheckBagHasItem(ITEM_SACRED_ASH, 1));
    EXPECT_EQ(gPlayerPartyCount, 1);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), SPECIES_CHARMANDER);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), 1);

    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, releaseFaintedMons);
    ClearRecoveryTestState();
}

TEST("Surviving a wild battle queues Sacred Ash recovery for party losses")
{
    bool8 releaseFaintedMons = Rogue_GetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS);
    bool8 runActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    u8 previousBattleOutcome = gBattleOutcome;

    ClearRecoveryTestState();
    CreateRecoveryTestParty();
    AddBagItem(ITEM_SACRED_ASH, 1);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, TRUE);
    Rogue_SetCurrentDifficulty(0);
    gBattleOutcome = B_OUTCOME_WON;

    Rogue_Battle_EndWildBattle();
    EXPECT(Rogue_BufferSacredAshRecovery());

    EXPECT_EQ(gPlayerPartyCount, 2);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), 0);

    Rogue_DeclineSacredAshRecovery();

    if(runActive)
        FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    else
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, releaseFaintedMons);
    Rogue_SetCurrentDifficulty(previousDifficulty);
    gBattleOutcome = previousBattleOutcome;
    ClearRecoveryTestState();
}

TEST("Battle teardown defers losses until Sacred Ash availability is checked on the field")
{
    bool8 releaseFaintedMons = Rogue_GetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS);
    bool8 runActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    u8 previousBattleOutcome = gBattleOutcome;

    ClearRecoveryTestState();
    CreateRecoveryTestParty();
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, TRUE);
    Rogue_SetCurrentDifficulty(0);
    gBattleOutcome = B_OUTCOME_WON;

    Rogue_Battle_EndWildBattle();

    EXPECT_EQ(gPlayerPartyCount, 2);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), 0);
    EXPECT(!Rogue_BufferSacredAshRecovery());

    Rogue_DeclineSacredAshRecovery();
    EXPECT_EQ(gPlayerPartyCount, 1);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), SPECIES_CHARMANDER);

    if(runActive)
        FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    else
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, releaseFaintedMons);
    Rogue_SetCurrentDifficulty(previousDifficulty);
    gBattleOutcome = previousBattleOutcome;
    ClearRecoveryTestState();
}

TEST("Sacred Ash is not offered when the Final Quest preserves fainted Pokemon")
{
    bool8 trainerConfigToggles[ARRAY_COUNT(sTrainerConfigToggles)];
    bool8 runActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    bool8 questUnlocked = RogueQuest_GetStateFlag(QUEST_ID_THE_FINAL_RUN, QUEST_STATE_UNLOCKED);
    bool8 releaseFaintedMons = Rogue_GetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS);
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    u8 previousDexVariant = RoguePokedex_GetDexVariant();
    u8 previousGameMode = Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM);
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sTrainerConfigToggles); ++i)
    {
        trainerConfigToggles[i] = Rogue_GetConfigToggle(sTrainerConfigToggles[i]);
        Rogue_SetConfigToggle(sTrainerConfigToggles[i], FALSE);
    }

    ClearRecoveryTestState();
    CreateRecoveryTestParty();
    AddBagItem(ITEM_SACRED_ASH, 1);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    RogueQuest_SetStateFlag(QUEST_ID_THE_FINAL_RUN, QUEST_STATE_UNLOCKED, TRUE);
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_MAX);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, TRUE);
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_STANDARD);
    Rogue_SetCurrentDifficulty(ROGUE_FINAL_CHAMP_DIFFICULTY);

    EXPECT(Rogue_UseFinalQuestEffects());
    EXPECT(!RogueDebug_CanOfferSacredAshRecovery());

    Rogue_SetCurrentDifficulty(ROGUE_FINAL_CHAMP_DIFFICULTY - 1);
    EXPECT(RogueDebug_CanOfferSacredAshRecovery());

    for(i = 0; i < ARRAY_COUNT(sTrainerConfigToggles); ++i)
        Rogue_SetConfigToggle(sTrainerConfigToggles[i], trainerConfigToggles[i]);
    if(runActive)
        FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    else
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    RogueQuest_SetStateFlag(QUEST_ID_THE_FINAL_RUN, QUEST_STATE_UNLOCKED, questUnlocked);
    RoguePokedex_SetDexVariant(previousDexVariant);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, releaseFaintedMons);
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, previousGameMode);
    Rogue_SetCurrentDifficulty(previousDifficulty);
    ClearRecoveryTestState();
}
