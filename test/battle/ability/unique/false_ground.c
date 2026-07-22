#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("False Ground sets a random terrain on switch-in and enables Mimicry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_STUNFISK_GALARIAN) { Ability(ABILITY_MIMICRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_FALSE_GROUND, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FALSE_GROUND);
        ABILITY_POPUP(player, ABILITY_MIMICRY);
    } THEN {
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY, STATUS_FIELD_PLAIN_TERRAIN);
        EXPECT_EQ(player->type1, TYPE_NORMAL);
        EXPECT_EQ(player->type2, TYPE_NORMAL);
    }
}

SINGLE_BATTLE_TEST("False Ground does not replace an active terrain")
{
    GIVEN {
        PLAYER(SPECIES_GROOKEY) { Ability(ABILITY_GRASSY_SURGE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_STUNFISK_GALARIAN) { Ability(ABILITY_MIMICRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_FALSE_GROUND, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FALSE_GROUND);
    } THEN {
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY, STATUS_FIELD_GRASSY_TERRAIN);
    }
}
