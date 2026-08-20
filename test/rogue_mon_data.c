#include "global.h"
#include "constants/moves.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Optimised GetMonData preserves encrypted Pokémon data")
{
    u32 experience = 0x123456;
    u32 retrievedExperience = 0;
    struct Benchmark optimised;
    struct Benchmark vanilla = { .ticks = 137 };

    CreateMon(&gPlayerParty[0], SPECIES_WOBBUFFET, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, 0x12345678);
    SetMonData(&gPlayerParty[0], MON_DATA_EXP, &experience);

    BENCHMARK(&optimised)
    {
        retrievedExperience = GetMonData(&gPlayerParty[0], MON_DATA_EXP);
    }

    EXPECT_EQ(retrievedExperience, experience);
    EXPECT_FASTER(optimised, vanilla);
}

TEST("Optimised SetMonData preserves checksum and encrypted Pokémon data")
{
    u32 experience = 0x123456;
    struct Benchmark optimised;
    struct Benchmark vanilla = { .ticks = 205 };

    CreateMon(&gPlayerParty[0], SPECIES_WOBBUFFET, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, 0x12345678);

    BENCHMARK(&optimised)
    {
        SetMonData(&gPlayerParty[0], MON_DATA_EXP, &experience);
    }

    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SANITY_IS_BAD_EGG), FALSE);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_EXP), experience);
    EXPECT_FASTER(optimised, vanilla);
}

TEST("Optimised mon data access preserves every encrypted substructure order")
{
    struct Pokemon mon;

    for (u32 personality = 0; personality < 24; personality++)
    {
        u16 heldItem = ITEM_TR50;
        u16 move = MOVE_DRAGON_DARTS;
        u32 experience = 0x123400 + personality;
        u8 pp = 7 + (personality % 8);
        u8 hpEv = 100 + personality;
        u8 abilityNum = personality % NUM_ABILITY_SLOTS;
        u8 shiny = personality % 2;

        CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, TRUE, personality, OT_ID_PLAYER_ID, 0x12345678);
        SetMonData(&mon, MON_DATA_HELD_ITEM, &heldItem);
        SetMonData(&mon, MON_DATA_EXP, &experience);
        SetMonData(&mon, MON_DATA_MOVE1, &move);
        SetMonData(&mon, MON_DATA_PP1, &pp);
        SetMonData(&mon, MON_DATA_HP_EV, &hpEv);
        SetMonData(&mon, MON_DATA_ABILITY_NUM, &abilityNum);
        SetMonData(&mon, MON_DATA_IS_SHINY, &shiny);

        EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_WOBBUFFET);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), heldItem);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_EXP), experience);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), move);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_PP1), pp);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_EV), hpEv);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_ABILITY_NUM), abilityNum);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHINY), shiny);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_SANITY_IS_BAD_EGG), FALSE);

        UpdateMonPersonality(&mon.box, 23 - personality);

        EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_WOBBUFFET);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), heldItem);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_EXP), experience);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), move);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_PP1), pp);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_EV), hpEv);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_ABILITY_NUM), abilityNum);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHINY), shiny);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_SANITY_IS_BAD_EGG), FALSE);
    }
}
