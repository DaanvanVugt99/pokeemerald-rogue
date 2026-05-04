#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
    ASSUME(gSpeciesInfo[SPECIES_KOFFING].types[0] == TYPE_POISON || gSpeciesInfo[SPECIES_KOFFING].types[1] == TYPE_POISON);
    ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_POISON && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_POISON);
    ASSUME(gSpeciesInfo[SPECIES_CATERPIE].types[0] == TYPE_BUG || gSpeciesInfo[SPECIES_CATERPIE].types[1] == TYPE_BUG);
    ASSUME(gSpeciesInfo[SPECIES_CATERPIE].types[0] != TYPE_POISON && gSpeciesInfo[SPECIES_CATERPIE].types[1] != TYPE_POISON);
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

SINGLE_BATTLE_TEST("Acid rain damage does not affect Bug-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE);
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
    } SCENE {
        NONE_OF {
            MESSAGE("Foe Caterpie is scorched by acid rain!");
            MESSAGE("The acid rain restored Foe Caterpie's HP a little!");
        }
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

SINGLE_BATTLE_TEST("Venoshock doubles in power during Acid Rain", s16 damage)
{
    bool32 acidRain;
    PARAMETRIZE { acidRain = FALSE; }
    PARAMETRIZE { acidRain = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (acidRain)
            TURN { MOVE(player, MOVE_ACID_RAIN); }
        TURN { MOVE(player, MOVE_VENOSHOCK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VENOSHOCK, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Acid Armor raises Defense by 3 during Acid Rain")
{
    bool32 acidRain;
    PARAMETRIZE { acidRain = FALSE; }
    PARAMETRIZE { acidRain = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (acidRain)
            TURN { MOVE(player, MOVE_ACID_RAIN); }
        TURN { MOVE(player, MOVE_ACID_ARMOR); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (acidRain ? 3 : 2));
    }
}
