#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Hollow Nest ignores entry hazards on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SHEDINJA) { Ability(ABILITY_WONDER_GUARD); HP(1); MaxHP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Hollow Nest still takes sandstorm damage outside Infested Terrain")
{
    GIVEN {
        PLAYER(SPECIES_SHEDINJA) { Ability(ABILITY_WONDER_GUARD); HP(1); MaxHP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SANDSTORM); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SANDSTORM); }
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}

SINGLE_BATTLE_TEST("Hollow Nest ignores weather damage during Infested Terrain")
{
    GIVEN {
        PLAYER(SPECIES_SHEDINJA) { Ability(ABILITY_WONDER_GUARD); HP(1); MaxHP(1); Moves(MOVE_INFESTED_TERRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SANDSTORM); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); MOVE(opponent, MOVE_SANDSTORM); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}
