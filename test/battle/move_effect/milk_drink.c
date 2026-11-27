#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Adventure balance: Milk Drink heals its protected ally")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT) { HP(10); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_PROTECT); MOVE(playerLeft, MOVE_MILK_DRINK, target: playerRight); }
    } THEN {
        EXPECT_EQ(playerRight->hp, 60);
    }
}
