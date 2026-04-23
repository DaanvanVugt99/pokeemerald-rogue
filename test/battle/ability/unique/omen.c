#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SLASH].highCritRatio == 1);
}

SINGLE_BATTLE_TEST("Omen sets Eclipse on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_OMEN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
    }
}

SINGLE_BATTLE_TEST("Omen gives +1 crit stage to moves while Eclipse is active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SUPER_LUCK); UniqueAbility(ABILITY_OMEN); Moves(MOVE_SLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLASH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_OMEN);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Omen sets extended Eclipse with Dim Rock")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_DIM_ROCK); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_OMEN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_EXTENDED - 1);
    }
}
