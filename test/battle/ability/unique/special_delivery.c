#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PRESENT].effect == EFFECT_PRESENT);
}

SINGLE_BATTLE_TEST("Special Delivery can target an opponent and damage it")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); MaxHP(100); HP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); MaxHP(100); HP(1); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(opponent, 1, WITH_RNG(RNG_SPECIAL_DELIVERY, 0)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SPECIAL_DELIVERY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PRESENT, opponent);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_EQ(opponent->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Special Delivery can target the user and heal it")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DELIBIRD) { Ability(ABILITY_NO_GUARD); MaxHP(100); HP(1); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(opponent, 1, WITH_RNG(RNG_SPECIAL_DELIVERY, 1)); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->hp, 26);
    }
}

DOUBLE_BATTLE_TEST("Special Delivery can target an ally and heal it")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); MaxHP(100); HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DELIBIRD) { Ability(ABILITY_NO_GUARD); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            SWITCH_WITH_RNG(opponentRight, 1, WITH_RNG(RNG_SPECIAL_DELIVERY, 1));
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 26);
    }
}
