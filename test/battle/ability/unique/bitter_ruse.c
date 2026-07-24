#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BRICK_BREAK].type == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_BOOMBURST].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_BOOMBURST].target == MOVE_TARGET_FOES_AND_ALLY);
    ASSUME(gBattleMoves[MOVE_BITTER_MALICE].type == TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_BITTER_MALICE].effect == EFFECT_ATTACK_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Bitter Ruse reveals the user and retaliates when Illusion baits a type immunity")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK_HISUIAN) { Speed(1); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_BITTER_RUSE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_BRICK_BREAK); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BRICK_BREAK); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BITTER_RUSE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ILLUSION_OFF, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITTER_MALICE, player);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(GetIllusionMonSpecies(B_POSITION_PLAYER_LEFT), SPECIES_NONE);
    }
}

DOUBLE_BATTLE_TEST("Bitter Ruse remembers an immunity before a spread move's final target")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK_HISUIAN) { Speed(1); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_BITTER_RUSE); }
        PLAYER(SPECIES_SHUCKLE) { Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_BOOMBURST); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_BOOMBURST); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_BITTER_RUSE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ILLUSION_OFF, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITTER_MALICE, playerLeft);
    } THEN {
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(GetIllusionMonSpecies(B_POSITION_PLAYER_LEFT), SPECIES_NONE);
    }
}

DOUBLE_BATTLE_TEST("Bitter Ruse remembers an immunity in the other spread target position")
{
    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { Speed(1); }
        PLAYER(SPECIES_ZOROARK_HISUIAN) { Speed(1); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_BITTER_RUSE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_BOOMBURST); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_BOOMBURST); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_BITTER_RUSE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ILLUSION_OFF, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITTER_MALICE, playerRight);
    } THEN {
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(GetIllusionMonSpecies(B_POSITION_PLAYER_RIGHT), SPECIES_NONE);
    }
}

SINGLE_BATTLE_TEST("Bitter Ruse still reveals when Bitter Malice cannot affect the attacker")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK_HISUIAN) { Speed(1); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_BITTER_RUSE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_SNORLAX) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BITTER_RUSE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ILLUSION_OFF, player);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
        EXPECT_EQ(GetIllusionMonSpecies(B_POSITION_PLAYER_LEFT), SPECIES_NONE);
    }
}

SINGLE_BATTLE_TEST("Bitter Ruse does not activate when Protect blocks the move")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK_HISUIAN) { Speed(100); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_BITTER_RUSE); Moves(MOVE_PROTECT); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_BRICK_BREAK); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_BRICK_BREAK); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BITTER_RUSE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BITTER_MALICE, player);
        }
    } THEN {
        EXPECT_EQ(GetIllusionMonSpecies(B_POSITION_PLAYER_LEFT), SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Bitter Ruse requires an active Illusion")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK_HISUIAN) { Speed(1); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_BITTER_RUSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_BRICK_BREAK); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BRICK_BREAK); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BITTER_RUSE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BITTER_MALICE, player);
        }
    }
}
