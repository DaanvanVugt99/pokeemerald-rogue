#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_X_SCISSOR].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_POISON_JAB].type == TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Neurotoxin can lower Speed and Sp. Def on Bug/Poison hits")
{
    u16 move;
    bool8 shouldProc;
    PARAMETRIZE { move = MOVE_X_SCISSOR; shouldProc = TRUE; }
    PARAMETRIZE { move = MOVE_POISON_JAB; shouldProc = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; shouldProc = FALSE; }

    GIVEN {
        PLAYER(SPECIES_VENONAT) { Ability(ABILITY_COMPOUND_EYES); UniqueAbility(ABILITY_NEUROTOXIN); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_STENCH, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        if (shouldProc)
        {
            EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        }
        else
        {
            EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        }
    }
}

SINGLE_BATTLE_TEST("Neurotoxin does not lower stats when the 50% check fails")
{
    GIVEN {
        PLAYER(SPECIES_VENONAT) { Ability(ABILITY_COMPOUND_EYES); UniqueAbility(ABILITY_NEUROTOXIN); Moves(MOVE_X_SCISSOR); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_X_SCISSOR, WITH_RNG(RNG_STENCH, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
