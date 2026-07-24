#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_U_TURN].effect == EFFECT_HIT_ESCAPE);
    ASSUME(gBattleMoves[MOVE_TACKLE].effect != EFFECT_HIT_ESCAPE);
}

SINGLE_BATTLE_TEST("Eject gives pivot moves +1 priority")
{
    GIVEN {
        PLAYER(SPECIES_DRAGAPULT) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_EJECT); Moves(MOVE_U_TURN); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Eject does not give non-pivot moves priority")
{
    GIVEN {
        PLAYER(SPECIES_DRAGAPULT) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_EJECT); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}
