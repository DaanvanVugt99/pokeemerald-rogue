#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(B_PROTEAN_LIBERO < GEN_9);
}

SINGLE_BATTLE_TEST("Protean and Libero change the user's type every time they use a move")
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_PROTEAN; }
    PARAMETRIZE { ability = ABILITY_LIBERO; }

    GIVEN {
        PLAYER(SPECIES_REGIROCK);
        OPPONENT(SPECIES_KECLEON) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(opponent, ability);
        MESSAGE("Foe Kecleon transformed into the Water type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        ABILITY_POPUP(opponent, ability);
        MESSAGE("Foe Kecleon transformed into the Normal type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
    }
}
