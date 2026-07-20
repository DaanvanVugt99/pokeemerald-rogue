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

static void SetSwitchMoveCharms(u16 prepCount, u16 proteanCount)
{
    BeginCharmTestRun();
    AddCharmForTest(ITEM_PREP_CHARM, prepCount);
    AddCharmForTest(ITEM_PROTEAN_CHARM, proteanCount);
    FinishCharmTestSetup();
}

static void SetRegenCharms(u16 regenCount, u16 recoveryCount)
{
    BeginCharmTestRun();
    AddCharmForTest(ITEM_REGEN_CHARM, regenCount);
    AddCharmForTest(ITEM_RECOVERY_CHARM, recoveryCount);
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

static void ExpectMoodyCharmStages(struct BattlePokemon *mon, s32 raisedStages, s32 loweredStages)
{
    EXPECT_EQ(mon->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    EXPECT_EQ(mon->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    EXPECT_EQ(mon->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    EXPECT_EQ(mon->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + loweredStages);
    EXPECT_EQ(mon->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + raisedStages);
    EXPECT_EQ(mon->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    EXPECT_EQ(mon->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
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

SINGLE_BATTLE_TEST("charms: flow - Prep Charm gives only the first status move priority")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wynaut used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Wynaut used Celebrate!");
    } THEN {
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].preparationCharmUsed);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - damaging moves do not consume Prep Charm")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Wynaut used Tackle!");
        MESSAGE("Wynaut used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - a missed status move consumes Prep Charm")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_HYPNOSIS, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wynaut used Hypnosis!");
        MESSAGE("Wynaut's attack missed!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Wynaut used Celebrate!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - incapacity does not consume Prep Charm")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_FAKE_OUT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FAKE_OUT); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Fake Out!");
        MESSAGE("Wynaut flinched!");
        MESSAGE("Wynaut used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
    } THEN {
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].preparationCharmUsed);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Prep Charm stacks with Prankster")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_SABLEYE) { Speed(1); Ability(ABILITY_PRANKSTER); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        MESSAGE("Sableye used Swords Dance!");
        MESSAGE("Foe Wobbuffet used Quick Attack!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Prep Charm does not grant Prankster's Dark immunity")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_CONFUSE_RAY); }
        OPPONENT(SPECIES_UMBREON) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wynaut used Confuse Ray!");
        MESSAGE("Foe Umbreon became confused!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Quick Guard blocks a Prep Charm move")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_QUICK_GUARD); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_QUICK_GUARD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_GUARD, opponent);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        MESSAGE("Foe Wobbuffet protected itself!");
    } THEN {
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].preparationCharmUsed);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Psychic Terrain blocks a Prep Charm move")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_TACKLE, MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_PSYCHIC_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_PSYCHIC_TERRAIN); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Psychic Terrain!");
        MESSAGE("Wynaut used Tackle!");
        MESSAGE("Wynaut cannot use Growl!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
    } THEN {
        EXPECT(!gDisableStructs[B_POSITION_PLAYER_LEFT].preparationCharmUsed);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Taunt prevents Prep Charm consumption")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SABLEYE) { Speed(100); Ability(ABILITY_PRANKSTER); Moves(MOVE_TAUNT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TAUNT); }
    } THEN {
        EXPECT(!gDisableStructs[B_POSITION_PLAYER_LEFT].preparationCharmUsed);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Prep Charm resets after switching back in")
{
    GIVEN {
        SetSwitchMoveCharms(1, 0);
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SHUCKLE) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wynaut used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Wynaut used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Protean Charm changes type once without an ability popup")
{
    GIVEN {
        SetSwitchMoveCharms(0, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF { ABILITY_POPUP(player); }
        MESSAGE("Wobbuffet transformed into the Water type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_WATER);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].proteanCharmUsed);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - same-type moves leave Protean Charm available")
{
    GIVEN {
        SetSwitchMoveCharms(0, 1);
        PLAYER(SPECIES_KECLEON) { Moves(MOVE_TACKLE, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        MESSAGE("Kecleon transformed into the Water type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_WATER);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Protean Charm follows called moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_METRONOME].type == TYPE_NORMAL);
        SetSwitchMoveCharms(0, 1);
        PLAYER(SPECIES_KECLEON) { Moves(MOVE_METRONOME); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_METRONOME, WITH_RNG(RNG_METRONOME, MOVE_WATER_GUN)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Kecleon used Metronome!");
        MESSAGE("Kecleon transformed into the Water type!");
        MESSAGE("Kecleon used Water Gun!");
    } THEN {
        EXPECT_EQ(player->type1, TYPE_WATER);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Protean Charm resets after switching back in")
{
    GIVEN {
        SetSwitchMoveCharms(0, 1);
        PLAYER(SPECIES_KECLEON) { Moves(MOVE_WATER_GUN, MOVE_EMBER); }
        PLAYER(SPECIES_SHUCKLE) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Kecleon transformed into the Water type!");
        MESSAGE("Kecleon transformed into the Fire type!");
    } THEN {
        EXPECT_EQ(player->type1, TYPE_FIRE);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Terastallization does not consume Protean Charm")
{
    GIVEN {
        SetSwitchMoveCharms(0, 1);
        PLAYER(SPECIES_KECLEON) { TeraType(TYPE_GRASS); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, tera: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF { MESSAGE("Kecleon transformed into the Water type!"); }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
    } THEN {
        EXPECT(!gDisableStructs[B_POSITION_PLAYER_LEFT].proteanCharmUsed);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Struggle does not consume Protean Charm")
{
    bool32 charmActivated = FALSE;

    GIVEN {
        SetSwitchMoveCharms(0, 1);
        PLAYER(SPECIES_KECLEON);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        player->type1 = TYPE_WATER;
        player->type2 = TYPE_WATER;
        player->type3 = TYPE_MYSTERY;
        EXPECT(!ProteanTryChangeType(B_POSITION_PLAYER_LEFT, ABILITY_COLOR_CHANGE, MOVE_STRUGGLE, TYPE_NORMAL, TRUE, &charmActivated));
        EXPECT(!charmActivated);
        EXPECT(!gDisableStructs[B_POSITION_PLAYER_LEFT].proteanCharmUsed);
        EXPECT_EQ(player->type1, TYPE_WATER);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Protean ability activation leaves Protean Charm available")
{
    GIVEN {
        SetSwitchMoveCharms(0, 1);
        PLAYER(SPECIES_KECLEON) { Ability(ABILITY_PROTEAN); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PROTEAN);
        MESSAGE("Kecleon transformed into the Water type!");
    } THEN {
        EXPECT(!gDisableStructs[B_POSITION_PLAYER_LEFT].proteanCharmUsed);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - switch-move charms are player-only, unique rewards")
{
    GIVEN {
        SetSwitchMoveCharms(2, 2);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KECLEON) { Speed(1); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        MESSAGE("Wobbuffet used Celebrate!");
        NONE_OF { MESSAGE("Foe Kecleon transformed into the Water type!"); }
        MESSAGE("Foe Kecleon used Water Gun!");
    } THEN {
        EXPECT_EQ(GetCharmValue(EFFECT_PREP_CHARM), 1);
        EXPECT_EQ(GetCharmValue(EFFECT_PROTEAN_CHARM), 1);
        EXPECT(IsEffectDisabled(EFFECT_PREP_CHARM, FALSE));
        EXPECT(IsEffectDisabled(EFFECT_PROTEAN_CHARM, FALSE));
        EXPECT(!gDisableStructs[B_POSITION_OPPONENT_LEFT].preparationCharmUsed);
        EXPECT(!gDisableStructs[B_POSITION_OPPONENT_LEFT].proteanCharmUsed);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Regen Charm restores one quarter HP on switch out")
{
    GIVEN {
        SetRegenCharms(1, 0);
        PLAYER(SPECIES_WOBBUFFET) { HP(40); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP),
                  40 + GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP) / 4);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Regen Charm stacks independently with Regenerator")
{
    GIVEN {
        SetRegenCharms(1, 0);
        PLAYER(SPECIES_SLOWPOKE) { HP(40); Ability(ABILITY_REGENERATOR); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP),
                  40 + GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP) / 4
                     + GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP) / 3);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Regen Charm combines with Natural Cure switch updates")
{
    GIVEN {
        SetRegenCharms(1, 0);
        PLAYER(SPECIES_CHANSEY) { HP(40); Status1(STATUS1_BURN); Ability(ABILITY_NATURAL_CURE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), 0);
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP),
                  40 + GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP) / 4);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Regen Charm caps healing at maximum HP")
{
    GIVEN {
        SetRegenCharms(1, 0);
        PLAYER(SPECIES_WOBBUFFET) { HP(400); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP),
                  GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP));
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Recovery Charm does not amplify Regen Charm")
{
    GIVEN {
        SetRegenCharms(1, 1);
        PLAYER(SPECIES_WOBBUFFET) { HP(40); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP),
                  40 + GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP) / 4);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Regen Charm heals before pivoting out")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_U_TURN].effect == EFFECT_HIT_ESCAPE);
        SetRegenCharms(1, 0);
        PLAYER(SPECIES_WOBBUFFET) { HP(40); Speed(100); Moves(MOVE_U_TURN); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP),
                  40 + GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP) / 4);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Regen Charm heals friendly Pokemon forced out")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ROAR].effect == EFFECT_ROAR);
        SetRegenCharms(1, 0);
        PLAYER(SPECIES_WOBBUFFET) { HP(40); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ROAR); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ROAR); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP),
                  40 + GetMonData(&gPlayerParty[0], MON_DATA_MAX_HP) / 4);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Regen Charm does not heal opposing Pokemon")
{
    GIVEN {
        SetRegenCharms(2, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_HP), 400);
        EXPECT_EQ(GetCharmValue(EFFECT_REGEN_CHARM), 1);
        EXPECT(IsEffectDisabled(EFFECT_REGEN_CHARM, FALSE));
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Regen Charm does not revive a fainting switch user")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_HEALING_WISH].effect == EFFECT_HEALING_WISH);
        ASSUME(B_HEALING_WISH_SWITCH >= GEN_5);
        SetRegenCharms(1, 0);
        PLAYER(SPECIES_GARDEVOIR) { Moves(MOVE_HEALING_WISH); }
        PLAYER(SPECIES_WYNAUT) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HEALING_WISH); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), 0);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Moody Charm raises and lowers different eligible stats by one stage")
{
    GIVEN {
        SetSingleCharmForTest(ITEM_MOODY_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF { ABILITY_POPUP(player); }
    } THEN {
        ExpectMoodyCharmStages(player, 1, -1);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Moody Charm does not activate for opponents")
{
    GIVEN {
        SetSingleCharmForTest(ITEM_MOODY_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Moody Charm waits until the turn after switching in")
{
    GIVEN {
        SetSingleCharmForTest(ITEM_MOODY_CHARM, 1);
        PLAYER(SPECIES_WYNAUT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        ExpectMoodyCharmStages(player, 0, 0);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Moody Charm activates on a switched Pokemon's next full turn")
{
    GIVEN {
        SetSingleCharmForTest(ITEM_MOODY_CHARM, 1);
        PLAYER(SPECIES_WYNAUT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        ExpectMoodyCharmStages(player, 1, -1);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: flow - Moody Charm stacks independently with Moody")
{
    GIVEN {
        SetSingleCharmForTest(ITEM_MOODY_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_MOODY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOODY);
    } THEN {
        ExpectMoodyCharmStages(player, 3, -2);
        ClearCharmTestState();
    }
}
