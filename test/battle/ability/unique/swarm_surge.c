#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Swarm Surge sets Infested Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MOTHIM) { Ability(ABILITY_SWARM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SWARM_SURGE);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN);
    }
}
