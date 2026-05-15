#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_NO_RETREAT].effect == EFFECT_NO_RETREAT);
    ASSUME(gBattleMoves[MOVE_WORK_UP].effect == EFFECT_ATTACK_SPATK_UP);
    ASSUME(gBattleMoves[MOVE_WORK_UP].target == MOVE_TARGET_USER);
}

SINGLE_BATTLE_TEST("Formation uses a random formation move after this Pokemon uses No Retreat")
{
    GIVEN {
        PLAYER(SPECIES_FALINKS) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_FORMATION); Moves(MOVE_NO_RETREAT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_NO_RETREAT, WITH_RNG(RNG_ROGUE_FORMATION, MOVE_WORK_UP)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NO_RETREAT, player);
        ABILITY_POPUP(player, ABILITY_FORMATION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WORK_UP, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Formation does not trigger after moves other than No Retreat")
{
    GIVEN {
        PLAYER(SPECIES_FALINKS) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_FORMATION); Moves(MOVE_WORK_UP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WORK_UP, WITH_RNG(RNG_ROGUE_FORMATION, MOVE_ROCK_TOMB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FORMATION);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Formation chooses from every formation move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_FIRST_IMPRESSION,
        MOVE_QUICK_GUARD,
        MOVE_WIDE_GUARD,
        MOVE_PROTECT,
        MOVE_FEINT,
        MOVE_REVENGE,
        MOVE_RETALIATE,
        MOVE_FURY_ATTACK,
        MOVE_ROCK_TOMB,
        MOVE_WORK_UP,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_FORMATION);

    GIVEN {
        PLAYER(SPECIES_FALINKS) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_FORMATION); Moves(MOVE_NO_RETREAT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_NO_RETREAT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FORMATION);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
