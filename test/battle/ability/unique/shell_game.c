#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHELL_SMASH].effect == EFFECT_SHELL_SMASH);
    ASSUME(gBattleMoves[MOVE_MISTY_TERRAIN].effect == EFFECT_MISTY_TERRAIN);
}

SINGLE_BATTLE_TEST("Shell Game transfers Shell Smash's defensive drops in Misty Terrain")
{
    GIVEN {
        PLAYER(SPECIES_GOREBYSS) {
            Speed(1);
            Ability(ABILITY_SWIFT_SWIM);
            UniqueAbility(ABILITY_SHELL_GAME);
            Moves(MOVE_SHELL_SMASH);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_MISTY_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MISTY_TERRAIN); MOVE(player, MOVE_SHELL_SMASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MISTY_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SMASH, player);
        ABILITY_POPUP(player, ABILITY_SHELL_GAME);
        MESSAGE("The mist disappeared from the battlefield.");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
    }
}

SINGLE_BATTLE_TEST("Shell Game does not alter Shell Smash outside Misty Terrain")
{
    GIVEN {
        PLAYER(SPECIES_GOREBYSS) {
            Ability(ABILITY_SWIFT_SWIM);
            UniqueAbility(ABILITY_SHELL_GAME);
            Moves(MOVE_SHELL_SMASH);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHELL_SMASH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SMASH, player);
        NOT ABILITY_POPUP(player, ABILITY_SHELL_GAME);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shell Game transfers Shell Smash's drops through stat-loss prevention")
{
    GIVEN {
        PLAYER(SPECIES_GOREBYSS) {
            Speed(50);
            Ability(ABILITY_SWIFT_SWIM);
            UniqueAbility(ABILITY_SHELL_GAME);
            Moves(MOVE_SHELL_SMASH);
        }
        OPPONENT(SPECIES_BELDUM) { Ability(ABILITY_CLEAR_BODY); Speed(100); Moves(MOVE_MISTY_TERRAIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MISTY_TERRAIN); MOVE(player, MOVE_SHELL_SMASH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHELL_GAME);
        NOT ABILITY_POPUP(opponent, ABILITY_CLEAR_BODY);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Shell Game uses the foe's defensive stages for Shell Smash's failure check")
{
    GIVEN {
        PLAYER(SPECIES_GOREBYSS) {
            Speed(1);
            Ability(ABILITY_SWIFT_SWIM);
            UniqueAbility(ABILITY_SHELL_GAME);
            Moves(MOVE_SHELL_SMASH);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_MISTY_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SHELL_SMASH); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SHELL_SMASH); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SHELL_SMASH); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SHELL_SMASH); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SHELL_SMASH); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SHELL_SMASH); }
        TURN { MOVE(opponent, MOVE_MISTY_TERRAIN); MOVE(player, MOVE_SHELL_SMASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SMASH, player);
        ABILITY_POPUP(player, ABILITY_SHELL_GAME);
        NOT MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}
