#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_GUST].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Inflatable raises Defense and Sp. Def when hit by Fire- or Flying-type moves")
{
    u16 move;

    PARAMETRIZE { move = MOVE_EMBER; }
    PARAMETRIZE { move = MOVE_GUST; }

    GIVEN {
        PLAYER(SPECIES_DRIFBLIM) { Ability(ABILITY_AFTERMATH); UniqueAbility(ABILITY_INFLATABLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_INFLATABLE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Inflatable does not activate when hit by other move types")
{
    GIVEN {
        PLAYER(SPECIES_DRIFBLIM) { Ability(ABILITY_AFTERMATH); UniqueAbility(ABILITY_INFLATABLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_INFLATABLE);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Inflatable does not activate if the Fire- or Flying-type move has no effect")
{
    GIVEN {
        PLAYER(SPECIES_DRIFBLIM) { Ability(ABILITY_AFTERMATH); UniqueAbility(ABILITY_INFLATABLE); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_EMBER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_INFLATABLE);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
