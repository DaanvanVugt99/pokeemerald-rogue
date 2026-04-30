#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].strikeCount < 2);
}

SINGLE_BATTLE_TEST("Brutal makes damaging moves hit twice")
{
    GIVEN {
        PLAYER(SPECIES_HAXORUS) { Ability(ABILITY_MOLD_BREAKER); UniqueAbility(ABILITY_BRUTAL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Brutal second hit uses 0.25x damage", s16 hit1, s16 hit2)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_BRUTAL; }

    GIVEN {
        PLAYER(SPECIES_HAXORUS) { Ability(ABILITY_MOLD_BREAKER); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].hit1);
        if (uniqueAbility == ABILITY_BRUTAL)
            HP_BAR(opponent, captureDamage: &results[i].hit2);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].hit1, Q_4_12(1.25), results[1].hit1 + results[1].hit2);
    }
}
