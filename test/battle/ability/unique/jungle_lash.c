#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEAN_LOOK].effect == EFFECT_MEAN_LOOK);
}

SINGLE_BATTLE_TEST("Jungle Lash damages trapped foes when the user switches out")
{
    GIVEN {
        PLAYER(SPECIES_ZARUDE) { Moves(MOVE_MEAN_LOOK); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { HP(160); MaxHP(160); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEAN_LOOK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEAN_LOOK, player);
        ABILITY_POPUP(player, ABILITY_JUNGLE_LASH);
        HP_BAR(opponent, damage: 20);
    } THEN {
        EXPECT_EQ(opponent->hp, 140);
    }
}
