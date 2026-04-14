#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Marksman can raise Accuracy by 1 at end of turn")
{
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_MARKSMAN);
    GIVEN {
        PLAYER(SPECIES_SEADRA) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_MARKSMAN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(gBattleStruct->bonusCritStages[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], 0);
    }
}

SINGLE_BATTLE_TEST("Marksman can raise critical-hit ratio by 1 stage at end of turn")
{
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_MARKSMAN);
    GIVEN {
        PLAYER(SPECIES_SEADRA) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_MARKSMAN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gBattleStruct->bonusCritStages[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], 1);
    }
}
