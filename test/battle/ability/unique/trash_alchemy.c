#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItems[ITEM_SILK_SCARF].holdEffect != HOLD_EFFECT_NONE);
}

SINGLE_BATTLE_TEST("Trash Alchemy consumes a held item, heals for one sixteenth, and finds a random held item in trainer battles")
{
    GIVEN {
        PLAYER(SPECIES_GRIMER_ALOLAN) { HP(80); MaxHP(160); Ability(ABILITY_POISON_TOUCH); UniqueAbility(ABILITY_TRASH_ALCHEMY); Item(ITEM_SILK_SCARF); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRASH_ALCHEMY);
    } THEN {
        EXPECT_EQ(player->hp, 90);
        EXPECT_NE(player->item, ITEM_NONE);
        EXPECT_NE(gItems[player->item].holdEffect, HOLD_EFFECT_NONE);
    }
}

SINGLE_BATTLE_TEST("Trash Alchemy generated items persist after trainer battles")
{
    GIVEN {
        PLAYER(SPECIES_GRIMER_ALOLAN) { HP(80); MaxHP(160); Ability(ABILITY_POISON_TOUCH); UniqueAbility(ABILITY_TRASH_ALCHEMY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRASH_ALCHEMY);
    } THEN {
        EXPECT_EQ(player->hp, 80);
        EXPECT_NE(player->item, ITEM_NONE);
        EXPECT_NE(gItems[player->item].holdEffect, HOLD_EFFECT_NONE);

        TryRestoreHeldItems();
        EXPECT_NE(GetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

WILD_BATTLE_TEST("Trash Alchemy does not activate in wild battles")
{
    GIVEN {
        PLAYER(SPECIES_GRIMER_ALOLAN) { HP(80); MaxHP(160); Ability(ABILITY_POISON_TOUCH); UniqueAbility(ABILITY_TRASH_ALCHEMY); Item(ITEM_SILK_SCARF); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GRIMER_ALOLAN) { HP(80); MaxHP(160); Ability(ABILITY_POISON_TOUCH); UniqueAbility(ABILITY_TRASH_ALCHEMY); Item(ITEM_SILK_SCARF); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 80);
        EXPECT_EQ(opponent->hp, 80);
        EXPECT_EQ(player->item, ITEM_SILK_SCARF);
        EXPECT_EQ(opponent->item, ITEM_SILK_SCARF);
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM), ITEM_SILK_SCARF);
        EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM), ITEM_SILK_SCARF);
    }
}
