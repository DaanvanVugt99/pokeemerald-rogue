#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HELPING_HAND].effect == EFFECT_HELPING_HAND);
}

DOUBLE_BATTLE_TEST("Playmaker uses Helping Hand on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CINDERACE) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_PLAYMAKER); }
    } WHEN {
        TURN { SWITCH(opponentLeft, 2); MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponentLeft, ABILITY_PLAYMAKER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    }
}
