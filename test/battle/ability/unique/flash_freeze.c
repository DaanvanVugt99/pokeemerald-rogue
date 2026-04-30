#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
}

SINGLE_BATTLE_TEST("Flash Freeze grants Fire immunity and sets Snow")
{
    GIVEN {
        PLAYER(SPECIES_CRYOGONAL) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_FLASH_FREEZE); HP(160); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { Moves(MOVE_EMBER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_EMBER); }
    } THEN {
        EXPECT_EQ(player->hp, 160);
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
    }
}

SINGLE_BATTLE_TEST("Flash Freeze does not set Snow from non-Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_CRYOGONAL) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_FLASH_FREEZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SQUIRTLE) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SNOW));
    }
}

SINGLE_BATTLE_TEST("Flash Freeze restores 1/16 HP at end of turn in Snow")
{
    GIVEN {
        PLAYER(SPECIES_CRYOGONAL) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_FLASH_FREEZE); HP(80); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SNOWSCAPE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SNOWSCAPE); }
    } THEN {
        EXPECT_EQ(player->hp, 90);
    }
}
