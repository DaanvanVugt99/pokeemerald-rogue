#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Storm Call sets permanent Electric Terrain on switch-in if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_ZAPDOS)      { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_STORM_CALL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PIKACHU)     { Ability(ABILITY_STATIC); }
        PLAYER(SPECIES_FARFETCHD)   { Ability(ABILITY_SCRAPPY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
        EXPECT(gFieldStatuses & STATUS_FIELD_TERRAIN_PERMANENT);
    }
}

SINGLE_BATTLE_TEST("Storm Call does not set Electric Terrain on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_ZAPDOS)   { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_STORM_CALL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PIKACHU)    { Ability(ABILITY_STATIC); }
        PLAYER(SPECIES_GOLEM)   { Ability(ABILITY_STURDY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN));
    }
}
