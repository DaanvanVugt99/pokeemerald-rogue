#include "global.h"
#include "test/battle.h"
#include "battle_interface.h"
#include "item.h"

ASSUMPTIONS
{
    ASSUME(B_LAST_USED_BALL == TRUE);
}

static void ClearBallPocket(void)
{
    ClearItemSlots(gBagPockets[BALLS_POCKET].itemSlots, gBagPockets[BALLS_POCKET].capacity);
}

WILD_BATTLE_TEST("Automatic ball selection preserves a tied ball with greater catch potential")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        ClearBallPocket();
        AddBagItem(ITEM_HEAVY_BALL, 1);
        AddBagItem(ITEM_POKE_BALL, 1);
        SelectBestBallToDisplay();
        EXPECT_EQ(gBallToDisplay, ITEM_POKE_BALL);
    }
}

WILD_BATTLE_TEST("Automatic ball selection prioritizes the highest active catch modifier")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SQUIRTLE);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        ClearBallPocket();
        AddBagItem(ITEM_POKE_BALL, 1);
        AddBagItem(ITEM_NET_BALL, 1);
        SelectBestBallToDisplay();
        EXPECT_EQ(gBallToDisplay, ITEM_NET_BALL);
    }
}

WILD_BATTLE_TEST("Automatic ball selection does not select the Master Ball when another ball is available")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        ClearBallPocket();
        AddBagItem(ITEM_MASTER_BALL, 1);
        AddBagItem(ITEM_POKE_BALL, 1);
        SelectBestBallToDisplay();
        EXPECT_EQ(gBallToDisplay, ITEM_POKE_BALL);
    }
}

WILD_BATTLE_TEST("Automatic ball selection selects the Master Ball when it is the only ball available")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        ClearBallPocket();
        AddBagItem(ITEM_MASTER_BALL, 1);
        SelectBestBallToDisplay();
        EXPECT_EQ(gBallToDisplay, ITEM_MASTER_BALL);
    }
}

TO_DO_BATTLE_TEST("Poke Balls can't be thrown when there's 2 opposing wild battlers")
TO_DO_BATTLE_TEST("Poke Balls can't be thrown when there's no space in the Pokemon Storage System")
TO_DO_BATTLE_TEST("Poke Balls can't be thrown when an opposing wild battler is in a semi-invulnerable state")
TO_DO_BATTLE_TEST("Poke Balls can't be thrown when B_FLAG_NO_CATCHING is set")
