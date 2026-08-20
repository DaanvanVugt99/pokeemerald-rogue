#include "global.h"
#include "constants/moves.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "pokemon.h"
#include "test/test.h"

static const u8 sCanonicalSubstructOffsets[24][4] =
{
    {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 3, 1, 2},
    {0, 2, 3, 1}, {0, 3, 2, 1}, {1, 0, 2, 3}, {1, 0, 3, 2},
    {2, 0, 1, 3}, {3, 0, 1, 2}, {2, 0, 3, 1}, {3, 0, 2, 1},
    {1, 2, 0, 3}, {1, 3, 0, 2}, {2, 1, 0, 3}, {3, 1, 0, 2},
    {2, 3, 0, 1}, {3, 2, 0, 1}, {1, 2, 3, 0}, {1, 3, 2, 0},
    {2, 1, 3, 0}, {3, 1, 2, 0}, {2, 3, 1, 0}, {3, 2, 1, 0},
};

static void CreateCanonicalEncryptedMonFixture(struct Pokemon *mon, u32 personality)
{
    struct BoxPokemon *boxMon = &mon->box;
    const u8 *offsets = sCanonicalSubstructOffsets[personality % 24];
    u32 encryptionKey = 0x12345678 ^ personality;
    u32 checksum = 0;

    memset(mon, 0, sizeof(*mon));
    boxMon->personality = personality;
    boxMon->otId = 0x12345678;
    boxMon->hasSpecies = TRUE;
    boxMon->secure.substructs[offsets[0]].type0.species = SPECIES_WOBBUFFET;
    boxMon->secure.substructs[offsets[0]].type0.heldItem = ITEM_TR50 & 0x3FF;
    boxMon->secure.substructs[offsets[0]].type0.heldItemHi = ITEM_TR50 >> 10;
    boxMon->secure.substructs[offsets[0]].type0.experience = 0x123400 + personality;
    boxMon->secure.substructs[offsets[1]].type1.moves[0] = MOVE_DRAGON_DARTS;
    boxMon->secure.substructs[offsets[1]].type1.pp[0] = 7 + (personality % 8);
    boxMon->secure.substructs[offsets[2]].type2.hpEV = 100 + personality;
    boxMon->secure.substructs[offsets[3]].type3.abilityNum = personality % NUM_ABILITY_SLOTS;
    boxMon->secure.substructs[offsets[3]].type3.isShiny = personality % 2;

    for (u32 i = 0; i < ARRAY_COUNT(boxMon->secure.raw); i++)
        checksum += boxMon->secure.raw[i] + (boxMon->secure.raw[i] >> 16);
    boxMon->checksum = checksum;

    for (u32 i = 0; i < ARRAY_COUNT(boxMon->secure.raw); i++)
        boxMon->secure.raw[i] ^= encryptionKey;
}

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

TEST("Optimised mon data access preserves canonical substructure orders")
{
    struct Pokemon mon;

    for (u32 personality = 0; personality < 24; personality++)
    {
        u32 updatedExperience = 0x154300 + personality;

        CreateCanonicalEncryptedMonFixture(&mon, personality);

        EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_WOBBUFFET);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_TR50);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_EXP), 0x123400 + personality);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), MOVE_DRAGON_DARTS);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_PP1), 7 + (personality % 8));
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_EV), 100 + personality);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_ABILITY_NUM), personality % NUM_ABILITY_SLOTS);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHINY), personality % 2);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_SANITY_IS_BAD_EGG), FALSE);

        SetMonData(&mon, MON_DATA_EXP, &updatedExperience);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_EXP), updatedExperience);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_WOBBUFFET);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), MOVE_DRAGON_DARTS);
    }
}
