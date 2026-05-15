#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_POWDER_SNOW].power > 20);
    ASSUME(gBattleMoves[MOVE_POWDER_SNOW].target == MOVE_TARGET_BOTH);
}

SINGLE_BATTLE_TEST("Snowstorm uses Powder Snow at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_FROSMOTH) { Speed(100); SpAttack(100); Ability(ABILITY_ICE_SCALES); UniqueAbility(ABILITY_SNOWSTORM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SNOWSTORM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWDER_SNOW, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Snowstorm's called Powder Snow uses 20 base power", s16 damage)
{
    u16 move;
    u16 uniqueAbility;

    PARAMETRIZE { move = MOVE_POWDER_SNOW; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_CELEBRATE; uniqueAbility = ABILITY_SNOWSTORM; }

    GIVEN {
        PLAYER(SPECIES_FROSMOTH) { Speed(100); SpAttack(100); Ability(ABILITY_ICE_SCALES); UniqueAbility(uniqueAbility); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
        EXPECT_GT(results[1].damage, 0);
    }
}

DOUBLE_BATTLE_TEST("Snowstorm can hit both opposing Pokemon at end of turn")
{
    s16 leftDamage;
    s16 rightDamage;

    GIVEN {
        PLAYER(SPECIES_SNOM) { Speed(100); SpAttack(100); Ability(ABILITY_SHIELD_DUST); UniqueAbility(ABILITY_SNOWSTORM); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); SpDefense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_SNOWSTORM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWDER_SNOW, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &leftDamage);
        HP_BAR(opponentRight, captureDamage: &rightDamage);
    } THEN {
        EXPECT_GT(leftDamage, 0);
        EXPECT_EQ(leftDamage, rightDamage);
    }
}
