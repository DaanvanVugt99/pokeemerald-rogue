#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_SAND_TOMB].effect == EFFECT_TRAP);
}

SINGLE_BATTLE_TEST("Quicksand uses Sand Tomb after a Ground-type move")
{
    GIVEN {
        PLAYER(SPECIES_SANDACONDA) { Ability(ABILITY_SAND_SPIT); UniqueAbility(ABILITY_QUICKSAND); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_QUICKSAND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SAND_TOMB, player);
    }
}

SINGLE_BATTLE_TEST("Quicksand does not activate after a non-Ground move")
{
    GIVEN {
        PLAYER(SPECIES_SANDACONDA) { Ability(ABILITY_SAND_SPIT); UniqueAbility(ABILITY_QUICKSAND); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_QUICKSAND);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SAND_TOMB, player);
        }
    }
}
