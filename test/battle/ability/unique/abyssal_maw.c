#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(gBattleMoves[MOVE_BITE].power > 0);
    ASSUME(gBattleMoves[MOVE_BITE].strikeCount < 2);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
    ASSUME(gBattleMoves[MOVE_TACKLE].strikeCount < 2);
}

SINGLE_BATTLE_TEST("Abyssal Maw makes biting moves hit twice and does not affect non-biting moves")
{
    u16 move;
    bool32 shouldHitTwice;

    PARAMETRIZE { move = MOVE_BITE; shouldHitTwice = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; shouldHitTwice = FALSE; }

    GIVEN {
        PLAYER(SPECIES_HUNTAIL) { Ability(ABILITY_SWIFT_SWIM); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        if (shouldHitTwice)
            HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Abyssal Maw second hit uses 0.4x damage on biting moves", s16 hit1, s16 hit2)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ABYSSAL_MAW; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent, captureDamage: &results[i].hit1);
        if (uniqueAbility == ABILITY_ABYSSAL_MAW)
            HP_BAR(opponent, captureDamage: &results[i].hit2);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].hit1, Q_4_12(1.4), results[1].hit1 + results[1].hit2);
    }
}
