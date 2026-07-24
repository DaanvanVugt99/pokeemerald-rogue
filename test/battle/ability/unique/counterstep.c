#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AGILITY].effect == EFFECT_SPEED_UP_2);
    ASSUME(gBattleMoves[MOVE_DRAGON_DANCE].effect == EFFECT_DRAGON_DANCE);
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
    ASSUME(gBattleMoves[MOVE_LOW_SWEEP].effect == EFFECT_SPEED_DOWN_HIT);
}

SINGLE_BATTLE_TEST("Counterstep uses Low Sweep when an opposing Pokemon's Speed rises")
{
    GIVEN {
        PLAYER(SPECIES_LILLIGANT_HISUIAN) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Defense(120); HP(1000); MaxHP(1000); Moves(MOVE_AGILITY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_AGILITY); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AGILITY, opponent);
        ABILITY_POPUP(player, ABILITY_COUNTERSTEP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOW_SWEEP, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Counterstep triggers once for a move that raises multiple stats")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); UniqueAbility(ABILITY_COUNTERSTEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DRAGON_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_DANCE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_DANCE, opponent);
        ABILITY_POPUP(player, ABILITY_COUNTERSTEP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOW_SWEEP, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Counterstep does not react to a non-Speed stat rise")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); UniqueAbility(ABILITY_COUNTERSTEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_COUNTERSTEP);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LOW_SWEEP, player);
        }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Counterstep does not react to its own Speed rise")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); UniqueAbility(ABILITY_COUNTERSTEP); Moves(MOVE_AGILITY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_COUNTERSTEP);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LOW_SWEEP, player);
        }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

DOUBLE_BATTLE_TEST("Counterstep targets the opposing Pokemon whose Speed rose")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); UniqueAbility(ABILITY_COUNTERSTEP); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_AGILITY); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_AGILITY);
            MOVE(opponentRight, MOVE_CELEBRATE);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COUNTERSTEP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOW_SWEEP, playerLeft);
        HP_BAR(opponentLeft);
    } THEN {
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Each active Counterstep holder reacts to an opposing Speed rise")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); UniqueAbility(ABILITY_COUNTERSTEP); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); UniqueAbility(ABILITY_COUNTERSTEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Defense(120); HP(1000); MaxHP(1000); Moves(MOVE_AGILITY); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_AGILITY);
            MOVE(opponentRight, MOVE_CELEBRATE);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COUNTERSTEP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOW_SWEEP, playerLeft);
        HP_BAR(opponentLeft);
        ABILITY_POPUP(playerRight, ABILITY_COUNTERSTEP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOW_SWEEP, playerRight);
        HP_BAR(opponentLeft);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
