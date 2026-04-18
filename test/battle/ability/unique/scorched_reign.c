#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SUNNY_DAY].effect == EFFECT_SUNNY_DAY);
}

SINGLE_BATTLE_TEST("Scorched Reign sets permanent harsh sunlight on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_SCORCHED_REIGN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(gBattleWeather & B_WEATHER_SUN_PERMANENT);
    }
}

SINGLE_BATTLE_TEST("Scorched Reign upgrades active sunlight to permanent on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SUNNY_DAY); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_SCORCHED_REIGN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(gBattleWeather & B_WEATHER_SUN_PERMANENT);
    }
}

SINGLE_BATTLE_TEST("Scorched Reign halves the user's Attack while sunlight is active", s16 damage)
{
    bool32 uniqueAbility;

    PARAMETRIZE { uniqueAbility = FALSE; }
    PARAMETRIZE { uniqueAbility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility ? ABILITY_SCORCHED_REIGN : ABILITY_NONE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(uniqueAbility ? MOVE_CELEBRATE : MOVE_SUNNY_DAY, MOVE_CELEBRATE); HP(300); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, uniqueAbility ? MOVE_CELEBRATE : MOVE_SUNNY_DAY); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2.0), results[0].damage);
    }
}
