#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].split == SPLIT_SPECIAL);
}

DOUBLE_BATTLE_TEST("Moonlight boosts allies' Special Defense by 1.25x", s16 damageL, s16 damageR)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_MOONLIGHT; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(uniqueAbility); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_HYPER_VOICE, target: playerLeft); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, opponentLeft);
        HP_BAR(playerLeft, captureDamage: &results[i].damageL);
        HP_BAR(playerRight, captureDamage: &results[i].damageR);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damageL, UQ_4_12(1.25), results[0].damageL);
        EXPECT_EQ(results[0].damageR, results[1].damageR);
    }
}
