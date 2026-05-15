#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_OCTOLOCK].effect == EFFECT_OCTOLOCK);
    ASSUME(gBattleMoves[MOVE_BULK_UP].target == MOVE_TARGET_USER);
}

SINGLE_BATTLE_TEST("Octolock uses a random hold move after this Pokemon uses Octolock")
{
    GIVEN {
        PLAYER(SPECIES_GRAPPLOCT) { Speed(100); Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_OCTOLOCK); Moves(MOVE_OCTOLOCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_OCTOLOCK, WITH_RNG(RNG_ROGUE_OCTOLOCK, MOVE_BULK_UP)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_OCTOLOCK, player);
        ABILITY_POPUP(player, ABILITY_OCTOLOCK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULK_UP, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Octolock chooses from every hold move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_BIND,
        MOVE_WRAP,
        MOVE_CLAMP,
        MOVE_SUBMISSION,
        MOVE_CIRCLE_THROW,
        MOVE_LOW_SWEEP,
        MOVE_BULK_UP,
        MOVE_FOCUS_ENERGY,
        MOVE_DETECT,
        MOVE_REVENGE,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_OCTOLOCK);

    GIVEN {
        PLAYER(SPECIES_GRAPPLOCT) { Speed(100); Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_OCTOLOCK); Moves(MOVE_OCTOLOCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_OCTOLOCK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_OCTOLOCK);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
