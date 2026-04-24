#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItems[ITEM_ROTTEN_BERRY].holdEffect == HOLD_EFFECT_ROTTEN_BERRY);
}

SINGLE_BATTLE_TEST("Rotten Berry heals Grass-, Poison-, and Bug-type holders at the normal berry threshold")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_SNIVY; }
    PARAMETRIZE { species = SPECIES_TRUBBISH; }
    PARAMETRIZE { species = SPECIES_KRICKETOT; }

    GIVEN {
        PLAYER(species) { Item(ITEM_ROTTEN_BERRY); HP(45); MaxHP(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 75);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Rotten Berry badly poisons non-Grass/Poison/Bug holders")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_ROTTEN_BERRY); HP(40); MaxHP(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(player->status1 & STATUS1_TOXIC_POISON);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Rotten Berry deals 1/8 max HP damage instead when toxic poison cannot be applied to a non-Grass/Poison/Bug holder")
{
    GIVEN {
        PLAYER(SPECIES_KLINK) { Item(ITEM_ROTTEN_BERRY); HP(40); MaxHP(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 30);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Rotten Berry uses the fallback damage when the holder already cannot receive toxic poison")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_ROTTEN_BERRY); HP(40); MaxHP(80); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 30);
        EXPECT_EQ(player->status1, STATUS1_PARALYSIS);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Rotten Berry does not activate above the normal berry threshold")
{
    GIVEN {
        PLAYER(SPECIES_SNIVY) { Item(ITEM_ROTTEN_BERRY); HP(46); MaxHP(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 46);
        EXPECT_EQ(player->item, ITEM_ROTTEN_BERRY);
    }
}
