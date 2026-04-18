#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RAIN_DANCE].effect == EFFECT_RAIN_DANCE);
}

SINGLE_BATTLE_TEST("Misty Mirage sets permanent rain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_MISTY_MIRAGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT(gBattleWeather & B_WEATHER_RAIN_PERMANENT);
    }
}

SINGLE_BATTLE_TEST("Misty Mirage upgrades active rain to permanent on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_RAIN_DANCE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_MISTY_MIRAGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT(gBattleWeather & B_WEATHER_RAIN_PERMANENT);
    }
}

SINGLE_BATTLE_TEST("Misty Mirage halves the user's Defense while rain is active", s16 damage)
{
    bool32 uniqueAbility;

    PARAMETRIZE { uniqueAbility = FALSE; }
    PARAMETRIZE { uniqueAbility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility ? ABILITY_MISTY_MIRAGE : ABILITY_NONE); Moves(MOVE_CELEBRATE); HP(300); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(uniqueAbility ? MOVE_CELEBRATE : MOVE_RAIN_DANCE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, uniqueAbility ? MOVE_CELEBRATE : MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}
