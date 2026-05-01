#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Overgrowth raises Defense by 1 at end of turn in Grassy Terrain")
{
    GIVEN {
        PLAYER(SPECIES_GOGOAT) { Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_OVERGROWTH); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_GRASSY_SURGE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Overgrowth does not raise Defense without Grassy Terrain")
{
    GIVEN {
        PLAYER(SPECIES_GOGOAT) { Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_OVERGROWTH); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
