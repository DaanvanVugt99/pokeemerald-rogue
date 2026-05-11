#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Overclock uses Autotomize after first Multi-Attack each switch-in while holding a Memory")
{
    GIVEN {
        PLAYER(SPECIES_SILVALLY_NORMAL) { Item(ITEM_WATER_MEMORY); UniqueAbility(ABILITY_OVERCLOCK); Moves(MOVE_MULTI_ATTACK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MULTI_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Overclock does not trigger without a Memory")
{
    GIVEN {
        PLAYER(SPECIES_SILVALLY_NORMAL) { Item(ITEM_LEFTOVERS); UniqueAbility(ABILITY_OVERCLOCK); Moves(MOVE_MULTI_ATTACK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MULTI_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Overclock triggers only once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SILVALLY_NORMAL) { Item(ITEM_WATER_MEMORY); UniqueAbility(ABILITY_OVERCLOCK); Moves(MOVE_MULTI_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MULTI_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MULTI_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Overclock works for Type: Null")
{
    GIVEN {
        PLAYER(SPECIES_TYPE_NULL) { Item(ITEM_WATER_MEMORY); UniqueAbility(ABILITY_OVERCLOCK); Moves(MOVE_MULTI_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MULTI_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}
