#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Pestilent Drift sets Infested Terrain on switch-in if any foe is statused")
{
    GIVEN {
        PLAYER(SPECIES_MOTHIM) { Ability(ABILITY_SWARM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PESTILENT_DRIFT);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Pestilent Drift does not set Infested Terrain if no foe is statused")
{
    GIVEN {
        PLAYER(SPECIES_MOTHIM) { Ability(ABILITY_SWARM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_PESTILENT_DRIFT);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN));
    }
}

DOUBLE_BATTLE_TEST("Pestilent Drift checks both opposing battlers for status")
{
    GIVEN {
        PLAYER(SPECIES_MOTHIM) { Ability(ABILITY_SWARM); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_PESTILENT_DRIFT);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN);
    }
}
