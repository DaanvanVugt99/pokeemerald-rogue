#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Dread lowers a random opposing stat at end of turn in rain")
{
    GIVEN {
        PLAYER(SPECIES_GYARADOS) { Ability(ABILITY_MOXIE); UniqueAbility(ABILITY_DREAD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
    } THEN {
        EXPECT(opponent->statStages[STAT_ATK] < DEFAULT_STAT_STAGE
            || opponent->statStages[STAT_DEF] < DEFAULT_STAT_STAGE
            || opponent->statStages[STAT_SPEED] < DEFAULT_STAT_STAGE
            || opponent->statStages[STAT_SPATK] < DEFAULT_STAT_STAGE
            || opponent->statStages[STAT_SPDEF] < DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Dread does not lower stats outside rain")
{
    GIVEN {
        PLAYER(SPECIES_GYARADOS) { Ability(ABILITY_MOXIE); UniqueAbility(ABILITY_DREAD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
