#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sky Patrol uses Defog when a foe switches out")
{
    GIVEN {
        PLAYER(SPECIES_CORVIKNIGHT) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SKY_PATROL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SKY_PATROL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEFOG, player);
    }
}

SINGLE_BATTLE_TEST("Sky Patrol does not trigger when the foe does not switch")
{
    GIVEN {
        PLAYER(SPECIES_CORVIKNIGHT) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SKY_PATROL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SKY_PATROL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DEFOG, player);
        }
    }
}
