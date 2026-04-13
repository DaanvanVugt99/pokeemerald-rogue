#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GLARE].effect == EFFECT_PARALYZE);
    ASSUME(gBattleMoves[MOVE_MEAN_LOOK].effect == EFFECT_MEAN_LOOK);
}

SINGLE_BATTLE_TEST("Strike Fear uses Mean Look when using Glare")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_STRIKE_FEAR); Moves(MOVE_GLARE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GLARE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GLARE, player);
        ABILITY_POPUP(player, ABILITY_STRIKE_FEAR);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEAN_LOOK, player);
    }
}

SINGLE_BATTLE_TEST("Strike Fear does not trigger on non-Glare moves")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_STRIKE_FEAR); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STRIKE_FEAR);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MEAN_LOOK, player);
        }
    }
}
