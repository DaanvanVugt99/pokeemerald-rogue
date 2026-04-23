#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DYNAMIC_PUNCH].effect == EFFECT_CONFUSE_HIT);
    ASSUME(gBattleMoves[MOVE_DYNAMIC_PUNCH].secondaryEffectChance == 100);
}

SINGLE_BATTLE_TEST("Toxisphere sets Acid Rain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_TOXISPHERE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ACID_RAIN);
    }
}

SINGLE_BATTLE_TEST("Toxisphere sets Acid Rain on switch-in even with Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); UniqueAbility(ABILITY_TOXISPHERE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ACID_RAIN);
    }
}

SINGLE_BATTLE_TEST("Toxisphere sets extended Acid Rain with Acid Rock")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING) { Item(ITEM_ACID_ROCK); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_TOXISPHERE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ACID_RAIN);
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_EXTENDED - 1);
    }
}

SINGLE_BATTLE_TEST("Toxisphere blocks secondary effects from damaging moves during Acid Rain")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING) { Speed(50); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_TOXISPHERE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MACHAMP) { Speed(100); Ability(ABILITY_NO_GUARD); Moves(MOVE_DYNAMIC_PUNCH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DYNAMIC_PUNCH); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(player->hp < player->maxHP);
        EXPECT((player->status2 & STATUS2_CONFUSION) == 0);
    }
}

SINGLE_BATTLE_TEST("Toxisphere does not block secondaries after Acid Rain ends")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING) { Speed(50); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_TOXISPHERE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MACHAMP) { Speed(100); Ability(ABILITY_NO_GUARD); Moves(MOVE_SUNNY_DAY, MOVE_DYNAMIC_PUNCH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_DYNAMIC_PUNCH); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(player->status2 & STATUS2_CONFUSION);
    }
}
