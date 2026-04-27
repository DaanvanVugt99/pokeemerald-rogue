#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_SURF].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_SURF].target == MOVE_TARGET_FOES_AND_ALLY);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Undertow switches the user out after using a Water-type move")
{
    GIVEN {
        PLAYER(SPECIES_FLOATZEL) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_UNDERTOW); Moves(MOVE_WATER_GUN); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        ABILITY_POPUP(player, ABILITY_UNDERTOW);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Undertow does not switch the user out after using a non-Water move")
{
    GIVEN {
        PLAYER(SPECIES_FLOATZEL) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_UNDERTOW); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_UNDERTOW);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_FLOATZEL);
    }
}

SINGLE_BATTLE_TEST("Undertow does not switch out when the Pokemon is hit by a Water-type move")
{
    GIVEN {
        PLAYER(SPECIES_FLOATZEL) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_UNDERTOW); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_UNDERTOW);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_FLOATZEL);
    }
}

SINGLE_BATTLE_TEST("Undertow does not switch the user out if the Water-type move has no effect")
{
    GIVEN {
        PLAYER(SPECIES_FLOATZEL) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_UNDERTOW); Moves(MOVE_WATER_GUN); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_UNDERTOW);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_FLOATZEL);
    }
}

SINGLE_BATTLE_TEST("Undertow does not activate if the user has no reserve Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_FLOATZEL) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_UNDERTOW); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_UNDERTOW);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_FLOATZEL);
    }
}

DOUBLE_BATTLE_TEST("Undertow waits for a spread Water-type move to hit all targets before switching")
{
    GIVEN {
        PLAYER(SPECIES_FLOATZEL) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_UNDERTOW); Speed(100); Moves(MOVE_SURF); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        PLAYER(SPECIES_MAGIKARP) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SURF); SEND_OUT(playerLeft, 2); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(playerRight);
        HP_BAR(opponentRight);
        ABILITY_POPUP(playerLeft, ABILITY_UNDERTOW);
    } THEN {
        EXPECT_EQ(playerLeft->species, SPECIES_MAGIKARP);
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
    }
}
