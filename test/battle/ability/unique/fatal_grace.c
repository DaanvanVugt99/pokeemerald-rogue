#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SCRATCH].power > 0);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].power == 0);
}

SINGLE_BATTLE_TEST("Fatal Grace gives +1 priority only for damaging moves against targets at or below half HP")
{
    u16 move;
    u16 targetHp;
    bool8 shouldGoFirst;
    PARAMETRIZE { move = MOVE_SCRATCH; targetHp = 50; shouldGoFirst = TRUE; }
    PARAMETRIZE { move = MOVE_SCRATCH; targetHp = 51; shouldGoFirst = FALSE; }
    PARAMETRIZE { move = MOVE_CELEBRATE; targetHp = 50; shouldGoFirst = FALSE; }

    GIVEN {
        PLAYER(SPECIES_MEOWTH) { Speed(50); Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_FATAL_GRACE); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); MaxHP(100); HP(targetHp); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_AERIAL_ACE); }
    } SCENE {
        if (shouldGoFirst) {
            ANIMATION(ANIM_TYPE_MOVE, move, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, move, player);
        }
    }
}
