#include "global.h"
#include "constants/abilities.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "event_data.h"
#include "malloc.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_hub.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"
#include "rogue_trials.h"
#include "test/test.h"

static void ExpectGeneratedAbilityIsRerolled(u16 species, u32 customMonId)
{
    u8 i;
    u16 generatedAbility = RogueGift_GetCustomMonAbility(customMonId, 0);

    EXPECT_NE(generatedAbility, ABILITY_NONE);
    for(i = 0; i < NUM_ABILITY_SLOTS; ++i)
        EXPECT_NE(generatedAbility, GetAbilityBySpecies(species, i, 0));
}

static void ExpectNoDangerousLegendaryMoves(struct Pokemon *mon)
{
    u8 i;

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        u16 move = GetMonData(mon, MON_DATA_MOVE1 + i);

        EXPECT_NE(move, MOVE_ROAR);
        EXPECT_NE(move, MOVE_WHIRLWIND);
        EXPECT_NE(move, MOVE_EXPLOSION);
        EXPECT_NE(move, MOVE_SELF_DESTRUCT);
        EXPECT_NE(move, MOVE_TELEPORT);
    }
}

TEST("A portal-pregenerated initial path is reused on map entry")
{
    u8 originalRoomCount = gRogueAdvPath.roomCount;
    bool8 originalJustGenerated = gRogueAdvPath.justGenerated;
    u8 originalRoomId = gRogueRun.adventureRoomId;

    gRogueAdvPath.roomCount = 1;
    gRogueAdvPath.justGenerated = TRUE;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;

    EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());
    EXPECT_EQ(gRogueAdvPath.roomCount, 1);
    EXPECT(gRogueAdvPath.justGenerated);

    gRogueAdvPath.roomCount = originalRoomCount;
    gRogueAdvPath.justGenerated = originalJustGenerated;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Seeded standard paths keep replacement composition bounded")
{
    static const struct
    {
        u16 seed;
        u8 routeCount;
        u8 specialCount;
        u8 emptyCount;
    } cases[] =
    {
        {13579, 9, 5, 3},
        {24680, 5, 4, 3},
        {54321, 5, 3, 2},
        {65535, 7, 4, 4},
    };
    struct RogueAdvPath originalPath = gRogueAdvPath;
    u16 originalBaseSeed = gRogueRun.baseSeed;
    u8 originalGameMode = Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM);
    u8 originalDifficulty = Rogue_GetCurrentDifficulty();
    u8 originalRoomId = gRogueRun.adventureRoomId;
    u8 i;

    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_STANDARD);
    Rogue_SetCurrentDifficulty(2);

    for(i = 0; i < ARRAY_COUNT(cases); ++i)
    {
        u8 routeCount = 0;
        u8 specialCount = 0;
        u8 emptyCount = 0;
        u8 roomId;

        gRogueRun.baseSeed = cases[i].seed;
        gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;
        memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
        EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());

        for(roomId = 0; roomId < gRogueAdvPath.roomCount; ++roomId)
        {
            switch(gRogueAdvPath.rooms[roomId].roomType)
            {
            case ADVPATH_ROOM_ROUTE:
                ++routeCount;
                break;
            case ADVPATH_ROOM_NONE:
            case ADVPATH_ROOM_BOSS:
                if(gRogueAdvPath.rooms[roomId].roomType == ADVPATH_ROOM_NONE)
                    ++emptyCount;
                break;
            default:
                ++specialCount;
                break;
            }
        }

        EXPECT_EQ(routeCount, cases[i].routeCount);
        EXPECT_EQ(specialCount, cases[i].specialCount);
        EXPECT_EQ(emptyCount, cases[i].emptyCount);
        EXPECT_EQ(routeCount + specialCount + emptyCount, gRogueAdvPath.roomCount - 1);
    }

    gRogueAdvPath = originalPath;
    gRogueRun.baseSeed = originalBaseSeed;
    gRogueRun.adventureRoomId = originalRoomId;
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, originalGameMode);
    Rogue_SetCurrentDifficulty(originalDifficulty);
}

TEST("An exhausted path is replaced after its boss")
{
    struct RogueAdvPath *originalPath = Alloc(sizeof(*originalPath));
    u8 originalRoomId = gRogueRun.adventureRoomId;

    EXPECT_NE(originalPath, NULL);
    if(originalPath == NULL)
        return;

    *originalPath = gRogueAdvPath;
    gRogueAdvPath.roomCount = 1;
    gRogueAdvPath.justGenerated = FALSE;
    gRogueAdvPath.rooms[0].roomType = ADVPATH_ROOM_BOSS;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;

    EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());
    EXPECT(gRogueAdvPath.justGenerated);
    EXPECT_GT(gRogueAdvPath.roomCount, 1);

    gRogueAdvPath = *originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
    Free(originalPath);
}

TEST("Frontier Brain paths cache deterministic previews")
{
    struct RogueAdvPath *originalPath = Alloc(sizeof(*originalPath));
    u8 originalGameMode = Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM);
    u8 originalDifficulty = Rogue_GetCurrentDifficulty();
    u16 originalBaseSeed = gRogueRun.baseSeed;
    u8 originalRoomId = gRogueRun.adventureRoomId;
    RAND_TYPE originalRng = gRngRogueValue;
    u16 trainerNum;
    u16 aceRoomSeed = 0;
    u16 rewardSpeciesA = SPECIES_NONE;
    u16 rewardSpeciesB = SPECIES_NONE;
    u8 rewardMode = MINIBOSS_REWARD_MODE_NONE;
    u8 aceRoomId = ADVPATH_INVALID_ROOM_ID;
    u8 generatedRoomCount;
    u8 generatedPathLength;
    u8 roomCount = 0;
    u16 scheduledTrainers[ADVPATH_FRONTIER_BRAIN_COUNT];
    u8 scheduledDifficulties[ADVPATH_FRONTIER_BRAIN_COUNT];
    u8 i;

    EXPECT_NE(originalPath, NULL);
    if(originalPath == NULL)
        return;

    *originalPath = gRogueAdvPath;
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_STANDARD);
    gRogueRun.baseSeed = 13579;
    Rogue_GetFrontierBrainSchedule(scheduledTrainers, scheduledDifficulties);
    trainerNum = scheduledTrainers[0];
    Rogue_SetCurrentDifficulty(scheduledDifficulties[0]);
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;
    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));

    EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());
    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if(gRogueAdvPath.rooms[i].roomType == ADVPATH_ROOM_MINIBOSS)
        {
            ++roomCount;
            aceRoomId = i;
            aceRoomSeed = gRogueAdvPath.rooms[i].rngSeed;
            EXPECT_EQ(gRogueAdvPath.rooms[i].roomParams.perType.miniboss.trainerNum, trainerNum);
            EXPECT_NE(gRogueAdvPath.rooms[i].coords.x + 1, gRogueAdvPath.pathLength);
            EXPECT(gRogueAdvPath.rooms[i].roomParams.perType.miniboss.hasRewardPreview);
            rewardSpeciesA = gRogueAdvPath.rooms[i].roomParams.perType.miniboss.rewardSpeciesA;
            rewardSpeciesB = gRogueAdvPath.rooms[i].roomParams.perType.miniboss.rewardSpeciesB;
            rewardMode = gRogueAdvPath.rooms[i].roomParams.perType.miniboss.rewardMode;
        }
    }
    EXPECT_EQ(roomCount, 1);
    generatedRoomCount = gRogueAdvPath.roomCount;
    generatedPathLength = gRogueAdvPath.pathLength;

    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;
    EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());
    EXPECT_EQ(gRogueAdvPath.roomCount, generatedRoomCount);
    EXPECT_EQ(gRogueAdvPath.pathLength, generatedPathLength);
    EXPECT_NE(aceRoomId, ADVPATH_INVALID_ROOM_ID);
    EXPECT_EQ(gRogueAdvPath.rooms[aceRoomId].roomType, ADVPATH_ROOM_MINIBOSS);
    EXPECT_EQ(gRogueAdvPath.rooms[aceRoomId].rngSeed, aceRoomSeed);
    EXPECT_EQ(gRogueAdvPath.rooms[aceRoomId].roomParams.perType.miniboss.trainerNum, trainerNum);
    EXPECT(gRogueAdvPath.rooms[aceRoomId].roomParams.perType.miniboss.hasRewardPreview);
    EXPECT_EQ(gRogueAdvPath.rooms[aceRoomId].roomParams.perType.miniboss.rewardSpeciesA, rewardSpeciesA);
    EXPECT_EQ(gRogueAdvPath.rooms[aceRoomId].roomParams.perType.miniboss.rewardSpeciesB, rewardSpeciesB);
    EXPECT_EQ(gRogueAdvPath.rooms[aceRoomId].roomParams.perType.miniboss.rewardMode, rewardMode);

    // Reused paths must repair a missing preview before the overview becomes
    // interactive instead of generating the full team when the node is used.
    gRogueAdvPath.rooms[aceRoomId].roomParams.perType.miniboss.hasRewardPreview = FALSE;
    gRogueAdvPath.justGenerated = TRUE;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;
    EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());
    EXPECT(gRogueAdvPath.rooms[aceRoomId].roomParams.perType.miniboss.hasRewardPreview);

    gRogueAdvPath.rooms[aceRoomId].roomParams.perType.miniboss.hasRewardPreview = FALSE;
    gRogueAdvPath.justGenerated = FALSE;
    gRogueRun.adventureRoomId = aceRoomId;
    EXPECT(!RogueAdv_GenerateAdventurePathsIfRequired());
    EXPECT(gRogueAdvPath.rooms[aceRoomId].roomParams.perType.miniboss.hasRewardPreview);

    gRogueAdvPath = *originalPath;
    gRogueRun.baseSeed = originalBaseSeed;
    gRogueRun.adventureRoomId = originalRoomId;
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, originalGameMode);
    Rogue_SetCurrentDifficulty(originalDifficulty);
    gRngRogueValue = originalRng;
    Free(originalPath);
}

TEST("Frontier Brain previews are stable, RNG-neutral, and expose Brandon's anchor")
{
    struct RogueAdvPathRoom originalRoom = gRogueAdvPath.rooms[1];
    struct Pokemon originalEnemyParty[PARTY_SIZE];
    u8 originalRoomCount = gRogueAdvPath.roomCount;
    u16 originalRivalTrainer = gRogueRun.rivalTrainerNum;
    u8 originalTrialId = gRogueRun.trialState.trialId;
    u16 originalPreviewTrainer = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA);
    u16 originalPreviewSpeciesA = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1);
    u16 originalPreviewSpeciesB = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2);
    u16 originalPreviewMode = gSpecialVar_Result;
    u8 originalTrainerDifficulty = Rogue_GetConfigRange(CONFIG_RANGE_TRAINER);
    u8 originalDifficulty = Rogue_GetCurrentDifficulty();
    u8 originalDexVariant = RoguePokedex_GetDexVariant();
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE originalRng = gRngValue;
    RAND_TYPE originalRng2 = gRng2Value;
    bool8 itemFlags[FLAG_ROGUE_ITEM_END - FLAG_ROGUE_ITEM_START + 1];
    u16 itemVars[VAR_ROGUE_ITEM_END - VAR_ROGUE_ITEM_START + 1];
    u16 trainerNums[2] = {TRAINER_NONE, TRAINER_NONE};
    u16 trainerNum;
    u8 i;
    u8 itemIndex;

    memcpy(originalEnemyParty, gEnemyParty, sizeof(originalEnemyParty));
    if(wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_BRUTAL);
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_MAX);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    gRogueRun.rivalTrainerNum = TRAINER_NONE;
    gRogueRun.trialState.trialId = ROGUE_TRIAL_NONE;
    Rogue_SetCurrentDifficulty(6);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        if((gRogueTrainers[trainerNum].trainerFlags & TRAINER_FLAG_CLASS_MINIBOSS) == 0)
            continue;

        if(gRogueTrainers[trainerNum].classFlags & CLASS_FLAG_MINIBOSS_ANABEL)
            trainerNums[0] = trainerNum;
        else if(gRogueTrainers[trainerNum].classFlags & CLASS_FLAG_MINIBOSS_BRANDON)
            trainerNums[1] = trainerNum;
    }
    EXPECT_NE(trainerNums[0], TRAINER_NONE);
    EXPECT_NE(trainerNums[1], TRAINER_NONE);

    gRogueAdvPath.roomCount = 2;
    memset(&gRogueAdvPath.rooms[1], 0, sizeof(gRogueAdvPath.rooms[1]));
    gRogueAdvPath.rooms[1].roomType = ADVPATH_ROOM_MINIBOSS;
    gRogueAdvPath.rooms[1].rngSeed = 24680;
    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], SPECIES_PIKACHU, 10, 0, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
    CalculateEnemyPartyCount();

    for(itemIndex = 0; itemIndex < ARRAY_COUNT(itemFlags); ++itemIndex)
    {
        itemFlags[itemIndex] = FlagGet(FLAG_ROGUE_ITEM_START + itemIndex);
        itemVars[itemIndex] = VarGet(VAR_ROGUE_ITEM_START + itemIndex);
    }

    for(i = 0; i < ARRAY_COUNT(trainerNums); ++i)
    {
        RAND_TYPE rogueRngBefore;
        RAND_TYPE rngBefore;
        RAND_TYPE rng2Before;
        u16 previewSpeciesA;
        u16 previewSpeciesB;

        gRogueAdvPath.rooms[1].roomParams.perType.miniboss.trainerNum = trainerNums[i];
        gRogueAdvPath.rooms[1].roomParams.perType.miniboss.hasRewardPreview = FALSE;
        SeedRogueRng(1111 + i);
        SeedRng(2222 + i);
        SeedRng2(3333 + i);
        rogueRngBefore = gRngRogueValue;
        rngBefore = gRngValue;
        rng2Before = gRng2Value;

        Rogue_CacheMiniBossPreview(1);
        EXPECT(gRogueAdvPath.rooms[1].roomParams.perType.miniboss.hasRewardPreview);
        Rogue_BufferMiniBossPreview(1);
        previewSpeciesA = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1);
        previewSpeciesB = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2);
        EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA), trainerNums[i]);
        EXPECT_NE(previewSpeciesA, SPECIES_NONE);
        EXPECT_NE(previewSpeciesB, SPECIES_NONE);
        EXPECT_NE(previewSpeciesA, previewSpeciesB);
        EXPECT_EQ(gSpecialVar_Result, MINIBOSS_REWARD_MODE_DOUBLE);
        EXPECT_EQ(gRogueAdvPath.rooms[1].roomParams.perType.miniboss.rewardSpeciesA, previewSpeciesA);
        EXPECT_EQ(gRogueAdvPath.rooms[1].roomParams.perType.miniboss.rewardSpeciesB, previewSpeciesB);
        EXPECT_EQ(gRogueAdvPath.rooms[1].roomParams.perType.miniboss.rewardMode, gSpecialVar_Result);
        EXPECT_EQ(CalculateEnemyPartyCount(), 1);
        EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_PIKACHU);
        EXPECT_EQ(memcmp(&rogueRngBefore, &gRngRogueValue, sizeof(rogueRngBefore)), 0);
        EXPECT_EQ(memcmp(&rngBefore, &gRngValue, sizeof(rngBefore)), 0);
        EXPECT_EQ(memcmp(&rng2Before, &gRng2Value, sizeof(rng2Before)), 0);
        for(itemIndex = 0; itemIndex < ARRAY_COUNT(itemFlags); ++itemIndex)
        {
            EXPECT_EQ(FlagGet(FLAG_ROGUE_ITEM_START + itemIndex), itemFlags[itemIndex]);
            EXPECT_EQ(VarGet(VAR_ROGUE_ITEM_START + itemIndex), itemVars[itemIndex]);
        }

        Rogue_BufferMiniBossPreview(1);
        EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1), previewSpeciesA);
        EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2), previewSpeciesB);

        if(i == 1)
            EXPECT(RoguePokedex_IsSpeciesLegendary(previewSpeciesA) || RoguePokedex_IsSpeciesLegendary(previewSpeciesB));
    }

    gRogueAdvPath.rooms[1].roomParams.perType.miniboss.trainerNum = trainerNums[0];
    gRogueRun.trialState.trialId = ROGUE_TRIAL_LIMITED_CAPTURE;
    gRogueAdvPath.rooms[1].roomParams.perType.miniboss.hasRewardPreview = FALSE;
    Rogue_BufferMiniBossPreview(1);
    EXPECT_EQ(gSpecialVar_Result, MINIBOSS_REWARD_MODE_NONE);
    EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1), SPECIES_NONE);
    EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2), SPECIES_NONE);

    gRogueRun.trialState.trialId = ROGUE_TRIAL_ORRE_STYLE;
    gRogueAdvPath.rooms[1].roomParams.perType.miniboss.hasRewardPreview = FALSE;
    Rogue_BufferMiniBossPreview(1);
    EXPECT_EQ(gSpecialVar_Result, MINIBOSS_REWARD_MODE_SNAG);
    EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1), SPECIES_NONE);
    EXPECT_EQ(VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2), SPECIES_NONE);

    gRogueAdvPath.rooms[1] = originalRoom;
    gRogueAdvPath.roomCount = originalRoomCount;
    gRogueRun.rivalTrainerNum = originalRivalTrainer;
    gRogueRun.trialState.trialId = originalTrialId;
    memcpy(gEnemyParty, originalEnemyParty, sizeof(originalEnemyParty));
    CalculateEnemyPartyCount();
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, originalPreviewTrainer);
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, originalPreviewSpeciesA);
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, originalPreviewSpeciesB);
    gSpecialVar_Result = originalPreviewMode;
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, originalTrainerDifficulty);
    Rogue_SetCurrentDifficulty(originalDifficulty);
    RoguePokedex_SetDexVariant(originalDexVariant);
    RogueMonQuery_InvalidateSpeciesActiveCache();
    if(wasRunActive)
        FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    gRngRogueValue = originalRogueRng;
    gRngValue = originalRng;
    gRng2Value = originalRng2;
}

TEST("Full Rest Stops become more common during the Elite Four")
{
    EXPECT_EQ(RogueAdv_Debug_GetFullRestStopWeight(ROGUE_GYM_START_DIFFICULTY + 1), 0);
    EXPECT_EQ(RogueAdv_Debug_GetFullRestStopWeight(ROGUE_GYM_START_DIFFICULTY + 2), 6);
    EXPECT_EQ(RogueAdv_Debug_GetFullRestStopWeight(ROGUE_ELITE_START_DIFFICULTY - 1), 6);
    EXPECT_EQ(RogueAdv_Debug_GetFullRestStopWeight(ROGUE_ELITE_START_DIFFICULTY), 20);
}

TEST("Unique Typings is the final Lab upgrade")
{
    u8 originalUpgradeFlags[ARRAY_COUNT(gRogueSaveBlock->hubMap.upgradeFlags)];
    u8 originalAreaBuiltFlags[ARRAY_COUNT(gRogueSaveBlock->hubMap.areaBuiltFlags)];

    memcpy(originalUpgradeFlags, gRogueSaveBlock->hubMap.upgradeFlags, sizeof(originalUpgradeFlags));
    memcpy(originalAreaBuiltFlags, gRogueSaveBlock->hubMap.areaBuiltFlags, sizeof(originalAreaBuiltFlags));
    gRogueSaveBlock->hubMap.areaBuiltFlags[HUB_AREA_LABS / 8] |= 1 << (HUB_AREA_LABS % 8);

    EXPECT_EQ(ARRAY_COUNT(gRogueSaveBlock->hubMap.upgradeFlags), 7);
    EXPECT_EQ(gRogueHubUpgrades[HUB_UPGRADE_LAB_UNIQUE_TYPINGS].targetArea, HUB_AREA_LABS);
    EXPECT_EQ(gRogueHubUpgrades[HUB_UPGRADE_LAB_UNIQUE_TYPINGS].buildCost, 12);
    EXPECT_EQ(gRogueHubUpgrades[HUB_UPGRADE_LAB_UNIQUE_TYPINGS].requiredUpgrades[0], HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_LEGENDARY);

    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_LEGENDARY, FALSE);
    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_TYPINGS, FALSE);
    EXPECT(!RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_TYPINGS));
    EXPECT(!RogueHub_HasUpgradeRequirements(HUB_UPGRADE_LAB_UNIQUE_TYPINGS));

    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_LEGENDARY, TRUE);
    EXPECT(RogueHub_HasUpgradeRequirements(HUB_UPGRADE_LAB_UNIQUE_TYPINGS));

    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_TYPINGS, TRUE);
    EXPECT(RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_TYPINGS));

    memcpy(gRogueSaveBlock->hubMap.upgradeFlags, originalUpgradeFlags, sizeof(originalUpgradeFlags));
    memcpy(gRogueSaveBlock->hubMap.areaBuiltFlags, originalAreaBuiltFlags, sizeof(originalAreaBuiltFlags));
}

TEST("Unique Legendary generation is available by default, deterministic, and supports both rarities")
{
    u16 seed;
    u16 successSeed = 0;
    u16 nativeUniqueAbility = GetUniqueAbilityBySpecies(SPECIES_HO_OH);
    u32 customMonId;
    u32 epicId = 0;
    u32 legendaryId = 0;
    u32 replayId;
    bool8 foundFailure = FALSE;
    bool8 foundTwoSuccesses = FALSE;
    RAND_TYPE originalRng = gRngValue;
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE rngBefore;
    u8 originalUpgradeFlags[ARRAY_COUNT(gRogueSaveBlock->hubMap.upgradeFlags)];

    memcpy(originalUpgradeFlags, gRogueSaveBlock->hubMap.upgradeFlags, sizeof(originalUpgradeFlags));
    memset(gRogueSaveBlock->hubMap.upgradeFlags, 0, sizeof(gRogueSaveBlock->hubMap.upgradeFlags));

    for(seed = 0; seed < 1024 && (epicId == 0 || legendaryId == 0 || !foundFailure); ++seed)
    {
        SeedRogueRng(seed);
        customMonId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_HO_OH);

        if(customMonId == 0)
        {
            foundFailure = TRUE;
        }
        else
        {
            if(successSeed == 0)
                successSeed = seed;

            if(RogueGift_GetCustomMonRarity(customMonId) == UNIQUE_RARITY_EPIC)
                epicId = customMonId;
            else if(RogueGift_GetCustomMonRarity(customMonId) == UNIQUE_RARITY_LEGENDARY)
                legendaryId = customMonId;
        }
    }

    EXPECT(foundFailure);
    EXPECT_NE(epicId, 0);
    EXPECT_NE(legendaryId, 0);

    ExpectGeneratedAbilityIsRerolled(SPECIES_HO_OH, epicId);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(epicId), ABILITY_NONE);
    EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(SPECIES_HO_OH, epicId), nativeUniqueAbility);

    ExpectGeneratedAbilityIsRerolled(SPECIES_HO_OH, legendaryId);
    EXPECT_NE(RogueGift_GetCustomMonUniqueAbility(legendaryId), ABILITY_NONE);
    EXPECT_NE(RogueGift_GetCustomMonUniqueAbility(legendaryId), nativeUniqueAbility);

    SeedRng(12345);
    rngBefore = gRngValue;
    SeedRogueRng(successSeed);
    customMonId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_HO_OH);
    EXPECT_EQ(memcmp(&gRngValue, &rngBefore, sizeof(rngBefore)), 0);

    SeedRng(54321);
    SeedRogueRng(successSeed);
    replayId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_HO_OH);
    EXPECT_EQ(replayId, customMonId);

    for(seed = 0; seed < 4096 && !foundTwoSuccesses; ++seed)
    {
        SeedRogueRng(seed);
        customMonId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_HO_OH);
        replayId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_MEWTWO);
        foundTwoSuccesses = customMonId != 0 && replayId != 0;
    }
    EXPECT(foundTwoSuccesses);

    memcpy(gRogueSaveBlock->hubMap.upgradeFlags, originalUpgradeFlags, sizeof(originalUpgradeFlags));
    gRngValue = originalRng;
    gRngRogueValue = originalRogueRng;
}

TEST("Unique Den generation is replay deterministic and preserves general RNG")
{
    u32 customMonId;
    u32 replayId;
    RAND_TYPE originalRng = gRngValue;
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE rngBefore;

    SeedRng(12345);
    rngBefore = gRngValue;
    SeedRogueRng(6789);
    customMonId = RogueAdv_Debug_GenerateUniqueDenCustomMonId(SPECIES_HO_OH);

    EXPECT_NE(customMonId, 0);
    EXPECT_EQ(memcmp(&gRngValue, &rngBefore, sizeof(rngBefore)), 0);

    SeedRng(54321);
    SeedRogueRng(6789);
    replayId = RogueAdv_Debug_GenerateUniqueDenCustomMonId(SPECIES_HO_OH);

    EXPECT_EQ(replayId, customMonId);

    gRngValue = originalRng;
    gRngRogueValue = originalRogueRng;
}

TEST("Unique Legendary rooms use a gold statue")
{
    struct RogueAdvPath originalPath = gRogueAdvPath;
    struct ObjectEventTemplate objectEvents[1] = {0};
    u8 objectEventCount = 0;

    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
    gRogueAdvPath.roomCount = 1;
    gRogueAdvPath.justGenerated = TRUE;
    gRogueAdvPath.rooms[0].roomType = ADVPATH_ROOM_LEGENDARY;

    RogueAdv_ModifyObjectEvents(NULL, objectEvents, &objectEventCount, ARRAY_COUNT(objectEvents));
    EXPECT_EQ(objectEventCount, 1);
    EXPECT_EQ(objectEvents[0].graphicsId, OBJ_EVENT_GFX_TRICK_HOUSE_STATUE);

    gRogueAdvPath.rooms[0].roomParams.perType.legendary.customMonId = 1;
    RogueAdv_ModifyObjectEvents(NULL, objectEvents, &objectEventCount, ARRAY_COUNT(objectEvents));
    EXPECT_EQ(objectEventCount, 1);
    EXPECT_EQ(objectEvents[0].graphicsId, OBJ_EVENT_GFX_GOLD_LEGENDARY_STATUE);

    gRogueAdvPath = originalPath;
}

TEST("Unique Legendary rooms convert roamers into stationary encounters")
{
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u16 originalRoamerSpecies = gRogueRun.legendarySpecies[ADVPATH_LEGEND_ROAMER];
    struct RogueAdvPath originalPath = gRogueAdvPath;

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    gRogueRun.legendarySpecies[ADVPATH_LEGEND_ROAMER] = SPECIES_HO_OH;
    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_LEGENDARY;
    gRogueAdvPath.currentRoomParams.perType.legendary.customMonId = 0;

    EXPECT(Rogue_IsBattleRoamerMon(SPECIES_HO_OH));

    gRogueAdvPath.currentRoomParams.perType.legendary.customMonId = 1;
    EXPECT(!Rogue_IsBattleRoamerMon(SPECIES_HO_OH));

    gRogueRun.legendarySpecies[ADVPATH_LEGEND_ROAMER] = originalRoamerSpecies;
    gRogueAdvPath = originalPath;
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}

TEST("Unique Legendary battles and catches preserve their generated payload")
{
    u8 i;
    u8 rarities[] = { UNIQUE_RARITY_EPIC, UNIQUE_RARITY_LEGENDARY };
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    struct RogueAdvPath originalPath = gRogueAdvPath;
    RAND_TYPE originalRng = gRngValue;

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_LEGENDARY;

    for(i = 0; i < ARRAY_COUNT(rarities); ++i)
    {
        struct Pokemon mon;
        u32 customMonId;
        u32 shinyState = TRUE;

        SeedRng(100 + i);
        customMonId = RogueGift_CreateDynamicMonIdRaw(rarities[i], SPECIES_HO_OH);
        gRogueAdvPath.currentRoomParams.perType.legendary.customMonId = customMonId;

        CreateMon(&mon, SPECIES_HO_OH, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
        SetMonData(&mon, MON_DATA_IS_SHINY, &shinyState);
        Rogue_ModifyWildMon(&mon);

        EXPECT_EQ(RogueGift_GetCustomMonId(&mon), customMonId);
        EXPECT_EQ(RogueGift_GetCustomMonRarity(RogueGift_GetCustomMonId(&mon)), rarities[i]);
        EXPECT(GetMonData(&mon, MON_DATA_IS_SHINY));
        ExpectNoDangerousLegendaryMoves(&mon);

        Rogue_ModifyCaughtMon(&mon);
        EXPECT_EQ(RogueGift_GetCustomMonId(&mon), customMonId);
        EXPECT_EQ(RogueGift_GetCustomBoxMonId(&mon.box), customMonId);
    }

    gRogueAdvPath = originalPath;
    gRngValue = originalRng;
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}
