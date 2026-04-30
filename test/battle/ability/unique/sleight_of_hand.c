#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Sleight of Hand swaps held items after a contact move")
{
    GIVEN {
        PLAYER(SPECIES_LIEPARD) { Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_SLEIGHT_OF_HAND); Item(ITEM_CHARCOAL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_MYSTIC_WATER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SLEIGHT_OF_HAND);
    } THEN {
        EXPECT_EQ(player->item, ITEM_MYSTIC_WATER);
        EXPECT_EQ(opponent->item, ITEM_CHARCOAL);
    }
}

SINGLE_BATTLE_TEST("Sleight of Hand gives its held item to a target with no item")
{
    GIVEN {
        PLAYER(SPECIES_LIEPARD) { Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_SLEIGHT_OF_HAND); Item(ITEM_CHARCOAL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_NONE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SLEIGHT_OF_HAND);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_CHARCOAL);
    }
}

SINGLE_BATTLE_TEST("Sleight of Hand does not trigger without contact")
{
    GIVEN {
        PLAYER(SPECIES_LIEPARD) { Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_SLEIGHT_OF_HAND); Item(ITEM_CHARCOAL); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_MYSTIC_WATER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SLEIGHT_OF_HAND);
    } THEN {
        EXPECT_EQ(player->item, ITEM_CHARCOAL);
        EXPECT_EQ(opponent->item, ITEM_MYSTIC_WATER);
    }
}

SINGLE_BATTLE_TEST("Sleight of Hand does not trigger if the contact move fails")
{
    GIVEN {
        PLAYER(SPECIES_LIEPARD) { Speed(50); Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_SLEIGHT_OF_HAND); Item(ITEM_CHARCOAL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Item(ITEM_MYSTIC_WATER); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SLEIGHT_OF_HAND);
    } THEN {
        EXPECT_EQ(player->item, ITEM_CHARCOAL);
        EXPECT_EQ(opponent->item, ITEM_MYSTIC_WATER);
    }
}

SINGLE_BATTLE_TEST("Sleight of Hand is blocked by Sticky Hold")
{
    GIVEN {
        PLAYER(SPECIES_LIEPARD) { Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_SLEIGHT_OF_HAND); Item(ITEM_CHARCOAL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_STICKY_HOLD); Item(ITEM_MYSTIC_WATER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SLEIGHT_OF_HAND);
        ABILITY_POPUP(opponent, ABILITY_STICKY_HOLD);
        MESSAGE("Foe Wobbuffet's item cannot be removed!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_CHARCOAL);
        EXPECT_EQ(opponent->item, ITEM_MYSTIC_WATER);
    }
}

