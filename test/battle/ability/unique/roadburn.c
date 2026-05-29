#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_U_TURN].effect == EFFECT_HIT_ESCAPE);
    ASSUME(gBattleMoves[MOVE_SHED_TAIL].effect == EFFECT_SHED_TAIL);
    ASSUME(gBattleMoves[MOVE_RAPID_SPIN].effect == EFFECT_RAPID_SPIN);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
    ASSUME(gBattleMoves[MOVE_RAPID_SPIN].power > 20);
}

SINGLE_BATTLE_TEST("Roadburn uses Rapid Spin before a switching move switches Cyclizar out")
{
    GIVEN {
        PLAYER(SPECIES_CYCLIZAR) { Speed(100); Ability(ABILITY_SHED_SKIN); UniqueAbility(ABILITY_ROADBURN); Moves(MOVE_U_TURN); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_ROADBURN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
    }
}

SINGLE_BATTLE_TEST("Roadburn uses Rapid Spin before Shed Tail switches Cyclizar out")
{
    GIVEN {
        PLAYER(SPECIES_CYCLIZAR) { HP(100); MaxHP(100); Speed(100); Ability(ABILITY_SHED_SKIN); UniqueAbility(ABILITY_ROADBURN); Moves(MOVE_SHED_TAIL); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(100); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHED_TAIL); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHED_TAIL, player);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_ROADBURN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
        EXPECT(player->status2 & STATUS2_SUBSTITUTE);
    }
}

SINGLE_BATTLE_TEST("Roadburn clears hazards with Rapid Spin before Cyclizar switches out")
{
    GIVEN {
        PLAYER(SPECIES_CYCLIZAR) { Speed(1); Ability(ABILITY_SHED_SKIN); UniqueAbility(ABILITY_ROADBURN); Moves(MOVE_CELEBRATE, MOVE_U_TURN); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROADBURN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Roadburn does not trigger if the switching move fails")
{
    GIVEN {
        PLAYER(SPECIES_CYCLIZAR) { Speed(1); Ability(ABILITY_SHED_SKIN); UniqueAbility(ABILITY_ROADBURN); Moves(MOVE_U_TURN); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_U_TURN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ROADBURN);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_CYCLIZAR);
    }
}

SINGLE_BATTLE_TEST("Roadburn does not trigger if Cyclizar cannot switch out")
{
    GIVEN {
        PLAYER(SPECIES_CYCLIZAR) { Attack(100); Speed(100); Ability(ABILITY_SHED_SKIN); UniqueAbility(ABILITY_ROADBURN); Moves(MOVE_U_TURN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(100); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_U_TURN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ROADBURN);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_CYCLIZAR);
    }
}
