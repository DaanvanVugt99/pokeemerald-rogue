#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CORROSIVE_CLOUDS].effect == EFFECT_CORROSIVE_CLOUDS);
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
        TURN { MOVE(player, MOVE_CORROSIVE_CLOUDS); }
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
        TURN { MOVE(player, MOVE_CORROSIVE_CLOUDS); }
    } SCENE {
        NOT MESSAGE("Foe Koffing is scorched by acid rain!");
    }
}

SINGLE_BATTLE_TEST("Acid rain reduces the Speed of non-Poison Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE, MOVE_CORROSIVE_CLOUDS); }
        OPPONENT(SPECIES_KOFFING) { Speed(95); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CORROSIVE_CLOUDS); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Corrosive Clouds!");
        MESSAGE("Foe Koffing used Celebrate!");
        MESSAGE("Foe Koffing used Celebrate!");
        MESSAGE("Wobbuffet used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("Acid rain does not reduce the Speed of Poison-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_KOFFING) { Speed(100); Moves(MOVE_CELEBRATE, MOVE_CORROSIVE_CLOUDS); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(95); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CORROSIVE_CLOUDS); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Koffing used Corrosive Clouds!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Koffing used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
    }
}
