#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mindwing sets Psychic Terrain on switch-in if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_LATIOS)      { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_MINDWING); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_ALAKAZAM)    { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_FLYGON)      { Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TERRAIN_PERMANENT));
    }
}

SINGLE_BATTLE_TEST("Mindwing does not set Psychic Terrain on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_LATIOS)      { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_MINDWING); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_ALAKAZAM)    { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_GOLEM)       { Ability(ABILITY_STURDY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN));
    }
}
