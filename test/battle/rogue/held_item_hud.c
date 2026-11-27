#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Held item HUD singles updates after Trick")
{
    GIVEN {
        PLAYER(SPECIES_JIRACHI) { Item(ITEM_LEFTOVERS); Moves(MOVE_TRICK); }
        OPPONENT(SPECIES_FLYGON) { Item(ITEM_CHOICE_SCARF); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_CHOICE_SCARF);
        EXPECT_EQ(opponent->item, ITEM_LEFTOVERS);
    }
}

DOUBLE_BATTLE_TEST("Held item HUD doubles supports occupied and empty slots")
{
    GIVEN {
        PLAYER(SPECIES_JIRACHI) { Item(ITEM_LEFTOVERS); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_AMPHAROS) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FLYGON) { Item(ITEM_CHOICE_SCARF); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GARDEVOIR) { Item(ITEM_LIFE_ORB); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(playerRight->item, ITEM_NONE);
    }
}
