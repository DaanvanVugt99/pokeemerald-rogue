#include "global.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "event_data.h"
#include "item.h"
#include "battle.h"
#include "pokemon.h"
#include "rogue.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"
#include "test/test.h"

#include "battle/rogue/charm_test.h"

static void RestoreLevelCharmTestState(u8 difficulty, u8 levelOffset, bool8 overLevelEnabled, u32 battleTypeFlags)
{
    gBattleTypeFlags = battleTypeFlags;
    Rogue_SetConfigToggle(CONFIG_TOGGLE_OVER_LVL, overLevelEnabled);
    Rogue_SetCurrentDifficulty(difficulty);
    gRogueRun.currentLevelOffset = levelOffset;
    ClearCharmTestState();
}

TEST("charms: progression - Level Charm raises only the player level caps")
{
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    u8 previousLevelOffset = gRogueRun.currentLevelOffset;
    bool8 previousOverLevelEnabled = Rogue_GetConfigToggle(CONFIG_TOGGLE_OVER_LVL);
    u32 previousBattleTypeFlags = gBattleTypeFlags;

    BeginCharmTestRun();
    Rogue_SetCurrentDifficulty(1);
    gRogueRun.currentLevelOffset = 0;
    AddCharmForTest(ITEM_LEVEL_CHARM, 1);
    FinishCharmTestSetup();

    EXPECT_EQ(Rogue_CalculateBossMonLvl(), 25);
    EXPECT_EQ(Rogue_CalculatePlayerMonLvl(), 25);
    EXPECT_EQ(Rogue_CalculatePlayerLvlCap(), 28);
    EXPECT_EQ(Rogue_CalculatePlayerMaxLvl(), 28);

    RestoreLevelCharmTestState(previousDifficulty, previousLevelOffset, previousOverLevelEnabled, previousBattleTypeFlags);
}

TEST("charms: progression - Level Charm follows the moving cap without changing progression levels")
{
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    u8 previousLevelOffset = gRogueRun.currentLevelOffset;
    bool8 previousOverLevelEnabled = Rogue_GetConfigToggle(CONFIG_TOGGLE_OVER_LVL);
    u32 previousBattleTypeFlags = gBattleTypeFlags;
    u8 bossLevel;
    u8 progressionLevel;
    u8 rivalLevel;

    BeginCharmTestRun();
    Rogue_SetCurrentDifficulty(1);
    gRogueRun.currentLevelOffset = 4;
    FinishCharmTestSetup();
    bossLevel = Rogue_CalculateBossMonLvl();
    progressionLevel = Rogue_CalculatePlayerMonLvl();
    rivalLevel = Rogue_CalculateRivalMonLvl();

    AddCharmForTest(ITEM_LEVEL_CHARM, 1);
    FinishCharmTestSetup();

    EXPECT_EQ(Rogue_CalculateBossMonLvl(), bossLevel);
    EXPECT_EQ(Rogue_CalculatePlayerMonLvl(), progressionLevel);
    EXPECT_EQ(Rogue_CalculateRivalMonLvl(), rivalLevel);
    EXPECT_EQ(Rogue_CalculatePlayerLvlCap(), progressionLevel + 3);
    EXPECT_EQ(Rogue_CalculatePlayerMaxLvl(), bossLevel + 3);

    RestoreLevelCharmTestState(previousDifficulty, previousLevelOffset, previousOverLevelEnabled, previousBattleTypeFlags);
}

TEST("charms: progression - Level Charm permits experience above the base cap")
{
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    u8 previousLevelOffset = gRogueRun.currentLevelOffset;
    bool8 previousOverLevelEnabled = Rogue_GetConfigToggle(CONFIG_TOGGLE_OVER_LVL);
    u32 previousBattleTypeFlags = gBattleTypeFlags;
    struct Pokemon mon;
    s32 expGain;

    BeginCharmTestRun();
    Rogue_SetCurrentDifficulty(1);
    gRogueRun.currentLevelOffset = 0;
    Rogue_SetConfigToggle(CONFIG_TOGGLE_OVER_LVL, FALSE);
    gBattleTypeFlags = 0;
    AddCharmForTest(ITEM_LEVEL_CHARM, 1);
    FinishCharmTestSetup();
    CreateMon(&mon, SPECIES_MAGIKARP, 25, 0, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

    expGain = 1;
    Rogue_ModifyExpGained(&mon, &expGain);
    EXPECT_GT(expGain, 0);

    RemoveBagItem(ITEM_LEVEL_CHARM, 1);
    FinishCharmTestSetup();
    expGain = 1;
    Rogue_ModifyExpGained(&mon, &expGain);
    EXPECT_EQ(expGain, 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 25);

    RestoreLevelCharmTestState(previousDifficulty, previousLevelOffset, previousOverLevelEnabled, previousBattleTypeFlags);
}

TEST("charms: progression - new charms clamp, leave inactive runs unchanged, and become unique rewards")
{
    u8 previousDifficulty = Rogue_GetCurrentDifficulty();
    u8 previousLevelOffset = gRogueRun.currentLevelOffset;
    bool8 previousOverLevelEnabled = Rogue_GetConfigToggle(CONFIG_TOGGLE_OVER_LVL);
    u32 previousBattleTypeFlags = gBattleTypeFlags;

    BeginCharmTestRun();
    Rogue_SetCurrentDifficulty(ROGUE_CHAMP_START_DIFFICULTY);
    gRogueRun.currentLevelOffset = 0;
    AddCharmForTest(ITEM_LEVEL_CHARM, 2);
    AddCharmForTest(ITEM_REGEN_CHARM, 2);
    AddCharmForTest(ITEM_MOODY_CHARM, 2);
    AddCharmForTest(ITEM_EVIOLITE_CHARM, 2);
    FinishCharmTestSetup();

    EXPECT_EQ(GetCharmValue(EFFECT_LEVEL_CHARM), 1);
    EXPECT_EQ(GetCharmValue(EFFECT_REGEN_CHARM), 1);
    EXPECT_EQ(GetCharmValue(EFFECT_MOODY_CHARM), 1);
    EXPECT_EQ(GetCharmValue(EFFECT_EVIOLITE_CHARM), 1);
    EXPECT(IsEffectDisabled(EFFECT_LEVEL_CHARM, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_REGEN_CHARM, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_MOODY_CHARM, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_EVIOLITE_CHARM, FALSE));
    EXPECT_EQ(Rogue_CalculatePlayerLvlCap(), MAX_LEVEL);
    EXPECT_EQ(Rogue_CalculatePlayerMaxLvl(), MAX_LEVEL);

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    EXPECT_EQ(GetCharmValue(EFFECT_LEVEL_CHARM), 0);
    EXPECT_EQ(GetCharmValue(EFFECT_REGEN_CHARM), 0);
    EXPECT_EQ(GetCharmValue(EFFECT_MOODY_CHARM), 0);
    EXPECT_EQ(GetCharmValue(EFFECT_EVIOLITE_CHARM), 0);
    EXPECT_EQ(Rogue_CalculatePlayerLvlCap(), MAX_LEVEL);

    RestoreLevelCharmTestState(previousDifficulty, previousLevelOffset, previousOverLevelEnabled, previousBattleTypeFlags);
}

TEST("charms: legacy - older charms use their rebalanced unique values")
{
    BeginCharmTestRun();
    AddCharmForTest(ITEM_SHOP_PRICE_CHARM, 2);
    AddCharmForTest(ITEM_FLINCH_CHARM, 2);
    AddCharmForTest(ITEM_CRIT_CHARM, 2);
    AddCharmForTest(ITEM_SHED_SKIN_CHARM, 2);
    AddCharmForTest(ITEM_WILD_IV_CHARM, 2);
    AddCharmForTest(ITEM_CATCHING_CHARM, 2);
    AddCharmForTest(ITEM_GRACE_CHARM, 2);
    AddCharmForTest(ITEM_WILD_ENCOUNTER_CHARM, 2);
    AddCharmForTest(ITEM_MOVE_PRIORITY_CHARM, 2);
    AddCharmForTest(ITEM_ENDURE_CHARM, 2);
    AddCharmForTest(ITEM_TORMENT_CHARM, 2);
    AddCharmForTest(ITEM_PRESSURE_CHARM, 2);
    AddCharmForTest(ITEM_UNAWARE_CHARM, 2);
    AddCharmForTest(ITEM_ADAPTABILITY_CHARM, 2);
    FinishCharmTestSetup();

    EXPECT_EQ(GetCharmValue(EFFECT_SHOP_PRICE), 50);
    EXPECT_EQ(GetCharmValue(EFFECT_FLINCH_CHANCE), 15);
    EXPECT_EQ(GetCharmValue(EFFECT_CRIT_CHANCE), 1);
    EXPECT_EQ(GetCharmValue(EFFECT_SHED_SKIN_CHANCE), 50);
    EXPECT_EQ(GetCharmValue(EFFECT_WILD_IV_RATE), 31);
    EXPECT_EQ(GetCharmValue(EFFECT_CATCH_RATE), 200);
    EXPECT_EQ(GetCharmValue(EFFECT_SERENE_GRACE_CHANCE), 100);
    EXPECT_EQ(GetCharmValue(EFFECT_WILD_ENCOUNTER_COUNT), 2);
    EXPECT_EQ(GetCharmValue(EFFECT_MOVE_PRIORITY_CHANCE), 25);
    EXPECT_EQ(GetCharmValue(EFFECT_ENDURE_CHANCE), 100);
    EXPECT_EQ(GetCharmValue(EFFECT_TORMENT_STATUS), 1);
    EXPECT_EQ(GetCharmValue(EFFECT_PRESSURE_STATUS), 1);
    EXPECT_EQ(GetCharmValue(EFFECT_UNAWARE_STATUS), 1);
    EXPECT_EQ(GetCharmValue(EFFECT_ADAPTABILITY_RATE), 5);

    EXPECT(IsEffectDisabled(EFFECT_SHOP_PRICE, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_FLINCH_CHANCE, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_CRIT_CHANCE, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_SHED_SKIN_CHANCE, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_WILD_IV_RATE, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_CATCH_RATE, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_SERENE_GRACE_CHANCE, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_WILD_ENCOUNTER_COUNT, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_MOVE_PRIORITY_CHANCE, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_ENDURE_CHANCE, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_TORMENT_STATUS, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_PRESSURE_STATUS, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_UNAWARE_STATUS, FALSE));
    EXPECT(IsEffectDisabled(EFFECT_ADAPTABILITY_RATE, FALSE));

    ClearCharmTestState();
}

TEST("charms: curses - paired combat curses match their charm values")
{
    BeginCharmTestRun();
    AddCharmForTest(ITEM_FLINCH_CHARM, 1);
    AddCharmForTest(ITEM_FLINCH_CURSE, 1);
    AddCharmForTest(ITEM_CRIT_CHARM, 1);
    AddCharmForTest(ITEM_CRIT_CURSE, 1);
    AddCharmForTest(ITEM_SHED_SKIN_CHARM, 1);
    AddCharmForTest(ITEM_SHED_SKIN_CURSE, 1);
    AddCharmForTest(ITEM_GRACE_CHARM, 1);
    AddCharmForTest(ITEM_GRACE_CURSE, 1);
    AddCharmForTest(ITEM_MOVE_PRIORITY_CHARM, 1);
    AddCharmForTest(ITEM_MOVE_PRIORITY_CURSE, 1);
    AddCharmForTest(ITEM_ENDURE_CHARM, 1);
    AddCharmForTest(ITEM_ENDURE_CURSE, 1);
    AddCharmForTest(ITEM_TORMENT_CHARM, 1);
    AddCharmForTest(ITEM_TORMENT_CURSE, 1);
    AddCharmForTest(ITEM_PRESSURE_CHARM, 1);
    AddCharmForTest(ITEM_PRESSURE_CURSE, 1);
    AddCharmForTest(ITEM_UNAWARE_CHARM, 1);
    AddCharmForTest(ITEM_UNAWARE_CURSE, 1);
    AddCharmForTest(ITEM_ADAPTABILITY_CHARM, 1);
    AddCharmForTest(ITEM_ADAPTABILITY_CURSE, 1);
    FinishCharmTestSetup();

    EXPECT_EQ(GetCurseValue(EFFECT_FLINCH_CHANCE), GetCharmValue(EFFECT_FLINCH_CHANCE));
    EXPECT_EQ(GetCurseValue(EFFECT_CRIT_CHANCE), GetCharmValue(EFFECT_CRIT_CHANCE));
    EXPECT_EQ(GetCurseValue(EFFECT_SHED_SKIN_CHANCE), GetCharmValue(EFFECT_SHED_SKIN_CHANCE));
    EXPECT_EQ(GetCurseValue(EFFECT_SERENE_GRACE_CHANCE), GetCharmValue(EFFECT_SERENE_GRACE_CHANCE));
    EXPECT_EQ(GetCurseValue(EFFECT_MOVE_PRIORITY_CHANCE), GetCharmValue(EFFECT_MOVE_PRIORITY_CHANCE));
    EXPECT_EQ(GetCurseValue(EFFECT_ENDURE_CHANCE), GetCharmValue(EFFECT_ENDURE_CHANCE));
    EXPECT_EQ(GetCurseValue(EFFECT_TORMENT_STATUS), GetCharmValue(EFFECT_TORMENT_STATUS));
    EXPECT_EQ(GetCurseValue(EFFECT_PRESSURE_STATUS), GetCharmValue(EFFECT_PRESSURE_STATUS));
    EXPECT_EQ(GetCurseValue(EFFECT_UNAWARE_STATUS), GetCharmValue(EFFECT_UNAWARE_STATUS));
    EXPECT_EQ(GetCurseValue(EFFECT_ADAPTABILITY_RATE), GetCharmValue(EFFECT_ADAPTABILITY_RATE));

    ClearCharmTestState();
}

TEST("charms: curses - Dark Deals exclude persistent nonbattle curses")
{
    u16 history[1];
    u16 itemId;
    u16 i;

    for (i = 0; i < 256; i++)
    {
        itemId = Rogue_NextDarkDealCurseItem(history, 0);

        EXPECT_NE(itemId, ITEM_SHOP_PRICE_CURSE);
        EXPECT_NE(itemId, ITEM_WILD_IV_CURSE);
        EXPECT_NE(itemId, ITEM_CATCHING_CURSE);
        EXPECT_NE(itemId, ITEM_WILD_ENCOUNTER_CURSE);
        EXPECT_NE(itemId, ITEM_EVERSTONE_CURSE);
        EXPECT_NE(itemId, ITEM_RANDOMAN_ROUTE_SPAWN_CURSE);
    }
}
