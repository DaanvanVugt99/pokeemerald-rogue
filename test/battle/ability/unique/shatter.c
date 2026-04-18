#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Shatter sets Stealth Rock after knocking out a target")
{
    GIVEN {
        PLAYER(SPECIES_RHYPERIOR) { Ability(ABILITY_SOLID_ROCK); UniqueAbility(ABILITY_SHATTER); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}
