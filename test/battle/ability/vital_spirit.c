#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Vital Spirit prevents self stat drops from damaging moves")
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SUPERPOWER].effect == EFFECT_SUPERPOWER);
        ASSUME(gBattleMoves[MOVE_SUPERPOWER].power != 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_SUPERPOWER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUPERPOWER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPERPOWER, player);
        if (ability == ABILITY_VITAL_SPIRIT) {
            ABILITY_POPUP(player, ABILITY_VITAL_SPIRIT);
            MESSAGE("Wobbuffet's Vital Spirit\nprevents stat loss!");
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], ability == ABILITY_VITAL_SPIRIT ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], ability == ABILITY_VITAL_SPIRIT ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Vital Spirit prevents harsh self stat drops from damaging moves")
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_DRACO_METEOR].power != 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_DRACO_METEOR); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRACO_METEOR); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRACO_METEOR, player);
        if (ability == ABILITY_VITAL_SPIRIT) {
            ABILITY_POPUP(player, ABILITY_VITAL_SPIRIT);
            MESSAGE("Wobbuffet's Vital Spirit\nprevents stat loss!");
        } else {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("Wobbuffet's Sp. Atk harshly fell!");
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], ability == ABILITY_VITAL_SPIRIT ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Vital Spirit prevents SetMoveEffect self stat drops without corrupting the script")
{
    u32 ability, move, statId, stages;
    PARAMETRIZE { ability = ABILITY_NONE; move = MOVE_HAMMER_ARM; statId = STAT_SPEED; stages = 1; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; move = MOVE_HAMMER_ARM; statId = STAT_SPEED; stages = 1; }
    PARAMETRIZE { ability = ABILITY_NONE; move = MOVE_SPIN_OUT; statId = STAT_SPEED; stages = 2; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; move = MOVE_SPIN_OUT; statId = STAT_SPEED; stages = 2; }
    PARAMETRIZE { ability = ABILITY_NONE; move = MOVE_MAKE_IT_RAIN; statId = STAT_SPATK; stages = 1; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; move = MOVE_MAKE_IT_RAIN; statId = STAT_SPATK; stages = 1; }

    GIVEN {
        ASSUME(gBattleMoves[move].power != 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(move, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        if (ability == ABILITY_VITAL_SPIRIT) {
            ABILITY_POPUP(player, ABILITY_VITAL_SPIRIT);
            MESSAGE("Wobbuffet's Vital Spirit\nprevents stat loss!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(player->statStages[statId], ability == ABILITY_VITAL_SPIRIT ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - stages);
    }
}

SINGLE_BATTLE_TEST("Vital Spirit prevents V-create self stat drops before the stat animation")
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_V_CREATE].effect == EFFECT_V_CREATE);
        ASSUME(gBattleMoves[MOVE_V_CREATE].power != 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_V_CREATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_V_CREATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_V_CREATE, player);
        if (ability == ABILITY_VITAL_SPIRIT) {
            ABILITY_POPUP(player, ABILITY_VITAL_SPIRIT);
            MESSAGE("Wobbuffet's Vital Spirit\nprevents stat loss!");
            NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        } else {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("Wobbuffet's Defense fell!");
            MESSAGE("Wobbuffet's Sp. Def fell!");
            MESSAGE("Wobbuffet's Speed fell!");
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], ability == ABILITY_VITAL_SPIRIT ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], ability == ABILITY_VITAL_SPIRIT ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], ability == ABILITY_VITAL_SPIRIT ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Vital Spirit does not prevent opposing stat drops")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_GROWL].effect == EFFECT_ATTACK_DOWN);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_VITAL_SPIRIT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}
