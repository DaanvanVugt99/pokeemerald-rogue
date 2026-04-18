#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Cold Read raises Speed by 1 after using Focus Energy")
{
    GIVEN {
        PLAYER(SPECIES_SNEASEL) { Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_COLD_READ); Moves(MOVE_FOCUS_ENERGY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOCUS_ENERGY); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Cold Read does not raise Speed after other moves")
{
    GIVEN {
        PLAYER(SPECIES_SNEASEL) { Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_COLD_READ); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
