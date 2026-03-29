#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SCARY_FACE].effect == EFFECT_SPEED_DOWN_2);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Venom Rush doubles Speed if the target is poisoned or slowed")
{
    u32 targetStatus1;
    u16 turn1Move;
    bool8 shouldMoveFirst;
    PARAMETRIZE { targetStatus1 = STATUS1_POISON; turn1Move = MOVE_CELEBRATE; shouldMoveFirst = TRUE; }
    PARAMETRIZE { targetStatus1 = STATUS1_NONE; turn1Move = MOVE_SCARY_FACE; shouldMoveFirst = TRUE; }
    PARAMETRIZE { targetStatus1 = STATUS1_NONE; turn1Move = MOVE_CELEBRATE; shouldMoveFirst = FALSE; }
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { HP(1); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_VENOM_RUSH); Speed(60); Moves(turn1Move, MOVE_TACKLE); }
        OPPONENT(SPECIES_CLEFABLE) { HP(1); Status1(targetStatus1); Ability(ABILITY_MAGIC_GUARD); Speed(100); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, turn1Move); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->hp != 0, shouldMoveFirst);
    }
}
