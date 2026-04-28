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

SINGLE_BATTLE_TEST("Whiteout is only assigned to Froslass in the Snorunt line")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Ability(ABILITY_SNOW_CLOAK); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_FROSLASS), ABILITY_WHITEOUT);
        EXPECT_NE(GetUniqueAbilityBySpecies(SPECIES_SNORUNT), ABILITY_WHITEOUT);
        EXPECT_NE(GetUniqueAbilityBySpecies(SPECIES_GLALIE), ABILITY_WHITEOUT);
        EXPECT_NE(GetUniqueAbilityBySpecies(SPECIES_GLALIE_MEGA), ABILITY_WHITEOUT);
    }
}
