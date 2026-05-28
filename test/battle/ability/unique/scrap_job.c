#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLING].effect == EFFECT_FLING);
    ASSUME(gBattleMoves[MOVE_KNOCK_OFF].effect == EFFECT_KNOCK_OFF);
    ASSUME(gBattleMoves[MOVE_TRICK].effect == EFFECT_TRICK);
}

SINGLE_BATTLE_TEST("Scrap Job scatters Spikes when Tinkaton consumes its item")
{
    GIVEN {
        PLAYER(SPECIES_TINKATON) { MaxHP(100); HP(100); Item(ITEM_SITRUS_BERRY); Moves(MOVE_BELLY_DRUM); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].spikesAmount, 0);
    }
}

SINGLE_BATTLE_TEST("Scrap Job scatters Spikes when Tinkaton loses its item with Fling")
{
    GIVEN {
        PLAYER(SPECIES_TINKATON) { Item(ITEM_REPEL); Moves(MOVE_FLING); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(1000); HP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLING); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Tinkaton used Fling!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}

SINGLE_BATTLE_TEST("Scrap Job scatters Spikes when Tinkaton's item is removed")
{
    GIVEN {
        PLAYER(SPECIES_TINKATON) { Speed(1); MaxHP(500); HP(500); Item(ITEM_LEFTOVERS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_KNOCK_OFF); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_KNOCK_OFF); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].spikesAmount, 0);
    }
}

SINGLE_BATTLE_TEST("Scrap Job scatters Spikes on the player's side when opposing Tinkaton loses its item")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_KNOCK_OFF); }
        OPPONENT(SPECIES_TINKATON) { Speed(1); MaxHP(500); HP(500); Item(ITEM_LEFTOVERS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_KNOCK_OFF); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].spikesAmount, 1);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 0);
    }
}

SINGLE_BATTLE_TEST("Scrap Job scatters Spikes when Tinkaton swaps away its item")
{
    GIVEN {
        PLAYER(SPECIES_TINKATON) { Item(ITEM_LEFTOVERS); Moves(MOVE_TRICK); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ORAN_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_ORAN_BERRY);
        EXPECT_EQ(opponent->item, ITEM_LEFTOVERS);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}
