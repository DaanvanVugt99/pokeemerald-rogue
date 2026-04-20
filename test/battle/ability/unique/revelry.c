#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sun Revelry sets Reflect on switch-in in sun")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NINETALES) { Ability(ABILITY_FLASH_FIRE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT);
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_LIGHTSCREEN));
    }
}

SINGLE_BATTLE_TEST("Frost Revelry sets Reflect on switch-in in sun")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NINETALES_ALOLAN) { Ability(ABILITY_SNOW_CLOAK); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT);
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_LIGHTSCREEN));
    }
}

SINGLE_BATTLE_TEST("Sun Revelry checks weather before Drought changes weather")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_NINETALES) { Ability(ABILITY_DROUGHT); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT);
    }
}

SINGLE_BATTLE_TEST("Sun Revelry sets Light Screen on switch-in in hail")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_HAIL, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NINETALES) { Ability(ABILITY_FLASH_FIRE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HAIL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_HAIL);
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT));
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_LIGHTSCREEN);
    }
}

SINGLE_BATTLE_TEST("Frost Revelry checks weather before Snow Warning changes weather")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_NINETALES_ALOLAN) { Ability(ABILITY_SNOW_WARNING); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & (B_WEATHER_HAIL | B_WEATHER_SNOW));
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT);
    }
}
