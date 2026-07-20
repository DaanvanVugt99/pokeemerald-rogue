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
