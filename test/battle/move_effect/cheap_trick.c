#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CHEAP_TRICK].effect == EFFECT_CHEAP_TRICK);
    ASSUME(gBattleMoves[MOVE_CHEAP_TRICK].priority == gBattleMoves[MOVE_FAKE_OUT].priority);
    ASSUME(gBattleMoves[MOVE_FAKE_OUT].effect == EFFECT_FAKE_OUT);
    ASSUME(gBattleMoves[MOVE_TAUNT].effect == EFFECT_TAUNT);
    ASSUME(gBattleMoves[MOVE_SAND_ATTACK].effect == EFFECT_ACCURACY_DOWN);
}

SINGLE_BATTLE_TEST("Cheap Trick works on the user's first turn and can use Fake Out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Attack(200); Moves(MOVE_CHEAP_TRICK); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); HP(400); MaxHP(400); Defense(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CHEAP_TRICK, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_FAKE_OUT)); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHEAP_TRICK, player);
        MESSAGE("Wobbuffet used Fake Out!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, player);
        MESSAGE("Foe Wynaut flinched!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Cheap Trick fails entirely after the user's first turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_CHEAP_TRICK); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CHEAP_TRICK, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_TAUNT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Cheap Trick!");
        MESSAGE("But it failed!");
        NONE_OF {
            MESSAGE("Wobbuffet used Taunt!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ((int)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Cheap Trick does not use a cheap trick if the accuracy drop fails")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CHEAP_TRICK); }
        OPPONENT(SPECIES_BELDUM) { Ability(ABILITY_CLEAR_BODY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CHEAP_TRICK, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_TAUNT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_CLEAR_BODY);
        NONE_OF {
            MESSAGE("Wobbuffet used Taunt!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ((int)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Cheap Trick can use an opposing-target cheap trick move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CHEAP_TRICK); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CHEAP_TRICK, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_TAUNT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Taunt!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
        EXPECT_NE((int)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Cheap Trick does not crash if the random cheap trick move fails after the accuracy drop")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CHEAP_TRICK); }
        OPPONENT(SPECIES_DUSKULL) { HP(400); MaxHP(400); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CHEAP_TRICK, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_FAKE_OUT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Fake Out!");
        MESSAGE("It doesn't affect Foe Duskull…");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
