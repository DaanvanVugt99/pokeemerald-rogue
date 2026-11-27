#include "global.h"
#include "event_data.h"
#include "item.h"
#include "money.h"
#include "pokemon.h"
#include "party_menu.h"
#include "constants/abilities.h"
#include "constants/weather.h"
#include "random.h"
#include "rogue.h"
#include "rogue_ascension.h"
#include "rogue_gifts.h"
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

TEST("Ascension: boss IV and party size milestones")
{
    for (u8 level = 0; level <= ASCENSION_MAX; ++level)
    {
        EXPECT_EQ(RogueAscension_PartySize(level, 3, 6, TRUE), level >= 12 ? 6 : level >= 5 ? 4 : 3);
        EXPECT_EQ(RogueAscension_PartySize(level, 3, 6, FALSE), 3);
        EXPECT_EQ(RogueAscension_PartySize(level, 6, 3, TRUE), 3);
        EXPECT_EQ(RogueAscension_CalculateIV(level, 0, FALSE), 0);
        EXPECT_EQ(RogueAscension_CalculateIV(level, 0, TRUE), level >= 9 ? 31 : 0);
        EXPECT_EQ(RogueAscension_CalculateIV(level, 8, TRUE), level >= 9 ? 31 : 5);
        EXPECT_EQ(RogueAscension_CalculateIV(level, 13, TRUE), level >= 9 ? 31 : 15);
    }
}

TEST("Ascension: competitive sets apply together to boss aces then whole boss teams")
{
    struct Pokemon original, mon;
    struct RogueAdventureConfig config;
    const struct RoguePokemonCompetitiveSet *profile;
    u16 boss, i;
    u8 a, role;
    static const u8 setLevels[] = {4, 7, ASCENSION_NONE, ASCENSION_NONE};
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
    SetNature(&original, NATURE_ADAMANT);
    for (role = 0; role < 4; ++role)
    for (a = 0; a <= ASCENSION_MAX; ++a)
    {
        config.ascension = a;
        Rogue_SetRunStartConfigOverride(&config);
        mon = original;
        RogueTest_ApplyAscensionProfile(boss, &mon, profile, role < 2, role == 0 || role == 2);
        EXPECT_EQ(GetNature(&mon), a >= setLevels[role] ? profile->nature : NATURE_ADAMANT);
        EXPECT_EQ(GetMonUniqueAbility(&mon), GetMonUniqueAbility(&original));
        if (a < setLevels[role])
        {
            EXPECT_EQ(GetMonAbility(&mon), GetMonAbility(&original));
            for (i = 0; i < MAX_MON_MOVES; ++i)
                EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1 + i), GetMonData(&original, MON_DATA_MOVE1 + i));
        }
        else
            EXPECT_EQ(GetMonAbility(&mon), profile->ability);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), a >= setLevels[role] ? profile->heldItem : ITEM_NONE);
    }
    Rogue_ClearRunStartConfigOverride();
}

TEST("Ascension: generated A0 and A1 parties retain their natural natures")
{
    struct RogueAdventureConfig config;
    u16 trainers[2] = {0};
    u16 t;
    u8 role, a, i, count;
    ResetAscensionTest();
    RogueTest_UseContentTrainers(TRUE);
    Rogue_CopyAdventureConfig(&config);
    Rogue_SetCurrentDifficulty(0);
    for (t = 1; t < gRogueTrainerCount; ++t)
    {
        if (!trainers[0] && Rogue_IsBossTrainer(t))
            trainers[0] = t;
        if (!trainers[1] && !Rogue_IsKeyTrainer(t)
         && !(Rogue_GetTrainer(t)->trainerFlags & TRAINER_FLAG_CLASS_SPECIAL))
            trainers[1] = t;
    }
    for (role = 0; role < ARRAY_COUNT(trainers); ++role)
    {
        bool8 sawNonNeutral = FALSE;
        EXPECT_NE(trainers[role], 0);
        for (a = 0; a <= 1; ++a)
        {
            config.ascension = a;
            Rogue_SetRunStartConfigOverride(&config);
            ZeroEnemyPartyMons();
            SeedRogueRng(1234);
            SeedRng(4321);
            count = Rogue_CreateTrainerParty(trainers[role], gEnemyParty, PARTY_SIZE, TRUE);
            EXPECT_GT(count, 0);
            for (i = 0; i < count; ++i)
            {
                u8 nature = GetNature(&gEnemyParty[i]);
                sawNonNeutral |= nature % 6 != 0;
                EXPECT_EQ(GetMonData(&gEnemyParty[i], MON_DATA_ATK_IV), 0);
            }
        }
        EXPECT(sawNonNeutral);
    }
    Rogue_ClearRunStartConfigOverride();
    RogueTest_UseContentTrainers(FALSE);
}

TEST("Ascension: broader teams start at A3 and gym fields start at A6")
{
    struct RogueAdventureConfig config;
    u16 boss;
    ResetAscensionTest();
    RogueTest_UseContentTrainers(TRUE);
    Rogue_CopyAdventureConfig(&config);
    Rogue_SetCurrentDifficulty(ROGUE_ELITE_START_DIFFICULTY);
    for (boss = 1; boss < gRogueTrainerCount; ++boss)
        if (Rogue_IsBossTrainer(boss) && Rogue_GetTrainerWeather(boss) != WEATHER_NONE) break;
    EXPECT_LT(boss, gRogueTrainerCount);
    u8 preferred = Rogue_GetTrainerWeather(boss);
    Rogue_SetCurrentDifficulty(0);
    for (u8 level = 0; level <= ASCENSION_MAX; ++level)
    {
        config.ascension = level;
        Rogue_SetRunStartConfigOverride(&config);
        EXPECT_EQ(Rogue_GetConfigToggle(CONFIG_TOGGLE_DIVERSE_TRAINERS), level >= 3);
        EXPECT_EQ(Rogue_GetTrainerWeather(boss), level >= 6 ? preferred : WEATHER_NONE);
    }
    Rogue_ClearRunStartConfigOverride();
    RogueTest_UseContentTrainers(FALSE);
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
    gRogueSaveBlock->activeAdventureConfig.ascension = 12;
    gRogueSaveBlock->activeAdventureConfig.battleFormat = BATTLE_FORMAT_MIXED;
    gRogueSaveBlock->bestAscension[BATTLE_FORMAT_DOUBLES] = 11;
    gRogueSaveBlock->selectedAscension[BATTLE_FORMAT_DOUBLES] = 12;
    gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_INSANE_MODE][BATTLE_FORMAT_MIXED] = 8;
    gRogueSaveBlock->ascensionEligible = TRUE;
    gRogueSaveBlock->ascensionRecorded = TRUE;
    gRogueSaveBlock->adventureReplay[0].rulesVersion = ASCENSION_RULES_VERSION;
    gRogueSaveBlock->adventureReplay[0].adventureConfig = gRogueSaveBlock->activeAdventureConfig;
    RogueSave_FormatForWriting();
    RogueSave_FormatForReading();
    EXPECT_EQ(gRogueSaveBlock->activeAdventureConfig.ascension, 12);
    EXPECT_EQ(gRogueSaveBlock->adventureConfig.ascension, 0);
    EXPECT_EQ(gRogueSaveBlock->bestAscension[BATTLE_FORMAT_DOUBLES], 11);
    EXPECT_EQ(gRogueSaveBlock->selectedAscension[BATTLE_FORMAT_DOUBLES], 12);
    EXPECT_EQ(gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_INSANE_MODE][BATTLE_FORMAT_MIXED], 8);
    EXPECT(gRogueSaveBlock->ascensionEligible && gRogueSaveBlock->ascensionRecorded);
    EXPECT_EQ(gRogueSaveBlock->adventureReplay[0].adventureConfig.ascension, 12);
    EXPECT_EQ(gRogueSaveBlock->adventureReplay[0].rulesVersion, ASCENSION_RULES_VERSION);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    EXPECT_EQ(Rogue_GetAscension(), 12);
    Rogue_SetAscension(0);
    EXPECT_EQ(Rogue_GetAscension(), 12);
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}

static void CheckGeneratedBossParty(u8 kind, u8 stage)
{
    static const u8 levels[] = {0, 1, 2, 5, 8, 11, 12};
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
                EXPECT_EQ(count, levels[a] == 12 ? 6 : levels[a] >= 5 ? min(6, baseline + 1) : baseline);
                u8 legends = 0, uniques = 0;
                for (i = 0; i < count; ++i)
                {
                    legends += RoguePokedex_IsSpeciesLegendary(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES));
                    uniques += RogueGift_GetCustomMonId(&gEnemyParty[i]) != CUSTOM_MON_NONE;
                    EXPECT_NE(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES), SPECIES_NONE);
                    EXPECT_NE(GetMonData(&gEnemyParty[i], MON_DATA_MOVE1), MOVE_NONE);
                    for (stat = 0; stat < NUM_STATS; ++stat)
                    {
                        EXPECT_EQ(GetMonData(&gEnemyParty[i], MON_DATA_HP_IV + stat), RogueAscension_CalculateIV(levels[a], stage, TRUE));
                        EXPECT_EQ(GetMonData(&gEnemyParty[i], MON_DATA_HP_EV + stat), 0);
                    }

                    if (levels[a] <= 1)
                    {
                        u16 item = GetMonData(&gEnemyParty[i], MON_DATA_HELD_ITEM);
                        if (kind == 0 && stage < 8 && levels[a] == 1 && i == count - 1)
                            EXPECT_EQ(item, ITEM_SITRUS_BERRY);
                        else EXPECT(item == ITEM_NONE || IS_MEGA_STONE_ITEM(item) || item == ITEM_RED_ORB || item == ITEM_BLUE_ORB || (item >= ITEM_NORMALIUM_Z && item <= ITEM_ULTRANECROZIUM_Z));
                    }
                }
                if (kind == 0 && stage == 8) EXPECT_EQ(legends, levels[a] >= 8);
                EXPECT_EQ(uniques, (kind == 2 && levels[a] >= 2) || (kind == 0 && levels[a] >= 11));
            }
        }
    }
    Rogue_ClearRunStartConfigOverride();
    RogueTest_UseContentTrainers(FALSE);
}

TEST("Ascension: rival keeps the same Unique partner and coverage through the championship")
{
    struct RogueAdventureConfig config;
    u16 rival;
    u32 firstGift = 0;
    ResetAscensionTest();
    RogueTest_UseContentTrainers(TRUE);
    Rogue_CopyAdventureConfig(&config);
    config.ascension = ASCENSION_BOSS_UNIQUE;
    Rogue_SetRunStartConfigOverride(&config);
    for (rival = 1; rival < gRogueTrainerCount; ++rival)
        if (Rogue_IsRivalTrainer(rival)) break;
    EXPECT_LT(rival, gRogueTrainerCount);
    gRogueRun.rivalTrainerNum = rival;
    gRogueRun.baseSeed = 1741;
    for (u8 battle = 0; battle < 2; ++battle)
    {
        u8 count, uniques = 0;
        Rogue_SetCurrentDifficulty(battle ? ROGUE_FINAL_CHAMP_DIFFICULTY : 0);
        ZeroEnemyPartyMons();
        count = Rogue_CreateTrainerParty(rival, gEnemyParty, PARTY_SIZE, TRUE);
        for (u8 i = 0; i < count; ++i)
        {
            u32 gift = RogueGift_GetCustomMonId(&gEnemyParty[i]);
            if (!gift) continue;
            ++uniques;
            if (battle == 0) firstGift = gift;
            EXPECT_EQ(gift, firstGift);
            if (RogueGift_GetCustomMonUniqueAbility(gift) != ABILITY_NONE)
                EXPECT_EQ(GetMonUniqueAbility(&gEnemyParty[i]), RogueGift_GetCustomMonUniqueAbility(gift));
            for (u8 j = 0; j < RogueGift_GetCustomMonMoveCount(gift); ++j)
                EXPECT(MonKnowsMove(&gEnemyParty[i], RogueGift_GetCustomMonMove(gift, j)));
        }
        EXPECT_EQ(uniques, 1);
    }
    Rogue_ClearRunStartConfigOverride();
    RogueTest_UseContentTrainers(FALSE);
}

TEST("Ascension: boss Uniques vary party slot, rarity and typing across seeds")
{
    struct RogueAdventureConfig config;
    u16 boss;
    const u8 allRarities = (1 << UNIQUE_RARITY_COMMON) | (1 << UNIQUE_RARITY_RARE)
                        | (1 << UNIQUE_RARITY_EPIC) | (1 << UNIQUE_RARITY_LEGENDARY);
    u8 slots = 0, rarities = 0;
    bool8 sawTyped = FALSE;
    ResetAscensionTest();
    RogueTest_UseContentTrainers(TRUE);
    Rogue_CopyAdventureConfig(&config);
    config.ascension = ASCENSION_BOSS_UNIQUE;
    Rogue_SetRunStartConfigOverride(&config);
    Rogue_SetCurrentDifficulty(0);
    for (boss = 1; boss < gRogueTrainerCount; ++boss)
        if (Rogue_IsBossTrainer(boss)) break;
    EXPECT_LT(boss, gRogueTrainerCount);
    for (u16 seed = 0; seed < 64; ++seed)
    {
        gRogueRun.baseSeed = seed;
        ZeroEnemyPartyMons();
        u8 count = Rogue_CreateTrainerParty(boss, gEnemyParty, PARTY_SIZE, TRUE);
        for (u8 i = 0; i < count; ++i)
        {
            u32 gift = RogueGift_GetCustomMonId(&gEnemyParty[i]);
            if (!gift) continue;
            slots |= 1 << i;
            rarities |= 1 << RogueGift_GetCustomMonRarity(gift);
            sawTyped |= RogueGift_GetCustomMonType(gift, 0) != TYPE_NONE
                     || RogueGift_GetCustomMonType(gift, 1) != TYPE_NONE;
        }
        if ((slots & (slots - 1)) && sawTyped && rarities == allRarities) break;
    }
    EXPECT(slots & (slots - 1));
    EXPECT(sawTyped);
    EXPECT_EQ(rarities, allRarities);
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

TEST("Ascension: A10 supplies retain preparation and override imported inventory")
{
    struct RogueAdventureConfig config;
    u16 heldItem = ITEM_LEFTOVERS;
    u8 ev = 252;
    ResetAscensionTest();
    Rogue_CopyAdventureConfig(&config);
    config.ascension = 10;
    ClearBag();
    EXPECT(AddBagItem(ITEM_ULTRA_BALL, 40));
    EXPECT(AddBagItem(ITEM_POTION, 99));
    EXPECT(AddBagItem(ITEM_C_GEAR, 1));
    EXPECT(AddBagItem(ITEM_FLINCH_CHARM, 1));
    RecalcCharmCurseValues();
    config.ascension = 9;
    EXPECT_EQ(RogueAscension_GetEligibility(&config, RUN_START_SOURCE_NORMAL), ASCENSION_INELIGIBLE_CHARMS);
    config.ascension = 10;
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
    config.ascension = 11;
    config.battleFormat = BATTLE_FORMAT_DOUBLES;
    Rogue_SetRunStartConfigOverride(&config);
    Debug_RogueQuest_CompleteQuest(quest);
    EXPECT_EQ(RogueQuest_GetBestAscension(quest, BATTLE_FORMAT_SINGLES), 5);
    EXPECT_EQ(RogueQuest_GetBestAscension(quest, BATTLE_FORMAT_DOUBLES), 11);
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
        if (a == 0 || a == ASCENSION_DIVERSE_TRAINERS) ace = GetMonData(&gEnemyParty[count - 1], MON_DATA_SPECIES);
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
    config.ascension = 12;
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
    gRogueSaveBlock->activeAdventureConfig.ascension = 12;
    gRogueRun.enteredRoomCounter = 11;
    gRogueRun.victoryLapTotalWins = 2;
    SetMoney(&gSaveBlock1Ptr->money, 0);
    EXPECT_EQ(Rogue_PostRunRewardMoney(), 12 * 310);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 12 * 310);
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
    config.ascension = 12;
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
    gRogueSaveBlock->activeAdventureConfig.ascension = 11;
    ZeroMonData(&gPlayerParty[1]);
    CalculatePlayerPartyCount();
    RogueQuest_SetStateFlag(quest, QUEST_STATE_ACTIVE, TRUE);
    RogueQuest_OnTrigger(QUEST_TRIGGER_ENTER_HALL_OF_FAME);
    EXPECT_EQ(gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_ORRE_STYLE][BATTLE_FORMAT_DOUBLES], 10);
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gRogueRun.trialState.trialId = ROGUE_TRIAL_NONE;
    ZeroPlayerPartyMons();
}
