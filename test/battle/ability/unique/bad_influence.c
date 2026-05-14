#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_TAUNT].effect == EFFECT_TAUNT);
}

SINGLE_BATTLE_TEST("Bad Influence calls Taunt after the first status move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_GRIMMSNARL) { Ability(ABILITY_FRISK); UniqueAbility(ABILITY_BAD_INFLUENCE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_BAD_INFLUENCE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BAD_INFLUENCE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
        }
    }
}

SINGLE_BATTLE_TEST("Bad Influence does not trigger after non-status moves")
{
    GIVEN {
        PLAYER(SPECIES_GRIMMSNARL) { Ability(ABILITY_FRISK); UniqueAbility(ABILITY_BAD_INFLUENCE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BAD_INFLUENCE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
        }
    }
}
