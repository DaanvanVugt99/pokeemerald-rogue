#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Ultra Fallout sets Acid Rain on switch-in if any foe is statused and it is the only Ultra Beast")
{
    GIVEN {
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ULTRA_FALLOUT);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ACID_RAIN);
    }
}

SINGLE_BATTLE_TEST("Ultra Fallout does not set Acid Rain if no foe is statused")
{
    GIVEN {
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_FALLOUT);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ACID_RAIN));
    }
}

SINGLE_BATTLE_TEST("Ultra Fallout does not set Acid Rain if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BUZZWOLE) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_FALLOUT);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ACID_RAIN));
    }
}
