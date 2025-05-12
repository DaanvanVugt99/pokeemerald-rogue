#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Molten Down makes Fire moves super effective against Rock types and lowers Speed")
{
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
        PLAYER(SPECIES_MAGMORTAR) { Ability(ABILITY_MOLTEN_DOWN); }
        OPPONENT(SPECIES_GOLEM);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_EMBER); }
    }
    SCENE
    {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("Golem's Speed fell!");
    }
}

SINGLE_BATTLE_TEST("Molten Down does not trigger on non-Rock types")
{
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
        PLAYER(SPECIES_MAGMORTAR) { Ability(ABILITY_MOLTEN_DOWN); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_EMBER); }
    }
    SCENE
    {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
        NONE_OF
        {
            MESSAGE("It's super effective!");
            MESSAGE("Speed fell!");
        }
    }
}