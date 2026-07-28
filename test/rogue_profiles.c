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
