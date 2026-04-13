#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BLAZE_KICK].kickingMove);
    ASSUME(gBattleMoves[MOVE_JUMP_KICK].kickingMove);
    ASSUME(gBattleMoves[MOVE_HIGH_JUMP_KICK].kickingMove);
    ASSUME(gBattleMoves[MOVE_TRIPLE_AXEL].kickingMove);
    ASSUME(gBattleMoves[MOVE_TRIPLE_KICK].kickingMove);
    ASSUME(gBattleMoves[MOVE_STOMP].kickingMove);
    ASSUME(gBattleMoves[MOVE_MEGA_KICK].kickingMove);
    ASSUME(gBattleMoves[MOVE_LOW_KICK].kickingMove);
    ASSUME(gBattleMoves[MOVE_ROLLING_KICK].kickingMove);
    ASSUME(gBattleMoves[MOVE_TROP_KICK].kickingMove);
    ASSUME(gBattleMoves[MOVE_PYRO_BALL].kickingMove);
    ASSUME(gBattleMoves[MOVE_HIGH_HORSEPOWER].kickingMove);
    ASSUME(gBattleMoves[MOVE_DOUBLE_KICK].kickingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].kickingMove);
}

SINGLE_BATTLE_TEST("Striker boosts kicking moves by 1.5x when moving before the target", s16 firstHit, s16 secondHit)
{
    u16 move;
    PARAMETRIZE { move = MOVE_STOMP; }
    PARAMETRIZE { move = MOVE_BLAZE_KICK; }

    GIVEN {
        PLAYER(SPECIES_HITMONLEE) { Speed(100); Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_STRIKER); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, move); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].firstHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
        HP_BAR(opponent, captureDamage: &results[i].secondHit);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].secondHit, Q_4_12(1.5), results[0].firstHit);
        EXPECT_MUL_EQ(results[1].secondHit, Q_4_12(1.5), results[1].firstHit);
    }
}

SINGLE_BATTLE_TEST("Striker does not boost non-kicking moves", s16 firstHit, s16 secondHit)
{
    u16 move;
    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_HEADBUTT; }

    GIVEN {
        PLAYER(SPECIES_HITMONLEE) { Speed(100); Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_STRIKER); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, move); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].firstHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
        HP_BAR(opponent, captureDamage: &results[i].secondHit);
    } FINALLY {
        EXPECT_EQ(results[0].firstHit, results[0].secondHit);
        EXPECT_EQ(results[1].firstHit, results[1].secondHit);
    }
}
