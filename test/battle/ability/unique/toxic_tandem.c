#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_POISON_STING].type == TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_POISON_STING].power > 0);
    ASSUME(gBattleMoves[MOVE_POISON_STING].strikeCount < 2);
    ASSUME(gBattleMoves[MOVE_JUDGMENT].type != TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_JUDGMENT].power > 0);
    ASSUME(gBattleMoves[MOVE_JUDGMENT].strikeCount < 2);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_TACKLE].strikeCount < 2);
}

SINGLE_BATTLE_TEST("Toxic Tandem makes Poison-type moves hit twice and does not affect non-Poison moves")
{
    u16 move;
    bool32 shouldHitTwice;

    PARAMETRIZE { move = MOVE_POISON_STING; shouldHitTwice = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; shouldHitTwice = FALSE; }

    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Ability(ABILITY_SWARM); Moves(move); }
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

SINGLE_BATTLE_TEST("Toxic Tandem second hit uses 0.4x damage on Poison-type moves", s16 hit1, s16 hit2)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_TOXIC_TANDEM; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_POISON_STING); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_STING); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POISON_STING, player);
        HP_BAR(opponent, captureDamage: &results[i].hit1);
        if (uniqueAbility == ABILITY_TOXIC_TANDEM)
            HP_BAR(opponent, captureDamage: &results[i].hit2);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].hit1, Q_4_12(1.4), results[1].hit1 + results[1].hit2);
    }
}

SINGLE_BATTLE_TEST("Toxic Tandem uses the battle-resolved Poison move type")
{
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Ability(ABILITY_SWARM); Item(ITEM_TOXIC_PLATE); Moves(MOVE_JUDGMENT); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_JUDGMENT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_JUDGMENT, player);
        HP_BAR(opponent);
        HP_BAR(opponent);
    }
}
