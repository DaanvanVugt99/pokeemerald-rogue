#include "global.h"
#include "test/battle.h"

// Please add Rain interactions with move, item and ability effects on their respective files.
ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
}

SINGLE_BATTLE_TEST("Rain multiplies the power of Fire-type moves by 0.5x", s16 damage)
{
    u32 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_RAIN_DANCE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Rain multiplies the power of Water-type moves by 1.5x", s16 damage)
{
    u32 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_RAIN_DANCE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Rain Dance lasts for 8 turns by default")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_TURNS - 1);
    }
}

SINGLE_BATTLE_TEST("Rain Dance lasts for 12 turns with Damp Rock")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_DAMP_ROCK); Moves(MOVE_RAIN_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_EXTENDED - 1);
    }
}

SINGLE_BATTLE_TEST("Weather still overwrites correctly")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE, MOVE_SUNNY_DAY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_SUNNY_DAY); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(!(gBattleWeather & B_WEATHER_RAIN));
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_TURNS - 1);
    }
}
