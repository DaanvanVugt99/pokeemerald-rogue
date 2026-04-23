#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_COLD_SNAP].effect == EFFECT_SUCKER_PUNCH);
    ASSUME(gBattleMoves[MOVE_COLD_SNAP].type == TYPE_ICE);
    ASSUME(gBattleMoves[MOVE_COLD_SNAP].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_COLD_SNAP].power == 60);
    ASSUME(gBattleMoves[MOVE_COLD_SNAP].accuracy == 100);
    ASSUME(gBattleMoves[MOVE_COLD_SNAP].pp == 10);
}

SINGLE_BATTLE_TEST("Cold Snap move works when the target uses a damaging move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_COLD_SNAP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_COLD_SNAP); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COLD_SNAP, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
    }
}

SINGLE_BATTLE_TEST("Cold Snap move fails when the target uses a status move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_COLD_SNAP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_COLD_SNAP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Cold Snap!");
        MESSAGE("But it failed!");
        NONE_OF { HP_BAR(opponent); }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}
