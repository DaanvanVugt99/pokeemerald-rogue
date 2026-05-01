#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SURF].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_SACRED_SWORD].type == TYPE_FIGHTING);
}

SINGLE_BATTLE_TEST("Flowing Lesson: Water move primes next Fighting move to lower Defense")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO) { Speed(100); Moves(MOVE_SURF, MOVE_SACRED_SWORD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SURF); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SACRED_SWORD); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Flowing Lesson: Fighting move without Water prime does not lower Defense")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO) { Speed(100); Moves(MOVE_SACRED_SWORD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SACRED_SWORD); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Resolute Blade: Fighting move primes next Water move to lower Sp. Def")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO_RESOLUTE) { Speed(100); Moves(MOVE_SACRED_SWORD, MOVE_SURF); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(200); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SACRED_SWORD); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SURF); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Resolute Blade is consumed after one Water move")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO_RESOLUTE) { Speed(100); Moves(MOVE_SACRED_SWORD, MOVE_SURF); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(200); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SACRED_SWORD); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SURF); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SURF); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}
