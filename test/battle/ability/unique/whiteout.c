#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Whiteout sets Misty Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Ability(ABILITY_SNOW_CLOAK); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TERRAIN_PERMANENT));
    }
}
