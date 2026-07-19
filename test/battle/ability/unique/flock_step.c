#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
    ASSUME(gBattleMoves[MOVE_SHELL_SMASH].effect == EFFECT_SHELL_SMASH);
    ASSUME(gBattleMoves[MOVE_POWER_UP_PUNCH].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_ROTOTILLER].effect == EFFECT_ROTOTILLER);
    ASSUME(gBattleMoves[MOVE_FLOWER_SHIELD].effect == EFFECT_FLOWER_SHIELD);
}

SINGLE_BATTLE_TEST("Flock Step repeats a foe's successful self-boosting status move")
{
    GIVEN {
        PLAYER(SPECIES_FLAMIGO) { Ability(ABILITY_SCRAPPY); UniqueAbility(ABILITY_FLOCK_STEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
        ABILITY_POPUP(player, ABILITY_FLOCK_STEP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Flock Step can repeat a setup move after switching in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_FLAMIGO) { Ability(ABILITY_SCRAPPY); UniqueAbility(ABILITY_FLOCK_STEP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SWORDS_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
        ABILITY_POPUP(player, ABILITY_FLOCK_STEP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

DOUBLE_BATTLE_TEST("Flock Step repeats an ally's self-boosting status move")
{
    GIVEN {
        PLAYER(SPECIES_FLAMIGO) { Speed(50); Ability(ABILITY_SCRAPPY); UniqueAbility(ABILITY_FLOCK_STEP); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(25); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SWORDS_DANCE); MOVE(playerLeft, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_FLOCK_STEP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Flock Step repeats the full Shell Smash effect")
{
    GIVEN {
        PLAYER(SPECIES_FLAMIGO) { Ability(ABILITY_SCRAPPY); UniqueAbility(ABILITY_FLOCK_STEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SHELL_SMASH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SHELL_SMASH); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Flock Step activates only once per battle")
{
    GIVEN {
        PLAYER(SPECIES_FLAMIGO) { Ability(ABILITY_SCRAPPY); UniqueAbility(ABILITY_FLOCK_STEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE, MOVE_CALM_MIND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CALM_MIND); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLOCK_STEP);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FLOCK_STEP);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Flock Step ignores damaging moves that raise the user's stats")
{
    GIVEN {
        PLAYER(SPECIES_FLAMIGO) { Ability(ABILITY_SCRAPPY); UniqueAbility(ABILITY_FLOCK_STEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1); Moves(MOVE_POWER_UP_PUNCH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_POWER_UP_PUNCH); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FLOCK_STEP);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Flock Step requires the move to actually raise the user's stats")
{
    GIVEN {
        PLAYER(SPECIES_FLAMIGO) { Ability(ABILITY_SCRAPPY); UniqueAbility(ABILITY_FLOCK_STEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_CONTRARY); Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FLOCK_STEP);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Flock Step repeats field-wide moves that raise the user's stats")
{
    u16 move;
    u32 stat;

    PARAMETRIZE { move = MOVE_ROTOTILLER; stat = STAT_ATK; }
    PARAMETRIZE { move = MOVE_FLOWER_SHIELD; stat = STAT_DEF; }

    GIVEN {
        PLAYER(SPECIES_CHERRIM_OVERCAST) { Speed(50); Ability(ABILITY_FLOWER_GIFT); UniqueAbility(ABILITY_FLOCK_STEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(100); Ability(ABILITY_OVERGROW); Moves(move); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        ABILITY_POPUP(player, ABILITY_FLOCK_STEP);
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    } THEN {
        EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 2);
    }
}
