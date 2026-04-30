#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
}

SINGLE_BATTLE_TEST("Web Tailor makes slicing moves also use Sticky Web with two other Bug-type allies")
{
    GIVEN {
        PLAYER(SPECIES_LEAVANNY) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TAILOR); Moves(MOVE_LEAF_BLADE); }
        PLAYER(SPECIES_WURMPLE);
        PLAYER(SPECIES_CATERPIE);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ABILITY_POPUP(player, ABILITY_WEB_TAILOR);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STICKY_WEB, player);
        MESSAGE("A sticky web spreads out on the ground around the opposing team!");
    }
}

SINGLE_BATTLE_TEST("Web Tailor does not trigger without two other Bug-type allies")
{
    GIVEN {
        PLAYER(SPECIES_LEAVANNY) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TAILOR); Moves(MOVE_LEAF_BLADE); }
        PLAYER(SPECIES_WURMPLE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WEB_TAILOR);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STICKY_WEB, player);
        }
    }
}
