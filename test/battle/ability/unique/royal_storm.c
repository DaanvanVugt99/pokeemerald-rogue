#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Royal Storm sets rain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Ability(ABILITY_DAMP); UniqueAbility(ABILITY_ROYAL_STORM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROYAL_STORM);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
    }
}

SINGLE_BATTLE_TEST("Royal Storm blocks opposing priority moves while rain is active")
{
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Ability(ABILITY_DAMP); UniqueAbility(ABILITY_ROYAL_STORM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROYAL_STORM);
        NOT { HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Royal Storm stops blocking priority once rain is gone")
{
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Ability(ABILITY_DAMP); UniqueAbility(ABILITY_ROYAL_STORM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUNNY_DAY); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}
