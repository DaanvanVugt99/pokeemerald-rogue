#include "global.h"
#include "test/test.h"
#include "constants/species.h"
#include "event_data.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"
#include "pokemon.h"

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
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_FURFROU_NATURAL), RogueDebug_GetWildFormFamilyKey(SPECIES_FURFROU_PHARAOH_TRIM));
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_PIKACHU), RogueDebug_GetWildFormFamilyKey(SPECIES_PIKACHU_PARTNER_CAP));
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_PIKACHU), RogueDebug_GetWildFormFamilyKey(SPECIES_PIKACHU_ORIGINAL_CAP));
    EXPECT_NE(RogueDebug_GetWildFormFamilyKey(SPECIES_PIKACHU), RogueDebug_GetWildFormFamilyKey(SPECIES_PICHU));
    EXPECT_NE(RogueDebug_GetWildFormFamilyKey(SPECIES_PIKACHU), RogueDebug_GetWildFormFamilyKey(SPECIES_RAICHU));
    EXPECT_NE(RogueDebug_GetWildFormFamilyKey(SPECIES_PIKACHU), RogueDebug_GetWildFormFamilyKey(SPECIES_PIKACHU_GIGANTAMAX));

    EXPECT_NE(RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS), RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_COMBAT_BREED));
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_COMBAT_BREED), RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_BLAZE_BREED));
    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_COMBAT_BREED), RogueDebug_GetWildFormFamilyKey(SPECIES_TAUROS_PALDEAN_AQUA_BREED));
#else
    ASSUME(FALSE);
#endif
}

TEST("Wild encounter Pikachu family preserves ordinary Pikachu two thirds of the time")
{
#if defined(ROGUE_EXPANSION)
    static const u16 sExpectedSpecialForms[] =
    {
        SPECIES_PIKACHU_COSPLAY,
        SPECIES_PIKACHU_ROCK_STAR,
        SPECIES_PIKACHU_BELLE,
        SPECIES_PIKACHU_POP_STAR,
        SPECIES_PIKACHU_PH_D,
        SPECIES_PIKACHU_LIBRE,
        SPECIES_PIKACHU_HOENN_CAP,
        SPECIES_PIKACHU_SINNOH_CAP,
        SPECIES_PIKACHU_UNOVA_CAP,
        SPECIES_PIKACHU_KALOS_CAP,
        SPECIES_PIKACHU_ALOLA_CAP,
        SPECIES_PIKACHU_PARTNER_CAP,
        SPECIES_PIKACHU_WORLD_CAP,
    };
    u16 i;

    for(i = 0; i < ARRAY_COUNT(sExpectedSpecialForms); ++i)
    {
        EXPECT_EQ(RogueDebug_GetWildApprovedFamilyForm(SPECIES_PIKACHU, i * 3), sExpectedSpecialForms[i]);
        EXPECT_EQ(RogueDebug_GetWildApprovedFamilyForm(SPECIES_PIKACHU, i * 3 + 1), SPECIES_PIKACHU);
        EXPECT_EQ(RogueDebug_GetWildApprovedFamilyForm(SPECIES_PIKACHU, i * 3 + 2), SPECIES_PIKACHU);
    }
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

TEST("Wild encounter approved random forms only spawn Gimmighoul Chest Form")
{
#if defined(ROGUE_EXPANSION)
    u16 i;

    EXPECT_EQ(RogueDebug_GetWildFormFamilyKey(SPECIES_GIMMIGHOUL_CHEST), RogueDebug_GetWildFormFamilyKey(SPECIES_GIMMIGHOUL_ROAMING));

    for(i = 0; i < 8; ++i)
        EXPECT_EQ(RogueDebug_GetWildApprovedFamilyForm(SPECIES_GIMMIGHOUL_ROAMING, i), SPECIES_GIMMIGHOUL_CHEST);
#else
    ASSUME(FALSE);
#endif
}

TEST("Furfrou trims have distinct secondary types")
{
#if defined(ROGUE_EXPANSION)
    static const struct
    {
        u16 species;
        u8 type;
    } sFurfrouTrimTypes[] =
    {
        { SPECIES_FURFROU_NATURAL,        TYPE_NORMAL },
        { SPECIES_FURFROU_HEART_TRIM,     TYPE_FAIRY },
        { SPECIES_FURFROU_STAR_TRIM,      TYPE_ELECTRIC },
        { SPECIES_FURFROU_DIAMOND_TRIM,   TYPE_ROCK },
        { SPECIES_FURFROU_DEBUTANTE_TRIM, TYPE_GRASS },
        { SPECIES_FURFROU_MATRON_TRIM,    TYPE_PSYCHIC },
        { SPECIES_FURFROU_DANDY_TRIM,     TYPE_DARK },
        { SPECIES_FURFROU_LA_REINE_TRIM,  TYPE_ICE },
        { SPECIES_FURFROU_KABUKI_TRIM,    TYPE_FIRE },
        { SPECIES_FURFROU_PHARAOH_TRIM,   TYPE_GROUND },
    };
    u16 i;

    for(i = 0; i < ARRAY_COUNT(sFurfrouTrimTypes); ++i)
    {
        EXPECT_EQ(GetTypeBySpecies(sFurfrouTrimTypes[i].species, 0, 0), TYPE_NORMAL);
        EXPECT_EQ(GetTypeBySpecies(sFurfrouTrimTypes[i].species, 1, 0), sFurfrouTrimTypes[i].type);
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

TEST("Wild encounter Furfrou family is eligible only through Natural Form")
{
#if defined(ROGUE_EXPANSION)
    RogueMonQuery_Begin();

    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_FURFROU_DEBUTANTE_TRIM);

    EXPECT_EQ(RogueDebug_SelectWildSpeciesFromCurrentQuery(0, 99, FALSE), SPECIES_NONE);

    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_FURFROU_NATURAL);

    EXPECT_EQ(RogueDebug_SelectWildSpeciesFromCurrentQuery(0, 99, FALSE), SPECIES_FURFROU_DEBUTANTE_TRIM);

    RogueMonQuery_End();
#else
    ASSUME(FALSE);
#endif
}

TEST("Active run queries include Furfrou trims for their distinct types")
{
#if defined(ROGUE_EXPANSION)
    u8 originalDexVariant = RoguePokedex_GetDexVariant();
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_MAX);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    RogueMonQuery_InvalidateSpeciesActiveCache();

    RogueMonQuery_Begin();
    RogueMonQuery_IsSpeciesActive();
    RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, MON_TYPE_VAL_TO_FLAGS(TYPE_GRASS));

    EXPECT(RogueMiscQuery_CheckState(SPECIES_FURFROU_DEBUTANTE_TRIM));
    EXPECT(!RogueMiscQuery_CheckState(SPECIES_FURFROU_STAR_TRIM));

    RogueMonQuery_End();

    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    RoguePokedex_SetDexVariant(originalDexVariant);
    RogueMonQuery_InvalidateSpeciesActiveCache();
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

TEST("Pokedex family eligibility does not change canonical numbering")
{
#if defined(ROGUE_EXPANSION)
    u8 originalDexVariant = RoguePokedex_GetDexVariant();

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_KANTO_RBY);

    EXPECT_EQ(RoguePokedex_GetCurrentDexLimit(), 151);
    EXPECT_EQ(RoguePokedex_GetSpeciesCurrentNum(SPECIES_MR_MIME), 122);

    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_MIME_JR));
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_CROBAT));
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_ANNIHILAPE));
    EXPECT_EQ(RoguePokedex_GetSpeciesCurrentNum(SPECIES_MIME_JR), 0);
    EXPECT_EQ(RoguePokedex_GetSpeciesCurrentNum(SPECIES_CROBAT), 0);
    EXPECT_EQ(RoguePokedex_GetSpeciesCurrentNum(SPECIES_ANNIHILAPE), 0);

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_KANTO_LETSGO);
    EXPECT_EQ(RoguePokedex_GetCurrentDexLimit(), 153);

    RoguePokedex_SetDexVariant(originalDexVariant);
#else
    ASSUME(FALSE);
#endif
}

TEST("Mega forms inherit display eligibility from their base species")
{
#if defined(ROGUE_EXPANSION)
    u8 originalDexVariant = RoguePokedex_GetDexVariant();

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_KANTO_LETSGO);
    EXPECT(RoguePokedex_IsSpeciesEnabledForDisplay(SPECIES_CHARIZARD));
    EXPECT(RoguePokedex_IsSpeciesEnabledForDisplay(SPECIES_CHARIZARD_MEGA_X));
    EXPECT(RoguePokedex_IsSpeciesEnabledForDisplay(SPECIES_CLEFABLE));
    EXPECT(RoguePokedex_IsSpeciesEnabledForDisplay(SPECIES_CLEFABLE_MEGA));

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_HOENN_RSE);
    EXPECT(!RoguePokedex_IsSpeciesEnabledForDisplay(SPECIES_CHARIZARD_MEGA_X));

    RoguePokedex_SetDexVariant(originalDexVariant);
#else
    ASSUME(FALSE);
#endif
}

TEST("Pokedex family eligibility only permits appropriate regional forms")
{
#if defined(ROGUE_EXPANSION)
    u8 originalDexVariant = RoguePokedex_GetDexVariant();

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_KANTO_LETSGO);
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_RAICHU_ALOLAN));
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_MR_MIME_GALARIAN));
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_MR_RIME));
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_PERRSERKER));

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_GALAR_SWSH);
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_MR_MIME_GALARIAN));
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_MR_RIME));
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_PERRSERKER));
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_RAICHU_ALOLAN));

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_KANTO_RBY);
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_RAICHU_ALOLAN));

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_PALDEA_SCVI);
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_EXEGGUTOR_ALOLAN));

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_PALDEA_BLUEBERRY);
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_EXEGGUTOR_ALOLAN));

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_PALDEA_FULLDLC);
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_EXEGGUTOR_ALOLAN));
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_RAICHU_ALOLAN));

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_UNOVA_BW);
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_BASCULIN_WHITE_STRIPED));
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_BASCULEGION_MALE));
    EXPECT(!RoguePokedex_IsSpeciesEnabled(SPECIES_BASCULEGION_FEMALE));

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_EXTRAS_LEGENDSARCEUS);
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_BASCULIN_WHITE_STRIPED));
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_BASCULEGION_MALE));
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_BASCULEGION_FEMALE));

    RoguePokedex_SetDexVariant(originalDexVariant);
#else
    ASSUME(FALSE);
#endif
}

TEST("Encounter egg transforms can use invisible Pokedex relatives")
{
#if defined(ROGUE_EXPANSION)
    u8 originalDexVariant = RoguePokedex_GetDexVariant();

    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_KANTO_LETSGO);

    RogueMonQuery_Begin();
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, SPECIES_MR_MIME);
    RogueMonQuery_TransformIntoEggSpecies();

    EXPECT(RogueMiscQuery_CheckState(SPECIES_MIME_JR));
    EXPECT(!RogueMiscQuery_CheckState(SPECIES_MR_MIME));
    EXPECT(RoguePokedex_IsSpeciesEnabled(SPECIES_MIME_JR));
    EXPECT_EQ(RoguePokedex_GetSpeciesCurrentNum(SPECIES_MIME_JR), 0);

    RogueMonQuery_End();
    RoguePokedex_SetDexVariant(originalDexVariant);
#else
    ASSUME(FALSE);
#endif
}
