#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Shadow Grasp hits a switching foe with Shadow Sneak")
{
    GIVEN {
        PLAYER(SPECIES_DUSCLOPS) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SHADOW_GRASP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHADOW_GRASP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_SNEAK, player);
    }
}

SINGLE_BATTLE_TEST("Shadow Grasp does not trigger when the opponent does not switch")
{
    GIVEN {
        PLAYER(SPECIES_DUSCLOPS) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SHADOW_GRASP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SHADOW_GRASP);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_SNEAK, player);
        }
    }
}
