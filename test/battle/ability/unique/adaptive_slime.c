#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_GUST].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_NASTY_PLOT].effect == EFFECT_SPECIAL_ATTACK_UP_2);
}

SINGLE_BATTLE_TEST("Adaptive Slime raises Defense and lowers Sp. Def after a physical hit")
{
    GIVEN {
        PLAYER(SPECIES_GASTRODON) { Ability(ABILITY_STORM_DRAIN); UniqueAbility(ABILITY_ADAPTIVE_SLIME); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_SLIME);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Adaptive Slime raises Sp. Def and lowers Defense after a special hit")
{
    GIVEN {
        PLAYER(SPECIES_GASTRODON) { Ability(ABILITY_STORM_DRAIN); UniqueAbility(ABILITY_ADAPTIVE_SLIME); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GUST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_SLIME);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Adaptive Slime works after Storm Drain absorbs a special Water move while Sp. Atk is capped")
{
    GIVEN {
        PLAYER(SPECIES_GASTRODON) { Ability(ABILITY_STORM_DRAIN); UniqueAbility(ABILITY_ADAPTIVE_SLIME); Speed(100); Moves(MOVE_NASTY_PLOT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_NASTY_PLOT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_NASTY_PLOT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_NASTY_PLOT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STORM_DRAIN);
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_SLIME);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Adaptive Slime works after Storm Drain absorbs a special Water move")
{
    GIVEN {
        PLAYER(SPECIES_GASTRODON) { Ability(ABILITY_STORM_DRAIN); UniqueAbility(ABILITY_ADAPTIVE_SLIME); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STORM_DRAIN);
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_SLIME);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}
