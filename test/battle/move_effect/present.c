#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PRESENT].effect == EFFECT_PRESENT);
    ASSUME(gBattleMoves[MOVE_PRESENT].target == MOVE_TARGET_SELECTED);
}

DOUBLE_BATTLE_TEST("Present always heals an ally")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_NO_GUARD); Speed(4); Moves(MOVE_PRESENT); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(3); MaxHP(100); HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Speed(2); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PRESENT, target: playerRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(playerRight->hp, 26);
    }
}

SINGLE_BATTLE_TEST("Present always damages an opponent")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_NO_GUARD); Moves(MOVE_PRESENT); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); MaxHP(100); HP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PRESENT); }
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}
