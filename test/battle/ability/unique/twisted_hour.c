#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Twisted Hour reverses move order while the user is active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOTHITELLE) { Speed(50); Ability(ABILITY_FRISK); UniqueAbility(ABILITY_TWISTED_HOUR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

