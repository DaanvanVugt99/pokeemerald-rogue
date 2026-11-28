#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Monsoon sets rain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_PELIPPER) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MONSOON);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
    }
}

SINGLE_BATTLE_TEST("Monsoon extends rain on switch-out even at its starting duration")
{
    bool32 wait, pivot;
    u16 item;
    PARAMETRIZE { wait = FALSE; pivot = FALSE; item = ITEM_NONE; }
    PARAMETRIZE { wait = TRUE; pivot = FALSE; item = ITEM_NONE; }
    PARAMETRIZE { wait = TRUE; pivot = TRUE; item = ITEM_DAMP_ROCK; }

    GIVEN {
        PLAYER(SPECIES_PELIPPER) { Ability(ABILITY_KEEN_EYE); Item(item); Moves(MOVE_CELEBRATE, MOVE_U_TURN); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (wait)
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        if (pivot)
            TURN { MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        else
            TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gWishFutureKnock.weatherDuration, (item == ITEM_DAMP_ROCK ? WEATHER_DURATION_EXTENDED : WEATHER_DURATION_TURNS) - wait);
    }
}
