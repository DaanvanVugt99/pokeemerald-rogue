#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(!gBattleMoves[MOVE_PSYSHOCK].makesContact);
    ASSUME(gBattleMoves[MOVE_PSYSHOCK].effect == EFFECT_PSYSHOCK);
    ASSUME(gBattleMoves[MOVE_IRON_DEFENSE].effect == EFFECT_DEFENSE_UP_2);
}

SINGLE_BATTLE_TEST("High Climber contact moves ignore ungrounded targets' defensive boosts", s16 damageBefore, s16 damageAfter)
{
    u16 move;
    u16 targetAbility;
    bool32 shouldIgnoreBoost;

    PARAMETRIZE { move = MOVE_TACKLE; targetAbility = ABILITY_LEVITATE; shouldIgnoreBoost = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; targetAbility = ABILITY_SHADOW_TAG; shouldIgnoreBoost = FALSE; }
    PARAMETRIZE { move = MOVE_PSYSHOCK; targetAbility = ABILITY_LEVITATE; shouldIgnoreBoost = FALSE; }

    GIVEN {
        PLAYER(SPECIES_SNEASLER) { Speed(100); Ability(ABILITY_PRESSURE); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(targetAbility); Moves(MOVE_IRON_DEFENSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damageBefore);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, opponent);
        HP_BAR(opponent, captureDamage: &results[i].damageAfter);
    } FINALLY {
        if (shouldIgnoreBoost)
            EXPECT_EQ(results[i].damageBefore, results[i].damageAfter);
        else
            EXPECT_LT(results[i].damageAfter, results[i].damageBefore);
    }
}
