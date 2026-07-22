#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_YAWN));
    ASSUME(!IS_MOVE_STATUS(MOVE_CONFUSION));
}

SINGLE_BATTLE_TEST("Royal Rebuke lowers the target's highest stat after a status move")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_ROYAL_REBUKE); Moves(MOVE_YAWN); }
        OPPONENT(SPECIES_BLISSEY) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_YAWN); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Royal Rebuke does not trigger on damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_ROYAL_REBUKE); Moves(MOVE_CONFUSION); }
        OPPONENT(SPECIES_BLISSEY) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSION); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
