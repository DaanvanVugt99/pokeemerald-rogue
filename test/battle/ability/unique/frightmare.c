#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].baseSpeed < gSpeciesInfo[SPECIES_SNORLAX].baseAttack);
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].baseSpeed < gSpeciesInfo[SPECIES_SNORLAX].baseDefense);
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].baseSpeed < gSpeciesInfo[SPECIES_SNORLAX].baseSpAttack);
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].baseSpeed < gSpeciesInfo[SPECIES_SNORLAX].baseSpDefense);
    ASSUME(gSpeciesInfo[SPECIES_CLOYSTER].baseSpDefense < gSpeciesInfo[SPECIES_CLOYSTER].baseAttack);
    ASSUME(gSpeciesInfo[SPECIES_CLOYSTER].baseSpDefense < gSpeciesInfo[SPECIES_CLOYSTER].baseDefense);
    ASSUME(gSpeciesInfo[SPECIES_CLOYSTER].baseSpDefense < gSpeciesInfo[SPECIES_CLOYSTER].baseSpeed);
    ASSUME(gSpeciesInfo[SPECIES_CLOYSTER].baseSpDefense < gSpeciesInfo[SPECIES_CLOYSTER].baseSpAttack);
    ASSUME(gSpeciesInfo[SPECIES_MAGIKARP].baseAttack < gSpeciesInfo[SPECIES_MAGIKARP].baseDefense);
    ASSUME(gSpeciesInfo[SPECIES_MAGIKARP].baseAttack < gSpeciesInfo[SPECIES_MAGIKARP].baseSpeed);
    ASSUME(gSpeciesInfo[SPECIES_MAGIKARP].baseAttack < gSpeciesInfo[SPECIES_MAGIKARP].baseSpAttack);
    ASSUME(gSpeciesInfo[SPECIES_MAGIKARP].baseAttack < gSpeciesInfo[SPECIES_MAGIKARP].baseSpDefense);
}

SINGLE_BATTLE_TEST("Frightmare lowers Speed for Snorlax on switch-in")
{
    u32 loweredCount;
    u32 loweredStatId;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GENGAR) { Ability(ABILITY_CURSED_BODY); UniqueAbility(ABILITY_FRIGHTMARE); }
        OPPONENT(SPECIES_SNORLAX) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        loweredCount = 0;
        loweredStatId = 0;
        if (opponent->statStages[STAT_ATK] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_ATK; }
        if (opponent->statStages[STAT_DEF] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_DEF; }
        if (opponent->statStages[STAT_SPEED] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_SPEED; }
        if (opponent->statStages[STAT_SPATK] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_SPATK; }
        if (opponent->statStages[STAT_SPDEF] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_SPDEF; }
        EXPECT_EQ(loweredCount, 1);
        EXPECT_EQ(loweredStatId, STAT_SPEED);
    }
}

SINGLE_BATTLE_TEST("Frightmare lowers Sp. Def for Cloyster on switch-in")
{
    u32 loweredCount;
    u32 loweredStatId;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GENGAR) { Ability(ABILITY_CURSED_BODY); UniqueAbility(ABILITY_FRIGHTMARE); }
        OPPONENT(SPECIES_CLOYSTER) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        loweredCount = 0;
        loweredStatId = 0;
        if (opponent->statStages[STAT_ATK] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_ATK; }
        if (opponent->statStages[STAT_DEF] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_DEF; }
        if (opponent->statStages[STAT_SPEED] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_SPEED; }
        if (opponent->statStages[STAT_SPATK] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_SPATK; }
        if (opponent->statStages[STAT_SPDEF] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_SPDEF; }
        EXPECT_EQ(loweredCount, 1);
        EXPECT_EQ(loweredStatId, STAT_SPDEF);
    }
}

SINGLE_BATTLE_TEST("Frightmare lowers Attack for Magikarp on switch-in")
{
    u32 loweredCount;
    u32 loweredStatId;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GENGAR) { Ability(ABILITY_CURSED_BODY); UniqueAbility(ABILITY_FRIGHTMARE); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        loweredCount = 0;
        loweredStatId = 0;
        if (opponent->statStages[STAT_ATK] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_ATK; }
        if (opponent->statStages[STAT_DEF] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_DEF; }
        if (opponent->statStages[STAT_SPEED] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_SPEED; }
        if (opponent->statStages[STAT_SPATK] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_SPATK; }
        if (opponent->statStages[STAT_SPDEF] < DEFAULT_STAT_STAGE) { loweredCount++; loweredStatId = STAT_SPDEF; }
        EXPECT_EQ(loweredCount, 1);
        EXPECT_EQ(loweredStatId, STAT_ATK);
    }
}

DOUBLE_BATTLE_TEST("Frightmare affects the battler in the switched-in user's opposite slot in doubles")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GENGAR) { Ability(ABILITY_CURSED_BODY); UniqueAbility(ABILITY_FRIGHTMARE); }
        OPPONENT(SPECIES_ONIX) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SNORLAX) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            SWITCH(playerRight, 2);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}
