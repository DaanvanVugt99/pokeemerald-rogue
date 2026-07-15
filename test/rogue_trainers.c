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
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    RAND_TYPE rngOriginal = gRngRogueValue;
    RAND_TYPE rngBefore;

    memset(&gRogueRun, 0, sizeof(gRogueRun));
    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], SPECIES_PIKACHU, 10, 0, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_AVERAGE);
    Rogue_SetCurrentDifficulty(3);
    gRogueRun.baseSeed = 12345;
    gRogueRun.rivalTrainerNum = 0;
    memset(gRogueRun.rivalSpecies, SPECIES_NONE, sizeof(gRogueRun.rivalSpecies));
    SeedRogueRng(23456);
    rngBefore = gRngRogueValue;

    Rogue_GenerateRivalTeamForNewAdventure();

    EXPECT_EQ(Rogue_GetCurrentDifficulty(), 3);
    EXPECT_EQ(memcmp(&gRngRogueValue, &rngBefore, sizeof(rngBefore)), 0);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_PIKACHU);
    for(i = 0; i < ROGUE_RIVAL_TOTAL_MON_COUNT; ++i)
        EXPECT_NE(gRogueRun.rivalSpecies[i], SPECIES_NONE);

    ZeroEnemyPartyMons();
    memset(&gRogueRun, 0, sizeof(gRogueRun));
    gRngRogueValue = rngOriginal;
    Rogue_SetCurrentDifficulty(previousDifficulty);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, previousTrainerDifficulty);
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}
