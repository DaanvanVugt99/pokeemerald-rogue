#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_ICY_WIND].type == TYPE_ICE);
    ASSUME(gBattleMoves[MOVE_POWER_GEM].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_POWER_GEM].accuracy == 100);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_HARDEN].effect == EFFECT_DEFENSE_UP);
    ASSUME(gBattleMoves[MOVE_SHELL_SMASH].effect == EFFECT_SHELL_SMASH);
    ASSUME(gBattleMoves[MOVE_LAVA_PLUME].effect == EFFECT_BURN_HIT);
    ASSUME(gBattleMoves[MOVE_SMOKESCREEN].effect == EFFECT_ACCURACY_DOWN);
    ASSUME(gBattleMoves[MOVE_ROCK_TOMB].effect == EFFECT_SPEED_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
    ASSUME(gBattleMoves[MOVE_EXPLOSION].effect == EFFECT_EXPLOSION);
}

SINGLE_BATTLE_TEST("Basalt Shell uses a random crust move after being hit by Water, Ice, or Rock moves")
{
    u16 move;

    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_ICY_WIND; }
    PARAMETRIZE { move = MOVE_POWER_GEM; }

    GIVEN {
        PLAYER(SPECIES_MAGCARGO) { HP(5000); MaxHP(5000); Speed(50); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_BASALT_SHELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move, WITH_RNG(RNG_ROGUE_BASALT_SHELL, MOVE_HARDEN)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_BASALT_SHELL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HARDEN, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Basalt Shell does not trigger after other move types")
{
    GIVEN {
        PLAYER(SPECIES_MAGCARGO) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_BASALT_SHELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_EMBER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_EMBER, WITH_RNG(RNG_ROGUE_BASALT_SHELL, MOVE_HARDEN)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, opponent);
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_BASALT_SHELL);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Basalt Shell chooses from every crust move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_HARDEN,
        MOVE_SHELL_SMASH,
        MOVE_LAVA_PLUME,
        MOVE_SMOKESCREEN,
        MOVE_ROCK_TOMB,
        MOVE_RECOVER,
        MOVE_EXPLOSION,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_BASALT_SHELL);

    GIVEN {
        PLAYER(SPECIES_MAGCARGO) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_BASALT_SHELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BASALT_SHELL);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
