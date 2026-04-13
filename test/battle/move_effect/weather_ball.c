#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].effect == EFFECT_WEATHER_BALL);
    ASSUME(gBattleMoves[MOVE_CORROSIVE_CLOUDS].effect == EFFECT_CORROSIVE_CLOUDS);
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
    ASSUME(gSpeciesInfo[SPECIES_MEGANIUM].types[0] == TYPE_GRASS
        || gSpeciesInfo[SPECIES_MEGANIUM].types[1] == TYPE_GRASS);
    ASSUME(gSpeciesInfo[SPECIES_ARCANINE].types[0] == TYPE_FIRE
        || gSpeciesInfo[SPECIES_ARCANINE].types[1] == TYPE_FIRE);
    ASSUME(gSpeciesInfo[SPECIES_MAGMAR].types[0] == TYPE_FIRE
        || gSpeciesInfo[SPECIES_MAGMAR].types[1] == TYPE_FIRE);
    ASSUME(gSpeciesInfo[SPECIES_DRAGONAIR].types[0] == TYPE_DRAGON
        || gSpeciesInfo[SPECIES_DRAGONAIR].types[1] == TYPE_DRAGON);
    ASSUME(gSpeciesInfo[SPECIES_DROWZEE].types[0] == TYPE_PSYCHIC
        || gSpeciesInfo[SPECIES_DROWZEE].types[1] == TYPE_PSYCHIC);
    ASSUME(gSpeciesInfo[SPECIES_WIGGLYTUFF].types[0] == TYPE_FAIRY
        || gSpeciesInfo[SPECIES_WIGGLYTUFF].types[1] == TYPE_FAIRY);
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to a Fire-type move in Sunlight", s16 damage)
{
    u16 move;
    PARAMETRIZE{ move = MOVE_CELEBRATE; }
    PARAMETRIZE{ move = MOVE_SUNNY_DAY; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MEGANIUM);
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(6.0), results[1].damage); // double base power + type effectiveness + sun 50% boost
    }
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to a Water-type move in Rain", s16 damage)
{
    u16 move;
    PARAMETRIZE{ move = MOVE_CELEBRATE; }
    PARAMETRIZE{ move = MOVE_RAIN_DANCE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARCANINE);
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(6.0), results[1].damage); // double base power + type effectiveness + rain 50% boost
    }
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to a Rock-type move in a Sandstorm", s16 damage)
{
    u16 move;
    PARAMETRIZE{ move = MOVE_CELEBRATE; }
    PARAMETRIZE{ move = MOVE_SANDSTORM; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MAGMAR) { Item(ITEM_SAFETY_GOGGLES); };
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage); // double base power + type effectiveness.
    }
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to an Ice-type move in Hail and Snow", s16 damage)
{
    u16 move;
    PARAMETRIZE{ move = MOVE_CELEBRATE; }
    PARAMETRIZE{ move = MOVE_HAIL; }
    PARAMETRIZE{ move = MOVE_SNOWSCAPE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DRAGONAIR) { Item(ITEM_SAFETY_GOGGLES); };
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage); // double base power + type effectiveness.
    }
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to a Poison-type move in Acid Rain", s16 damage)
{
    u16 move;
    PARAMETRIZE{ move = MOVE_CELEBRATE; }
    PARAMETRIZE{ move = MOVE_CORROSIVE_CLOUDS; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WIGGLYTUFF) { Item(ITEM_SAFETY_GOGGLES); };
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage); // double base power + Poison super effective.
    }
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to a Dark-type move in Eclipse", s16 damage)
{
    u16 move;
    PARAMETRIZE{ move = MOVE_CELEBRATE; }
    PARAMETRIZE{ move = MOVE_ECLIPSE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DROWZEE);
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(6.0), results[1].damage); // double base power + Dark super effective + Eclipse 50% boost.
    }
}
