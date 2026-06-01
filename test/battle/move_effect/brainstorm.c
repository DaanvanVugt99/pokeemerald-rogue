#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BRAINSTORM].effect == EFFECT_BRAINSTORM);
    ASSUME(gBattleMoves[MOVE_BRAINSTORM].type == TYPE_PSYCHIC);
    ASSUME(gBattleMoves[MOVE_BRAINSTORM].power == 60);
    ASSUME(gBattleMoves[MOVE_CONFUSION].effect == EFFECT_CONFUSE_HIT);
    ASSUME(gBattleMoves[MOVE_KINESIS].effect == EFFECT_ACCURACY_DOWN);
    ASSUME(gBattleMoves[MOVE_DISABLE].effect == EFFECT_DISABLE);
    ASSUME(gBattleMoves[MOVE_PSYWAVE].effect == EFFECT_PSYWAVE);
    ASSUME(gBattleMoves[MOVE_CALM_MIND].effect == EFFECT_CALM_MIND);
}

SINGLE_BATTLE_TEST("Brainstorm damages the target, then uses a random mental move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Moves(MOVE_BRAINSTORM); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BRAINSTORM, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_KINESIS)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BRAINSTORM, player);
        HP_BAR(opponent);
        MESSAGE("Wobbuffet used Kinesis!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KINESIS, player);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Brainstorm can use a self-targeting mental move on the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Moves(MOVE_BRAINSTORM); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BRAINSTORM, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_CALM_MIND)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Calm Mind!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, player);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Brainstorm does not use a mental move if the initial hit has no effect")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_BRAINSTORM); }
        OPPONENT(SPECIES_POOCHYENA) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BRAINSTORM, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_KINESIS)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("Wobbuffet used Kinesis!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_KINESIS, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Brainstorm does not crash if the random mental move fails after the hit")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Moves(MOVE_BRAINSTORM); }
        OPPONENT(SPECIES_BELDUM) { Ability(ABILITY_CLEAR_BODY); HP(400); MaxHP(400); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BRAINSTORM, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_KINESIS)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Kinesis!");
        ABILITY_POPUP(opponent, ABILITY_CLEAR_BODY);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}
