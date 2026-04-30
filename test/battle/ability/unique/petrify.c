#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Petrify clears removable boosts and lowers opposing Speed on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_COFAGRIGUS) { Ability(ABILITY_MUMMY); UniqueAbility(ABILITY_PETRIFY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_DANCE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PETRIFY);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Petrify does not clear boosts from Clear Body targets")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_COFAGRIGUS) { Ability(ABILITY_MUMMY); UniqueAbility(ABILITY_PETRIFY); }
        OPPONENT(SPECIES_METAGROSS) { Ability(ABILITY_CLEAR_BODY); Moves(MOVE_DRAGON_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_DANCE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PETRIFY);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Petrify does not clear boosts from Clear Amulet holders")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_COFAGRIGUS) { Ability(ABILITY_MUMMY); UniqueAbility(ABILITY_PETRIFY); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_CLEAR_AMULET); Moves(MOVE_DRAGON_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_DANCE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PETRIFY);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Petrify affects only opposing Pokemon and both foes in doubles")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_HARDEN, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_COFAGRIGUS) { Ability(ABILITY_MUMMY); UniqueAbility(ABILITY_PETRIFY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_DANCE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWTH, MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HARDEN);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_DRAGON_DANCE);
            MOVE(opponentRight, MOVE_GROWTH);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            SWITCH(playerRight, 2);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_PETRIFY);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}
