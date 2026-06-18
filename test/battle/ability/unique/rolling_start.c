#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DEFENSE_CURL].effect == EFFECT_DEFENSE_CURL);
    ASSUME(gBattleMoves[MOVE_ROLLOUT].effect == EFFECT_ROLLOUT);
    ASSUME(gBattleMoves[MOVE_ICE_BALL].effect == EFFECT_ROLLOUT);
    ASSUME(gBattleMoves[MOVE_ROLLOUT].accuracy == 90);
    ASSUME(gBattleMoves[MOVE_ICE_BALL].accuracy == 90);
}

SINGLE_BATTLE_TEST("Rolling Start uses Defense Curl on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_ROLLING_START); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_ROLLING_START);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEFENSE_CURL, opponent);
        MESSAGE("Foe Wobbuffet's Defense rose!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT(opponent->status2 & STATUS2_DEFENSE_CURL);
    }
}

SINGLE_BATTLE_TEST("Rolling Start makes Rollout and Ice Ball ignore accuracy")
{
    u32 move;

    PARAMETRIZE { move = MOVE_ROLLOUT; }
    PARAMETRIZE { move = MOVE_ICE_BALL; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_ROLLING_START); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
    }
}
