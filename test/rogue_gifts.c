#include "global.h"
#include "constants/abilities.h"
#include "constants/moves.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "constants/pokemon.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_safari.h"
#include "test/test.h"

#define TEST_FORMAT_MON_TYPE                    1
#define TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY    2
#define TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY    3
#define TEST_DYNAMIC_MOVE_POOL_CAPACITY       255
#define TEST_DYNAMIC_MOVE_PAIR_CODE_START     (TEST_DYNAMIC_MOVE_POOL_CAPACITY + 1)

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

static u32 DynamicOriginalCustomMonId(u32 move1, u32 move2, u32 ability)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | EncodeTestMoveSelection(move1, move2)
        | (ability << 21);
}

static u32 DynamicTypeCustomMonId(u32 type, u32 typeSlot, u32 typeMoveChoice, u32 move1, u32 move2, u32 ability)
{
    u32 rarity;
    u32 payload;

    if(ability != 0)
    {
        rarity = UNIQUE_RARITY_EPIC;
        payload = move1 | (ability << 8);
    }
    else if(move1 != 0 || move2 != 0)
    {
        rarity = UNIQUE_RARITY_RARE;
        payload = EncodeTestMoveSelection(move1, move2);
    }
    else
    {
        rarity = UNIQUE_RARITY_COMMON;
        payload = 0;
    }

    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | type
        | (typeSlot << 5)
        | (typeMoveChoice << 6)
        | (payload << 8)
        | (rarity << 23)
        | (TEST_FORMAT_MON_TYPE << 28);
}

static u32 DynamicOriginalUniqueAbilityCustomMonId(u32 move1, u32 move2, u32 abilitySeed, u32 uniqueAbility)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | EncodeTestMoveSelection(move1, move2)
        | (uniqueAbility << 15)
        | (abilitySeed << 25)
        | (TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY << 28);
}

static u32 DynamicTypeUniqueAbilityCustomMonId(u32 type, u32 typeSlot, u32 typeMoveChoice, u32 move1, u32 abilitySeed, u32 uniqueAbility)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | type
        | (typeSlot << 5)
        | (typeMoveChoice << 6)
        | (move1 << 8)
        | (uniqueAbility << 16)
        | (abilitySeed << 26)
        | (TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY << 28);
}

static bool8 IsExpectedCreationSynergyMove(u8 type, u16 move)
{
    static const u16 sMoves[NUMBER_OF_MON_TYPES][3] =
    {
        [TYPE_NORMAL] = { MOVE_SUBSTITUTE, MOVE_RECOVER, MOVE_SWORDS_DANCE },
        [TYPE_FIGHTING] = { MOVE_BULK_UP },
        [TYPE_FLYING] = { MOVE_ROOST },
        [TYPE_FIRE] = { MOVE_WILL_O_WISP },
        [TYPE_WATER] = { MOVE_AQUA_RING },
        [TYPE_POISON] = { MOVE_TOXIC_SPIKES, MOVE_TOXIC },
        [TYPE_GROUND] = { MOVE_SPIKES },
        [TYPE_ROCK] = { MOVE_STEALTH_ROCK, MOVE_ROCK_POLISH },
        [TYPE_BUG] = { MOVE_STICKY_WEB, MOVE_QUIVER_DANCE },
        [TYPE_GHOST] = { MOVE_DESTINY_BOND, MOVE_CURSE, MOVE_CONFUSE_RAY },
        [TYPE_STEEL] = { MOVE_SHIFT_GEAR, MOVE_IRON_DEFENSE },
        [TYPE_GRASS] = { MOVE_LEECH_SEED, MOVE_SPORE, MOVE_STRENGTH_SAP },
        [TYPE_ELECTRIC] = { MOVE_CHARGE, MOVE_THUNDER_WAVE, MOVE_MAGNET_RISE },
        [TYPE_PSYCHIC] = { MOVE_TRICK_ROOM, MOVE_CALM_MIND, MOVE_GRAVITY },
        [TYPE_ICE] = { MOVE_HAZE },
        [TYPE_DRAGON] = { MOVE_DRAGON_DANCE },
        [TYPE_DARK] = { MOVE_TAUNT, MOVE_NASTY_PLOT, MOVE_TORMENT },
        [TYPE_FAIRY] = { MOVE_CHARM, MOVE_SWEET_KISS },
    };
    u8 i;

    if(move == MOVE_NONE)
        return FALSE;

    for(i = 0; i < ARRAY_COUNT(sMoves[type]); ++i)
    {
        if(sMoves[type][i] == move)
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
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_IMPACT);
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

TEST("Safari reconstruction preserves a unique Pokemon's native ability slot")
{
    static const u16 sExpectedAbilities[] =
    {
        ABILITY_LEVITATE,
        ABILITY_NEUTRALIZING_GAS,
    };
    u32 customMonId = DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 0, 0, 0);
    u8 abilityNum;

    for(abilityNum = 0; abilityNum < ARRAY_COUNT(sExpectedAbilities); ++abilityNum)
    {
        struct Pokemon original;
        struct Pokemon restored;
        struct RogueSafariMon safariMon = {0};
        u16 eggSpecies = SPECIES_KOFFING;

        CreateMon(&original, SPECIES_WEEZING, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_CUSTOM_MON, customMonId);
        SetMonData(&original, MON_DATA_ABILITY_NUM, &abilityNum);
        RogueSafari_CopyToSafariMon(&original.box, &safariMon);

        ZeroMonData(&restored);
        CreateMon(&restored, SPECIES_KOFFING, 5, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
        RogueSafari_CopyFromSafariMon(&safariMon, &restored.box);
        SetMonData(&restored, MON_DATA_SPECIES, &eggSpecies);
        Rogue_ApplyCustomMonIdToMon(customMonId, &restored);

        EXPECT_EQ(GetMonData(&restored, MON_DATA_ABILITY_NUM), abilityNum);
        EXPECT_EQ(GetMonAbility(&restored), sExpectedAbilities[abilityNum]);
    }
}

TEST("Reapplying custom data keeps an explicit standard ability authoritative")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 1);
    struct Pokemon mon;
    u8 abilityNum = 1;

    CreateMon(&mon, SPECIES_WEEZING, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &abilityNum);
    Rogue_ApplyCustomMonIdToMon(customMonId, &mon);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_ABILITY_NUM), 0);
    EXPECT_EQ(GetMonAbility(&mon), RogueGift_GetCustomMonAbility(customMonId, 0));
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
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_IMPACT);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_IMPACT);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_LEGENDARY);
}

TEST("Dynamic unique ability pool includes Starmobile")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_STARMOBILE);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_STARMOBILE);
}

TEST("Dynamic unique ability eligibility audit contains 577 abilities")
{
    static const u16 sExpectedExclusions[] =
    {
        ABILITY_UNUSED_320,
        ABILITY_PASSIVE_INCOME,
        ABILITY_DEBUG,
        ABILITY_BOUNTY,
        ABILITY_BITTER_RUSE,
        ABILITY_ULTRA_FALLOUT,
        ABILITY_ULTRA_SWOLE,
        ABILITY_ULTRA_STRUT,
        ABILITY_ULTRA_CONDUCTOR,
        ABILITY_ULTRA_ASCENT,
        ABILITY_ULTRA_EDGE,
        ABILITY_ULTRA_DEVOUR,
        ABILITY_ULTRA_VENIN,
        ABILITY_ULTRA_INJECTION,
        ABILITY_ULTRA_BASTION,
        ABILITY_ULTRA_DETONATION,
        ABILITY_UNSPEAKABLE,
        ABILITY_BLACK_ICE,
        ABILITY_ECHO_INSTINCT,
        ABILITY_SPORE_INSTINCT,
        ABILITY_HEX_INSTINCT,
        ABILITY_MOLT_INSTINCT,
        ABILITY_FLUX_INSTINCT,
        ABILITY_DRIVE_PROTOCOL,
        ABILITY_AIM_PROTOCOL,
        ABILITY_IMPACT_PROTOCOL,
        ABILITY_LOCK_PROTOCOL,
        ABILITY_HEAT_PROTOCOL,
        ABILITY_SURGE_PROTOCOL,
        ABILITY_HUNT_INSTINCT,
        ABILITY_EDGE_PROTOCOL,
        ABILITY_MIND_PROTOCOL,
        ABILITY_ARRAY_PROTOCOL,
        ABILITY_GUARD_INSTINCT,
        ABILITY_STORM_INSTINCT,
        ABILITY_COUNTERPUNCH,
        ABILITY_KINGS_DOMAIN,
        ABILITY_COLOSSAL,
        ABILITY_TRASH_ALCHEMY,
        ABILITY_POTENTIAL,
        ABILITY_WHY_NOT,
        ABILITY_SIEGE_INSTINCT,
        ABILITY_CRASH_PROTOCOL,
        ABILITY_HIDDEN_STASH,
        ABILITY_VANISHING_ACT,
    };
    u16 ability;
    u16 eligibleCount = 0;
    u16 i;

    for(ability = ABILITY_STRONG_WINDS; ability <= ABILITY_COUNTERSTEP; ++ability)
    {
        if(RogueGift_IsDynamicUniqueAbilityEligible(ability))
            ++eligibleCount;
    }

    EXPECT_EQ(eligibleCount, 577);
    EXPECT_EQ(RogueGift_GetDynamicUniqueAbilityPoolCount(), 577);
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_STRONG_WINDS));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_STARMOBILE));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_FLOCK_STEP));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_COLD_PREMONITION));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_DEAD_HEAT));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SPIRIT_FEAST));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_EMERALD_ACCORD));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SAND_COMMAND));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_BLAST_SHIELD));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_FLAK_SHIELD));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_BOG_BODY));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_GUIDING_FLAMES));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_FALSE_GROUND));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_TUNDRA_REIGN));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_ASTRAL_REIGN));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SWORD_AND_BOARD));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_WANDERING_HUNTER));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SMOG_REFINERY));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SNOWBALL_FIGHT));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_PREMONITION));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_HORN_LOCK));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_COUNTERSPELL));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_FEVER_PITCH));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_RAPID_REPLICA));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_MENHIR));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_COUNTERSTEP));

    for(i = 0; i < ARRAY_COUNT(sExpectedExclusions); ++i)
        EXPECT(!RogueGift_IsDynamicUniqueAbilityEligible(sExpectedExclusions[i]));
}

TEST("Dynamic legendary payload stores direct ten bit unique ability ids")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 3, ABILITY_COUNTERSPELL);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_COUNTERSPELL);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
}

TEST("Dynamic legendary standard ability seed decodes stably")
{
    u32 seedOneId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_STARMOBILE);
    u32 seedThreeId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 3, ABILITY_STARMOBILE);

    EXPECT_EQ(RogueGift_GetCustomMonAbility(seedOneId, 0), RogueGift_GetCustomMonAbility(seedOneId, 0));
    EXPECT_NE(RogueGift_GetCustomMonAbility(seedOneId, 0), ABILITY_NONE);
    EXPECT_NE(RogueGift_GetCustomMonAbility(seedThreeId, 0), ABILITY_NONE);
    EXPECT_NE(RogueGift_GetCustomMonAbility(seedOneId, 0), RogueGift_GetCustomMonAbility(seedThreeId, 0));
}



TEST("Dynamic unique ability synergy profiles preserve all audited pairings")
{
    u16 ability;
    u16 pairedAbilityCount = 0;

    for(ability = ABILITY_STRONG_WINDS; ability <= ABILITY_COUNTERSTEP; ++ability)
    {
        u16 profileId = RogueGift_DebugGetDynamicSynergyProfileId(ability);
        u16 moves[3];
        u8 moveCount = 0;
        u8 i;
        u8 j;

        if(profileId == 0)
            continue;

        ++pairedAbilityCount;
        EXPECT_LE(RogueGift_DebugGetDynamicSynergyPolicy(ability), 1);
        for(i = 0; i < ARRAY_COUNT(moves); ++i)
        {
            moves[i] = RogueGift_DebugGetDynamicSynergyMove(ability, i);
            if(moves[i] == MOVE_NONE)
                continue;

            EXPECT_LT(moves[i], MOVES_COUNT);
            EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ability, moves[i]));
            EXPECT(RogueGift_DebugIsMoveInDynamicPool(moves[i]));
            ++moveCount;
            for(j = 0; j < i; ++j)
                EXPECT_NE(moves[i], moves[j]);
        }

        EXPECT_GE(moveCount, 1);
        EXPECT_LE(moveCount, 3);
    }

    EXPECT_EQ(pairedAbilityCount, 253);
}

TEST("Newest dynamic unique abilities use their required synergy profiles")
{
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_TUNDRA_REIGN), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_ASTRAL_REIGN), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SWORD_AND_BOARD, 0), MOVE_PROTECT);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_WANDERING_HUNTER), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_FALSE_GROUND), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_SMOG_REFINERY), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_PREMONITION), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_COUNTERSPELL), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_FEVER_PITCH), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_VANISHING_ACT), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_MENHIR), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_COUNTERSTEP), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_RAPID_REPLICA, 0), MOVE_TRANSFORM);
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_RAPID_REPLICA, MOVE_TRANSFORM));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_RAPID_REPLICA, MOVE_COPYCAT));
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_HORN_LOCK, 0), MOVE_RAGING_BULL);
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_HORN_LOCK, MOVE_RAGING_BULL));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_HORN_LOCK, MOVE_CLOSE_COMBAT));
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_HORN_LOCK, MOVE_TACKLE, MOVE_CELEBRATE), MOVE_RAGING_BULL);
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_HORN_LOCK, MOVE_RAGING_BULL, MOVE_CELEBRATE), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SNOWBALL_FIGHT, 0), MOVE_FLING);
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SNOWBALL_FIGHT, MOVE_FLING));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SNOWBALL_FIGHT, MOVE_U_TURN));
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_SNOWBALL_FIGHT, MOVE_TACKLE, MOVE_CELEBRATE), MOVE_FLING);
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_SNOWBALL_FIGHT, MOVE_FLING, MOVE_CELEBRATE), MOVE_NONE);
}

TEST("Direct move synergy uses battle semantics instead of learnability")
{
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_PROTECT));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_DETECT));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_SPIKY_SHIELD));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_BANEFUL_BUNKER));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_SWORDS_DANCE));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_GUIDING_FLAMES, MOVE_SHADOW_BALL));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_GUIDING_FLAMES, MOVE_CONFUSE_RAY));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_GUIDING_FLAMES, MOVE_FLAMETHROWER));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_FLOWER_FIELD, MOVE_PROTECT));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_FLOWER_FIELD, MOVE_TAUNT));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_FLOWER_FIELD, MOVE_BODY_SLAM));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_BRAVERY, MOVE_ROOST));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_WATER_GLIDE, MOVE_HURRICANE));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_WATER_GLIDE, MOVE_ROOST));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_BAG_OF_TRICKS, MOVE_TAUNT));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_BAG_OF_TRICKS, MOVE_HEAL_PULSE));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_RUMBLE_ROLL, MOVE_EARTH_POWER));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_RUMBLE_ROLL, MOVE_SPIKES));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SIGHTING_SYSTEM, MOVE_DYNAMIC_PUNCH));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SIGHTING_SYSTEM, MOVE_INFERNO));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SIGHTING_SYSTEM, MOVE_SING));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SIGHTING_SYSTEM, MOVE_FOCUS_BLAST));

    SeedRng(0);
    EXPECT_LT(RogueGift_DebugSelectDynamicSynergyChoice(SPECIES_DITTO, ABILITY_SWORD_AND_BOARD, MOVE_NONE), 3);
}

TEST("Qualifying custom moves preserve the rest of the rolled payload")
{
    // A random exotic move can satisfy the interaction.
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_TOXIC_TANDEM, MOVE_GUNK_SHOT, MOVE_HYDRO_STEAM), MOVE_NONE);

    // The changed-type move is checked by the same semantic matcher.
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_THERMAL_LIFT, MOVE_FIRE_LASH, MOVE_HYDRO_STEAM), MOVE_NONE);

    // Without a qualifying custom move, one curated candidate is requested.
    SeedRng(0);
    EXPECT_NE(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_SWORD_AND_BOARD, MOVE_HYDRO_STEAM, MOVE_CHILLING_WATER), MOVE_NONE);
}

TEST("Galarian bird unique abilities use the required legendary synergy profiles")
{
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_COLD_PREMONITION, 0), MOVE_FREEZING_GLARE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_COLD_PREMONITION, 1), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_COLD_PREMONITION, 2), MOVE_NONE);

    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_DEAD_HEAT, 0), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_DEAD_HEAT, 1), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_DEAD_HEAT, 2), MOVE_NONE);

    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SPIRIT_FEAST, 0), MOVE_NIGHT_SLASH);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SPIRIT_FEAST, 1), MOVE_DARK_PULSE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SPIRIT_FEAST, 2), MOVE_FOUL_PLAY);
}

TEST("Final Step generated synergy guarantees a sound move")
{
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_FINAL_STEP, 0), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_FINAL_STEP, 1), MOVE_BOOMBURST);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_FINAL_STEP, 2), MOVE_SNARL);
}

TEST("Dynamic Unique pool only includes Gimmighoul Chest Form")
{
#ifdef ROGUE_EXPANSION
    EXPECT(RogueGift_DebugIsSpeciesInDynamicUniquePool(SPECIES_GIMMIGHOUL_CHEST));
    EXPECT(!RogueGift_DebugIsSpeciesInDynamicUniquePool(SPECIES_GIMMIGHOUL_ROAMING));
#else
    ASSUME(FALSE);
#endif
}

TEST("Every eligible dynamic species has enough family-exotic moves")
{
    EXPECT(RogueGift_DebugAllDynamicSpeciesHaveExoticMoves(2));
}


TEST("Dynamic synergy selection respects physical special and mixed profiles")
{
    u16 physicalCounts[3] = {0};
    u16 specialCounts[3] = {0};
    u16 seed;

    for(seed = 0; seed < 256; ++seed)
    {
        SeedRng(seed);
        ++physicalCounts[RogueGift_DebugSelectDynamicSynergyChoice(SPECIES_NINJASK, ABILITY_TOXIC_TANDEM, MOVE_NONE)];
        SeedRng(seed);
        ++specialCounts[RogueGift_DebugSelectDynamicSynergyChoice(SPECIES_ALAKAZAM, ABILITY_TOXIC_TANDEM, MOVE_NONE)];
    }

    // All valid categories remain possible, while the stronger offensive stat
    // makes its matching category more likely.
    EXPECT_GT(physicalCounts[0], 0);
    EXPECT_GT(physicalCounts[1], 0);
    EXPECT_GT(physicalCounts[2], 0);
    EXPECT_GT(specialCounts[0], 0);
    EXPECT_GT(specialCounts[1], 0);
    EXPECT_GT(specialCounts[2], 0);
    EXPECT_GT(physicalCounts[0], physicalCounts[1]);
    EXPECT_GT(specialCounts[1], specialCounts[0]);
}






TEST("Dynamic type unique ability format decodes legendary payload")
{
    u32 customMonId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 0, 0, 1, 1, ABILITY_IMPACT);

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
    u8 typeMoveChoice;

    for(typeSlot = 0; typeSlot < 2; ++typeSlot)
    {
        for(typeMoveChoice = 0; typeMoveChoice < 4; ++typeMoveChoice)
        {
            u32 typedId = DynamicTypeCustomMonId(TYPE_FIRE, typeSlot, typeMoveChoice, 0, 0, 0);
            u32 typedUniqueId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, typeSlot, typeMoveChoice, 0, 1, ABILITY_STARMOBILE);

            EXPECT_EQ(RogueGift_GetCustomMonType(typedId, typeSlot), TYPE_FIRE);
            EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 0), sExpectedMoves[typeMoveChoice]);
            EXPECT_EQ(RogueGift_GetCustomMonType(typedUniqueId, typeSlot), TYPE_FIRE);
            EXPECT_EQ(RogueGift_GetCustomMonMove(typedUniqueId, 0), sExpectedMoves[typeMoveChoice]);
        }
    }
}

TEST("Generated altered typings can roll every move choice in either slot")
{
    bool8 seen[2][4] = {{FALSE}};
    u16 seed;
    u8 seenCount = 0;

    for(seed = 0; seed < 2048 && seenCount < 8; ++seed)
    {
        u32 customMonId;
        u8 slot;
        u8 choice;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_COMMON, SPECIES_BULBASAUR);
        if(((customMonId >> 28) & 3) != TEST_FORMAT_MON_TYPE)
            continue;

        slot = (customMonId >> 5) & 1;
        choice = (customMonId >> 6) & 3;
        if(!seen[slot][choice])
        {
            seen[slot][choice] = TRUE;
            ++seenCount;
        }
    }

    EXPECT_EQ(seenCount, 8);
}

TEST("Dynamic general move index 165 decodes the expanded pool boundary")
{
    u32 customMonId = DynamicOriginalCustomMonId(165, 0, 0);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 1);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_SWEET_KISS);
}

TEST("Expanded dynamic move selections decode high pairs in every payload format")
{
    u16 upper = RogueGift_DebugGetDynamicMovePoolCount();
    u32 originalId = DynamicOriginalCustomMonId(upper - 1, upper, 1);
    u32 typedId = DynamicTypeCustomMonId(TYPE_FIRE, 1, 1, upper - 1, upper, 0);
    u32 originalUniqueId = DynamicOriginalUniqueAbilityCustomMonId(upper - 1, upper, 1, ABILITY_STARMOBILE);
    u32 typedUniqueId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 1, 1, upper, 1, ABILITY_STARMOBILE);

    EXPECT_LE(upper, 255);
    EXPECT_EQ(RogueGift_DebugGetDynamicExoticMoveCount(), 165);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(originalId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalId, 0), RogueGift_DebugGetDynamicMoveByIndex(upper - 1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalId, 1), RogueGift_DebugGetDynamicMoveByIndex(upper));

    EXPECT_EQ(RogueGift_GetCustomMonType(typedId, 1), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(typedId), 3);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 1), RogueGift_DebugGetDynamicMoveByIndex(upper - 1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 2), RogueGift_DebugGetDynamicMoveByIndex(upper));

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(originalUniqueId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalUniqueId, 0), RogueGift_DebugGetDynamicMoveByIndex(upper - 1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalUniqueId, 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(originalUniqueId), ABILITY_STARMOBILE);

    EXPECT_EQ(RogueGift_GetCustomMonType(typedUniqueId, 1), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(typedUniqueId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedUniqueId, 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(typedUniqueId), ABILITY_STARMOBILE);
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

        if(((customMonId >> 28) & 3) != 0)
            continue;

        move1 = RogueGift_GetCustomMonMove(customMonId, 0);
        move2 = RogueGift_GetCustomMonMove(customMonId, 1);
        EXPECT_NE(move1, move2);
        EXPECT(!RogueGift_DebugIsMoveNativeToEvolutionFamily(SPECIES_BULBASAUR, move1));
        EXPECT(!RogueGift_DebugIsMoveNativeToEvolutionFamily(SPECIES_BULBASAUR, move2));
        ++checkedCount;
    }

    EXPECT_EQ(checkedCount, 32);
}

TEST("Received movesets keep custom moves first without duplicates after evolution")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 0);
    struct Pokemon baseMon;
    struct Pokemon evolvedMon;
    u8 i;
    u8 j;

    RogueGift_CreateMon(customMonId, &baseMon, SPECIES_BULBASAUR, 50, USE_RANDOM_IVS);
    RogueGift_CreateMon(customMonId, &evolvedMon, SPECIES_VENUSAUR, 50, USE_RANDOM_IVS);

    EXPECT_EQ(GetMonData(&baseMon, MON_DATA_MOVE1), RogueGift_GetCustomMonMove(customMonId, 0));
    EXPECT_EQ(GetMonData(&baseMon, MON_DATA_MOVE2), RogueGift_GetCustomMonMove(customMonId, 1));
    EXPECT_EQ(GetMonData(&evolvedMon, MON_DATA_MOVE1), RogueGift_GetCustomMonMove(customMonId, 0));
    EXPECT_EQ(GetMonData(&evolvedMon, MON_DATA_MOVE2), RogueGift_GetCustomMonMove(customMonId, 1));

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        u16 baseMove = GetMonData(&baseMon, MON_DATA_MOVE1 + i);
        u16 evolvedMove = GetMonData(&evolvedMon, MON_DATA_MOVE1 + i);

        for(j = i + 1; j < MAX_MON_MOVES; ++j)
        {
            if(baseMove != MOVE_NONE)
                EXPECT_NE(baseMove, GetMonData(&baseMon, MON_DATA_MOVE1 + j));
            if(evolvedMove != MOVE_NONE)
                EXPECT_NE(evolvedMove, GetMonData(&evolvedMon, MON_DATA_MOVE1 + j));
        }
    }
}

TEST("Evolved dynamic gifts can permanently relearn their custom moves")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 0);
    struct Pokemon evolvedMon;
    u16 relearnableMoves[255];
    u16 replacementMove = MOVE_TACKLE;
    u8 relearnableCount;
    u8 customMove;
    u8 i;

    RogueGift_CreateMon(customMonId, &evolvedMon, SPECIES_VENUSAUR, 50, USE_RANDOM_IVS);
    for(i = 0; i < MAX_MON_MOVES; ++i)
        SetMonData(&evolvedMon, MON_DATA_MOVE1 + i, &replacementMove);

    relearnableCount = GetMoveRelearnerMoves(&evolvedMon, relearnableMoves);
    for(customMove = 0; customMove < RogueGift_GetCustomMonMoveCount(customMonId); ++customMove)
    {
        bool8 found = FALSE;

        for(i = 0; i < relearnableCount; ++i)
        {
            if(relearnableMoves[i] == RogueGift_GetCustomMonMove(customMonId, customMove))
                found = TRUE;
        }
        EXPECT(found);
    }
}

TEST("Generated abilities are non-native across rarity and evolution branches")
{
    static const u16 sSpecies[] =
    {
        SPECIES_BULBASAUR,
        SPECIES_EEVEE,
        SPECIES_RALTS,
    };
    static const u8 sRarities[] =
    {
        UNIQUE_RARITY_RARE,
        UNIQUE_RARITY_EPIC,
        UNIQUE_RARITY_LEGENDARY,
    };
    u8 speciesIndex;
    u8 rarityIndex;
    u16 seed;

    for(speciesIndex = 0; speciesIndex < ARRAY_COUNT(sSpecies); ++speciesIndex)
    {
        for(rarityIndex = 0; rarityIndex < ARRAY_COUNT(sRarities); ++rarityIndex)
        {
            for(seed = 0; seed < 32; ++seed)
            {
                u32 customMonId;
                u16 standardAbility;
                u16 uniqueAbility;

                SeedRng(seed);
                customMonId = RogueGift_CreateDynamicMonIdRaw(sRarities[rarityIndex], sSpecies[speciesIndex]);
                standardAbility = RogueGift_GetCustomMonAbility(customMonId, 0);
                uniqueAbility = RogueGift_GetCustomMonUniqueAbility(customMonId);

                if(standardAbility != ABILITY_NONE)
                    EXPECT(!RogueGift_DebugIsStandardAbilityNativeToEvolutionFamily(sSpecies[speciesIndex], standardAbility));
                if(uniqueAbility != ABILITY_NONE)
                    EXPECT(!RogueGift_DebugIsUniqueAbilityNativeToEvolutionFamily(sSpecies[speciesIndex], uniqueAbility));
            }
        }
    }
}

TEST("Generated altered type is distinct from both natural types")
{
    static const u16 sSpecies[] =
    {
        SPECIES_BULBASAUR,
        SPECIES_CHARIZARD,
        SPECIES_GYARADOS,
    };
    u8 speciesIndex;
    u16 seed;

    for(speciesIndex = 0; speciesIndex < ARRAY_COUNT(sSpecies); ++speciesIndex)
    {
        for(seed = 0; seed < 256; ++seed)
        {
            u32 customMonId;
            u8 format;
            u8 newType;

            SeedRng(seed);
            customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_COMMON, sSpecies[speciesIndex]);
            format = (customMonId >> 28) & 3;
            if(format != TEST_FORMAT_MON_TYPE)
                continue;

            newType = customMonId & 0x1F;
            EXPECT_NE(newType, gSpeciesInfo[sSpecies[speciesIndex]].types[0]);
            EXPECT_NE(newType, gSpeciesInfo[sSpecies[speciesIndex]].types[1]);
        }
    }
}

TEST("Every packed rarity format decodes low and upper move indices")
{
    u16 upper = RogueGift_DebugGetDynamicMovePoolCount();
    u32 ids[] =
    {
        DynamicOriginalCustomMonId(1, upper, 0),
        DynamicOriginalCustomMonId(upper, 0, 1),
        DynamicOriginalCustomMonId(1, upper, 1),
        DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 0, 0, 0),
        DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 1, upper, 0),
        DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, upper, 0, 1),
        DynamicOriginalUniqueAbilityCustomMonId(1, upper, 1, ABILITY_STARMOBILE),
        DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 0, 0, upper, 1, ABILITY_STARMOBILE),
    };

    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[0]), UNIQUE_RARITY_COMMON);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[0], 0), RogueGift_DebugGetDynamicMoveByIndex(1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[0], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[1]), UNIQUE_RARITY_RARE);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[1], 0), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[2]), UNIQUE_RARITY_EPIC);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[2], 0), RogueGift_DebugGetDynamicMoveByIndex(1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[2], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));

    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[3]), UNIQUE_RARITY_COMMON);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[4]), UNIQUE_RARITY_RARE);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[4], 1), RogueGift_DebugGetDynamicMoveByIndex(1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[4], 2), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[5]), UNIQUE_RARITY_EPIC);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[5], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));

    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[6]), UNIQUE_RARITY_LEGENDARY);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[6], 0), RogueGift_DebugGetDynamicMoveByIndex(1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[6], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[7]), UNIQUE_RARITY_LEGENDARY);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[7], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
}

TEST("Generated legendary standard ability seeds avoid native abilities")
{
    u16 seed;

    for(seed = 0; seed < 128; ++seed)
    {
        u32 customMonId;
        u16 generatedAbility;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_BULBASAUR);
        generatedAbility = RogueGift_GetCustomMonAbility(customMonId, 0);

        EXPECT_NE(generatedAbility, ABILITY_NONE);
        EXPECT(!RogueGift_DebugIsStandardAbilityNativeToEvolutionFamily(SPECIES_BULBASAUR, generatedAbility));
    }
}

TEST("Generated legendary unique ability never duplicates the species native unique ability")
{
    static const struct
    {
        u16 species;
        u16 uniqueAbility;
    } sCases[] =
    {
        { SPECIES_SHUCKLE,  ABILITY_SILVER_LINING },
        { SPECIES_RAYQUAZA, ABILITY_EMERALD_ACCORD },
        { SPECIES_LANDORUS, ABILITY_SAND_COMMAND },
        { SPECIES_MINIOR,   ABILITY_BLAST_SHIELD },
        { SPECIES_CLODSIRE, ABILITY_BOG_BODY },
    };
    u8 i;
    u16 seed;

    for(i = 0; i < ARRAY_COUNT(sCases); ++i)
    {
        for(seed = 0; seed < 128; ++seed)
        {
            u32 customMonId;

            SeedRng(seed);
            customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, sCases[i].species);
            EXPECT(!RogueGift_DebugIsUniqueAbilityNativeToEvolutionFamily(sCases[i].species, RogueGift_GetCustomMonUniqueAbility(customMonId)));
            EXPECT_NE(RogueGift_GetCustomMonUniqueAbility(customMonId), sCases[i].uniqueAbility);
        }
    }
}

TEST("Generated legendary direct interactions are usable in both typing formats")
{
    bool8 sawOriginal = FALSE;
    bool8 sawTyped = FALSE;
    u16 seed;

    for(seed = 0; seed < 4096 && (!sawOriginal || !sawTyped); ++seed)
    {
        u32 customMonId;
        u16 ability;
        u8 format;
        u8 i;
        bool8 found = FALSE;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_DITTO);
        ability = RogueGift_GetCustomMonUniqueAbility(customMonId);
        if(RogueGift_DebugGetDynamicSynergyProfileId(ability) == 0)
            continue;

        for(i = 0; i < RogueGift_GetCustomMonMoveCount(customMonId); ++i)
        {
            if(RogueGift_DebugDoesMoveMatchDynamicSynergy(ability, RogueGift_GetCustomMonMove(customMonId, i)))
                found = TRUE;
        }
        EXPECT(found);

        format = (customMonId >> 28) & 3;
        if(format == TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY)
            sawOriginal = TRUE;
        else if(format == TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY)
            sawTyped = TRUE;
    }

    EXPECT(sawOriginal);
    EXPECT(sawTyped);
}



TEST("Generated Creation legendary includes a status move matching its effective primary type")
{
    bool8 sawOriginal = FALSE;
    bool8 sawTyped = FALSE;
    u32 seed;

    for(seed = 0; seed < 8192 && (!sawOriginal || !sawTyped); ++seed)
    {
        u32 customMonId;
        u8 format;
        u8 creationType;
        u16 i;
        bool8 foundMove = FALSE;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_DITTO);
        if(RogueGift_GetCustomMonUniqueAbility(customMonId) != ABILITY_CREATION)
            continue;

        format = (customMonId >> 28) & 3;
        creationType = RogueGift_GetCustomMonType(customMonId, 0);
        if(creationType == TYPE_NONE)
            creationType = TYPE_NORMAL;
        for(i = 0; i < RogueGift_GetCustomMonMoveCount(customMonId); ++i)
        {
            if(RogueGift_DebugDoesMoveMatchCreationSynergy(creationType, RogueGift_GetCustomMonMove(customMonId, i)))
                foundMove = TRUE;
        }

        EXPECT(foundMove);
        if(format == TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY)
            sawOriginal = TRUE;
        else if(format == TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY)
            sawTyped = TRUE;
    }

    EXPECT(sawOriginal);
    EXPECT(sawTyped);
}

TEST("Creation synergy profiles select valid moves for every supported type")
{
    static const bool8 sSupportedTypes[NUMBER_OF_MON_TYPES] =
    {
        [TYPE_NORMAL] = TRUE,
        [TYPE_FIGHTING] = TRUE,
        [TYPE_FLYING] = TRUE,
        [TYPE_FIRE] = TRUE,
        [TYPE_WATER] = TRUE,
        [TYPE_POISON] = TRUE,
        [TYPE_GROUND] = TRUE,
        [TYPE_ROCK] = TRUE,
        [TYPE_BUG] = TRUE,
        [TYPE_GHOST] = TRUE,
        [TYPE_STEEL] = TRUE,
        [TYPE_GRASS] = TRUE,
        [TYPE_ELECTRIC] = TRUE,
        [TYPE_PSYCHIC] = TRUE,
        [TYPE_ICE] = TRUE,
        [TYPE_DRAGON] = TRUE,
        [TYPE_DARK] = TRUE,
        [TYPE_FAIRY] = TRUE,
    };
    u8 type;

    for(type = 0; type < NUMBER_OF_MON_TYPES; ++type)
    {
        u16 move;

        SeedRng(type);
        move = RogueGift_DebugSelectCreationSynergyMove(SPECIES_DITTO, type, MOVE_NONE);
        if(sSupportedTypes[type])
            EXPECT(IsExpectedCreationSynergyMove(type, move));
        else
            EXPECT_EQ(move, MOVE_NONE);
    }
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
    static const u8 sOriginalMoveCounts[] = { 2, 1, 2, 2 };
    static const u8 sTypedMoveCounts[] = { 1, 3, 2, 2 };
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
            format = (customMonId >> 28) & 3;

            EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), sRarities[i]);
            if(format == TEST_FORMAT_MON_TYPE || format == TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY)
            {
                sawTyped = TRUE;
                EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), sTypedMoveCounts[i]);
            }
            else
            {
                sawOriginal = TRUE;
                EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), sOriginalMoveCounts[i]);
            }
        }

        EXPECT(sawOriginal);
        EXPECT(sawTyped);
    }
}
