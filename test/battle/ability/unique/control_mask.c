#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_SWORDS_DANCE));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(!IS_MOVE_STATUS(MOVE_CLOSE_COMBAT));
}

SINGLE_BATTLE_TEST("Control Mask preserves stat changes until Type: Null completes a damaging move")
{
    GIVEN {
        PLAYER(SPECIES_TYPE_NULL) { Speed(100); Moves(MOVE_SWORDS_DANCE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_CONTROL_MASK);
        MESSAGE("Type: Null's control mask\nstabilized its stat changes!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Control Mask does not reset stat changes after a status move")
{
    GIVEN {
        PLAYER(SPECIES_TYPE_NULL) { Speed(100); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CONTROL_MASK);
            MESSAGE("Type: Null's control mask\nstabilized its stat changes!");
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Control Mask resets the stat drops caused by Type: Null's damaging move")
{
    GIVEN {
        PLAYER(SPECIES_TYPE_NULL) { Speed(100); Moves(MOVE_CLOSE_COMBAT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CLOSE_COMBAT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CLOSE_COMBAT, player);
        ABILITY_POPUP(player, ABILITY_CONTROL_MASK);
        MESSAGE("Type: Null's control mask\nstabilized its stat changes!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Control Mask does not activate after a damaging move with no stat changes")
{
    GIVEN {
        PLAYER(SPECIES_TYPE_NULL) { Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CONTROL_MASK);
            MESSAGE("Type: Null's control mask\nstabilized its stat changes!");
        }
    }
}
