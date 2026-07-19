#include "global.h"
#include "test/battle.h"

#include "battle_script_commands.h"
#include "charm_test.h"

static void SetFlowCharms(u16 retaliateCount, u16 momentumCount, u16 standCount)
{
    BeginCharmTestRun();
    AddCharmForTest(ITEM_RETALIATE_CHARM, retaliateCount);
    AddCharmForTest(ITEM_MOMENTUM_CHARM, momentumCount);
    AddCharmForTest(ITEM_STAND_CHARM, standCount);
    FinishCharmTestSetup();
}

static void ExpectFiveStatBoosts(struct BattlePokemon *mon, u32 stages)
{
    EXPECT_EQ(mon->statStages[STAT_ATK], DEFAULT_STAT_STAGE + stages);
    EXPECT_EQ(mon->statStages[STAT_DEF], DEFAULT_STAT_STAGE + stages);
    EXPECT_EQ(mon->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + stages);
    EXPECT_EQ(mon->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + stages);
    EXPECT_EQ(mon->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + stages);
}

SINGLE_BATTLE_TEST("charms: flow - Retaliate Charm boosts the next entrant after an ally faints")
{
    GIVEN {
        SetFlowCharms(1, 0, 0);
        PLAYER(SPECIES_WYNAUT) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Retaliate Charm ignores ordinary switches")
{
    GIVEN {
        SetFlowCharms(1, 0, 0);
        PLAYER(SPECIES_WYNAUT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        ClearCharmTestState();
    }
}

DOUBLE_BATTLE_TEST("charms: flow - multiple faints collapse to one Retaliate Charm charge")
{
    GIVEN {
        SetFlowCharms(1, 0, 0);
        PLAYER(SPECIES_WYNAUT) { Status1(STATUS1_SLEEP); HP(1); }
        PLAYER(SPECIES_WYNAUT) { Status1(STATUS1_SLEEP); HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DARKRAI);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SEND_OUT(playerLeft, 2); SEND_OUT(playerRight, 3); }
    } THEN {
        EXPECT_EQ((playerLeft->statStages[STAT_ATK] - DEFAULT_STAT_STAGE)
                + (playerRight->statStages[STAT_ATK] - DEFAULT_STAT_STAGE), 1);
        EXPECT_EQ((playerLeft->statStages[STAT_SPATK] - DEFAULT_STAT_STAGE)
                + (playerRight->statStages[STAT_SPATK] - DEFAULT_STAT_STAGE), 1);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Retaliate Charm consumes its charge on the first entrant")
{
    GIVEN {
        SetFlowCharms(1, 0, 0);
        PLAYER(SPECIES_WYNAUT) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_SHUCKLE);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
        TURN { SWITCH(player, 2); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Retaliate Charm does not boost opposing replacements")
{
    GIVEN {
        SetFlowCharms(1, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Momentum Charm raises Speed after a direct knockout")
{
    GIVEN {
        SetFlowCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Momentum Charm does not trigger without a knockout")
{
    GIVEN {
        SetFlowCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Momentum Charm excludes residual knockouts")
{
    GIVEN {
        SetFlowCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Status1(STATUS1_POISON); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        ClearCharmTestState();
    }
}

DOUBLE_BATTLE_TEST("charms: flow - Momentum Charm grants two stages for a spread double knockout")
{
    GIVEN {
        SetFlowCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DAZZLING_GLEAM); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); Speed(10); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DAZZLING_GLEAM); MOVE(playerRight, MOVE_CELEBRATE); SEND_OUT(opponentLeft, 2); SEND_OUT(opponentRight, 3); }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        ClearCharmTestState();
    }
}

DOUBLE_BATTLE_TEST("charms: flow - Momentum Charm excludes allied knockouts")
{
    GIVEN {
        SetFlowCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WYNAUT) { HP(1); Speed(50); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SHUCKLE) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: playerRight); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); SEND_OUT(playerRight, 2); }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Momentum Charm stacks with Moxie")
{
    GIVEN {
        SetFlowCharms(0, 1, 0);
        PLAYER(SPECIES_GYARADOS) { Ability(ABILITY_MOXIE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Momentum Charm does not boost opposing attackers")
{
    GIVEN {
        SetFlowCharms(0, 1, 0);
        PLAYER(SPECIES_WYNAUT) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Stand Charm boosts all five stats for a solo lead")
{
    GIVEN {
        SetFlowCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        MESSAGE("Wobbuffet's Attack rose!");
        MESSAGE("Wobbuffet's Defense rose!");
        MESSAGE("Wobbuffet's Speed rose!");
        MESSAGE("Wobbuffet's Sp. Atk rose!");
        MESSAGE("Wobbuffet's Sp. Def rose!");
    } THEN {
        ExpectFiveStatBoosts(player, 1);
        ClearCharmTestState();
    }
}

DOUBLE_BATTLE_TEST("charms: flow - Stand Charm ignores an absent partner slot")
{
    GIVEN {
        SetFlowCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        u32 battler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
        u16 hp = 0;

        SetMonData(&PLAYER_PARTY[1], MON_DATA_HP, &hp);
        gBattleMons[battler].hp = 0;
        gAbsentBattlerFlags |= gBitTable[battler];
        gSpecialStatuses[battler].switchInStandCharmDone = FALSE;

        EXPECT(!TryActivateSwitchInCharms(battler));
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Stand Charm boosts the final forced replacement")
{
    GIVEN {
        SetFlowCharms(0, 0, 1);
        PLAYER(SPECIES_WYNAUT) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } THEN {
        ExpectFiveStatBoosts(player, 1);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Stand Charm does not trigger while another party member survives")
{
    GIVEN {
        SetFlowCharms(0, 0, 1);
        PLAYER(SPECIES_WYNAUT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        ExpectFiveStatBoosts(player, 0);
        ClearCharmTestState();
    }
}

DOUBLE_BATTLE_TEST("charms: flow - Stand Charm does not trigger while a living partner remains")
{
    GIVEN {
        SetFlowCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        ExpectFiveStatBoosts(playerLeft, 0);
        ExpectFiveStatBoosts(playerRight, 0);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Retaliate and Stand Charms stack on the final replacement")
{
    GIVEN {
        SetFlowCharms(1, 0, 1);
        PLAYER(SPECIES_WYNAUT) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - duplicate copies clamp to one")
{
    GIVEN {
        SetFlowCharms(2, 2, 2);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetCharmValue(EFFECT_RETALIATE_CHARM), 1);
        EXPECT_EQ(GetCharmValue(EFFECT_MOMENTUM_CHARM), 1);
        EXPECT_EQ(GetCharmValue(EFFECT_STAND_CHARM), 1);
        ClearCharmTestState();
    }
}
