#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_SWIFT].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Sharp Quills sets Spikes when hit by a physical move")
{
    u32 move;
    bool8 shouldSetSpikes;
    PARAMETRIZE { move = MOVE_TACKLE; shouldSetSpikes = TRUE; }
    PARAMETRIZE { move = MOVE_SWIFT; shouldSetSpikes = FALSE; }
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SHARP_QUILLS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        if (shouldSetSpikes)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIKES, player);
        else
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIKES, player);
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount > 0, shouldSetSpikes);
    }
}

SINGLE_BATTLE_TEST("Sharp Quills does not set more than three Spikes layers")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SHARP_QUILLS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 3);
    }
}

SINGLE_BATTLE_TEST("Sharp Quills still sets Spikes while the user is asleep")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SHARP_QUILLS); Status1(STATUS1_SLEEP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}

DOUBLE_BATTLE_TEST("Sharp Quills sets Spikes on the opposing side when hit by an ally")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SHARP_QUILLS); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_TACKLE, target: playerLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].spikesAmount, 0);
    }
}

DOUBLE_BATTLE_TEST("Sharp Quills can trigger from an ally hit when the user's side already has full Spikes")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SHARP_QUILLS); Moves(MOVE_CELEBRATE); Speed(5); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); Speed(5); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_CELEBRATE); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(5); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SPIKES);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(opponentLeft, MOVE_SPIKES);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(opponentLeft, MOVE_SPIKES);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_TACKLE, target: playerLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].spikesAmount, 3);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}
