#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].priority == 0);
    ASSUME(!gBattleMoves[MOVE_AERIAL_ACE].soundMove);
    ASSUME(gBattleMoves[MOVE_ROUND].soundMove);
    ASSUME(gBattleMoves[MOVE_ROUND].priority == 0);
}

SINGLE_BATTLE_TEST("Chatterbox gives the next sound-based move +1 priority after a Flying-type move")
{
    GIVEN {
        PLAYER(SPECIES_CHATOT) { Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_CHATTERBOX); Moves(MOVE_AERIAL_ACE, MOVE_ROUND); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ROUND); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ROUND); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROUND, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROUND, player);
    }
}

SINGLE_BATTLE_TEST("Chatterbox does not give sound-based moves priority before a Flying-type move")
{
    GIVEN {
        PLAYER(SPECIES_CHATOT) { Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_CHATTERBOX); Moves(MOVE_ROUND); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROUND); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROUND, player);
    }
}
