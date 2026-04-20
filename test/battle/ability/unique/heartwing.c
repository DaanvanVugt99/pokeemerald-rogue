#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Heartwing sets Misty Terrain on switch-in if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_LATIAS)      { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_HEARTWING); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GARDEVOIR)   { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_ALTARIA)     { Ability(ABILITY_NATURAL_CURE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TERRAIN_PERMANENT));
    }
}

SINGLE_BATTLE_TEST("Heartwing does not set Misty Terrain on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_LATIAS)      { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_HEARTWING); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GARDEVOIR)   { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_PIKACHU)     { Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
    }
}
