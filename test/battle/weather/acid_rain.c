#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
    ASSUME(gSpeciesInfo[SPECIES_KOFFING].types[0] == TYPE_POISON || gSpeciesInfo[SPECIES_KOFFING].types[1] == TYPE_POISON);
    ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_POISON && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_POISON);
}

SINGLE_BATTLE_TEST("Acid rain deals 1/16 damage per turn")
{
    s16 acidRainDamage;

    GIVEN {
        PLAYER(SPECIES_KOFFING);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
    } SCENE {
        MESSAGE("Foe Wobbuffet is scorched by acid rain!");
        HP_BAR(opponent, captureDamage: &acidRainDamage);
    } THEN {
        EXPECT_EQ(acidRainDamage, opponent->maxHP / 16);
    }
}

SINGLE_BATTLE_TEST("Acid rain damage does not affect Poison-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KOFFING);
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
    } SCENE {
        NOT MESSAGE("Foe Koffing is scorched by acid rain!");
    }
}

SINGLE_BATTLE_TEST("Acid rain heals Poison-type Pokemon by 1/16 per turn")
{
    s16 acidRainHeal;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KOFFING) { HP(1); MaxHP(160); }
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
    } SCENE {
        MESSAGE("The acid rain restored Foe Koffing's HP a little!");
        HP_BAR(opponent, captureDamage: &acidRainHeal);
    } THEN {
        EXPECT_EQ(acidRainHeal, -(opponent->maxHP / 16));
    }
}

SINGLE_BATTLE_TEST("Acid rain does not heal Poison-type Pokemon at max HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KOFFING);
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
    } SCENE {
        NOT MESSAGE("The acid rain restored Foe Koffing's HP a little!");
    }
}

SINGLE_BATTLE_TEST("Acid Rain lasts for 12 turns with Acid Rock")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ACID_ROCK); Moves(MOVE_ACID_RAIN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ACID_RAIN);
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_EXTENDED - 1);
    }
}
