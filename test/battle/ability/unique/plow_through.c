#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BULLDOZE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_ICY_WIND].type == TYPE_ICE);
    ASSUME(gBattleMoves[MOVE_ICY_WIND].power > 40);
}

SINGLE_BATTLE_TEST("Plow Through adds a 20 BP Icy Wind after Ground-type moves")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_MAMOSWINE) { Ability(ABILITY_OBLIVIOUS); UniqueAbility(ABILITY_PLOW_THROUGH); Moves(MOVE_BULLDOZE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BULLDOZE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLDOZE, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ABILITY_POPUP(player, ABILITY_PLOW_THROUGH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICY_WIND, player);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_GT(firstHit, 0);
        EXPECT_GT(secondHit, 0);
        EXPECT_LT(secondHit, firstHit);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Plow Through makes the extra Icy Wind 40 BP in Snow", s16 extraDamage)
{
    u16 opponentMove;
    PARAMETRIZE { opponentMove = MOVE_CELEBRATE; }
    PARAMETRIZE { opponentMove = MOVE_SNOWSCAPE; }

    GIVEN {
        PLAYER(SPECIES_MAMOSWINE) { Speed(1); Ability(ABILITY_OBLIVIOUS); UniqueAbility(ABILITY_PLOW_THROUGH); Moves(MOVE_BULLDOZE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(opponentMove); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULLDOZE); MOVE(opponent, opponentMove); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent, captureDamage: &results[i].extraDamage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].extraDamage, UQ_4_12(2.0), results[1].extraDamage);
    }
}
