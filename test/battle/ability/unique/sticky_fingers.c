#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_COVET].effect == EFFECT_THIEF);
}

SINGLE_BATTLE_TEST("Sticky Fingers makes Dark-type moves also use Covet at 20 BP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_STICKY_FINGERS); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        ABILITY_POPUP(player, ABILITY_STICKY_FINGERS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COVET, player);
    }
}

SINGLE_BATTLE_TEST("Sticky Fingers does not trigger from non-Dark-type moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_STICKY_FINGERS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STICKY_FINGERS);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_COVET, player);
        }
    }
}
