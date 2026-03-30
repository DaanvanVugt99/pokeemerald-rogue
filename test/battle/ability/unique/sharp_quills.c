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
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount > 0, shouldSetSpikes);
    }
}

SINGLE_BATTLE_TEST("Sharp Quills does not set more than three Spikes layers")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SHARP_QUILLS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 3);
    }
}
