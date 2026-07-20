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
#define TEST_DYNAMIC_MOVE_POOL_CAPACITY       180
#define TEST_DYNAMIC_MOVE_PAIR_CODE_START     (TEST_DYNAMIC_MOVE_POOL_CAPACITY + 1)
#define TEST_TYPED_MOVE_SELECTION_STRIDE      (TEST_DYNAMIC_MOVE_POOL_CAPACITY + 1)
#define TEST_SYNERGY_CHOICE_NATURAL           3

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
        | (ability << 21);
}

static u32 DynamicTypeCustomMonId(u32 type, u32 typeSlot, u32 typeMoveFlip, u32 move1, u32 move2, u32 ability)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | type
        | (typeSlot << 5)
        | (typeMoveFlip << 6)
        | (EncodeTestMoveSelection(move1, move2) << 7)
        | (ability << 21)
        | (TEST_FORMAT_MON_TYPE << 28);
}

static u32 DynamicOriginalUniqueAbilityCustomMonIdWithChoice(u32 move1, u32 move2, u32 abilitySeed, u32 uniqueAbility, u32 synergyChoice)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | EncodeTestMoveSelection(move1, move2)
        | (uniqueAbility << 14)
        | (abilitySeed << 24)
        | (synergyChoice << 26)
        | (TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY << 28);
}

static u32 DynamicOriginalUniqueAbilityCustomMonId(u32 move1, u32 move2, u32 abilitySeed, u32 uniqueAbility)
{
    return DynamicOriginalUniqueAbilityCustomMonIdWithChoice(move1, move2, abilitySeed, uniqueAbility, 0);
}

static u32 DynamicTypeUniqueAbilityCustomMonIdWithChoice(u32 type, u32 typeSlot, u32 typeMoveFlip, u32 move1, u32 abilitySeed, u32 uniqueAbility, u32 synergyChoice)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | EncodeTestTypedUniqueMoveSelection(type, typeSlot, typeMoveFlip, move1)
        | (uniqueAbility << 14)
        | (abilitySeed << 24)
        | (synergyChoice << 26)
        | (TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY << 28);
}

static u32 DynamicTypeUniqueAbilityCustomMonId(u32 type, u32 typeSlot, u32 typeMoveFlip, u32 move1, u32 abilitySeed, u32 uniqueAbility)
{
    return DynamicTypeUniqueAbilityCustomMonIdWithChoice(type, typeSlot, typeMoveFlip, move1, abilitySeed, uniqueAbility, 0);
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

static bool8 IsExpectedCreationSynergyMove(u8 type, u16 move)
{
    static const u16 sMoves[NUMBER_OF_MON_TYPES][3] =
    {
        [TYPE_NORMAL] = { MOVE_SUBSTITUTE, MOVE_RECOVER, MOVE_SWORDS_DANCE },
        [TYPE_FIRE] = { MOVE_WILL_O_WISP },
        [TYPE_WATER] = { MOVE_AQUA_RING },
        [TYPE_POISON] = { MOVE_TOXIC_SPIKES, MOVE_TOXIC },
        [TYPE_ROCK] = { MOVE_STEALTH_ROCK, MOVE_ROCK_POLISH },
        [TYPE_BUG] = { MOVE_STICKY_WEB, MOVE_QUIVER_DANCE },
        [TYPE_GHOST] = { MOVE_DESTINY_BOND, MOVE_CURSE, MOVE_CONFUSE_RAY },
        [TYPE_GRASS] = { MOVE_LEECH_SEED, MOVE_SPORE, MOVE_STRENGTH_SAP },
        [TYPE_ELECTRIC] = { MOVE_CHARGE, MOVE_THUNDER_WAVE, MOVE_MAGNET_RISE },
        [TYPE_PSYCHIC] = { MOVE_TRICK_ROOM, MOVE_CALM_MIND, MOVE_GRAVITY },
        [TYPE_ICE] = { MOVE_HAZE },
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

TEST("Dynamic unique ability eligibility audit contains 555 abilities")
{
    static const u16 sExpectedExclusions[] =
    {
        ABILITY_UNUSED_320,
        ABILITY_PASSIVE_INCOME,
        ABILITY_DEBUG,
        ABILITY_BOUNTY,
        ABILITY_GRAND_REVEAL,
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
        ABILITY_PRIMAL_ECHO,
        ABILITY_PRIMAL_PARASITE,
        ABILITY_PRIMAL_WRAITH,
        ABILITY_PRIMAL_MOLTING,
        ABILITY_PRIMAL_CURRENT,
        ABILITY_SINGULARITY_DRIVE,
        ABILITY_SINGULARITY_TARGETING,
        ABILITY_SINGULARITY_IMPACT,
        ABILITY_SINGULARITY_AIRSPACE,
        ABILITY_SINGULARITY_REACTOR,
        ABILITY_SINGULARITY_OVERLOAD,
        ABILITY_PRIMAL_TIRANNY,
        ABILITY_SINGULARITY_EDGE,
        ABILITY_SINGULARITY_PRISM,
        ABILITY_SINGULARITY_ARRAY,
        ABILITY_PRIMAL_CREST,
        ABILITY_PRIMAL_THUNDER,
        ABILITY_COUNTERPUNCH,
        ABILITY_RAPID_REPLICA,
        ABILITY_COLOSSAL,
        ABILITY_TRASH_ALCHEMY,
        ABILITY_POTENTIAL,
        ABILITY_WHY_NOT,
        ABILITY_PRIMAL_ONSLAUGHT,
        ABILITY_SINGULARITY_CRASH,
        ABILITY_HIDDEN_STASH,
    };
    u16 ability;
    u16 eligibleCount = 0;
    u16 i;

    for(ability = ABILITY_STRONG_WINDS; ability <= ABILITY_FLOCK_STEP; ++ability)
    {
        if(RogueGift_IsDynamicUniqueAbilityEligible(ability))
            ++eligibleCount;
    }

    EXPECT_EQ(eligibleCount, 555);
    EXPECT_EQ(RogueGift_GetDynamicUniqueAbilityPoolCount(), 555);
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_STRONG_WINDS));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_STARMOBILE));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_FLOCK_STEP));

    for(i = 0; i < ARRAY_COUNT(sExpectedExclusions); ++i)
        EXPECT(!RogueGift_IsDynamicUniqueAbilityEligible(sExpectedExclusions[i]));
}

TEST("Dynamic legendary payload stores direct ten bit unique ability ids")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 3, ABILITY_FLOCK_STEP);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_FLOCK_STEP);
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

TEST("Dynamic original legendary replaces one random move with synergy")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_FORMATION);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_NO_RETREAT);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 1), MOVE_CHILLING_WATER);
}

TEST("Dynamic legendary supplies the remaining audited fixed move pairings")
{
    static const struct
    {
        u16 ability;
        u16 move;
        u8 choice;
    } sPairings[] =
    {
        { ABILITY_VOLT_BREAK, MOVE_BOOMBURST, 1 },
        { ABILITY_DYNAMO_FISTS, MOVE_THUNDER, 1 },
        { ABILITY_SOLARBOOST, MOVE_OVERHEAT, 1 },
        { ABILITY_VENDETTA, MOVE_LEAF_BLADE, 0 },
        { ABILITY_SIDEWINDER, MOVE_CRUNCH, 0 },
        { ABILITY_REGAL_DECREE, MOVE_TAUNT, 0 },
        { ABILITY_PSYCHIC_PARRY, MOVE_LEAF_BLADE, 0 },
        { ABILITY_BAG_OF_TRICKS, MOVE_DARK_PULSE, 1 },
        { ABILITY_FAULT_FINDER, MOVE_EARTH_POWER, 1 },
        { ABILITY_ROLLING_START, MOVE_ROLLOUT, 0 },
        { ABILITY_FAMILY_FEUD, MOVE_SURGING_STRIKES, 0 },
    };
    u16 i;

    for(i = 0; i < ARRAY_COUNT(sPairings); ++i)
    {
        u32 customMonId = DynamicOriginalUniqueAbilityCustomMonIdWithChoice(1, 2, 1, sPairings[i].ability, sPairings[i].choice);

        EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
        EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), sPairings[i].move);
        EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 1), MOVE_CHILLING_WATER);
    }
}

TEST("Dynamic unique ability synergy profiles preserve all audited pairings")
{
    u16 ability;
    u16 pairedAbilityCount = 0;

    for(ability = ABILITY_STRONG_WINDS; ability <= ABILITY_FLOCK_STEP; ++ability)
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
            ++moveCount;
            for(j = 0; j < i; ++j)
                EXPECT_NE(moves[i], moves[j]);
        }

        EXPECT_GE(moveCount, 1);
        EXPECT_LE(moveCount, 3);
    }

    EXPECT_EQ(pairedAbilityCount, 247);
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

TEST("Dynamic synergy choices decode all three profile slots and the natural sentinel")
{
    u32 physicalId = DynamicOriginalUniqueAbilityCustomMonIdWithChoice(1, 2, 1, ABILITY_TOXIC_TANDEM, 0);
    u32 specialId = DynamicOriginalUniqueAbilityCustomMonIdWithChoice(1, 2, 1, ABILITY_TOXIC_TANDEM, 1);
    u32 wildcardId = DynamicOriginalUniqueAbilityCustomMonIdWithChoice(1, 2, 1, ABILITY_TOXIC_TANDEM, 2);
    u32 naturalId = DynamicOriginalUniqueAbilityCustomMonIdWithChoice(1, 2, 1, ABILITY_TOXIC_TANDEM, TEST_SYNERGY_CHOICE_NATURAL);
    u32 typedPhysicalId = DynamicTypeUniqueAbilityCustomMonIdWithChoice(TYPE_FIRE, 0, 0, 1, 1, ABILITY_TOXIC_TANDEM, 0);
    u32 typedSpecialId = DynamicTypeUniqueAbilityCustomMonIdWithChoice(TYPE_FIRE, 0, 0, 1, 1, ABILITY_TOXIC_TANDEM, 1);
    u32 typedWildcardId = DynamicTypeUniqueAbilityCustomMonIdWithChoice(TYPE_FIRE, 0, 0, 1, 1, ABILITY_TOXIC_TANDEM, 2);
    u32 typedNaturalId = DynamicTypeUniqueAbilityCustomMonIdWithChoice(TYPE_FIRE, 0, 0, 1, 1, ABILITY_TOXIC_TANDEM, TEST_SYNERGY_CHOICE_NATURAL);
    u32 rollingId = DynamicOriginalUniqueAbilityCustomMonIdWithChoice(1, 2, 1, ABILITY_ROLLING_START, 1);

    EXPECT_EQ(RogueGift_GetCustomMonMove(physicalId, 0), MOVE_GUNK_SHOT);
    EXPECT_EQ(RogueGift_GetCustomMonMove(specialId, 0), MOVE_SLUDGE_BOMB);
    EXPECT_EQ(RogueGift_GetCustomMonMove(wildcardId, 0), MOVE_POISON_JAB);
    EXPECT_EQ(RogueGift_GetCustomMonMove(naturalId, 0), MOVE_HYDRO_STEAM);
    EXPECT_EQ(RogueGift_GetCustomMonMove(naturalId, 1), MOVE_CHILLING_WATER);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedPhysicalId, 1), MOVE_GUNK_SHOT);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedSpecialId, 1), MOVE_SLUDGE_BOMB);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedWildcardId, 1), MOVE_POISON_JAB);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedNaturalId, 1), MOVE_HYDRO_STEAM);
    EXPECT_EQ(RogueGift_GetCustomMonMove(rollingId, 0), MOVE_ICE_BALL);
}

TEST("Dynamic synergy selection respects physical special and mixed profiles")
{
    bool8 mixedSawPhysical = FALSE;
    bool8 mixedSawSpecial = FALSE;
    bool8 mixedSawWildcard = FALSE;
    u16 seed;

    EXPECT(!SpeciesProfileContainsMove(SPECIES_NINJASK, MOVE_GUNK_SHOT));
    EXPECT(!SpeciesProfileContainsMove(SPECIES_NINJASK, MOVE_POISON_JAB));
    EXPECT(!SpeciesProfileContainsMove(SPECIES_ALAKAZAM, MOVE_SLUDGE_BOMB));
    EXPECT(!SpeciesProfileContainsMove(SPECIES_ALAKAZAM, MOVE_POISON_JAB));

    for(seed = 0; seed < 64; ++seed)
    {
        u8 physicalChoice;
        u8 specialChoice;
        u8 mixedChoice;

        SeedRng(seed);
        physicalChoice = RogueGift_DebugSelectDynamicSynergyChoice(SPECIES_NINJASK, ABILITY_TOXIC_TANDEM, MOVE_NONE);
        EXPECT(physicalChoice == 0 || physicalChoice == 2);

        SeedRng(seed);
        specialChoice = RogueGift_DebugSelectDynamicSynergyChoice(SPECIES_ALAKAZAM, ABILITY_TOXIC_TANDEM, MOVE_NONE);
        EXPECT(specialChoice == 1 || specialChoice == 2);

        SeedRng(seed);
        mixedChoice = RogueGift_DebugSelectDynamicSynergyChoice(SPECIES_DITTO, ABILITY_TOXIC_TANDEM, MOVE_NONE);
        if(mixedChoice == 0)
            mixedSawPhysical = TRUE;
        else if(mixedChoice == 1)
            mixedSawSpecial = TRUE;
        else if(mixedChoice == 2)
            mixedSawWildcard = TRUE;
    }

    EXPECT(mixedSawPhysical);
    EXPECT(mixedSawSpecial);
    EXPECT(mixedSawWildcard);
}

TEST("Dynamic typed legendary preserves its type move and replaces its random move with synergy")
{
    u32 customMonId = DynamicTypeUniqueAbilityCustomMonIdWithChoice(TYPE_FIRE, 0, 0, 1, 1, ABILITY_THERMAL_LIFT, 1);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_FIRE_LASH);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 1), MOVE_OVERHEAT);
}

TEST("Dynamic typed legendary retains random move when type move already supplies synergy")
{
    u32 customMonId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FLYING, 0, 0, 1, 1, ABILITY_BRAVERY);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_BRAVE_BIRD);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 1), MOVE_HYDRO_STEAM);
}

TEST("Dynamic original legendary does not duplicate synergy already in encoded moves")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 48, 1, ABILITY_FORMATION);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_HYDRO_STEAM);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 1), MOVE_NO_RETREAT);
}

TEST("Dynamic original legendary preserves both random moves when synergy is naturally learned")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonIdWithChoice(1, 2, 1, ABILITY_FORMATION, TEST_SYNERGY_CHOICE_NATURAL);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_HYDRO_STEAM);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 1), MOVE_CHILLING_WATER);
}

TEST("Dynamic typed legendary preserves its random move when synergy is naturally learned")
{
    u32 customMonId = DynamicTypeUniqueAbilityCustomMonIdWithChoice(TYPE_FIRE, 0, 0, 1, 1, ABILITY_THERMAL_LIFT, TEST_SYNERGY_CHOICE_NATURAL);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_FIRE_LASH);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 1), MOVE_HYDRO_STEAM);
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
    u8 typeMoveFlip;

    for(typeSlot = 0; typeSlot < 2; ++typeSlot)
    {
        for(typeMoveFlip = 0; typeMoveFlip < 2; ++typeMoveFlip)
        {
            u8 selector = (typeSlot << 1) | typeMoveFlip;
            u32 typedId = DynamicTypeCustomMonId(TYPE_FIRE, typeSlot, typeMoveFlip, 0, 0, 0);
            u32 typedUniqueId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, typeSlot, typeMoveFlip, 0, 1, ABILITY_STARMOBILE);

            EXPECT_EQ(RogueGift_GetCustomMonType(typedId, typeSlot), TYPE_FIRE);
            EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 0), sExpectedMoves[selector]);
            EXPECT_EQ(RogueGift_GetCustomMonType(typedUniqueId, typeSlot), TYPE_FIRE);
            EXPECT_EQ(RogueGift_GetCustomMonMove(typedUniqueId, 0), sExpectedMoves[selector]);
        }
    }
}

TEST("Dynamic general move index 165 decodes the expanded pool boundary")
{
    u32 customMonId = DynamicOriginalCustomMonId(165, 0, 0);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 1);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_SWEET_KISS);
}

TEST("Expanded dynamic move selections decode high pairs in every payload format")
{
    u32 originalId = DynamicOriginalCustomMonId(164, 165, 1);
    u32 typedId = DynamicTypeCustomMonId(TYPE_FIRE, 1, 1, 164, 165, 0);
    u32 originalUniqueId = DynamicOriginalUniqueAbilityCustomMonId(164, 165, 1, ABILITY_STARMOBILE);
    u32 typedUniqueId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 1, 1, 165, 1, ABILITY_STARMOBILE);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(originalId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalId, 0), MOVE_MAGNET_RISE);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalId, 1), MOVE_SWEET_KISS);

    EXPECT_EQ(RogueGift_GetCustomMonType(typedId, 1), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(typedId), 3);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 1), MOVE_MAGNET_RISE);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 2), MOVE_SWEET_KISS);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(originalUniqueId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalUniqueId, 0), MOVE_MAGNET_RISE);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalUniqueId, 1), MOVE_SWEET_KISS);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(originalUniqueId), ABILITY_STARMOBILE);

    EXPECT_EQ(RogueGift_GetCustomMonType(typedUniqueId, 1), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(typedUniqueId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedUniqueId, 1), MOVE_SWEET_KISS);
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
        EXPECT(!SpeciesProfileContainsMove(SPECIES_BULBASAUR, move1));
        EXPECT(!SpeciesProfileContainsMove(SPECIES_BULBASAUR, move2));
        ++checkedCount;
    }

    EXPECT_EQ(checkedCount, 32);
}

TEST("Generated legendary standard ability seeds avoid native abilities")
{
    u16 seed;

    for(seed = 0; seed < 128; ++seed)
    {
        u32 customMonId;
        u16 generatedAbility;
        u8 slot;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_BULBASAUR);
        generatedAbility = RogueGift_GetCustomMonAbility(customMonId, 0);

        EXPECT_NE(generatedAbility, ABILITY_NONE);
        for(slot = 0; slot < NUM_ABILITY_SLOTS; ++slot)
            EXPECT_NE(generatedAbility, GetAbilityBySpecies(SPECIES_BULBASAUR, slot, 0));
    }
}

TEST("Generated legendary unique ability never duplicates the species native unique ability")
{
    u16 seed;

    for(seed = 0; seed < 128; ++seed)
    {
        u32 customMonId;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_SHUCKLE);
        EXPECT_NE(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_SILVER_LINING);
    }
}

TEST("Generated legendary preserves random moves when its synergy is already in the species profile")
{
    bool8 sawOriginal = FALSE;
    bool8 sawTyped = FALSE;
    u16 seed;

    for(seed = 0; seed < 2048 && (!sawOriginal || !sawTyped); ++seed)
    {
        u32 customMonId;
        u8 format;
        u16 randomMove;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_GARDEVOIR);
        if(((customMonId >> 26) & 3) != TEST_SYNERGY_CHOICE_NATURAL)
            continue;

        format = (customMonId >> 28) & 3;
        if(format == TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY)
        {
            sawOriginal = TRUE;
            EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
            EXPECT(!SpeciesProfileContainsMove(SPECIES_GARDEVOIR, RogueGift_GetCustomMonMove(customMonId, 0)));
            EXPECT(!SpeciesProfileContainsMove(SPECIES_GARDEVOIR, RogueGift_GetCustomMonMove(customMonId, 1)));
        }
        else if(format == TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY)
        {
            sawTyped = TRUE;
            EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
            randomMove = RogueGift_GetCustomMonMove(customMonId, 1);
            EXPECT(!SpeciesProfileContainsMove(SPECIES_GARDEVOIR, randomMove));
        }
    }

    EXPECT(sawOriginal);
    EXPECT(sawTyped);
}

TEST("Generated flexible synergy excludes every profile candidate from random moves")
{
    bool8 found = FALSE;
    u32 seed;

    for(seed = 0; seed < 8192 && !found; ++seed)
    {
        u32 customMonId;
        u8 profileMoveCount = 0;
        u8 i;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_DITTO);
        if(((customMonId >> 28) & 3) != TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY
         || RogueGift_GetCustomMonUniqueAbility(customMonId) != ABILITY_TOXIC_TANDEM)
            continue;

        for(i = 0; i < RogueGift_GetCustomMonMoveCount(customMonId); ++i)
        {
            u16 move = RogueGift_GetCustomMonMove(customMonId, i);

            if(move == MOVE_GUNK_SHOT || move == MOVE_SLUDGE_BOMB || move == MOVE_POISON_JAB)
                ++profileMoveCount;
        }

        EXPECT_EQ(profileMoveCount, 1);
        found = TRUE;
    }

    EXPECT(found);
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
            if(IsExpectedCreationSynergyMove(creationType, RogueGift_GetCustomMonMove(customMonId, i)))
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
        [TYPE_FIRE] = TRUE,
        [TYPE_WATER] = TRUE,
        [TYPE_POISON] = TRUE,
        [TYPE_ROCK] = TRUE,
        [TYPE_BUG] = TRUE,
        [TYPE_GHOST] = TRUE,
        [TYPE_GRASS] = TRUE,
        [TYPE_ELECTRIC] = TRUE,
        [TYPE_PSYCHIC] = TRUE,
        [TYPE_ICE] = TRUE,
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
                sawTyped = TRUE;
            else
                sawOriginal = TRUE;
        }

        EXPECT(sawOriginal);
        EXPECT(sawTyped);
    }
}
