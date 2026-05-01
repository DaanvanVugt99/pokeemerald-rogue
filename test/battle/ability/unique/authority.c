#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].priority > 0);
    ASSUME(gBattleMoves[MOVE_BITE].priority == 0);
    ASSUME(gBattleMoves[MOVE_RECOVER].split == SPLIT_STATUS);
}

SINGLE_BATTLE_TEST("Authority blocks opposing priority moves while active")
{
    GIVEN {
        PLAYER(SPECIES_AEGISLASH_SHIELD);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_AUTHORITY);
        NOT { HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Authority does not block opposing non-priority moves")
{
    GIVEN {
        PLAYER(SPECIES_AEGISLASH_SHIELD);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Authority does not block opposing self-targeting priority moves")
{
    GIVEN {
        PLAYER(SPECIES_SABLEYE) { Ability(ABILITY_PRANKSTER); HP(1); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_AEGISLASH_SHIELD) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        HP_BAR(player);
    } THEN {
        EXPECT_GT(player->hp, 1);
    }
}

DOUBLE_BATTLE_TEST("Authority blocks priority moves targeting its partner")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_AEGISLASH_SHIELD) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_QUICK_ATTACK, target: playerLeft); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_AUTHORITY);
        NOT { HP_BAR(playerLeft); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Authority does not block allied priority moves")
{
    GIVEN {
        PLAYER(SPECIES_AEGISLASH_SHIELD) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_QUICK_ATTACK, target: opponentLeft); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, playerRight);
        HP_BAR(opponentLeft);
    } THEN {
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
    }
}
