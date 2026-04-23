#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDERCRUSH].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_THUNDERCRUSH].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_THUNDERCRUSH].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_THUNDERCRUSH].power == 100);
    ASSUME(gBattleMoves[MOVE_THUNDERCRUSH].accuracy == 80);
    ASSUME(gBattleMoves[MOVE_THUNDERCRUSH].pp == 5);
    ASSUME(gBattleMoves[MOVE_THUNDERCRUSH].highCritRatio == TRUE);
}

SINGLE_BATTLE_TEST("Thundercrush deals damage normally")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(MOVE_THUNDERCRUSH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERCRUSH); }
    } SCENE {
        MESSAGE("Wobbuffet used Thundercrush!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERCRUSH, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Thundercrush has a high critical-hit ratio")
{
    PASSES_RANDOMLY(1, 8, RNG_CRITICAL_HIT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(MOVE_THUNDERCRUSH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERCRUSH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERCRUSH, player);
        MESSAGE("A critical hit!");
    }
}
