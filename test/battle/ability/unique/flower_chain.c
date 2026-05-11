#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ABSORB].effect == EFFECT_ABSORB);
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
    ASSUME(gBattleMoves[MOVE_HEAL_PULSE].effect == EFFECT_HEAL_PULSE);
}

SINGLE_BATTLE_TEST("Flower Chain raises Sp. Def after the user heals itself with a move")
{
    GIVEN {
        PLAYER(SPECIES_COMFEY) { HP(50); MaxHP(100); Ability(ABILITY_TRIAGE); UniqueAbility(ABILITY_FLOWER_CHAIN); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        ABILITY_POPUP(player, ABILITY_FLOWER_CHAIN);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Flower Chain raises an ally's Sp. Def after healing that ally with a move")
{
    GIVEN {
        PLAYER(SPECIES_COMFEY)      { Ability(ABILITY_TRIAGE); UniqueAbility(ABILITY_FLOWER_CHAIN); Moves(MOVE_HEAL_PULSE); }
        PLAYER(SPECIES_WOBBUFFET)   { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HEAL_PULSE, target: playerRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_PULSE, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_FLOWER_CHAIN);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Flower Chain does not raise Sp. Def when healing an opponent")
{
    GIVEN {
        PLAYER(SPECIES_COMFEY) { Ability(ABILITY_TRIAGE); UniqueAbility(ABILITY_FLOWER_CHAIN); Moves(MOVE_HEAL_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HEAL_PULSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_PULSE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FLOWER_CHAIN);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Flower Chain does not raise Sp. Def if a draining move does not restore HP")
{
    GIVEN {
        PLAYER(SPECIES_COMFEY) { HP(100); MaxHP(100); Ability(ABILITY_TRIAGE); UniqueAbility(ABILITY_FLOWER_CHAIN); Moves(MOVE_ABSORB); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FLOWER_CHAIN);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
