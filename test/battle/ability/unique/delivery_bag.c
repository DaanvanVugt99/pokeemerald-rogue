#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PRESENT].effect == EFFECT_PRESENT);
    ASSUME(gBattleMoves[MOVE_RECYCLE].effect == EFFECT_RECYCLE);
    ASSUME(gBattleMoves[MOVE_ICE_SHARD].power > 0);
    ASSUME(gBattleMoves[MOVE_KNOCK_OFF].effect == EFFECT_KNOCK_OFF);
    ASSUME(gBattleMoves[MOVE_INCINERATE].effect == EFFECT_INCINERATE);
    ASSUME(gBattleMoves[MOVE_BESTOW].effect == EFFECT_BESTOW);
    ASSUME(gItems[ITEM_RAWST_BERRY].holdEffect == HOLD_EFFECT_CURE_BRN);
    ASSUME(gItems[ITEM_EJECT_BUTTON].holdEffect == HOLD_EFFECT_EJECT_BUTTON);
}

SINGLE_BATTLE_TEST("Delivery Bag uses a random gift move after consuming its held item")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Item(ITEM_ORAN_BERRY); HP(1); MaxHP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_ICE_SHARD)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DELIVERY_BAG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_SHARD, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Delivery Bag can choose Recycle after consuming its held item")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Item(ITEM_RAWST_BERRY); Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_RECYCLE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DELIVERY_BAG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECYCLE, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_RAWST_BERRY);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Delivery Bag uses a random gift move after its held item is knocked off")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_KNOCK_OFF); }
        OPPONENT(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Item(ITEM_ORAN_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_KNOCK_OFF, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_ICE_SHARD)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, player);
        ABILITY_POPUP(opponent, ABILITY_DELIVERY_BAG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_SHARD, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Delivery Bag uses a random gift move after its held item is incinerated")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_INCINERATE); }
        OPPONENT(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Item(ITEM_ORAN_BERRY); HP(100); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_INCINERATE, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_ICE_SHARD)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INCINERATE, player);
        ABILITY_POPUP(opponent, ABILITY_DELIVERY_BAG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_SHARD, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Delivery Bag uses a random gift move after bestowing its held item")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Item(ITEM_ORAN_BERRY); Moves(MOVE_BESTOW); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BESTOW, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_ICE_SHARD)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BESTOW, player);
        ABILITY_POPUP(player, ABILITY_DELIVERY_BAG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_SHARD, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_ORAN_BERRY);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Delivery Bag pending item loss does not transfer to a forced switch replacement")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_EJECT_BUTTON); }
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_ICE_SHARD)); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet is switched out with the Eject Button!");
        MESSAGE("Go! Delibird!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DELIVERY_BAG);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_SHARD, player);
        }
    }
}
