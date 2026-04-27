#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].priority == 0);
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_GROWL].priority == 0);
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
}

SINGLE_BATTLE_TEST("Fatal Grace gives +1 priority to damaging moves against targets at or below half HP")
{
    GIVEN {
        PLAYER(SPECIES_PURUGLY) { Speed(50); Ability(ABILITY_THICK_FAT); UniqueAbility(ABILITY_FATAL_GRACE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); HP(200); MaxHP(400); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Fatal Grace does not give priority when the target is above half HP")
{
    GIVEN {
        PLAYER(SPECIES_PURUGLY) { Speed(50); Ability(ABILITY_THICK_FAT); UniqueAbility(ABILITY_FATAL_GRACE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); HP(51); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

SINGLE_BATTLE_TEST("Fatal Grace does not give priority to status moves")
{
    GIVEN {
        PLAYER(SPECIES_PURUGLY) { Speed(50); Ability(ABILITY_THICK_FAT); UniqueAbility(ABILITY_FATAL_GRACE); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
    }
}
