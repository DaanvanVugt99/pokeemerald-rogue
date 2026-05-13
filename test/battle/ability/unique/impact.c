#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAIN_PUNCH].punchingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
}

SINGLE_BATTLE_TEST("Impact lowers Speed when a punching move deals at least 30 percent max HP")
{
    GIVEN {
        PLAYER(SPECIES_MELMETAL) { Attack(999); Speed(100); Moves(MOVE_DRAIN_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); Defense(100); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAIN_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_IMPACT);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_GE(opponent->maxHP - opponent->hp, opponent->maxHP * 30 / 100);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Impact does not lower Speed when a punching move deals less than 30 percent max HP")
{
    GIVEN {
        PLAYER(SPECIES_MELMETAL) { Attack(1); Speed(100); Moves(MOVE_DRAIN_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(999); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAIN_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_IMPACT);
    } THEN {
        EXPECT_LT(opponent->maxHP - opponent->hp, opponent->maxHP * 30 / 100);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Impact does not lower Speed with non-punching moves even if they deal enough damage")
{
    GIVEN {
        PLAYER(SPECIES_MELMETAL) { Attack(999); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); Defense(100); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_IMPACT);
    } THEN {
        EXPECT_GE(opponent->maxHP - opponent->hp, opponent->maxHP * 30 / 100);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Impact uses total damage from multi-hit punching moves")
{
    GIVEN {
        PLAYER(SPECIES_MELTAN) { Attack(999); Speed(100); Moves(MOVE_DOUBLE_IRON_BASH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_IRON_BASH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_IRON_BASH, player);
        HP_BAR(opponent);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_IMPACT);
    } THEN {
        EXPECT_GE(opponent->maxHP - opponent->hp, opponent->maxHP * 30 / 100);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}
