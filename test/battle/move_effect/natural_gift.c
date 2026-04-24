#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_NATURAL_GIFT].effect == EFFECT_NATURAL_GIFT);
}

SINGLE_BATTLE_TEST("Natural Gift uses Rotten Berry safely and consumes it")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ROTTEN_BERRY); Moves(MOVE_NATURAL_GIFT); }
        OPPONENT(SPECIES_CHIKORITA) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_NATURAL_GIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Natural Gift!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NATURAL_GIFT, player);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}
