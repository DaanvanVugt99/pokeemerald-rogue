#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_TOXIC_SPIKES].effect == EFFECT_TOXIC_SPIKES);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
}

SINGLE_BATTLE_TEST("Pickup clears entry hazards from its side on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MEOWTH) { Ability(ABILITY_PICKUP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_TOXIC_SPIKES, MOVE_STEALTH_ROCK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PICKUP);
        MESSAGE("Meowth's Pickup\ncleared away the hazards!");
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TOXIC_SPIKES));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Pickup clears Sticky Web from its side on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MEOWTH) { Ability(ABILITY_PICKUP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_STICKY_WEB, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PICKUP);
        MESSAGE("Meowth's Pickup\ncleared away the hazards!");
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STICKY_WEB));
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
