#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ZAP_CANNON].accuracy == 50);
    ASSUME(gBattleMoves[MOVE_ZAP_CANNON].priority == 0);
    ASSUME(gBattleMoves[MOVE_THUNDERBOLT].accuracy > 50);
    ASSUME(gBattleMoves[MOVE_THUNDERBOLT].priority == 0);
    ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].priority == 0);
}

SINGLE_BATTLE_TEST("Sighting System makes damaging moves never miss")
{
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SIGHTING_SYSTEM); Moves(MOVE_ZAP_CANNON); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ZAP_CANNON); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, player);
        NONE_OF { MESSAGE("Magnezone's attack missed!"); }
    }
}

SINGLE_BATTLE_TEST("Sighting System gives all damaging moves -3 priority")
{
    GIVEN {
        PLAYER(SPECIES_MAGNETON) { Speed(200); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SIGHTING_SYSTEM); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, player);
    }
}

SINGLE_BATTLE_TEST("Sighting System gives status moves +1 priority")
{
    GIVEN {
        PLAYER(SPECIES_MAGNEMITE) { Speed(1); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SIGHTING_SYSTEM); Moves(MOVE_THUNDER_WAVE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_WAVE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}
