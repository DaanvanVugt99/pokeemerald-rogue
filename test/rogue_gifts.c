#include "global.h"
#include "constants/abilities.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "constants/pokemon.h"
#include "pokemon.h"
#include "rogue_gifts.h"
#include "test/test.h"

#define TEST_FORMAT_MON_TYPE                    1
#define TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY    2
#define TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY    3

static u32 DynamicOriginalCustomMonId(u32 move1, u32 move2, u32 move3, u32 ability)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | move1
        | (move2 << 7)
        | (move3 << 14)
        | (ability << 23);
}

static u32 DynamicTypeCustomMonId(u32 type, u32 move1, u32 move2, u32 ability)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | type
        | (move1 << 7)
        | (move2 << 14)
        | (TEST_FORMAT_MON_TYPE << 21)
        | (ability << 23);
}

static u32 DynamicOriginalUniqueAbilityCustomMonId(u32 move1, u32 move2, u32 ability, u32 uniqueAbility)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | move1
        | (move2 << 7)
        | (uniqueAbility << 14)
        | (TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY << 21)
        | (ability << 23);
}

static u32 DynamicTypeUniqueAbilityCustomMonId(u32 type, u32 move1, u32 ability, u32 uniqueAbility)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | type
        | (move1 << 7)
        | (uniqueAbility << 14)
        | (TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY << 21)
        | (ability << 23);
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
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 3, 1);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(SPECIES_SHUCKLE, customMonId), ABILITY_SILVER_LINING);
}

TEST("Dynamic original format decodes epic payload without unique ability")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 0, 1);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_EPIC);
}

TEST("Dynamic type format decodes epic payload without unique ability")
{
    u32 customMonId = DynamicTypeCustomMonId(TYPE_FIRE, 1, 0, 1);

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
    u32 customMonId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 1, 1, 1);

    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 0), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_IMPACT);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_LEGENDARY);
}

TEST("Existing dynamic type format decodes without a unique ability")
{
    u32 customMonId = DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 0);

    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 0), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 1);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_COMMON);
}
