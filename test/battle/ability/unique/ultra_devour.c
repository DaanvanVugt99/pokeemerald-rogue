#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Ultra Devour restores half HP after knocking out a target")
{
    GIVEN {
        PLAYER(SPECIES_GUZZLORD) { HP(100); MaxHP(300); Speed(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(300); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ULTRA_DEVOUR);
        HP_BAR(player);
        MESSAGE("Guzzlord regained health!");
    } THEN {
        EXPECT_EQ(player->hp, 250);
    }
}

SINGLE_BATTLE_TEST("Ultra Devour does not restore HP without a knockout")
{
    GIVEN {
        PLAYER(SPECIES_GUZZLORD) { HP(100); MaxHP(300); Speed(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_DEVOUR);
    } THEN {
        EXPECT_EQ(player->hp, 100);
    }
}

SINGLE_BATTLE_TEST("Ultra Devour does not restore HP if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_GUZZLORD) { HP(100); MaxHP(300); Speed(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_NIHILEGO) { Speed(1); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(300); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_DEVOUR);
    } THEN {
        EXPECT_EQ(player->hp, 100);
    }
}

SINGLE_BATTLE_TEST("Ultra Devour respects Heal Block")
{
    GIVEN {
        PLAYER(SPECIES_GUZZLORD) { HP(100); MaxHP(300); Speed(50); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(300); Speed(100); Moves(MOVE_HEAL_BLOCK); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ULTRA_DEVOUR);
        NONE_OF { HP_BAR(player, damage: -150); }
    } THEN {
        EXPECT_EQ(player->hp, 100);
    }
}
