#include "global.h"
#include "event_data.h"
#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_pokedex_selection.h"
#include "rogue_run_start.h"
#include "rogue_settings.h"
#include "rogue_trials.h"
#include "string_util.h"
#include "text.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/vars.h"
#include "test/test.h"

#define POOL_BIT(v) (1ULL << (v))
#define ALL_POOLS (~0ULL >> (64 - POKEDEX_VARIANT_COUNT))

static u64 CuratedPools(void)
{
    u64 mask = 0;
    u8 v;
    for (v = 0; v < POKEDEX_VARIANT_COUNT; ++v)
        if (RoguePokedex_IsCuratedVariant(v)) mask |= POOL_BIT(v);
    return mask;
}

TEST("Pokemon pool: every roster is reachable under its category")
{
    struct RoguePokedexSelection selection;
    u8 v, count, i, choices[POKEDEX_VARIANT_COUNT];
    EXPECT_EQ(RogueDexSelection_GetRegion(POKEDEX_VARIANT_EXTRAS_LEGENDSARCEUS), DEX_REGION_HISUI);
    EXPECT_EQ(RogueDexSelection_GetRegion(POKEDEX_VARIANT_LEGENDS_ZA), POKEDEX_REGION_KALOS);
    EXPECT_EQ(RogueDexSelection_GetType(POKEDEX_VARIANT_EXTRAS_COLOSSEUM), DEX_POOL_SPECIAL);
    for (v = 0; v < POKEDEX_VARIANT_COUNT; ++v)
    {
        EXPECT(RogueDexSelection_Init(&selection, ALL_POOLS, v, ITEM_NONE, FALSE));
        EXPECT_EQ(selection.variant, v);
        count = RogueDexSelection_GetChoices(&selection, DEX_FIELD_VARIANT, choices);
        for (i = 0; i < count && choices[i] != v; ++i) {}
        EXPECT_LT(i, count);
        if (RogueDexSelection_GetType(v) == DEX_POOL_REGIONAL)
        {
            count = RogueDexSelection_GetChoices(&selection, DEX_FIELD_REGION, choices);
            for (i = 0; i < count && choices[i] != RogueDexSelection_GetRegion(v); ++i) {}
            EXPECT_LT(i, count);
        }
    }
}

TEST("Pokemon pool: curated navigation never exposes excluded rosters")
{
    struct RoguePokedexSelection selection;
    u64 allowed = CuratedPools();
    u8 v, field, step, choices[POKEDEX_VARIANT_COUNT];
    s8 direction;
    for (v = 0; v < POKEDEX_VARIANT_COUNT; ++v)
    {
        if (!(allowed & POOL_BIT(v))) continue;
        for (direction = -1; direction <= 1; direction += 2)
            for (field = DEX_FIELD_TYPE; field <= DEX_FIELD_VARIANT; ++field)
            {
                EXPECT(RogueDexSelection_Init(&selection, allowed, v, ITEM_NONE, TRUE));
                for (step = 0; step < POKEDEX_VARIANT_COUNT; ++step)
                {
                    RogueDexSelection_Cycle(&selection, field, direction);
                    EXPECT(allowed & POOL_BIT(selection.variant));
                    EXPECT_NE(RogueDexSelection_GetType(selection.variant), DEX_POOL_SPECIAL);
                    EXPECT_NE(RogueDexSelection_GetRegion(selection.variant), DEX_REGION_HISUI);
                }
            }
    }
    EXPECT(RogueDexSelection_Init(&selection, allowed, POKEDEX_VARIANT_NONE, ITEM_NONE, TRUE));
    EXPECT_EQ(selection.variant, POKEDEX_VARIANT_KANTO_RBY);
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_TYPE, choices), 2);
    EXPECT(RogueDexSelection_Init(&selection, allowed, POKEDEX_VARIANT_EXTRAS_COLOSSEUM, ITEM_NONE, FALSE));
    EXPECT_EQ(selection.variant, POKEDEX_VARIANT_EXTRAS_COLOSSEUM);
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_TYPE, choices), 3);
    EXPECT(!RogueDexSelection_Init(&selection, 0, 0, ITEM_NONE, TRUE));
    EXPECT_EQ(selection.variant, POKEDEX_VARIANT_NONE);
    EXPECT(!RogueDexSelection_Cycle(&selection, DEX_FIELD_TYPE, 1));
}

TEST("Pokemon pool: single choices vanish and restricted variants wrap")
{
    struct RoguePokedexSelection selection;
    u8 choices[POKEDEX_VARIANT_COUNT];
    u64 allowed = POOL_BIT(POKEDEX_VARIANT_SINNOH_DP) | POOL_BIT(POKEDEX_VARIANT_SINNOH_PL);
    EXPECT(RogueDexSelection_Init(&selection, allowed, POKEDEX_VARIANT_SINNOH_DP, ITEM_NONE, TRUE));
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_TYPE, choices), 1);
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_REGION, choices), 1);
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_VARIANT, choices), 2);
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_GIMMICK, choices), 0);
    EXPECT(!RogueDexSelection_Cycle(&selection, DEX_FIELD_REGION, 1));
    EXPECT(RogueDexSelection_Cycle(&selection, DEX_FIELD_VARIANT, -1));
    EXPECT_EQ(selection.variant, POKEDEX_VARIANT_SINNOH_PL);
    EXPECT(RogueDexSelection_Cycle(&selection, DEX_FIELD_VARIANT, 1));
    EXPECT_EQ(selection.variant, POKEDEX_VARIANT_SINNOH_DP);
    EXPECT(RogueDexSelection_Init(&selection, POOL_BIT(POKEDEX_VARIANT_EXTRAS_LEGENDSARCEUS), 0, ITEM_NONE, FALSE));
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_VARIANT, choices), 1);
}

TEST("Pokemon pool: National gimmicks survive regional previews without leaking")
{
    struct RoguePokedexSelection selection;
    u8 choices[POKEDEX_VARIANT_COUNT];
    u64 allowed = POOL_BIT(POKEDEX_VARIANT_HOENN_ORAS) | POOL_BIT(POKEDEX_VARIANT_NATIONAL_GEN9);
    EXPECT(RogueDexSelection_Init(&selection, allowed, POKEDEX_VARIANT_NATIONAL_GEN9, ITEM_TERA_ORB, TRUE));
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_GIMMICK, choices), 5);
    EXPECT_EQ(RogueDexSelection_GetGimmick(&selection), ITEM_TERA_ORB);
    RogueDexSelection_Cycle(&selection, DEX_FIELD_TYPE, -1);
    EXPECT_EQ(selection.variant, POKEDEX_VARIANT_HOENN_ORAS);
    EXPECT_EQ(RogueDexSelection_GetGimmick(&selection), ITEM_MEGA_RING);
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_GIMMICK, choices), 0);
    EXPECT(!RogueDexSelection_Cycle(&selection, DEX_FIELD_GIMMICK, 1));
    RogueDexSelection_Cycle(&selection, DEX_FIELD_TYPE, 1);
    EXPECT_EQ(RogueDexSelection_GetGimmick(&selection), ITEM_TERA_ORB);
    RogueDexSelection_Cycle(&selection, DEX_FIELD_GIMMICK, 1);
    EXPECT_EQ(RogueDexSelection_GetGimmick(&selection), ITEM_NONE);
    RogueDexSelection_Cycle(&selection, DEX_FIELD_GIMMICK, -1);
    EXPECT_EQ(RogueDexSelection_GetGimmick(&selection), ITEM_TERA_ORB);
    selection.intro = FALSE;
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_GIMMICK, choices), 0);
}

TEST("Pokemon pool: every intro choice saves the previewed pool and gimmick")
{
    struct RoguePokedexSelection selection;
    struct RogueAdventureConfig before, after;
    u8 v, gimmick;
    RogueRunStart_Clear();
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_ResetSettingsToDefaults();
    Rogue_CopyAdventureConfig(&before);
    for (v = 0; v < POKEDEX_VARIANT_COUNT; ++v)
    {
        if (!RoguePokedex_IsCuratedVariant(v) || RogueDexSelection_GetType(v) == DEX_POOL_SPECIAL
         || RogueDexSelection_GetRegion(v) == DEX_REGION_HISUI) continue;
        EXPECT(RogueDexSelection_Init(&selection, CuratedPools(), v, ITEM_NONE, TRUE));
        for (gimmick = 0; gimmick < 5; ++gimmick)
        {
            gSpecialVar_0x8006 = selection.variant;
            gSpecialVar_0x8005 = RogueDexSelection_GetGimmick(&selection);
            RoguePokedex_StoreInitialSelection();
            EXPECT(gSpecialVar_Result);
            EXPECT_EQ(VarGet(VAR_ROGUE_INITIAL_DEX_SELECTION), v);
            EXPECT_EQ(RoguePokedex_GetDexVariant(), v);
            EXPECT_EQ(VarGet(VAR_ROGUE_INITIAL_GIMMICK_ITEM), RogueDexSelection_GetGimmick(&selection));
            Rogue_CopyAdventureConfig(&after);
            EXPECT_EQ(after.battleFormat, before.battleFormat);
            EXPECT_EQ(after.overworldMons, before.overworldMons);
            EXPECT_NE(after.trainerRegions, 0);
            RogueDexSelection_Cycle(&selection, DEX_FIELD_GIMMICK, 1);
        }
    }
}

TEST("Pokemon pool: discarded previews leave remembered setup unchanged")
{
    struct RoguePokedexSelection selection;
    struct RogueAdventureConfig before, after;
    u16 initialVariant, initialItem;
    RogueRunStart_Clear();
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_ResetSettingsToDefaults();
    Rogue_CopyAdventureConfig(&before);
    initialVariant = VarGet(VAR_ROGUE_INITIAL_DEX_SELECTION);
    initialItem = VarGet(VAR_ROGUE_INITIAL_GIMMICK_ITEM);
    EXPECT(RogueDexSelection_Init(&selection, ALL_POOLS, before.pokedexVariant, ITEM_NONE, TRUE));
    RogueDexSelection_Cycle(&selection, DEX_FIELD_TYPE, 1);
    RogueDexSelection_Cycle(&selection, DEX_FIELD_VARIANT, 1);
    Rogue_CopyAdventureConfig(&after);
    EXPECT_EQ(memcmp(&before, &after, sizeof(before)), 0);
    EXPECT_EQ(VarGet(VAR_ROGUE_INITIAL_DEX_SELECTION), initialVariant);
    EXPECT_EQ(VarGet(VAR_ROGUE_INITIAL_GIMMICK_ITEM), initialItem);
}

TEST("Pokemon pool: Trial restrictions permit only legal choices")
{
    struct RogueAdventureConfig config;
    struct RoguePokedexSelection selection;
    u64 mask = 0;
    u8 v, choices[POKEDEX_VARIANT_COUNT];
    RogueRunStart_Clear();
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_ResetSettingsToDefaults();
    for (v = 0; v < POKEDEX_VARIANT_COUNT; ++v)
    {
        Rogue_CopyAdventureConfig(&config);
        if (RogueTrial_BuildSelectionConfig(ROGUE_TRIAL_REGION_HOENN, 0, v, &config)) mask |= POOL_BIT(v);
    }
    EXPECT(RogueDexSelection_Init(&selection, mask, POKEDEX_VARIANT_NATIONAL_GEN9, ITEM_NONE, FALSE));
    EXPECT_EQ(RogueDexSelection_GetRegion(selection.variant), POKEDEX_REGION_HOENN);
    EXPECT_EQ(RogueDexSelection_GetChoices(&selection, DEX_FIELD_REGION, choices), 1);
    for (v = 0; v < POKEDEX_VARIANT_COUNT; ++v)
    {
        RogueDexSelection_Cycle(&selection, DEX_FIELD_VARIANT, 1);
        Rogue_CopyAdventureConfig(&config);
        EXPECT(RogueTrial_BuildSelectionConfig(ROGUE_TRIAL_REGION_HOENN, 0, selection.variant, &config));
    }
}

TEST("Pokemon pool: roster names and summaries fit the GBA columns")
{
    struct RoguePokedexSelection selection;
    u8 v, i, text[160];
    static const u16 gimmicks[] = { ITEM_NONE, ITEM_MEGA_RING, ITEM_Z_POWER_RING, ITEM_DYNAMAX_BAND, ITEM_TERA_ORB };
    for (v = 0; v < POKEDEX_VARIANT_COUNT; ++v)
    {
        EXPECT_LE(GetStringWidth(FONT_SMALL_NARROW, gPokedexVariants[v].displayName, 0), 134);
        EXPECT(RogueDexSelection_Init(&selection, ALL_POOLS, v, ITEM_NONE, TRUE));
        RogueDexSelection_Describe(&selection, text);
        EXPECT_LE(GetStringWidth(FONT_SMALL_NARROW, text, 0), 224);
    }
    for (i = 0; i < ARRAY_COUNT(gimmicks); ++i)
    {
        StringCopy(StringCopy(text, COMPOUND_STRING("Start: ")), RogueDexSelection_GetGimmickName(gimmicks[i]));
        EXPECT_LE(GetStringWidth(FONT_SMALL_NARROW, text, 0), 142);
    }
    EXPECT_LE(GetStringWidth(FONT_SMALL_NARROW, COMPOUND_STRING("Starting gimmick"), 0), 88);
}
