#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Diamond Storm rolls its Defense boost once per target hit")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_DIAMOND_STORM].effect == EFFECT_HIT);
        ASSUME(gBattleMoves[MOVE_DIAMOND_STORM].secondaryEffectChance == 50);
        ASSUME(gBattleMoves[MOVE_DIAMOND_STORM].target == MOVE_TARGET_BOTH);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DIAMOND_STORM, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 4);
    }
}
