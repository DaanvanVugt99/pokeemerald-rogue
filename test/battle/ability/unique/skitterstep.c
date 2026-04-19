#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Skitterstep prevents trapping effects like Mean Look")
{
    GIVEN {
        PLAYER(SPECIES_NINJASK) { Ability(ABILITY_SPEED_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_MEAN_LOOK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_MEAN_LOOK); }
    } THEN {
        EXPECT_EQ(player->status2 & STATUS2_ESCAPE_PREVENTION, 0);
    }
}

SINGLE_BATTLE_TEST("Skitterstep is affected by entry hazards outside Infested Terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_NINCADA) { Ability(ABILITY_COMPOUND_EYES); HP(100); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Skitterstep ignores entry hazards during Infested Terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_INFESTED_TERRAIN, MOVE_CELEBRATE); }
        PLAYER(SPECIES_NINCADA) { Ability(ABILITY_COMPOUND_EYES); HP(100); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}
