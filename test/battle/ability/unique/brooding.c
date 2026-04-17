#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Brooding sets Infested Terrain on switch-in if the target side has no hazards")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS)     { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_BROODING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Brooding does not set Infested Terrain if the target side already has hazards")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET)   { Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
        PLAYER(SPECIES_ARIADOS)     { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_BROODING); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN));
    }
}
