#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Toxisphere summons acid rain")
{
    GIVEN
    {
        PLAYER(SPECIES_WEEZING) { Ability(ABILITY_TOXISPHERE); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    }
    SCENE
    {
        ABILITY_POPUP(player, ABILITY_TOXISPHERE);
        MESSAGE("Foe Wobbuffet is scorched by the acid rain!");
    }
}
