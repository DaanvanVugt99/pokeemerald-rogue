#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LOCK_ON].effect == EFFECT_LOCK_ON);
    ASSUME(gBattleMoves[MOVE_U_TURN].effect == EFFECT_HIT_ESCAPE);
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

SINGLE_BATTLE_TEST("Take Aim does not suppress the next move's attack string")
{
    GIVEN {
        PLAYER(SPECIES_INTELEON) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_TAKE_AIM); Moves(MOVE_U_TURN); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TAKE_AIM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOCK_ON, player);
        MESSAGE("Inteleon used U-turn!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player);
    }
}
