#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Liquid Ooze boosts draining move damage by 1.3x", s16 damage)
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_LIQUID_OOZE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_ABSORB].effect == EFFECT_ABSORB);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_ABSORB); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage);
    }
}
