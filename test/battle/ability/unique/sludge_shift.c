#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sludge Shift uses Psycho Shift at end of turn if the user is statused")
{
    GIVEN {
        PLAYER(SPECIES_GOODRA) { Speed(50); Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_SLUDGE_SHIFT); Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Sludge Shift does not trigger if the user has no status condition")
{
    GIVEN {
        PLAYER(SPECIES_GOODRA) { Speed(50); Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_SLUDGE_SHIFT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Sludge Shift does not trigger if the target is already statused")
{
    GIVEN {
        PLAYER(SPECIES_GOODRA) { Speed(50); Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_SLUDGE_SHIFT); Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_BURN);
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
    }
}

DOUBLE_BATTLE_TEST("Sludge Shift can target the other foe if the opposite foe cannot receive the user's status")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_PSYCHO_SHIFT].effect == EFFECT_PSYCHO_SHIFT);
        PLAYER(SPECIES_GOOMY) { Speed(50); Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_SLUDGE_SHIFT); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_SLUDGE_SHIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHO_SHIFT, playerLeft);
        STATUS_ICON(opponentRight, paralysis: TRUE);
    } THEN {
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
        EXPECT_EQ(opponentRight->status1, STATUS1_PARALYSIS);
    }
}
