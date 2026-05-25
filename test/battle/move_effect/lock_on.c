#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mind Reader is a Psychic-type Lock-On move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMoves[MOVE_MIND_READER].effect, EFFECT_LOCK_ON);
        EXPECT_EQ(gBattleMoves[MOVE_MIND_READER].type, TYPE_PSYCHIC);
        EXPECT_EQ(gBattleMoves[MOVE_MIND_READER].split, SPLIT_STATUS);
        EXPECT_EQ(gBattleMoves[MOVE_MIND_READER].power, 0);
        EXPECT_EQ(gBattleMoves[MOVE_MIND_READER].pp, 5);
    }
}
