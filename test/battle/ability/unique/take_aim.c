#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LOCK_ON].effect == EFFECT_LOCK_ON);
}

SINGLE_BATTLE_TEST("Take Aim uses Lock-On on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_INTELEON) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_TAKE_AIM); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_TAKE_AIM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOCK_ON, opponent);
    }
}
