#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Frost Call sets permanent Snow on switch-in if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_ARTICUNO)    { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_FROST_CALL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DEWGONG)     { Ability(ABILITY_THICK_FAT); }
        PLAYER(SPECIES_GYARADOS)    { Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
        EXPECT(gBattleWeather & B_WEATHER_SNOW_PERMANENT);
    }
}

SINGLE_BATTLE_TEST("Frost Call does not set Snow on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_ARTICUNO)   { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_FROST_CALL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DEWGONG)    { Ability(ABILITY_THICK_FAT); }
        PLAYER(SPECIES_PIKACHU)    { Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SNOW));
        EXPECT(!(gBattleWeather & B_WEATHER_SNOW_PERMANENT));
    }
}
