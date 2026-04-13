#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Living Roots grants Ingrain to the incoming ally when switching out")
{
    GIVEN {
        PLAYER(SPECIES_TANGELA) { Speed(10); Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_LIVING_ROOTS); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Moves(MOVE_CELEBRATE, MOVE_ROAR); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ROAR); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Living Roots does not trigger when the user faints")
{
    GIVEN {
        PLAYER(SPECIES_TANGELA) { Speed(10); HP(1); Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_LIVING_ROOTS); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(15); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_QUICK_ATTACK, MOVE_ROAR); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); SEND_OUT(player, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ROAR); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
    }
}
