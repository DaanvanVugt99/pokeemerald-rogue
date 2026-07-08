#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Migration sets rain when switching out")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_DUCKLETT; }
    PARAMETRIZE { species = SPECIES_SWANNA; }

    GIVEN {
        PLAYER(species) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MIGRATION);
        if (species == SPECIES_DUCKLETT)
            MESSAGE("Ducklett's Migration made it rain!");
        else
            MESSAGE("Swanna's Migration made it rain!");
    } THEN {
        EXPECT((gBattleWeather & B_WEATHER_RAIN) != 0);
    }
}

SINGLE_BATTLE_TEST("Migration does not trigger if the user faints instead of switching out")
{
    GIVEN {
        PLAYER(SPECIES_SWANNA) { HP(1); Ability(ABILITY_KEEN_EYE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); SEND_OUT(player, 1); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MIGRATION);
            MESSAGE("Swanna's Migration made it rain!");
        }
    } THEN {
        EXPECT((gBattleWeather & B_WEATHER_RAIN) == 0);
    }
}
