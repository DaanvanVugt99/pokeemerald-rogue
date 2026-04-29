#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_SANDSTORM].effect == EFFECT_SANDSTORM);
    ASSUME(gBattleMoves[MOVE_DOUBLE_KICK].strikeCount == 2);
}

SINGLE_BATTLE_TEST("Desert Shroud halves incoming damage on the first turn out", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_OPEN_FIELD; }
    PARAMETRIZE { uniqueAbility = ABILITY_DESERT_SHROUD; }

    GIVEN {
        PLAYER(SPECIES_FLYGON) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Desert Shroud only applies on the first turn out")
{
    s16 turnOneDamage;
    s16 turnTwoDamage;

    GIVEN {
        PLAYER(SPECIES_FLYGON) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_DESERT_SHROUD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &turnOneDamage);
        HP_BAR(player, captureDamage: &turnTwoDamage);
    } THEN {
        EXPECT_MUL_EQ(turnOneDamage, UQ_4_12(2.0), turnTwoDamage);
    }
}

SINGLE_BATTLE_TEST("Desert Shroud shows a popup when it reduces damage")
{
    GIVEN {
        PLAYER(SPECIES_FLYGON) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_DESERT_SHROUD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DESERT_SHROUD);
        MESSAGE("Flygon's Desert Shroud softened the attack!");
    }
}

SINGLE_BATTLE_TEST("Desert Shroud only halves the first hit it takes")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_FLYGON) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_DESERT_SHROUD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DOUBLE_KICK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DOUBLE_KICK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_KICK, opponent);
        HP_BAR(player, captureDamage: &firstHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_KICK, opponent);
        HP_BAR(player, captureDamage: &secondHit);
    } THEN {
        EXPECT_MUL_EQ(firstHit, UQ_4_12(2.0), secondHit);
    }
}

DOUBLE_BATTLE_TEST("Desert Shroud makes damaging moves miss in sandstorm on the first turn out")
{
    GIVEN {
        PLAYER(SPECIES_FLYGON) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_DESERT_SHROUD); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SAND_STREAM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_WATER_GUN, target: playerLeft);
        }
    } SCENE {
        ABILITY_POPUP(opponentLeft, ABILITY_SAND_STREAM);
        ABILITY_POPUP(playerLeft, ABILITY_DESERT_SHROUD);
        NONE_OF {
            HP_BAR(playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
    }
}
