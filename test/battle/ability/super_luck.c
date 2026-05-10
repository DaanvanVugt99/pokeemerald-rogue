#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Super Luck gives a 10 percent chance to evade attacks")
{
    PASSES_RANDOMLY(1, 10, RNG_ACCURACY);
    GIVEN {
        ASSUME(gBattleMoves[MOVE_POUND].accuracy == 100);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SUPER_LUCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_POUND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_POUND); }
    } SCENE {
        NONE_OF {
            HP_BAR(player);
        }
    }
}

SINGLE_BATTLE_TEST("Super Luck can still be hit")
{
    PASSES_RANDOMLY(90, 100, RNG_ACCURACY);
    GIVEN {
        ASSUME(gBattleMoves[MOVE_POUND].accuracy == 100);
        PLAYER(SPECIES_TOGEPI) { Ability(ABILITY_SUPER_LUCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_POUND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_POUND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POUND, opponent);
        HP_BAR(player);
    }
}
