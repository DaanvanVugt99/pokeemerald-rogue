#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Headache makes all confused Pokemon have a 75% chance to hurt themselves")
{
    PASSES_RANDOMLY(3, 4, RNG_CONFUSION);
    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Ability(ABILITY_DAMP); UniqueAbility(ABILITY_HEADACHE); Moves(MOVE_CONFUSE_RAY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet became confused!");
        MESSAGE("It hurt itself in its confusion!");
    }
}
