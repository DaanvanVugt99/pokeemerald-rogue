#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SWIRLING_BLADE].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_SWIRLING_BLADE].type == TYPE_STEEL);
    ASSUME(gBattleMoves[MOVE_SWIRLING_BLADE].power == 70);
    ASSUME(gBattleMoves[MOVE_SWIRLING_BLADE].accuracy == 100);
    ASSUME(gBattleMoves[MOVE_SWIRLING_BLADE].pp == 15);
    ASSUME(gBattleMoves[MOVE_SWIRLING_BLADE].split == SPLIT_PHYSICAL);
}

SINGLE_BATTLE_TEST("Swirling Blade receives Sharpness's slicing boost", s16 damage)
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_SHARPNESS; }
    PARAMETRIZE { ability = ABILITY_RUN_AWAY; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWIRLING_BLADE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIRLING_BLADE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Swirling Blade has an increased critical-hit chance")
{
    PASSES_RANDOMLY(1, 8, RNG_CRITICAL_HIT);
    GIVEN {
        ASSUME(B_CRIT_CHANCE >= GEN_6);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWIRLING_BLADE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIRLING_BLADE, player);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Swirling Blade makes contact with Rough Skin")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_ROUGH_SKIN); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIRLING_BLADE); }
    } SCENE {
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_ROUGH_SKIN);
        HP_BAR(player);
    }
}
