#include "global.h"
#include "constants/abilities.h"
#include "constants/moves.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "constants/pokemon.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_gifts.h"
#include "test/test.h"

#define TEST_FORMAT_MON_TYPE                    1
#define TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY    2
#define TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY    3
#define TEST_DYNAMIC_MOVE_POOL_CAPACITY       160
#define TEST_DYNAMIC_MOVE_PAIR_CODE_START     (TEST_DYNAMIC_MOVE_POOL_CAPACITY + 1)
#define TEST_TYPED_MOVE_SELECTION_STRIDE      (TEST_DYNAMIC_MOVE_POOL_CAPACITY + 1)

static u32 EncodeTestMoveSelection(u32 move1, u32 move2)
{
    u32 first;
    u32 second;
    u32 pairRank;

    if(move1 == 0)
        return move2;
    if(move2 == 0)
        return move1;

    first = move1 - 1;
    second = move2 - 1;
    if(first > second)
    {
        u32 temp = first;
        first = second;
        second = temp;
    }

    pairRank = first * (2 * TEST_DYNAMIC_MOVE_POOL_CAPACITY - first - 1) / 2;
    pairRank += second - first - 1;
    return TEST_DYNAMIC_MOVE_PAIR_CODE_START + pairRank;
}

static u32 EncodeTestTypedUniqueMoveSelection(u32 type, u32 typeSlot, u32 typeMoveFlip, u32 move)
{
    u32 typeSelection = ((type * 2 + typeSlot) * 2 + typeMoveFlip);

    return typeSelection * TEST_TYPED_MOVE_SELECTION_STRIDE + move;
}

static u32 DynamicOriginalCustomMonId(u32 move1, u32 move2, u32 ability)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | EncodeTestMoveSelection(move1, move2)
        | (ability << 23);
}

static u32 DynamicTypeCustomMonId(u32 type, u32 typeSlot, u32 typeMoveFlip, u32 move1, u32 move2, u32 ability)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | type
        | (typeSlot << 5)
        | (typeMoveFlip << 6)
        | (EncodeTestMoveSelection(move1, move2) << 7)
        | (TEST_FORMAT_MON_TYPE << 21)
        | (ability << 23);
}

static u32 DynamicOriginalUniqueAbilityCustomMonId(u32 move1, u32 move2, u32 ability, u32 uniqueAbility)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | EncodeTestMoveSelection(move1, move2)
        | (uniqueAbility << 14)
        | (TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY << 21)
        | (ability << 23);
}

static u32 DynamicTypeUniqueAbilityCustomMonId(u32 type, u32 typeSlot, u32 typeMoveFlip, u32 move1, u32 ability, u32 uniqueAbility)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | EncodeTestTypedUniqueMoveSelection(type, typeSlot, typeMoveFlip, move1)
        | (uniqueAbility << 14)
        | (TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY << 21)
        | (ability << 23);
}

static bool8 SpeciesProfileContainsMove(u16 species, u16 move)
{
    u16 i;

    for(i = 0; gRoguePokemonProfiles[species].levelUpMoves[i].move != MOVE_NONE; ++i)
    {
        if(gRoguePokemonProfiles[species].levelUpMoves[i].move == move)
            return TRUE;
    }

    for(i = 0; gRoguePokemonProfiles[species].tutorMoves[i] != MOVE_NONE; ++i)
    {
        if(gRoguePokemonProfiles[species].tutorMoves[i] == move)
            return TRUE;
    }

    return FALSE;
}

TEST("Dynamic custom unique ability lookup falls back for ordinary species")
{
    EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(SPECIES_SHUCKLE, 0), ABILITY_SILVER_LINING);
}

TEST("Dynamic custom unique ability lookup overrides species mapping")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, 1);
    struct Pokemon mon;

    CreateMon(&mon, SPECIES_SHUCKLE, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_CUSTOM_MON, customMonId);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_IMPACT);
    EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(SPECIES_SHUCKLE, customMonId), ABILITY_IMPACT);
    EXPECT_EQ(GetMonUniqueAbility(&mon), ABILITY_IMPACT);
}

TEST("Dynamic custom unique ability lookup falls back when the encoded id has none")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 1);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(SPECIES_SHUCKLE, customMonId), ABILITY_SILVER_LINING);
}

TEST("Dynamic original format decodes epic payload without unique ability")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 1);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_EPIC);
}

TEST("Dynamic type format decodes epic payload without unique ability")
{
    u32 customMonId = DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 1, 0, 1);

    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 0), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_EPIC);
}

TEST("Dynamic original unique ability format decodes legendary payload")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, 1);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_IMPACT);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_LEGENDARY);
}

TEST("Dynamic type unique ability format decodes legendary payload")
{
    u32 customMonId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 0, 0, 1, 1, 1);

    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 0), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_IMPACT);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_LEGENDARY);
}

TEST("Existing dynamic type format decodes without a unique ability")
{
    u32 customMonId = DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 0, 0, 0);

    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 0), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 1);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_COMMON);
}

TEST("Dynamic type formats decode all four altered-type move selectors")
{
    static const u16 sExpectedMoves[] =
    {
        MOVE_FIRE_LASH,
        MOVE_WILL_O_WISP,
        MOVE_OVERHEAT,
        MOVE_SACRED_FIRE,
    };
    u8 typeSlot;
    u8 typeMoveFlip;

    for(typeSlot = 0; typeSlot < 2; ++typeSlot)
    {
        for(typeMoveFlip = 0; typeMoveFlip < 2; ++typeMoveFlip)
        {
            u8 selector = (typeSlot << 1) | typeMoveFlip;
            u32 typedId = DynamicTypeCustomMonId(TYPE_FIRE, typeSlot, typeMoveFlip, 0, 0, 0);
            u32 typedUniqueId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, typeSlot, typeMoveFlip, 0, 1, 1);

            EXPECT_EQ(RogueGift_GetCustomMonType(typedId, typeSlot), TYPE_FIRE);
            EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 0), sExpectedMoves[selector]);
            EXPECT_EQ(RogueGift_GetCustomMonType(typedUniqueId, typeSlot), TYPE_FIRE);
            EXPECT_EQ(RogueGift_GetCustomMonMove(typedUniqueId, 0), sExpectedMoves[selector]);
        }
    }
}

TEST("Dynamic general move index 154 decodes the expanded pool boundary")
{
    u32 customMonId = DynamicOriginalCustomMonId(154, 0, 0);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 1);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_FELL_STINGER);
}

TEST("Expanded dynamic move selections decode high pairs in every payload format")
{
    u32 originalId = DynamicOriginalCustomMonId(153, 154, 1);
    u32 typedId = DynamicTypeCustomMonId(TYPE_FIRE, 1, 1, 153, 154, 0);
    u32 originalUniqueId = DynamicOriginalUniqueAbilityCustomMonId(153, 154, 1, 1);
    u32 typedUniqueId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 1, 1, 154, 1, 1);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(originalId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalId, 0), MOVE_DESTINY_BOND);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalId, 1), MOVE_FELL_STINGER);

    EXPECT_EQ(RogueGift_GetCustomMonType(typedId, 1), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(typedId), 3);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 1), MOVE_DESTINY_BOND);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 2), MOVE_FELL_STINGER);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(originalUniqueId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalUniqueId, 0), MOVE_DESTINY_BOND);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalUniqueId, 1), MOVE_FELL_STINGER);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(originalUniqueId), ABILITY_IMPACT);

    EXPECT_EQ(RogueGift_GetCustomMonType(typedUniqueId, 1), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(typedUniqueId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedUniqueId, 1), MOVE_FELL_STINGER);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(typedUniqueId), ABILITY_IMPACT);
}

TEST("Generated dynamic general moves are distinct and exclude the species profile")
{
    u16 seed;
    u8 checkedCount = 0;

    for(seed = 0; seed < 256 && checkedCount < 32; ++seed)
    {
        u32 customMonId;
        u16 move1;
        u16 move2;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_EPIC, SPECIES_BULBASAUR);

        if(((customMonId >> 21) & 3) != 0)
            continue;

        move1 = RogueGift_GetCustomMonMove(customMonId, 0);
        move2 = RogueGift_GetCustomMonMove(customMonId, 1);
        EXPECT_NE(move1, move2);
        EXPECT(!SpeciesProfileContainsMove(SPECIES_BULBASAUR, move1));
        EXPECT(!SpeciesProfileContainsMove(SPECIES_BULBASAUR, move2));
        ++checkedCount;
    }

    EXPECT_EQ(checkedCount, 32);
}

TEST("Dynamic generation round-trips every rarity and typing format")
{
    static const u8 sRarities[] =
    {
        UNIQUE_RARITY_COMMON,
        UNIQUE_RARITY_RARE,
        UNIQUE_RARITY_EPIC,
        UNIQUE_RARITY_LEGENDARY,
    };
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sRarities); ++i)
    {
        bool8 sawOriginal = FALSE;
        bool8 sawTyped = FALSE;
        u16 seed;

        for(seed = 0; seed < 256 && (!sawOriginal || !sawTyped); ++seed)
        {
            u8 format;
            u32 customMonId;

            SeedRng(seed);
            customMonId = RogueGift_CreateDynamicMonIdRaw(sRarities[i], SPECIES_BULBASAUR);
            format = (customMonId >> 21) & 3;

            EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), sRarities[i]);
            if(format == TEST_FORMAT_MON_TYPE || format == TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY)
                sawTyped = TRUE;
            else
                sawOriginal = TRUE;
        }

        EXPECT(sawOriginal);
        EXPECT(sawTyped);
    }
}
