#include "global.h"
#include "constants/abilities.h"
#include "constants/moves.h"
#include "pokemon.h"
#include "rogue.h"
#include "test/test.h"

static bool32 SpeciesHasCompetitiveAbility(u16 species, u16 ability)
{
    u32 abilityIndex;

    if (ability == ABILITY_NONE)
        return TRUE;

    for (abilityIndex = 0; abilityIndex < NUM_ABILITY_SLOTS; ++abilityIndex)
    {
        if (gSpeciesInfo[species].abilities[abilityIndex] == ability)
            return TRUE;
    }

    return FALSE;
}

static bool32 SpeciesFormFamilyHasCompetitiveAbility(u16 species, u16 ability)
{
    const u16 *formSpeciesIdTable;
    u32 formIndex;

    if (SpeciesHasCompetitiveAbility(species, ability))
        return TRUE;

    formSpeciesIdTable = gSpeciesInfo[species].formSpeciesIdTable;
    if (formSpeciesIdTable == NULL)
        return FALSE;

    for (formIndex = 0; formSpeciesIdTable[formIndex] != FORM_SPECIES_END; ++formIndex)
    {
        if (SpeciesHasCompetitiveAbility(formSpeciesIdTable[formIndex], ability))
            return TRUE;
    }

    return FALSE;
}

static bool32 IsFirstCompetitiveProfileOwner(u16 species)
{
    const struct RoguePokemonProfile *profile = &gRoguePokemonProfiles[species];
    u32 priorSpecies;

    for (priorSpecies = 1; priorSpecies < species; ++priorSpecies)
    {
        if (gRoguePokemonProfiles[priorSpecies].competitiveSets == profile->competitiveSets)
            return FALSE;
    }

    return TRUE;
}

static bool32 ProfileCanLearnMove(const struct RoguePokemonProfile *profile, u16 move)
{
    u32 moveIndex;

    for (moveIndex = 0; profile->levelUpMoves[moveIndex].move != MOVE_NONE; ++moveIndex)
    {
        if (profile->levelUpMoves[moveIndex].move == move)
            return TRUE;
    }

    for (moveIndex = 0; profile->tutorMoves[moveIndex] != MOVE_NONE; ++moveIndex)
    {
        if (profile->tutorMoves[moveIndex] == move)
            return TRUE;
    }

    return FALSE;
}

TEST("Competitive profile abilities belong to their species form family")
{
    u32 species;
    u32 setIndex;
    u32 mismatchCount = 0;

    for (species = 1; species < NUM_SPECIES; ++species)
    {
        const struct RoguePokemonProfile *profile = &gRoguePokemonProfiles[species];

        if (profile->competitiveSetCount == 0 || !IsFirstCompetitiveProfileOwner(species))
            continue;

        for (setIndex = 0; setIndex < profile->competitiveSetCount; ++setIndex)
        {
            u16 ability = profile->competitiveSets[setIndex].ability;

            if (!SpeciesFormFamilyHasCompetitiveAbility(species, ability))
            {
                MgbaPrintf_(
                    "Invalid competitive profile ability: species=%d (%S), set=%d, ability=%d",
                    species,
                    gSpeciesInfo[species].speciesName,
                    setIndex,
                    ability
                );
                ++mismatchCount;
            }
        }
    }

    EXPECT_EQ(mismatchCount, 0);
}

TEST("Refreshed Divergence competitive profiles use legal moves")
{
    static const u16 sRefreshedSpecies[] =
    {
        SPECIES_PARASECT,
        SPECIES_VOLBEAT,
        SPECIES_HUNTAIL,
        SPECIES_GOREBYSS,
        SPECIES_ELECTIVIRE,
        SPECIES_FLORGES,
    };
    u32 speciesIndex;
    u32 setIndex;
    u32 mismatchCount = 0;

    for (speciesIndex = 0; speciesIndex < ARRAY_COUNT(sRefreshedSpecies); ++speciesIndex)
    {
        u16 species = sRefreshedSpecies[speciesIndex];
        const struct RoguePokemonProfile *profile = &gRoguePokemonProfiles[species];

        for (setIndex = 0; setIndex < profile->competitiveSetCount; ++setIndex)
        {
            const struct RoguePokemonCompetitiveSet *set = &profile->competitiveSets[setIndex];
            u32 moveIndex;

            for (moveIndex = 0; moveIndex < MAX_MON_MOVES; ++moveIndex)
            {
                u16 move = set->moves[moveIndex];

                if (move == MOVE_NONE || ProfileCanLearnMove(profile, move))
                    continue;

                MgbaPrintf_(
                    "Invalid competitive profile move: species=%d (%S), set=%d, move=%d",
                    species,
                    gSpeciesInfo[species].speciesName,
                    setIndex,
                    move
                );
                ++mismatchCount;
            }
        }
    }

    EXPECT_EQ(mismatchCount, 0);
}

TEST("Liquid Voice Politoed competitive sets contain Hyper Voice")
{
    const struct RoguePokemonProfile *profile = &gRoguePokemonProfiles[SPECIES_POLITOED];
    u32 setIndex;

    for (setIndex = 0; setIndex < profile->competitiveSetCount; ++setIndex)
    {
        const struct RoguePokemonCompetitiveSet *set = &profile->competitiveSets[setIndex];
        bool32 hasHyperVoice = FALSE;
        u32 moveIndex;

        if (set->ability != ABILITY_LIQUID_VOICE)
            continue;

        for (moveIndex = 0; moveIndex < MAX_MON_MOVES; ++moveIndex)
        {
            if (set->moves[moveIndex] == MOVE_HYPER_VOICE)
            {
                hasHyperVoice = TRUE;
                break;
            }
        }

        EXPECT(hasHyperVoice);
    }
}

TEST("Unown forms share their expanded Power profile")
{
    static const u16 sExpectedMoves[] =
    {
        MOVE_HIDDEN_POWER,
        MOVE_SECRET_POWER,
        MOVE_ANCIENT_POWER,
        MOVE_COSMIC_POWER,
        MOVE_POWER_GEM,
        MOVE_STORED_POWER,
        MOVE_EARTH_POWER,
    };
    const struct RoguePokemonProfile *profile = &gRoguePokemonProfiles[SPECIES_UNOWN];
    const u16 *formSpeciesIdTable = gSpeciesInfo[SPECIES_UNOWN].formSpeciesIdTable;
    u32 formIndex;
    u32 moveIndex;
    u32 setIndex;

    EXPECT_EQ(gSpeciesInfo[SPECIES_UNOWN].baseSpeed, 72);

    for (formIndex = 0; formSpeciesIdTable[formIndex] != FORM_SPECIES_END; ++formIndex)
    {
        u16 formSpecies = formSpeciesIdTable[formIndex];
        const struct RoguePokemonProfile *formProfile = &gRoguePokemonProfiles[formSpecies];

        EXPECT_EQ(gSpeciesInfo[formSpecies].baseSpeed, 72);
        EXPECT(formProfile->levelUpMoves == profile->levelUpMoves);
        EXPECT(formProfile->tutorMoves == profile->tutorMoves);
        EXPECT(formProfile->competitiveSets == profile->competitiveSets);
    }

    for (moveIndex = 0; moveIndex < ARRAY_COUNT(sExpectedMoves); ++moveIndex)
        EXPECT(ProfileCanLearnMove(profile, sExpectedMoves[moveIndex]));

    EXPECT_EQ(profile->competitiveSetCount, 2);
    for (setIndex = 0; setIndex < profile->competitiveSetCount; ++setIndex)
    {
        for (moveIndex = 0; moveIndex < MAX_MON_MOVES; ++moveIndex)
        {
            EXPECT_NE(profile->competitiveSets[setIndex].moves[moveIndex], MOVE_NONE);
            EXPECT(ProfileCanLearnMove(profile, profile->competitiveSets[setIndex].moves[moveIndex]));
        }
    }
}

TEST("Mega Starmie keeps its Huge Power adjusted Attack")
{
    EXPECT_EQ(gSpeciesInfo[SPECIES_STARMIE_MEGA].baseAttack, 100);
    EXPECT_EQ(gSpeciesInfo[SPECIES_STARMIE_MEGA].abilities[0], ABILITY_HUGE_POWER);
}

TEST("Mightyena has the Speed to capitalize on Full Moon")
{
    EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA].baseSpeed, 100);
}

TEST("Focused stat buffs support underperforming Pokemon's unique roles")
{
    EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK].baseHP, 80);
    EXPECT_EQ(gSpeciesInfo[SPECIES_WIGGLYTUFF].baseDefense, 65);
    EXPECT_EQ(gSpeciesInfo[SPECIES_WIGGLYTUFF].baseSpDefense, 65);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PARASECT].baseHP, 80);
    EXPECT_EQ(gSpeciesInfo[SPECIES_DEWGONG].baseDefense, 100);
    EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO].baseSpAttack, 95);
}
