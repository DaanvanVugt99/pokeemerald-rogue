#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Solar Call sets permanent Sun on switch-in if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_MOLTRES)    { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SOLAR_CALL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SLUGMA)     { Ability(ABILITY_MAGMA_ARMOR); }
        PLAYER(SPECIES_FARFETCHD)  { Ability(ABILITY_SCRAPPY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(gBattleWeather & B_WEATHER_SUN_PERMANENT);
    }
}

SINGLE_BATTLE_TEST("Solar Call does not set Sun on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_MOLTRES)    { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SOLAR_CALL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SLUGMA)     { Ability(ABILITY_MAGMA_ARMOR); }
        PLAYER(SPECIES_SQUIRTLE)   { Ability(ABILITY_TORRENT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SUN));
        EXPECT(!(gBattleWeather & B_WEATHER_SUN_PERMANENT));
    }
}
