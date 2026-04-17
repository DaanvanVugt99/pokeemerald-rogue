#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Abyssal Light lowers the attacker's Accuracy by 2 when hit by a super-effective move")
{
    GIVEN {
        PLAYER(SPECIES_LANTURN)     { Ability(ABILITY_VOLT_ABSORB); UniqueAbility(ABILITY_ABYSSAL_LIGHT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].statStages[STAT_ACC], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Abyssal Light does not trigger on non-super-effective moves")
{
    GIVEN {
        PLAYER(SPECIES_LANTURN)     { Ability(ABILITY_VOLT_ABSORB); UniqueAbility(ABILITY_ABYSSAL_LIGHT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}
