#include "global.h"
#include "event_data.h"
#include "item.h"
#include "money.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_ascension.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_quest.h"
#include "rogue_run_start.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"
#include "rogue_trials.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/rogue_pokedex.h"
#include "constants/species.h"
#include "test/test.h"
#include "text.h"
#include "string_util.h"
#include "rogue_charms.h"

void Debug_RogueQuest_CompleteQuest(u16 questId);
void RogueTest_ApplyAscensionParty(u16 trainerNum, struct Pokemon *party, u8 count);

static void ResetAscensionTest(void)
{
    RogueTest_UseContentTrainers(FALSE);
    RogueRunStart_Clear();
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
    memset(&gRogueRun, 0, sizeof(gRogueRun));
    Rogue_ResetSettingsToDefaults();
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_GEN9);
    ZeroEnemyPartyMons();
}

TEST("Ascension: every cumulative threshold and distribution")
{
    // Explicit expected rows keep each step independently reviewable.
    static const u8 expected[21][5] = {
        {0,0,0,0,0}, {0,0,0,5,0}, {1,0,0,5,0}, {1,1,0,5,0}, {1,1,1,5,0},
        {1,1,1,10,0}, {2,1,1,10,0}, {2,2,1,10,0}, {2,2,2,10,0}, {2,2,2,10,10},
        {2,2,2,10,10}, {3,2,2,10,10}, {3,3,2,10,10}, {3,3,2,20,10}, {3,3,3,20,10},
        {3,3,3,20,10}, {3,3,3,20,20}, {3,3,3,20,20}, {3,3,3,20,20}, {3,3,3,20,20}, {3,3,3,20,20}
    };
    struct RogueAscensionRules r;
    u8 a, d, stage;
    for (a = 0; a <= 20; ++a)
    {
        RogueAscension_Resolve(a, &r);
        EXPECT_EQ(r.natures, expected[a][0]);
        EXPECT_EQ(r.moves, expected[a][1]);
        EXPECT_EQ(r.items, expected[a][2]);
        EXPECT_EQ(r.bossIvBonus, expected[a][3]);
        EXPECT_EQ(r.trainerIv, expected[a][4]);
        EXPECT_EQ(r.expandedParties, a >= 10);
        EXPECT_EQ(r.diverseTrainers, a >= 15);
        EXPECT_EQ(r.perfectAces, a >= 17);
        EXPECT_EQ(r.perfectTeams, a >= 18);
        EXPECT_EQ(r.limitedSupplies, a >= 19);
        EXPECT_EQ(r.fullParties, a >= 20);
        EXPECT_EQ(RogueAscension_PartySize(a, 3, 6, TRUE), a == 20 ? 6 : a >= 10 ? 4 : 3);
        EXPECT_EQ(RogueAscension_PartySize(a, 3, 6, FALSE), 3);
        EXPECT_EQ(RogueAscension_PartySize(a, 6, 3, TRUE), 3);
        EXPECT_LE(RogueAscension_PartySize(a, 6, 9, TRUE), PARTY_SIZE);
        for (stage = 0; stage < 14; ++stage)
        {
            EXPECT_LE(RogueAscension_CalculateIV(a, stage, TRUE, TRUE), 31);
            EXPECT_GE(RogueAscension_CalculateIV(a, stage, TRUE, TRUE), RogueAscension_CalculateIV(a, stage, TRUE, FALSE));
        }
    }
    for (d = 0; d < 4; ++d)
    {
        EXPECT_EQ(RogueAscension_Applies(d, FALSE, TRUE), d == 3);
        EXPECT_EQ(RogueAscension_Applies(d, TRUE, TRUE), d != 0);
        EXPECT_EQ(RogueAscension_Applies(d, TRUE, FALSE), d >= 2);
    }
    EXPECT_EQ(RogueAscension_CalculateIV(0, 8, TRUE, FALSE), 5);
    EXPECT_EQ(RogueAscension_CalculateIV(13, 13, TRUE, FALSE), 31);
    EXPECT_EQ(RogueAscension_CalculateIV(17, 0, TRUE, TRUE), 31);
    EXPECT_EQ(RogueAscension_CalculateIV(17, 0, TRUE, FALSE), 20);
    EXPECT_EQ(RogueAscension_CalculateIV(17, 0, FALSE, TRUE), 20);
    EXPECT_EQ(RogueAscension_CalculateIV(18, 0, FALSE, FALSE), 31);
}

TEST("Ascension: profile nature is independent from moves Ability and item")
{
    struct Pokemon original, mon;
    struct RogueAdventureConfig config;
    const struct RoguePokemonCompetitiveSet *profile;
    u16 boss, i;
    u8 a, role;
    static const u8 natureLevels[] = {2, 6, 11};
    static const u8 moveLevels[] = {3, 7, 12};
    static const u8 itemLevels[] = {4, 8, 14};
    ResetAscensionTest();
    Rogue_CopyAdventureConfig(&config);
    for (boss = 1; boss < gRogueTrainerCount; ++boss)
        if (Rogue_IsKeyTrainer(boss)) break;
    EXPECT_LT(boss, gRogueTrainerCount);
    EXPECT_GT(gRoguePokemonProfiles[SPECIES_VENUSAUR].competitiveSetCount, 0);
    for (i = 0; i < gRoguePokemonProfiles[SPECIES_VENUSAUR].competitiveSetCount; ++i)
        if (gRoguePokemonProfiles[SPECIES_VENUSAUR].competitiveSets[i].heldItem == ITEM_BLACK_SLUDGE) break;
    EXPECT_LT(i, gRoguePokemonProfiles[SPECIES_VENUSAUR].competitiveSetCount);
    profile = &gRoguePokemonProfiles[SPECIES_VENUSAUR].competitiveSets[i];
    CreateMon(&original, SPECIES_VENUSAUR, 100, 0, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
    SetNature(&original, NATURE_HARDY);
    for (role = 0; role < 3; ++role)
    for (a = 0; a <= 20; ++a)
    {
        config.ascension = a;
        Rogue_SetRunStartConfigOverride(&config);
        mon = original;
        RogueTest_ApplyAscensionProfile(boss, &mon, profile, role != 2, role == 0);
        EXPECT_EQ(GetNature(&mon), a >= natureLevels[role] ? profile->nature : NATURE_HARDY);
        EXPECT_EQ(GetMonUniqueAbility(&mon), GetMonUniqueAbility(&original));
        if (a < moveLevels[role])
        {
            EXPECT_EQ(GetMonAbility(&mon), GetMonAbility(&original));
            for (i = 0; i < MAX_MON_MOVES; ++i)
                EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1 + i), GetMonData(&original, MON_DATA_MOVE1 + i));
        }
        else
            EXPECT_EQ(GetMonAbility(&mon), profile->ability);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), a >= itemLevels[role] ? profile->heldItem : ITEM_NONE);
    }
    Rogue_ClearRunStartConfigOverride();
}

TEST("Ascension: separate ladders and once-only completion awards")
{
    u8 f;
    ResetAscensionTest();
    for (f = 0; f < 3; ++f) EXPECT_EQ(RogueAscension_GetUnlocked(f), 0);
    gRogueSaveBlock->activeAdventureConfig = gRogueSaveBlock->adventureConfig;
    gRogueSaveBlock->activeAdventureConfig.battleFormat = BATTLE_FORMAT_DOUBLES;
    gRogueSaveBlock->ascensionEligible = TRUE;
    RogueAscension_RecordWin();
    EXPECT_EQ(RogueAscension_GetUnlocked(BATTLE_FORMAT_DOUBLES), 1);
    EXPECT_EQ(RogueAscension_GetUnlocked(BATTLE_FORMAT_SINGLES), 0);
    EXPECT_EQ(RogueAscension_GetUnlocked(BATTLE_FORMAT_MIXED), 0);
    gRogueSaveBlock->activeAdventureConfig.ascension = 10;
    RogueAscension_RecordWin();
    EXPECT_EQ(RogueAscension_GetUnlocked(BATTLE_FORMAT_DOUBLES), 1);
    gRogueSaveBlock->ascensionEligible = FALSE;
    gRogueSaveBlock->ascensionRecorded = FALSE;
    gRogueSaveBlock->activeAdventureConfig.mode = ROGUE_GAME_MODE_SLOW_PATH;
    RogueAscension_RecordWin();
    EXPECT_EQ(gRogueSaveBlock->bestModeAscension[ROGUE_GAME_MODE_SLOW_PATH][BATTLE_FORMAT_DOUBLES], 10);
    EXPECT_EQ(RogueAscension_GetUnlocked(BATTLE_FORMAT_DOUBLES), 1);
    EXPECT_EQ(RogueAscension_GetEligibility(&gRogueSaveBlock->adventureConfig, RUN_START_SOURCE_REPLAY), ASCENSION_INELIGIBLE_REPLAY);
    EXPECT_EQ(RogueAscension_GetEligibility(&gRogueSaveBlock->adventureConfig, RUN_START_SOURCE_MULTIPLAYER_HOST), ASCENSION_INELIGIBLE_MULTIPLAYER);
    EXPECT_EQ(RogueAscension_GetEligibility(&gRogueSaveBlock->adventureConfig, RUN_START_SOURCE_TRIAL), ASCENSION_INELIGIBLE_TRIAL);
}

TEST("Ascension: Trial minimum uses the forced format ladder")
{
    struct RogueAdventureConfig config;
    ResetAscensionTest();
    Rogue_CopyAdventureConfig(&config);
    gRogueSaveBlock->bestAscension[BATTLE_FORMAT_SINGLES] = 20;
    EXPECT(!RogueTrial_BuildSelectionConfig(ROGUE_TRIAL_INSANE_MODE, 10, POKEDEX_VARIANT_NATIONAL_GEN9, &config));
    gRogueSaveBlock->bestAscension[BATTLE_FORMAT_MIXED] = 9;
    EXPECT(!RogueTrial_BuildSelectionConfig(ROGUE_TRIAL_INSANE_MODE, 9, POKEDEX_VARIANT_NATIONAL_GEN9, &config));
    EXPECT(RogueTrial_BuildSelectionConfig(ROGUE_TRIAL_INSANE_MODE, 10, POKEDEX_VARIANT_NATIONAL_GEN9, &config));
    EXPECT_EQ(config.battleFormat, BATTLE_FORMAT_MIXED);
    EXPECT(config.trialFreshStart);
    EXPECT(!RogueTrial_BuildSelectionConfig(ROGUE_TRIAL_INSANE_MODE, 11, POKEDEX_VARIANT_NATIONAL_GEN9, &config));
}

TEST("Ascension: save round trip preserves active rules progress and replay")
{
    ResetAscensionTest();
    gRogueSaveBlock->activeAdventureConfig = gRogueSaveBlock->adventureConfig;
    gRogueSaveBlock->activeAdventureConfig.ascension = 17;
    gRogueSaveBlock->activeAdventureConfig.battleFormat = BATTLE_FORMAT_MIXED;
    gRogueSaveBlock->bestAscension[BATTLE_FORMAT_DOUBLES] = 14;
    gRogueSaveBlock->selectedAscension[BATTLE_FORMAT_DOUBLES] = 12;
    gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_INSANE_MODE][BATTLE_FORMAT_MIXED] = 18;
    gRogueSaveBlock->ascensionEligible = TRUE;
    gRogueSaveBlock->ascensionRecorded = TRUE;
    gRogueSaveBlock->adventureReplay[0].rulesVersion = ASCENSION_RULES_VERSION;
    gRogueSaveBlock->adventureReplay[0].adventureConfig = gRogueSaveBlock->activeAdventureConfig;
    RogueSave_FormatForWriting();
    RogueSave_FormatForReading();
    EXPECT_EQ(gRogueSaveBlock->activeAdventureConfig.ascension, 17);
    EXPECT_EQ(gRogueSaveBlock->adventureConfig.ascension, 0);
    EXPECT_EQ(gRogueSaveBlock->bestAscension[BATTLE_FORMAT_DOUBLES], 14);
    EXPECT_EQ(gRogueSaveBlock->selectedAscension[BATTLE_FORMAT_DOUBLES], 12);
    EXPECT_EQ(gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_INSANE_MODE][BATTLE_FORMAT_MIXED], 18);
    EXPECT(gRogueSaveBlock->ascensionEligible && gRogueSaveBlock->ascensionRecorded);
    EXPECT_EQ(gRogueSaveBlock->adventureReplay[0].adventureConfig.ascension, 17);
    EXPECT_EQ(gRogueSaveBlock->adventureReplay[0].rulesVersion, ASCENSION_RULES_VERSION);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    EXPECT_EQ(Rogue_GetAscension(), 17);
    Rogue_SetAscension(0);
    EXPECT_EQ(Rogue_GetAscension(), 17);
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}

static void CheckGeneratedBossParty(u8 kind, u8 stage)
{
    static const u8 levels[] = {0, 10, 17, 20};
    struct RogueAdventureConfig config;
    u16 trainers[4] = {0};
    u16 t;
    u8 a, i, stat;
    ResetAscensionTest();
    Rogue_CopyAdventureConfig(&config);
    RogueTest_UseContentTrainers(TRUE);
    for (t = 1; t < gRogueTrainerCount; ++t)
    {
        if (!trainers[0] && Rogue_IsBossTrainer(t)) trainers[0] = t;
        if (!trainers[1] && Rogue_IsMiniBossTrainer(t)) trainers[1] = t;
        if (!trainers[2] && Rogue_IsRivalTrainer(t)) trainers[2] = t;
        if (!trainers[3] && Rogue_IsTeamBossTrainer(t)) trainers[3] = t;
    }
    {
        EXPECT_NE(trainers[kind], 0);
        {
            u8 baseline = 0;
            Rogue_SetCurrentDifficulty(stage);
            for (a = 0; a < ARRAY_COUNT(levels); ++a)
            {
                u8 count;
                config.ascension = levels[a];
                Rogue_SetRunStartConfigOverride(&config);
                memset(gRogueRun.rivalSpecies, 0, sizeof(gRogueRun.rivalSpecies));
                gRogueRun.rivalTrainerNum = trainers[kind];
                gRogueRun.baseSeed = 1741;
                ZeroEnemyPartyMons();
                SeedRogueRng(1234);
                SeedRng(4321);
                count = Rogue_CreateTrainerParty(trainers[kind], gEnemyParty, PARTY_SIZE, TRUE);
                if (levels[a] == 0) baseline = count;
                EXPECT_EQ(count, levels[a] == 20 ? 6 : levels[a] >= 10 ? min(6, baseline + 1) : baseline);
                for (i = 0; i < count; ++i)
                {
                    EXPECT_NE(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES), SPECIES_NONE);
                    EXPECT_NE(GetMonData(&gEnemyParty[i], MON_DATA_MOVE1), MOVE_NONE);
                    for (stat = 0; stat < NUM_STATS; ++stat)
                    {
                        EXPECT_EQ(GetMonData(&gEnemyParty[i], MON_DATA_HP_IV + stat), RogueAscension_CalculateIV(levels[a], stage, TRUE, i == count - 1));
                        EXPECT_EQ(GetMonData(&gEnemyParty[i], MON_DATA_HP_EV + stat), 0);
                    }
                    if (levels[a] == 0)
                    {
                        EXPECT_EQ(GetNature(&gEnemyParty[i]), NATURE_HARDY);
                        u16 item = GetMonData(&gEnemyParty[i], MON_DATA_HELD_ITEM);
                        EXPECT(item == ITEM_NONE || IS_MEGA_STONE_ITEM(item) || item == ITEM_RED_ORB || item == ITEM_BLUE_ORB || (item >= ITEM_NORMALIUM_Z && item <= ITEM_ULTRANECROZIUM_Z));
                    }
                }
            }
        }
    }
    Rogue_ClearRunStartConfigOverride();
    RogueTest_UseContentTrainers(FALSE);
}

TEST("Ascension: generated gym boss parties at stage 0")
{
    CheckGeneratedBossParty(0, 0);
}

TEST("Ascension: generated gym boss parties at stage 7")
{
    CheckGeneratedBossParty(0, 7);
}

TEST("Ascension: generated gym boss parties at stage 8")
{
    CheckGeneratedBossParty(0, 8);
}

TEST("Ascension: generated gym boss parties at stage 12")
{
    CheckGeneratedBossParty(0, 12);
}

TEST("Ascension: generated gym boss parties at stage 13")
{
    CheckGeneratedBossParty(0, 13);
}

TEST("Ascension: generated Frontier Brain parties at stage 0")
{
    CheckGeneratedBossParty(1, 0);
}

TEST("Ascension: generated Frontier Brain parties at stage 7")
{
    CheckGeneratedBossParty(1, 7);
}

TEST("Ascension: generated Frontier Brain parties at stage 8")
{
    CheckGeneratedBossParty(1, 8);
}

TEST("Ascension: generated Frontier Brain parties at stage 12")
{
    CheckGeneratedBossParty(1, 12);
}

TEST("Ascension: generated Frontier Brain parties at stage 13")
{
    CheckGeneratedBossParty(1, 13);
}

TEST("Ascension: generated rival parties at stage 0")
{
    CheckGeneratedBossParty(2, 0);
}

TEST("Ascension: generated rival parties at stage 7")
{
    CheckGeneratedBossParty(2, 7);
}

TEST("Ascension: generated rival parties at stage 8")
{
    CheckGeneratedBossParty(2, 8);
}

TEST("Ascension: generated rival parties at stage 12")
{
    CheckGeneratedBossParty(2, 12);
}

TEST("Ascension: generated rival parties at stage 13")
{
    CheckGeneratedBossParty(2, 13);
}

TEST("Ascension: generated team boss parties at stage 0")
{
    CheckGeneratedBossParty(3, 0);
}

TEST("Ascension: generated team boss parties at stage 7")
{
    CheckGeneratedBossParty(3, 7);
}

TEST("Ascension: generated team boss parties at stage 8")
{
    CheckGeneratedBossParty(3, 8);
}

TEST("Ascension: generated team boss parties at stage 12")
{
    CheckGeneratedBossParty(3, 12);
}

TEST("Ascension: generated team boss parties at stage 13")
{
    CheckGeneratedBossParty(3, 13);
}

TEST("Ascension: A19 supplies retain preparation and override imported inventory")
{
    struct RogueAdventureConfig config;
    u16 heldItem = ITEM_LEFTOVERS;
    u8 ev = 252;
    ResetAscensionTest();
    Rogue_CopyAdventureConfig(&config);
    config.ascension = 19;
    ClearBag();
    EXPECT(AddBagItem(ITEM_ULTRA_BALL, 40));
    EXPECT(AddBagItem(ITEM_POTION, 99));
    EXPECT(AddBagItem(ITEM_C_GEAR, 1));
    EXPECT(AddBagItem(ITEM_FLINCH_CHARM, 1));
    RecalcCharmCurseValues();
    config.ascension = 18;
    EXPECT_EQ(RogueAscension_GetEligibility(&config, RUN_START_SOURCE_NORMAL), ASCENSION_INELIGIBLE_CHARMS);
    config.ascension = 19;
    CreateMon(&gPlayerParty[0], SPECIES_VENUSAUR, 50, 31, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM, &heldItem);
    SetMonData(&gPlayerParty[0], MON_DATA_SPATK_EV, &ev);
    RogueSave_SaveHubStates();
    Rogue_SetRunStartConfigOverride(&config);
    RogueTest_SetupRunBag();
    EXPECT(CheckBagHasItem(ITEM_POKE_BALL, 15));
    EXPECT(!CheckBagHasItem(ITEM_POKE_BALL, 16));
    EXPECT(CheckBagHasItem(ITEM_POTION, 10));
    EXPECT(!CheckBagHasItem(ITEM_POTION, 11));
    EXPECT(CheckBagHasItem(ITEM_FULL_HEAL, 3));
    EXPECT(!CheckBagHasItem(ITEM_ULTRA_BALL, 1));
    EXPECT(CheckBagHasItem(ITEM_C_GEAR, 1));
    EXPECT(!CheckBagHasItem(ITEM_FLINCH_CHARM, 1));
    EXPECT(!AnyCharmsActive());
    EXPECT_EQ(RogueAscension_GetEligibility(&config, RUN_START_SOURCE_NORMAL), ASCENSION_ELIGIBLE);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 10000);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPATK_EV), 252);
    config.trialFreshStart = TRUE;
    Rogue_SetRunStartConfigOverride(&config);
    RogueTest_SetupRunBag();
    EXPECT(CheckBagHasItem(ITEM_POKE_BALL, 5));
    EXPECT(!CheckBagHasItem(ITEM_POKE_BALL, 6));
    EXPECT(CheckBagHasItem(ITEM_POTION, 1));
    EXPECT(!CheckBagHasItem(ITEM_POTION, 2));
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 0);
    Rogue_ClearRunStartConfigOverride();
    ClearBag();
}

TEST("Ascension: Trial reward claims remain one-time after improved clears")
{
    struct RogueAdventureConfig config;
    u32 money;
    u16 quest = QUEST_ID_WATER_MASTER;
    ResetAscensionTest();
    RogueQuest_OnNewGame();
    FlagSet(FLAG_SYS_TRIALS_UNLOCKED);
    RogueQuest_TryUnlockQuest(quest);
    ClearBag();
    Rogue_CopyAdventureConfig(&config);
    config.ascension = 5;
    Rogue_SetRunStartConfigOverride(&config);
    Debug_RogueQuest_CompleteQuest(quest);
    EXPECT(RogueQuest_HasPendingRewards(quest));
    EXPECT(RogueQuest_TryCollectRewards(quest));
    EXPECT(RogueQuest_IsRewardClaimed(quest));
    money = GetMoney(&gSaveBlock1Ptr->money);
    EXPECT(!RogueQuest_TryCollectRewards(quest));
    config.ascension = 17;
    config.battleFormat = BATTLE_FORMAT_DOUBLES;
    Rogue_SetRunStartConfigOverride(&config);
    Debug_RogueQuest_CompleteQuest(quest);
    EXPECT_EQ(RogueQuest_GetBestAscension(quest, BATTLE_FORMAT_SINGLES), 5);
    EXPECT_EQ(RogueQuest_GetBestAscension(quest, BATTLE_FORMAT_DOUBLES), 17);
    EXPECT_EQ(RogueQuest_GetBestAscension(quest, BATTLE_FORMAT_MIXED), ASCENSION_NONE);
    EXPECT(!RogueQuest_HasPendingRewards(quest));
    EXPECT(!RogueQuest_TryCollectRewards(quest));
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), money);
    Rogue_ClearRunStartConfigOverride();
    ClearBag();
}

TEST("Ascension: rules fit the GBA text window and describe every level")
{
    struct RogueAdventureConfig config;
    u8 a, row, mode;
    u8 text[160];
    ResetAscensionTest();
    Rogue_CopyAdventureConfig(&config);
    for (a = 0; a <= ASCENSION_MAX; ++a)
    {
        config.ascension = a;
        EXPECT_LE(GetStringWidth(FONT_SMALL_NARROW, RogueAscension_GetAddition(a), 0), 224);
        for (mode = 0; mode < 3; ++mode)
        {
            config.mode = mode == 1 ? ROGUE_GAME_MODE_GAUNTLET : ROGUE_GAME_MODE_STANDARD;
            config.trialFreshStart = mode == 2;
            for (row = 0; row < 12; ++row)
            {
                EXPECT(RogueAscension_GetConfigRuleLine(&config, row, text));
                EXPECT_LE(GetStringWidth(FONT_SMALL_NARROW, text, 0), 224);
            }
        }
    }
    for (row = 0; RogueAscension_GetBaseLine(row, text); ++row)
        EXPECT_LE(GetStringWidth(FONT_SMALL_NARROW, text, 0), 224);
}

TEST("Ascension: boss ace identity survives profile upgrades")
{
    struct RogueAdventureConfig config;
    u16 boss, ace = SPECIES_NONE;
    u8 a, count;
    ResetAscensionTest();
    RogueTest_UseContentTrainers(TRUE);
    Rogue_CopyAdventureConfig(&config);
    for (boss = 1; boss < gRogueTrainerCount; ++boss)
        if (Rogue_IsBossTrainer(boss)) break;
    Rogue_SetCurrentDifficulty(0);
    for (a = 0; a <= 4; ++a)
    {
        config.ascension = a;
        Rogue_SetRunStartConfigOverride(&config);
        ZeroEnemyPartyMons();
        SeedRogueRng(1234);
        SeedRng(4321);
        count = Rogue_CreateTrainerParty(boss, gEnemyParty, PARTY_SIZE, TRUE);
        if (a == 0) ace = GetMonData(&gEnemyParty[count - 1], MON_DATA_SPECIES);
        EXPECT_EQ(GetMonData(&gEnemyParty[count - 1], MON_DATA_SPECIES), ace);
    }
    RogueTest_UseContentTrainers(FALSE);
    Rogue_ClearRunStartConfigOverride();
}

TEST("Ascension: missing profiles retain a legal basic Pokemon with no EVs")
{
    struct RogueAdventureConfig config;
    u16 boss, species;
    u16 moves[MAX_MON_MOVES];
    u8 i;
    ResetAscensionTest();
    Rogue_CopyAdventureConfig(&config);
    config.ascension = 20;
    Rogue_SetRunStartConfigOverride(&config);
    for (boss = 1; boss < gRogueTrainerCount; ++boss)
        if (Rogue_IsMiniBossTrainer(boss)) break;
    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; ++species)
        if (gRoguePokemonProfiles[species].competitiveSetCount == 0 && gSpeciesInfo[species].baseHP != 0) break;
    EXPECT_LT(species, NUM_SPECIES);
    CreateMon(&gEnemyParty[0], species, 50, 0, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
    SetNature(&gEnemyParty[0], NATURE_HARDY);
    for (i = 0; i < MAX_MON_MOVES; ++i) moves[i] = GetMonData(&gEnemyParty[0], MON_DATA_MOVE1 + i);
    RogueTest_ApplyAscensionParty(boss, gEnemyParty, 1);
    EXPECT_EQ(GetNature(&gEnemyParty[0]), NATURE_HARDY);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM), ITEM_NONE);
    for (i = 0; i < MAX_MON_MOVES; ++i) EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_MOVE1 + i), moves[i]);
    for (i = 0; i < NUM_STATS; ++i)
    {
        EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_HP_IV + i), 31);
        EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_HP_EV + i), 0);
    }
    Rogue_ClearRunStartConfigOverride();
}

TEST("Ascension: hub payouts use the completed run snapshot")
{
    ResetAscensionTest();
    gRogueSaveBlock->adventureConfig.ascension = 0;
    gRogueSaveBlock->activeAdventureConfig.ascension = 20;
    gRogueRun.enteredRoomCounter = 11;
    gRogueRun.victoryLapTotalWins = 2;
    SetMoney(&gSaveBlock1Ptr->money, 0);
    EXPECT_EQ(Rogue_PostRunRewardMoney(), 12 * 350);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 12 * 350);
}

TEST("Ascension: Gauntlet full teams retain A0 profile gates")
{
    struct RogueAdventureConfig config;
    u16 boss;
    u8 i, count;
    ResetAscensionTest();
    RogueTest_UseContentTrainers(TRUE);
    Rogue_CopyAdventureConfig(&config);
    config.mode = ROGUE_GAME_MODE_GAUNTLET;
    config.ascension = 0;
    Rogue_SetRunStartConfigOverride(&config);
    for (boss = 1; boss < gRogueTrainerCount; ++boss)
        if (Rogue_IsBossTrainer(boss)) break;
    Rogue_SetCurrentDifficulty(0);
    count = Rogue_CreateTrainerParty(boss, gEnemyParty, PARTY_SIZE, TRUE);
    EXPECT_EQ(count, 6);
    for (i = 0; i < count; ++i)
    {
        EXPECT_EQ(GetNature(&gEnemyParty[i]), NATURE_HARDY);
        EXPECT_EQ(GetMonData(&gEnemyParty[i], MON_DATA_LEVEL), 100);
        EXPECT_EQ(GetMonData(&gEnemyParty[i], MON_DATA_ATK_EV), 0);
    }
    RogueTest_UseContentTrainers(FALSE);
    Rogue_ClearRunStartConfigOverride();
}

TEST("Ascension: service parties are excluded from adventure scaling")
{
    struct RogueAdventureConfig config;
    struct Pokemon before;
    u16 service;
    u8 ev = 120, found = 0;
    ResetAscensionTest();
    RogueTest_UseContentTrainers(TRUE);
    Rogue_CopyAdventureConfig(&config);
    config.ascension = 20;
    Rogue_SetRunStartConfigOverride(&config);
    CreateMon(&before, SPECIES_VENUSAUR, 50, 7, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
    SetMonData(&before, MON_DATA_SPATK_EV, &ev);
    for (service = 0; service < gRogueTrainerCount; ++service)
        if (Rogue_IsExpTrainer(service) || Rogue_IsBattleSimTrainer(service))
        {
            ++found;
            gEnemyParty[0] = before;
            RogueTest_ApplyAscensionParty(service, gEnemyParty, 1);
            EXPECT_EQ(memcmp(&before, &gEnemyParty[0], sizeof(before)), 0);
        }
    EXPECT_GE(found, 2);
    RogueTest_UseContentTrainers(FALSE);
    Rogue_ClearRunStartConfigOverride();
}

TEST("Ascension: failed reward delivery remains pending and retryable")
{
    u16 item;
    u16 quest = QUEST_ID_WATER_MASTER;
    u32 money;
    ResetAscensionTest();
    RogueQuest_OnNewGame();
    FlagSet(FLAG_SYS_TRIALS_UNLOCKED);
    RogueQuest_TryUnlockQuest(quest);
    Debug_RogueQuest_CompleteQuest(quest);
    ClearBag();
    for (item = 1; item < ITEMS_COUNT; ++item) AddBagItem(item, 999);
    money = GetMoney(&gSaveBlock1Ptr->money);
    EXPECT(!RogueQuest_TryCollectRewards(quest));
    EXPECT(RogueQuest_HasPendingRewards(quest));
    EXPECT(!RogueQuest_IsRewardClaimed(quest));
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), money);
    ClearBag();
    EXPECT(RogueQuest_TryCollectRewards(quest));
    EXPECT(RogueQuest_IsRewardClaimed(quest));
    EXPECT(!RogueQuest_HasPendingRewards(quest));
    ClearBag();
}

TEST("Ascension: Trial records require successful final conditions")
{
    u16 quest = QUEST_ID_ORRE_STYLE;
    ResetAscensionTest();
    RogueQuest_OnNewGame();
    FlagSet(FLAG_SYS_TRIALS_UNLOCKED);
    RogueQuest_TryUnlockQuest(quest);
    gRogueSaveBlock->activeAdventureConfig = gRogueSaveBlock->adventureConfig;
    gRogueSaveBlock->activeAdventureConfig.ascension = 10;
    gRogueSaveBlock->activeAdventureConfig.battleFormat = BATTLE_FORMAT_DOUBLES;
    gRogueRun.trialState.trialId = ROGUE_TRIAL_ORRE_STYLE;
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    ZeroPlayerPartyMons();
    CreateMon(&gPlayerParty[0], SPECIES_UMBREON, 100, 31, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CalculatePlayerPartyCount();
    RogueQuest_SetStateFlag(quest, QUEST_STATE_ACTIVE, TRUE);
    RogueAscension_RecordWin();
    RogueQuest_OnTrigger(QUEST_TRIGGER_ENTER_HALL_OF_FAME);
    EXPECT_EQ(gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_ORRE_STYLE][BATTLE_FORMAT_DOUBLES], ASCENSION_NONE);

    CreateMon(&gPlayerParty[1], SPECIES_ESPEON, 100, 31, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CalculatePlayerPartyCount();
    RogueQuest_SetStateFlag(quest, QUEST_STATE_ACTIVE, TRUE);
    RogueQuest_OnTrigger(QUEST_TRIGGER_ENTER_HALL_OF_FAME);
    EXPECT_EQ(gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_ORRE_STYLE][BATTLE_FORMAT_DOUBLES], 10);
    EXPECT_EQ(gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_ORRE_STYLE][BATTLE_FORMAT_SINGLES], ASCENSION_NONE);

    // A later failed attempt must not improve an already completed Trial.
    gRogueSaveBlock->activeAdventureConfig.ascension = 17;
    ZeroMonData(&gPlayerParty[1]);
    CalculatePlayerPartyCount();
    RogueQuest_SetStateFlag(quest, QUEST_STATE_ACTIVE, TRUE);
    RogueQuest_OnTrigger(QUEST_TRIGGER_ENTER_HALL_OF_FAME);
    EXPECT_EQ(gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_ORRE_STYLE][BATTLE_FORMAT_DOUBLES], 10);
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gRogueRun.trialState.trialId = ROGUE_TRIAL_NONE;
    ZeroPlayerPartyMons();
}
