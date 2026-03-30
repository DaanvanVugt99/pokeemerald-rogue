#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CRUNCH].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
}

SINGLE_BATTLE_TEST("Ambush gives +1 priority to biting moves on first turn")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK) { Speed(50); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_AMBUSH); Moves(MOVE_CRUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CRUNCH); MOVE(opponent, MOVE_AERIAL_ACE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CRUNCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
    }
}

SINGLE_BATTLE_TEST("Ambush does not affect non-biting moves")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK) { Speed(50); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_AMBUSH); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

SINGLE_BATTLE_TEST("Ambush no longer boosts biting move priority after first turn")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK) { Speed(50); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_AMBUSH); Moves(MOVE_CELEBRATE, MOVE_CRUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE, MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CRUNCH); MOVE(opponent, MOVE_AERIAL_ACE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CRUNCH, player);
    }
}
