#include "global.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "event_data.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_settings.h"
#include "rogue_query.h"
#include "test/test.h"
#include "rogue_trainers.h"

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

TEST("Rival roster planning caches species without constructing temporary mons")
{
    u8 i;
    u8 previousTrainerDifficulty = Rogue_GetConfigRange(CONFIG_RANGE_TRAINER);
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    bool8 previousKanto = Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO);
    bool8 previousJohto = Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO);
    bool8 previousHoenn = Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN);
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE rngOriginal = gRngRogueValue;
    RAND_TYPE rngBefore;

    memset(&gRogueRun, 0, sizeof(gRogueRun));
    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], SPECIES_PIKACHU, 10, 0, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_AVERAGE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, TRUE);
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
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, previousKanto);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, previousJohto);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, previousHoenn);
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
    bool8 previousKanto = Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO);
    bool8 previousJohto = Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO);
    bool8 previousHoenn = Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN);
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE rngOriginal = gRngRogueValue;

    memset(&gRogueRun, 0, sizeof(gRogueRun));
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_AVERAGE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, TRUE);
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
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, previousTrainerDifficulty);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, previousKanto);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, previousJohto);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, previousHoenn);
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}
