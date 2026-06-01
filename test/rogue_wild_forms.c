#include "global.h"
#include "test/test.h"
#include "constants/species.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"

#if defined(ROGUE_EXPANSION)
static bool8 IsMiniorMeteorForm(u16 species)
{
    switch(species)
    {
    case SPECIES_MINIOR_METEOR_RED:
    case SPECIES_MINIOR_METEOR_ORANGE:
    case SPECIES_MINIOR_METEOR_YELLOW:
    case SPECIES_MINIOR_METEOR_GREEN:
    case SPECIES_MINIOR_METEOR_BLUE:
    case SPECIES_MINIOR_METEOR_INDIGO:
    case SPECIES_MINIOR_METEOR_VIOLET:
        return TRUE;
    }

    return FALSE;
}
#endif

TEST("Wild encounter form families collapse only explicit curated forms")
{
#if defined(ROGUE_EXPANSION)
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_SQUAWKABILLY_GREEN_PLUMAGE), RogueDebug_GetWildFormFamilyKey(SPECIES_SQUAWKABILLY_BLUE_PLUMAGE));
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_ROTOM_HEAT), RogueDebug_GetWildFormFamilyKey(SPECIES_ROTOM_WASH));
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_UNOWN), RogueDebug_GetWildFormFamilyKey(SPECIES_UNOWN_QMARK));

    EXPECT_NE(RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS), RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_COMBAT_BREED));
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_COMBAT_BREED), RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_BLAZE_BREED));
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_COMBAT_BREED), RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_AQUA_BREED));
#else
    ASSUME(FALSE);
#endif
}

TEST("Wild encounter approved random forms exclude Minior core forms")
{
#if defined(ROGUE_EXPANSION)
    u16 i;

    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_MINIOR_METEOR_RED), RogueDebug_GetWildFormFamilyKey(SPECIES_MINIOR_CORE_RED));

    for(i = 0; i < 32; ++i)
    {
        u16 species = RogueDebug_GetWildApprovedFamilyForm(SPECIES_MINIOR_METEOR_RED, i);

        EXPECT(IsMiniorMeteorForm(species));
    }
#else
    ASSUME(FALSE);
#endif
}

TEST("Wild encounter family selection can select beyond old raw weight capacity")
{
#if defined(ROGUE_EXPANSION)
    u16 species;
    const u16 finalSpecies = 300;

    RogueMonQuery_Begin();

    for(species = SPECIES_NONE + 1; species <= finalSpecies; ++species)
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, species);

    EXPECT_EQ(RogueDebug_SelectWildSpeciesFromCurrentQuery(finalSpecies - 1, 0, FALSE), finalSpecies);

    RogueMonQuery_End();
#else
    ASSUME(FALSE);
#endif
}

TEST("Wild encounter family selection excludes all selected sibling forms")
{
#if defined(ROGUE_EXPANSION)
    RogueMonQuery_Begin();

    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_PIDGEY);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_SQUAWKABILLY_GREEN_PLUMAGE);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_SQUAWKABILLY_BLUE_PLUMAGE);

    EXPECT_EQ(RogueDebug_SelectWildSpeciesFromCurrentQuery(1, 0, TRUE), SPECIES_SQUAWKABILLY_GREEN_PLUMAGE);
    EXPECT(RogueMiscQuery_CheckState(SPECIES_PIDGEY));
    EXPECT(!RogueMiscQuery_CheckState(SPECIES_SQUAWKABILLY_GREEN_PLUMAGE));
    EXPECT(!RogueMiscQuery_CheckState(SPECIES_SQUAWKABILLY_BLUE_PLUMAGE));

    RogueMonQuery_End();
#else
    ASSUME(FALSE);
#endif
}

TEST("Wild encounter family selection collapses all eligible Rotom forms")
{
#if defined(ROGUE_EXPANSION)
    RogueMonQuery_Begin();

    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_ROTOM);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_ROTOM_HEAT);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_ROTOM_WASH);

    EXPECT_EQ(RogueDebug_SelectWildSpeciesFromCurrentQuery(0, 1, TRUE), SPECIES_ROTOM_HEAT);
    EXPECT(!RogueMiscQuery_CheckState(SPECIES_ROTOM));
    EXPECT(!RogueMiscQuery_CheckState(SPECIES_ROTOM_HEAT));
    EXPECT(!RogueMiscQuery_CheckState(SPECIES_ROTOM_WASH));

    RogueMonQuery_End();
#else
    ASSUME(FALSE);
#endif
}

TEST("Wild encounter family selection does not leak inactive Rotom forms")
{
#if defined(ROGUE_EXPANSION)
    RogueMonQuery_Begin();

    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_ROTOM_HEAT);

    EXPECT_EQ(RogueDebug_SelectWildSpeciesFromCurrentQuery(0, 99, FALSE), SPECIES_ROTOM_HEAT);

    RogueMonQuery_End();
#else
    ASSUME(FALSE);
#endif
}

TEST("Wild encounter Vivillon forms redirect to Vivillon in the Pokedex")
{
#if defined(ROGUE_EXPANSION)
    EXPECT_EQ(RoguePokedex_RedirectSpeciesGetSetFlag(SPECIES_VIVILLON_POLAR), SPECIES_VIVILLON);
    EXPECT_EQ(RoguePokedex_RedirectSpeciesGetSetFlag(SPECIES_VIVILLON_POKE_BALL), SPECIES_VIVILLON);
#else
    ASSUME(FALSE);
#endif
}
