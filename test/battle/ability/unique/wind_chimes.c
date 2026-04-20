#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BUG_BUZZ].soundMove);
    ASSUME(!IS_MOVE_STATUS(MOVE_BUG_BUZZ));
    ASSUME(gBattleMoves[MOVE_BUG_BUZZ].target == MOVE_TARGET_SELECTED);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(!IS_MOVE_STATUS(MOVE_HYPER_VOICE));
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].power > 30);
}

DOUBLE_BATTLE_TEST("Wind Chimes makes sound moves hit both foes")
{
    GIVEN {
        PLAYER(SPECIES_CHIMECHO) { Ability(ABILITY_LEVITATE); Moves(MOVE_BUG_BUZZ); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BUG_BUZZ, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
    }
}

SINGLE_BATTLE_TEST("Wind Chimes boosts sound move damage by 1.3x", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_WIND_CHIMES; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Wind Chimes uses 30 BP Hyper Voice after being hit")
{
    s16 damage;

    GIVEN {
        PLAYER(SPECIES_CHIMECHO) { Ability(ABILITY_LEVITATE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_WIND_CHIMES);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_GT(damage, 0);
        EXPECT_LT(damage, gBattleMoves[MOVE_HYPER_VOICE].power);
    }
}

DOUBLE_BATTLE_TEST("Wind Chimes still retaliates if the original attacker faints")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_WIND_CHIMES); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Item(ITEM_LIFE_ORB); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 0);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
    }
}
