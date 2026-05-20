#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BULLDOZE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_BULLDOZE].power > 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Rumble Roll raises Defense and clears hazards after Ground-type moves")
{
    GIVEN {
        PLAYER(SPECIES_PHANPY) { Ability(ABILITY_PICKUP); Moves(MOVE_CELEBRATE, MOVE_BULLDOZE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_BULLDOZE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RUMBLE_ROLL);
        MESSAGE("Phanpy's Rumble Roll\ncleared away the hazards!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Rumble Roll returns correctly when only clearing hazards")
{
    GIVEN {
        PLAYER(SPECIES_DONPHAN) { Ability(ABILITY_STURDY); Moves(MOVE_CELEBRATE, MOVE_IRON_DEFENSE, MOVE_BULLDOZE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_BULLDOZE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RUMBLE_ROLL);
        MESSAGE("Donphan's Rumble Roll\ncleared away the hazards!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], MAX_STAT_STAGE);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Rumble Roll does not trigger after non-Ground damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_DONPHAN) { Ability(ABILITY_STURDY); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES);
    }
}
