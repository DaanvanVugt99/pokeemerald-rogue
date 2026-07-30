#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_ABSORB].priority == 0);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].effect == EFFECT_WEATHER_BALL);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Solarboost sets sun on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SUNFLORA) { Ability(ABILITY_EARLY_BIRD); UniqueAbility(ABILITY_SOLARBOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SOLARBOOST);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
    }
}

SINGLE_BATTLE_TEST("Solarboost gives +1 priority to Fire-type moves on the first turn out")
{
    GIVEN {
        PLAYER(SPECIES_SUNFLORA) { Speed(50); Ability(ABILITY_EARLY_BIRD); UniqueAbility(ABILITY_SOLARBOOST); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_ABSORB); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_ABSORB); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SOLARBOOST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, opponent);
    }
}

SINGLE_BATTLE_TEST("Solarboost gives +1 priority to Weather Ball after setting sun")
{
    GIVEN {
        PLAYER(SPECIES_SUNFLORA) { Speed(50); Ability(ABILITY_EARLY_BIRD); UniqueAbility(ABILITY_SOLARBOOST); Moves(MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_ABSORB); }
    } WHEN {
        TURN { MOVE(player, MOVE_WEATHER_BALL); MOVE(opponent, MOVE_ABSORB); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SOLARBOOST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, opponent);
    }
}

SINGLE_BATTLE_TEST("Solarboost does not give priority to Fire-type moves after the first turn")
{
    GIVEN {
        PLAYER(SPECIES_SUNFLORA) { Speed(50); Ability(ABILITY_EARLY_BIRD); UniqueAbility(ABILITY_SOLARBOOST); Moves(MOVE_CELEBRATE, MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE, MOVE_ABSORB); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_ABSORB); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SOLARBOOST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
    }
}
