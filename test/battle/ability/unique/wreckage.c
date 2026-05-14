#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_ANCHOR_SHOT].power > 0);
    ASSUME(gBattleMoves[MOVE_WHIRLPOOL].effect == EFFECT_TRAP);
    ASSUME(gBattleMoves[MOVE_RAPID_SPIN].effect == EFFECT_RAPID_SPIN);
    ASSUME(gBattleMoves[MOVE_GYRO_BALL].effect == EFFECT_GYRO_BALL);
    ASSUME(gBattleMoves[MOVE_ASTONISH].effect == EFFECT_FLINCH_HIT);
}

SINGLE_BATTLE_TEST("Wreckage uses a random wreckage move after being hit by a resisted move")
{
    GIVEN {
        PLAYER(SPECIES_DHELMISE) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_STEELWORKER); UniqueAbility(ABILITY_WRECKAGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN, WITH_RNG(RNG_ROGUE_WRECKAGE, MOVE_ASTONISH)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_WRECKAGE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ASTONISH, player);
    }
}

SINGLE_BATTLE_TEST("Wreckage does not trigger after non-resisted moves")
{
    GIVEN {
        PLAYER(SPECIES_DHELMISE) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_STEELWORKER); UniqueAbility(ABILITY_WRECKAGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_EMBER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_EMBER, WITH_RNG(RNG_ROGUE_WRECKAGE, MOVE_ASTONISH)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, opponent);
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_WRECKAGE);
    }
}

SINGLE_BATTLE_TEST("Wreckage chooses from every wreckage move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_ANCHOR_SHOT,
        MOVE_WHIRLPOOL,
        MOVE_RAPID_SPIN,
        MOVE_GYRO_BALL,
        MOVE_ASTONISH,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_WRECKAGE);

    GIVEN {
        PLAYER(SPECIES_DHELMISE) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_STEELWORKER); UniqueAbility(ABILITY_WRECKAGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WRECKAGE);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
