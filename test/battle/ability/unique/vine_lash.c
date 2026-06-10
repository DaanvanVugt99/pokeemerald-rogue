#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].power > 0);
    ASSUME(gBattleMoves[MOVE_SMACK_DOWN].effect == EFFECT_SMACK_DOWN);
}

SINGLE_BATTLE_TEST("Vine Lash triggers only on the first Grass-type move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_VENUSAUR) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VINE_LASH); Moves(MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_MAGICAL_LEAF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGICAL_LEAF, player);
        ABILITY_POPUP(player, ABILITY_VINE_LASH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SMACK_DOWN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGICAL_LEAF, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VINE_LASH);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SMACK_DOWN, player);
        }
    }
}

SINGLE_BATTLE_TEST("Vine Lash refreshes after the user switches out and back in")
{
    GIVEN {
        PLAYER(SPECIES_VENUSAUR) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VINE_LASH); Moves(MOVE_MAGICAL_LEAF); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_MAGICAL_LEAF); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_VINE_LASH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SMACK_DOWN, player);
        ABILITY_POPUP(player, ABILITY_VINE_LASH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SMACK_DOWN, player);
    }
}
