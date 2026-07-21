#include "global.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "constants/pokemon.h"
#include "constants/rogue.h"
#include "constants/rogue_pokedex.h"
#include "constants/species.h"
#include "event_data.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_settings.h"
#include "rogue_query.h"
#include "rogue_trials.h"
#include "test/test.h"
#include "rogue_trainers.h"

static const u8 sTrainerConfigToggles[] =
{
    CONFIG_TOGGLE_TRAINER_ROGUE,
    CONFIG_TOGGLE_TRAINER_KANTO,
    CONFIG_TOGGLE_TRAINER_JOHTO,
    CONFIG_TOGGLE_TRAINER_HOENN,
    CONFIG_TOGGLE_TRAINER_SINNOH,
    CONFIG_TOGGLE_TRAINER_UNOVA,
    CONFIG_TOGGLE_TRAINER_KALOS,
    CONFIG_TOGGLE_TRAINER_ALOLA,
    CONFIG_TOGGLE_TRAINER_GALAR,
    CONFIG_TOGGLE_TRAINER_PALDEA,
};

static void SaveTrainerConfigToggles(bool8 *outValues)
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sTrainerConfigToggles); ++i)
        outValues[i] = Rogue_GetConfigToggle(sTrainerConfigToggles[i]);
}

static void SetBenchmarkTrainerConfig(void)
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sTrainerConfigToggles); ++i)
        Rogue_SetConfigToggle(sTrainerConfigToggles[i], FALSE);

    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, TRUE);
}

static void RestoreTrainerConfigToggles(const bool8 *values)
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sTrainerConfigToggles); ++i)
        Rogue_SetConfigToggle(sTrainerConfigToggles[i], values[i]);
}

static void SetMiniBossRewardTestParty(const u16 *species, u8 count)
{
    u8 i;

    ZeroEnemyPartyMons();
    for(i = 0; i < count; ++i)
        CreateMon(&gEnemyParty[i], species[i], 50, 0, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
    CalculateEnemyPartyCount();
}

TEST("Rogue trainer items: Black Sludge converts to Leftovers with tera")
{
#if defined(ROGUE_EXPANSION)
    EXPECT_EQ(RogueDebug_AdjustHeldItemForTera(ITEM_BLACK_SLUDGE, TRUE), ITEM_LEFTOVERS);
    EXPECT_EQ(RogueDebug_AdjustHeldItemForTera(ITEM_BLACK_SLUDGE, FALSE), ITEM_BLACK_SLUDGE);
    EXPECT_EQ(RogueDebug_AdjustHeldItemForTera(ITEM_LEFTOVERS, TRUE), ITEM_LEFTOVERS);
#else
    ASSUME(FALSE);
#endif
}

TEST("Frontier Brains schedule deterministically without duplicates in each mode window")
{
    u16 trainerNums[ADVPATH_FRONTIER_BRAIN_COUNT];
    u16 expectedTrainerNums[ADVPATH_FRONTIER_BRAIN_COUNT];
    u8 difficulties[ADVPATH_FRONTIER_BRAIN_COUNT];
    u8 expectedDifficulties[ADVPATH_FRONTIER_BRAIN_COUNT];
    u8 originalGameMode = Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM);
    u8 originalTrialId = gRogueRun.trialState.trialId;
    u16 originalBaseSeed = gRogueRun.baseSeed;
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE rogueRngBefore;
    u16 seed;
    u8 trialId;
    u8 i;
    u8 j;

    SeedRogueRng(9876);
    rogueRngBefore = gRngRogueValue;
    gRogueRun.trialState.trialId = ROGUE_TRIAL_NONE;
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_STANDARD);
    for(seed = 0; seed < 32; ++seed)
    {
        gRogueRun.baseSeed = seed;
        Rogue_GetFrontierBrainSchedule(trainerNums, difficulties);

        for(i = 0; i < ADVPATH_FRONTIER_BRAIN_COUNT; ++i)
        {
            EXPECT_NE(trainerNums[i], TRAINER_NONE);
            EXPECT(gRogueTrainers[trainerNums[i]].trainerFlags & TRAINER_FLAG_CLASS_MINIBOSS);
            EXPECT_GE(difficulties[i], 2 + 4 * i);
            EXPECT_LE(difficulties[i], 4 + 4 * i);
            EXPECT_EQ(Rogue_GetScheduledFrontierBrainTrainer(difficulties[i]), trainerNums[i]);

            for(j = 0; j < i; ++j)
                EXPECT_NE(trainerNums[i], trainerNums[j]);
        }
    }

    gRogueRun.baseSeed = 54321;
    Rogue_GetFrontierBrainSchedule(expectedTrainerNums, expectedDifficulties);
    Rogue_GetFrontierBrainSchedule(trainerNums, difficulties);
    EXPECT_EQ(memcmp(expectedTrainerNums, trainerNums, sizeof(expectedTrainerNums)), 0);
    EXPECT_EQ(memcmp(expectedDifficulties, difficulties, sizeof(expectedDifficulties)), 0);

    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_SLOW_PATH);
    for(seed = 0; seed < 32; ++seed)
    {
        gRogueRun.baseSeed = seed;
        Rogue_GetFrontierBrainSchedule(trainerNums, difficulties);

        for(i = 0; i < ADVPATH_FRONTIER_BRAIN_COUNT; ++i)
        {
            EXPECT_NE(trainerNums[i], TRAINER_NONE);
            EXPECT_GE(difficulties[i], 1 + 4 * i);
            EXPECT_LE(difficulties[i], 4 + 4 * i);

            for(j = 0; j < i; ++j)
                EXPECT_NE(trainerNums[i], trainerNums[j]);
        }
    }

    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_GAUNTLET);
    gRogueRun.baseSeed = 12345;
    Rogue_GetFrontierBrainSchedule(trainerNums, difficulties);
    for(i = 0; i < ADVPATH_FRONTIER_BRAIN_COUNT; ++i)
    {
        EXPECT_EQ(trainerNums[i], TRAINER_NONE);
        EXPECT_EQ(difficulties[i], ROGUE_MAX_BOSS_COUNT);
    }

    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_STANDARD);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    for(trialId = ROGUE_TRIAL_NONE + 1; trialId < ROGUE_TRIAL_COUNT; ++trialId)
    {
        gRogueRun.trialState.trialId = trialId;
        Rogue_GetFrontierBrainSchedule(trainerNums, difficulties);
        for(i = 0; i < ADVPATH_FRONTIER_BRAIN_COUNT; ++i)
        {
            if(trialId == ROGUE_TRIAL_LITTLE_CUP)
            {
                EXPECT_EQ(trainerNums[i], TRAINER_NONE);
                EXPECT_EQ(difficulties[i], ROGUE_MAX_BOSS_COUNT);
            }
            else
            {
                EXPECT_NE(trainerNums[i], TRAINER_NONE);
                EXPECT_NE(difficulties[i], ROGUE_MAX_BOSS_COUNT);
            }
        }
    }
    EXPECT_EQ(memcmp(&rogueRngBefore, &gRngRogueValue, sizeof(rogueRngBefore)), 0);

    gRogueRun.baseSeed = originalBaseSeed;
    gRogueRun.trialState.trialId = originalTrialId;
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, originalGameMode);
    gRngRogueValue = originalRogueRng;
}

TEST("Frontier Brain rewards only offer Trial-legal team members")
{
    static const u16 sTwoLegal[] = {SPECIES_PIKACHU, SPECIES_MAGIKARP, SPECIES_SQUIRTLE};
    static const u16 sOneLegal[] = {SPECIES_PIKACHU, SPECIES_MAGIKARP};
    static const u16 sNoneLegal[] = {SPECIES_PIKACHU, SPECIES_RATTATA};
    struct RogueAdvPathRoom originalRoom = gRogueAdvPath.rooms[0];
    struct Pokemon originalEnemyParty[PARTY_SIZE];
    u8 originalRoomId = gRogueRun.adventureRoomId;
    u8 originalTrialId = gRogueRun.trialState.trialId;
    u8 originalDexVariant = RoguePokedex_GetDexVariant();
    u16 originalTrainer = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA);
    u16 originalSpeciesA = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1);
    u16 originalSpeciesB = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2);
    u16 originalResult = gSpecialVar_Result;
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u16 trainerNum;

    memcpy(originalEnemyParty, gEnemyParty, sizeof(originalEnemyParty));
    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        if(gRogueTrainers[trainerNum].classFlags & CLASS_FLAG_MINIBOSS_ANABEL)
        {
            VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, trainerNum);
            break;
        }
    }
    EXPECT_LT(trainerNum, gRogueTrainerCount);

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_MAX);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    gRogueRun.adventureRoomId = 0;
    gRogueAdvPath.rooms[0].rngSeed = 24680;
    gRogueRun.trialState.trialId = ROGUE_TRIAL_TYPE_WATER;

    SetMiniBossRewardTestParty(sTwoLegal, ARRAY_COUNT(sTwoLegal));
    Rogue_SelectMiniBossRewardMons();
    EXPECT_EQ(gSpecialVar_Result, MINIBOSS_REWARD_MODE_DOUBLE);
    EXPECT(RogueTrial_IsSpeciesLegal(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1), 0));
    EXPECT(RogueTrial_IsSpeciesLegal(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2), 0));

    SetMiniBossRewardTestParty(sOneLegal, ARRAY_COUNT(sOneLegal));
    Rogue_SelectMiniBossRewardMons();
    EXPECT_EQ(gSpecialVar_Result, MINIBOSS_REWARD_MODE_SINGLE);
    EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1), SPECIES_MAGIKARP);

    SetMiniBossRewardTestParty(sNoneLegal, ARRAY_COUNT(sNoneLegal));
    Rogue_SelectMiniBossRewardMons();
    EXPECT_EQ(gSpecialVar_Result, MINIBOSS_REWARD_MODE_NO_LEGAL);

    gRogueAdvPath.rooms[0] = originalRoom;
    gRogueRun.adventureRoomId = originalRoomId;
    gRogueRun.trialState.trialId = originalTrialId;
    RoguePokedex_SetDexVariant(originalDexVariant);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, originalTrainer);
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, originalSpeciesA);
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, originalSpeciesB);
    gSpecialVar_Result = originalResult;
    memcpy(gEnemyParty, originalEnemyParty, sizeof(originalEnemyParty));
    CalculateEnemyPartyCount();
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}

TEST("Frontier Brains use competitive movesets on the first Average path")
{
    struct Pokemon originalEnemyParty[PARTY_SIZE];
    u8 originalTrainerDifficulty = Rogue_GetConfigRange(CONFIG_RANGE_TRAINER);
    u8 originalDifficulty = Rogue_GetCurrentDifficulty();
    u8 originalLevelOffset = gRogueRun.currentLevelOffset;
    u8 originalDexVariant = RoguePokedex_GetDexVariant();
    u8 originalTrialId = gRogueRun.trialState.trialId;
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE originalRng = gRngValue;
    u16 trainerNum;
    u8 partySize;
    u8 i;

    memcpy(originalEnemyParty, gEnemyParty, sizeof(originalEnemyParty));
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_AVERAGE);
    Rogue_SetCurrentDifficulty(0);
    gRogueRun.currentLevelOffset = 0;
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_MAX);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    gRogueRun.trialState.trialId = ROGUE_TRIAL_NONE;
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);

    trainerNum = TRAINER_NONE;
    for(i = 0; i < gRogueTrainerCount; ++i)
    {
        if((gRogueTrainers[i].trainerFlags & TRAINER_FLAG_CLASS_MINIBOSS)
            && (gRogueTrainers[i].classFlags & CLASS_FLAG_MINIBOSS_ANABEL))
        {
            trainerNum = i;
            break;
        }
    }
    EXPECT_NE(trainerNum, TRAINER_NONE);

    ZeroEnemyPartyMons();
    SeedRogueRng(24680);
    SeedRng(13579);
    partySize = Rogue_CreateTrainerParty(trainerNum, gEnemyParty, PARTY_SIZE, TRUE);
    EXPECT_GE(partySize, 1);
    for(i = 0; i < partySize; ++i)
    {
        u8 friendship = GetMonData(&gEnemyParty[i], MON_DATA_FRIENDSHIP);

        // Applying a competitive preset normalizes happiness for Return or
        // Frustration after its level-legal moves have been selected.
        EXPECT(friendship == 0 || friendship == MAX_FRIENDSHIP);
    }

    memcpy(gEnemyParty, originalEnemyParty, sizeof(originalEnemyParty));
    CalculateEnemyPartyCount();
    gRogueRun.trialState.trialId = originalTrialId;
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, originalTrainerDifficulty);
    Rogue_SetCurrentDifficulty(originalDifficulty);
    gRogueRun.currentLevelOffset = originalLevelOffset;
    RoguePokedex_SetDexVariant(originalDexVariant);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gRngRogueValue = originalRogueRng;
    gRngValue = originalRng;
}

TEST("Frontier Brain trophies preserve their competitive echo and reset transient identity")
{
    struct Pokemon originalEnemyParty[PARTY_SIZE];
    struct Pokemon echo;
    u32 originalOtId;
    u32 value;
    u16 moves[MAX_MON_MOVES] = {MOVE_MACH_PUNCH, MOVE_LEECH_SEED, MOVE_HEADBUTT, MOVE_STUN_SPORE};
    u16 heldItem = ITEM_LEFTOVERS;
    u8 originalRoomType = gRogueAdvPath.currentRoomType;
    u8 originalDifficulty = Rogue_GetCurrentDifficulty();
    u8 originalLevelOffset = gRogueRun.currentLevelOffset;
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE originalRng = gRngValue;
    u8 enemyAbilityNum = 1;
    u8 i;
    u8 perfectIvCount = 0;
    u8 expectedLevel;
    u8 expectedAbility;
    u8 expectedNature = NATURE_ADAMANT;
    u8 expectedHiddenPowerType;

    memcpy(originalEnemyParty, gEnemyParty, sizeof(originalEnemyParty));
    ZeroEnemyPartyMons();
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetCurrentDifficulty(6);
    gRogueRun.currentLevelOffset = 0;
    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_MINIBOSS;

    CreateMon(&gEnemyParty[0], SPECIES_BRELOOM, 70, 0, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
    SetMonData(&gEnemyParty[0], MON_DATA_ABILITY_NUM, &enemyAbilityNum);
    SetNature(&gEnemyParty[0], expectedNature);
    for(i = 0; i < MAX_MON_MOVES; ++i)
        SetMonMoveSlot(&gEnemyParty[0], moves[i], i);
    for(i = 0; i < NUM_STATS; ++i)
    {
        value = 21;
        SetMonData(&gEnemyParty[0], MON_DATA_HP_IV + i, &value);
        value = 252;
        SetMonData(&gEnemyParty[0], MON_DATA_HP_EV + i, &value);
    }
    value = TRUE;
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &value);
    value = STATUS1_POISON;
    SetMonData(&gEnemyParty[0], MON_DATA_STATUS, &value);
    SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &heldItem);
    CalculateMonStats(&gEnemyParty[0]);

    expectedAbility = GetMonAbility(&gEnemyParty[0]);
    expectedHiddenPowerType = CalcMonHiddenPowerType(&gEnemyParty[0]);
    expectedLevel = Rogue_CalculatePlayerMonLvl();

    CreateMon(&echo, SPECIES_BRELOOM, 1, 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
    originalOtId = GetMonData(&echo, MON_DATA_OT_ID);
    SeedRng(321);
    Rogue_ModifyScriptMon(&echo);

    EXPECT_EQ(GetMonData(&echo, MON_DATA_SPECIES), SPECIES_BRELOOM);
    EXPECT_EQ(GetMonData(&echo, MON_DATA_OT_ID), originalOtId);
    EXPECT_EQ(GetMonAbility(&echo), expectedAbility);
    EXPECT_EQ(GetNature(&echo), expectedNature);
    EXPECT_EQ(CalcMonHiddenPowerType(&echo), expectedHiddenPowerType);
    EXPECT_EQ(GetMonData(&echo, MON_DATA_LEVEL), expectedLevel);
    EXPECT_EQ(GetMonData(&echo, MON_DATA_MET_LEVEL), expectedLevel);
    EXPECT_EQ(GetMonData(&echo, MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(GetMonData(&echo, MON_DATA_IS_SHINY), FALSE);
    EXPECT_EQ(GetMonData(&echo, MON_DATA_STATUS), 0);
    EXPECT_EQ(GetMonData(&echo, MON_DATA_FRIENDSHIP), gSpeciesInfo[SPECIES_BRELOOM].friendship);

    for(i = 0; i < MAX_MON_MOVES; ++i)
        EXPECT_EQ(GetMonData(&echo, MON_DATA_MOVE1 + i), moves[i]);

    for(i = 0; i < NUM_STATS; ++i)
    {
        u8 iv = GetMonData(&echo, MON_DATA_HP_IV + i);

        if(iv == 31)
            ++perfectIvCount;
        else
            EXPECT(iv == 29 || iv == 30);
        EXPECT_EQ(GetMonData(&echo, MON_DATA_HP_EV + i), 0);
    }
    EXPECT_EQ(perfectIvCount, 2);

    memcpy(gEnemyParty, originalEnemyParty, sizeof(originalEnemyParty));
    CalculateEnemyPartyCount();
    gRogueAdvPath.currentRoomType = originalRoomType;
    Rogue_SetCurrentDifficulty(originalDifficulty);
    gRogueRun.currentLevelOffset = originalLevelOffset;
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gRngValue = originalRng;
}

TEST("Frontier Brain generators retain canonical anchors and Legendary boundaries")
{
    struct Pokemon originalEnemyParty[PARTY_SIZE];
    u8 originalTrainerDifficulty = Rogue_GetConfigRange(CONFIG_RANGE_TRAINER);
    u8 originalDifficulty = Rogue_GetCurrentDifficulty();
    u8 originalDexVariant = RoguePokedex_GetDexVariant();
    u8 originalTrialId = gRogueRun.trialState.trialId;
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE originalRng = gRngValue;
    u16 firstNolandSpecies = SPECIES_NONE;
    bool8 nolandVaried = FALSE;
    u16 trainerNum;
    u8 brainCount = 0;

    memcpy(originalEnemyParty, gEnemyParty, sizeof(originalEnemyParty));
    gRogueRun.trialState.trialId = ROGUE_TRIAL_NONE;
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_BRUTAL);
    Rogue_SetCurrentDifficulty(6);
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_MAX);
    RogueMonQuery_InvalidateSpeciesActiveCache();

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        const struct RogueTrainer* trainer = &gRogueTrainers[trainerNum];
        u16 expectedAnchor = SPECIES_NONE;
        u8 seedCount;
        u8 seed;

        if((trainer->trainerFlags & TRAINER_FLAG_CLASS_MINIBOSS) == 0)
            continue;

        ++brainCount;
        if(trainer->classFlags & CLASS_FLAG_MINIBOSS_ANABEL)
            expectedAnchor = SPECIES_SNORLAX;
        else if(trainer->classFlags & CLASS_FLAG_MINIBOSS_TUCKER)
            expectedAnchor = SPECIES_SWAMPERT;
        else if(trainer->classFlags & CLASS_FLAG_MINIBOSS_SPENSER)
            expectedAnchor = SPECIES_SLAKING;
        else if(trainer->classFlags & CLASS_FLAG_MINIBOSS_GRETA)
            expectedAnchor = SPECIES_UMBREON;
        else if(trainer->classFlags & CLASS_FLAG_MINIBOSS_LUCY)
            expectedAnchor = SPECIES_SEVIPER;

        seedCount = (trainer->classFlags & CLASS_FLAG_MINIBOSS_NOLAND) ? 3 : 1;
        for(seed = 0; seed < seedCount; ++seed)
        {
            u8 partySize;
            u8 legendaryCount = 0;
            bool8 foundAnchor = expectedAnchor == SPECIES_NONE;
            u8 i;
            u8 j;

            ZeroEnemyPartyMons();
            SeedRogueRng(1000 + trainerNum * 17 + seed);
            SeedRng(2000 + trainerNum * 19 + seed);
            partySize = Rogue_CreateTrainerParty(trainerNum, gEnemyParty, PARTY_SIZE, TRUE);
            EXPECT_GE(partySize, 3);

            for(i = 0; i < partySize; ++i)
            {
                u16 species = GetMonData(&gEnemyParty[i], MON_DATA_SPECIES);

                EXPECT_NE(species, SPECIES_NONE);
                if(species == expectedAnchor)
                    foundAnchor = TRUE;
                if(RoguePokedex_IsSpeciesLegendary(species))
                    ++legendaryCount;

                if(trainer->classFlags & CLASS_FLAG_MINIBOSS_NOLAND)
                    EXPECT(Rogue_CheckMonFlags(species, MON_FLAG_SINGLES_STRONG));

                for(j = 0; j < i; ++j)
                    EXPECT_NE(species, GetMonData(&gEnemyParty[j], MON_DATA_SPECIES));
            }

            if(trainer->classFlags & CLASS_FLAG_MINIBOSS_BRANDON)
            {
                EXPECT_EQ(legendaryCount, 1);
            }
            else
            {
                EXPECT_EQ(legendaryCount, 0);
                EXPECT(foundAnchor);
            }

            if(trainer->classFlags & CLASS_FLAG_MINIBOSS_NOLAND)
            {
                u16 leadSpecies = GetMonData(&gEnemyParty[0], MON_DATA_SPECIES);

                if(firstNolandSpecies == SPECIES_NONE)
                    firstNolandSpecies = leadSpecies;
                else if(firstNolandSpecies != leadSpecies)
                    nolandVaried = TRUE;
            }
        }
    }

    EXPECT_EQ(brainCount, 7);
    EXPECT(nolandVaried);

    memcpy(gEnemyParty, originalEnemyParty, sizeof(originalEnemyParty));
    CalculateEnemyPartyCount();
    gRogueRun.trialState.trialId = originalTrialId;
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, originalTrainerDifficulty);
    Rogue_SetCurrentDifficulty(originalDifficulty);
    RoguePokedex_SetDexVariant(originalDexVariant);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gRngRogueValue = originalRogueRng;
    gRngValue = originalRng;
}

TEST("Frontier Brain curated pools fall back to the active Trial Pokedex when exhausted")
{
#ifdef ROGUE_EXPANSION
    u16 plannedSpecies[PARTY_SIZE];
    u8 originalTrainerDifficulty = Rogue_GetConfigRange(CONFIG_RANGE_TRAINER);
    u8 originalDifficulty = Rogue_GetCurrentDifficulty();
    u8 originalLevelOffset = gRogueRun.currentLevelOffset;
    u8 originalDexVariant = RoguePokedex_GetDexVariant();
    u8 originalTrialId = gRogueRun.trialState.trialId;
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE originalRng = gRngValue;
    u16 gretaTrainer = TRAINER_NONE;
    bool8 foundGenericSpecies = FALSE;
    u16 trainerNum;
    u8 i;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        if(gRogueTrainers[trainerNum].classFlags & CLASS_FLAG_MINIBOSS_GRETA)
        {
            gretaTrainer = trainerNum;
            break;
        }
    }
    EXPECT_NE(gretaTrainer, TRAINER_NONE);

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_BRUTAL);
    Rogue_SetCurrentDifficulty(6);
    gRogueRun.currentLevelOffset = 0;
    gRogueRun.trialState.trialId = ROGUE_TRIAL_REGION_JOHTO;
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_JOHTO_GSC);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    SeedRogueRng(24680);
    SeedRng(13579);

    EXPECT_EQ(RogueTest_PlanTrainerSpecies(gretaTrainer, plannedSpecies, PARTY_SIZE), PARTY_SIZE);
    for(i = 0; i < PARTY_SIZE; ++i)
    {
        EXPECT(RogueTrial_IsSpeciesLegal(plannedSpecies[i], 0));
        if(plannedSpecies[i] != SPECIES_UMBREON
            && plannedSpecies[i] != SPECIES_HERACROSS
            && plannedSpecies[i] != SPECIES_GENGAR)
            foundGenericSpecies = TRUE;
    }
    EXPECT(foundGenericSpecies);

    gRogueRun.trialState.trialId = originalTrialId;
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, originalTrainerDifficulty);
    Rogue_SetCurrentDifficulty(originalDifficulty);
    gRogueRun.currentLevelOffset = originalLevelOffset;
    RoguePokedex_SetDexVariant(originalDexVariant);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gRngRogueValue = originalRogueRng;
    gRngValue = originalRng;
#else
    ASSUME(FALSE);
#endif
}

TEST("Rival roster planning caches species without constructing temporary mons")
{
    u8 i;
    u8 previousTrainerDifficulty = Rogue_GetConfigRange(CONFIG_RANGE_TRAINER);
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    bool8 previousTrainerToggles[ARRAY_COUNT(sTrainerConfigToggles)];
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE rngOriginal = gRngRogueValue;
    RAND_TYPE rngBefore;

    memset(&gRogueRun, 0, sizeof(gRogueRun));
    SaveTrainerConfigToggles(previousTrainerToggles);
    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], SPECIES_PIKACHU, 10, 0, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_AVERAGE);
    SetBenchmarkTrainerConfig();
    Rogue_SetCurrentDifficulty(3);
    gRogueRun.baseSeed = 12345;
    gRogueRun.rivalTrainerNum = 0;
    memset(gRogueRun.rivalSpecies, SPECIES_NONE, sizeof(gRogueRun.rivalSpecies));
    SeedRogueRng(23456);
    rngBefore = gRngRogueValue;

    Rogue_EnsureRivalBaseTeamForNewAdventure();

    EXPECT_EQ(Rogue_GetCurrentDifficulty(), 3);
    EXPECT_EQ(memcmp(&gRngRogueValue, &rngBefore, sizeof(rngBefore)), 0);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_PIKACHU);
    for(i = 0; i < 5; ++i)
        EXPECT_NE(gRogueRun.rivalSpecies[i], SPECIES_NONE);
    for(i = 5; i < ROGUE_RIVAL_TOTAL_MON_COUNT; ++i)
        EXPECT_EQ(gRogueRun.rivalSpecies[i], SPECIES_NONE);

    Rogue_EnsureRivalLateTeamForNewAdventure();
    for(i = 0; i < ROGUE_RIVAL_TOTAL_MON_COUNT; ++i)
        EXPECT_NE(gRogueRun.rivalSpecies[i], SPECIES_NONE);

    ZeroEnemyPartyMons();
    memset(&gRogueRun, 0, sizeof(gRogueRun));
    gRngRogueValue = rngOriginal;
    Rogue_SetCurrentDifficulty(previousDifficulty);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, previousTrainerDifficulty);
    RestoreTrainerConfigToggles(previousTrainerToggles);
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}

TEST("Active species cache preserves results and avoids repeated dex scans")
{
    bool8 pichuActive = FALSE;
    bool8 hoOhActive = FALSE;
    struct LongBenchmark uncached;
    struct LongBenchmark cached;
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    RogueMonQuery_InvalidateSpeciesActiveCache();

    LONG_BENCHMARK(&uncached)
    {
        RogueMonQuery_Begin();
        RogueMonQuery_IsSpeciesActive();
        pichuActive = RogueMiscQuery_CheckState(SPECIES_PICHU);
        hoOhActive = RogueMiscQuery_CheckState(SPECIES_HO_OH);
        RogueMonQuery_End();
    }

    LONG_BENCHMARK(&cached)
    {
        RogueMonQuery_Begin();
        RogueMonQuery_IsSpeciesActive();
        EXPECT_EQ(RogueMiscQuery_CheckState(SPECIES_PICHU), pichuActive);
        EXPECT_EQ(RogueMiscQuery_CheckState(SPECIES_HO_OH), hoOhActive);
        RogueMonQuery_End();
    }

    MgbaPrintf_("species query uncached: %d ticks, cached: %d ticks", uncached.ticks, cached.ticks);
    EXPECT_LT(cached.ticks, uncached.ticks);
    EXPECT_LT(cached.ticks, 500);

    RogueMonQuery_InvalidateSpeciesActiveCache();
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}

TEST("Run-start rival planning stays within its performance ceiling")
{
    struct LongBenchmark rivalBase;
    u8 previousTrainerDifficulty = Rogue_GetConfigRange(CONFIG_RANGE_TRAINER);
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    u8 previousDexVariant = RoguePokedex_GetDexVariant();
    bool8 previousTrainerToggles[ARRAY_COUNT(sTrainerConfigToggles)];
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE rngOriginal = gRngRogueValue;

    memset(&gRogueRun, 0, sizeof(gRogueRun));
    SaveTrainerConfigToggles(previousTrainerToggles);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_AVERAGE);
    SetBenchmarkTrainerConfig();
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_ROGUE_MODERN);
    Rogue_SetCurrentDifficulty(0);
    gRogueRun.baseSeed = 12345;
    gRogueRun.rivalTrainerNum = 0;
    memset(gRogueRun.rivalSpecies, SPECIES_NONE, sizeof(gRogueRun.rivalSpecies));
    RogueMonQuery_InvalidateSpeciesActiveCache();

    LONG_BENCHMARK(&rivalBase)
        Rogue_EnsureRivalBaseTeamForNewAdventure();

    MgbaPrintf_("rival base: %d ticks", rivalBase.ticks);
    // 1024-cycle ticks; these ceilings include 25% headroom over the measured
    // optimized fixed-seed baselines.
    EXPECT_LT(rivalBase.ticks, 5200);

    memset(&gRogueRun, 0, sizeof(gRogueRun));
    gRngRogueValue = rngOriginal;
    Rogue_SetCurrentDifficulty(previousDifficulty);
    RoguePokedex_SetDexVariant(previousDexVariant);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, previousTrainerDifficulty);
    RestoreTrainerConfigToggles(previousTrainerToggles);
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}
