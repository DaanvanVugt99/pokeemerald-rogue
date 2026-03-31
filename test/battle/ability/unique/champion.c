#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].strikeCount < 2);
    ASSUME(gBattleMoves[MOVE_TACKLE].strikeCount < 2);
}

SINGLE_BATTLE_TEST("Champion makes punch moves hit twice and does not affect non-punch moves")
{
    u16 move;
    bool32 shouldHitTwice;
    PARAMETRIZE { move = MOVE_MEGA_PUNCH; shouldHitTwice = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; shouldHitTwice = FALSE; }

    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Ability(ABILITY_NO_GUARD); Moves(move); }
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

SINGLE_BATTLE_TEST("Champion second hit uses 0.25x damage on punch moves", s16 hit1, s16 hit2)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_CHAMPION; }

    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Ability(ABILITY_NO_GUARD); UniqueAbility(uniqueAbility); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        HP_BAR(opponent, captureDamage: &results[i].hit1);
        if (uniqueAbility == ABILITY_CHAMPION)
            HP_BAR(opponent, captureDamage: &results[i].hit2);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].hit1, Q_4_12(1.25), results[1].hit1 + results[1].hit2);
    }
}
