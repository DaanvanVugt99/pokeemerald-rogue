#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TAKE_DOWN].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_TAKE_DOWN].accuracy < 100);
}

SINGLE_BATTLE_TEST("Haute Couture makes Normal-type moves never miss")
{
    GIVEN {
        PLAYER(SPECIES_FURFROU_NATURAL) { Ability(ABILITY_FUR_COAT); UniqueAbility(ABILITY_HAUTE_COUTURE); Moves(MOVE_TAKE_DOWN); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAKE_DOWN, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAKE_DOWN, player);
        NOT MESSAGE("Furfrou's attack missed!");
    }
}

SINGLE_BATTLE_TEST("Haute Couture gives Normal-type moves +1 critical-hit ratio")
{
    PASSES_RANDOMLY(1, 8, RNG_CRITICAL_HIT);
    GIVEN {
        PLAYER(SPECIES_FURFROU_HEART_TRIM) { Ability(ABILITY_FUR_COAT); UniqueAbility(ABILITY_HAUTE_COUTURE); Moves(MOVE_TAKE_DOWN); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAKE_DOWN); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}
