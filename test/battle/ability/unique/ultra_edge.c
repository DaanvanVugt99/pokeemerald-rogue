#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CUT].slicingMove);
    ASSUME(gBattleMoves[MOVE_CUT].type == TYPE_NORMAL);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Ultra Edge makes slicing moves Steel type")
{
    GIVEN {
        PLAYER(SPECIES_KARTANA) { Speed(100); Attack(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CUT); }
        OPPONENT(SPECIES_SUDOWOODO) { Speed(1); Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CUT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Ultra Edge makes slicing moves super effective against Grass Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_KARTANA) { Speed(100); Attack(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CUT); }
        OPPONENT(SPECIES_TANGELA) { Speed(1); Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CUT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Ultra Edge does not affect non-slicing moves")
{
    GIVEN {
        PLAYER(SPECIES_KARTANA) { Speed(100); Attack(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_TANGELA) { Speed(1); Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Ultra Edge does not activate if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_KARTANA) { Speed(100); Attack(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CUT); }
        PLAYER(SPECIES_NIHILEGO) { Speed(1); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TANGELA) { Speed(1); Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CUT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT MESSAGE("It's super effective!");
    }
}
