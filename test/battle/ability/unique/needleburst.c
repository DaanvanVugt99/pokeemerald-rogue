#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
}

SINGLE_BATTLE_TEST("Needleburst sets a layer of Spikes after using a contact move")
{
    GIVEN {
        PLAYER(SPECIES_CACTURNE) { Ability(ABILITY_SAND_VEIL); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(player, ABILITY_NEEDLEBURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIKES, player);
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}

SINGLE_BATTLE_TEST("Needleburst does not set Spikes after a non-contact move")
{
    GIVEN {
        PLAYER(SPECIES_CACTURNE) { Ability(ABILITY_SAND_VEIL); Moves(MOVE_ABSORB); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_NEEDLEBURST);
        }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 0);
    }
}

SINGLE_BATTLE_TEST("Needleburst does not set Spikes if the contact move is blocked")
{
    GIVEN {
        PLAYER(SPECIES_CACTURNE) { Ability(ABILITY_SAND_VEIL); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_NEEDLEBURST);
        }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 0);
    }
}
