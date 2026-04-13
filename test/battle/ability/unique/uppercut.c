#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
}

SINGLE_BATTLE_TEST("Uppercut gives +1 priority only on the first punching move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_HITMONCHAN) { Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_UPPERCUT); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
    }
}

SINGLE_BATTLE_TEST("Uppercut does not consume on non-punching moves")
{
    GIVEN {
        PLAYER(SPECIES_HITMONCHAN) { Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_UPPERCUT); Moves(MOVE_TACKLE, MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Uppercut refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_HITMONCHAN) { Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_UPPERCUT); Moves(MOVE_MEGA_PUNCH); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}
