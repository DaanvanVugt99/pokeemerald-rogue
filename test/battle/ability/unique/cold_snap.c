#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SNOWSCAPE].effect == EFFECT_SNOWSCAPE);
}

SINGLE_BATTLE_TEST("Cold Snap sets Hail when the user drops below 50 percent HP")
{
    GIVEN {
        PLAYER(SPECIES_GLALIE) { Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_COLD_SNAP); HP(100); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_HAIL);
    }
}

SINGLE_BATTLE_TEST("Cold Snap does not set Hail when the user stays above 50 percent HP")
{
    GIVEN {
        PLAYER(SPECIES_GLALIE) { Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_COLD_SNAP); HP(130); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_COLD_SNAP);
        }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_HAIL));
    }
}

SINGLE_BATTLE_TEST("Cold Snap sets Hail even if Snow is currently active")
{
    GIVEN {
        PLAYER(SPECIES_GLALIE) { Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_COLD_SNAP); HP(100); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SNOWSCAPE, MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SNOWSCAPE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_COLD_SNAP);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HAIL_CONTINUES);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_HAIL);
        EXPECT(!(gBattleWeather & B_WEATHER_SNOW));
    }
}

SINGLE_BATTLE_TEST("Cold Snap sets Hail even if the user faints from the threshold-crossing hit")
{
    GIVEN {
        PLAYER(SPECIES_GLALIE) { Level(100); Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_COLD_SNAP); HP(81); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_HAIL);
    }
}
