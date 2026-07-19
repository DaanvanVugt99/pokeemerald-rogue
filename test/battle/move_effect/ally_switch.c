#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ALLY_SWITCH].effect == EFFECT_ALLY_SWITCH);
}

DOUBLE_BATTLE_TEST("Ally Switch works without reserve Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_ABRA);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_ALLY_SWITCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ALLY_SWITCH, playerLeft);
        NOT MESSAGE("But it failed!");
    }
}

DOUBLE_BATTLE_TEST("Ally Switch works while the user is trapped")
{
    GIVEN {
        PLAYER(SPECIES_ABRA);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_ALLY_SWITCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ALLY_SWITCH, playerLeft);
        NOT MESSAGE("But it failed!");
    }
}
