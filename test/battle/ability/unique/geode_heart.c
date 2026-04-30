#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SMACK_DOWN].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_SMACK_DOWN].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_SURF].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_SURF].target == MOVE_TARGET_FOES_AND_ALLY);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
}

SINGLE_BATTLE_TEST("Geode Heart summons sandstorm on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ROGGENROLA) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_GEODE_HEART); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GEODE_HEART);
        MESSAGE("Roggenrola's Geode Heart whipped up a sandstorm!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SANDSTORM_CONTINUES);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SANDSTORM);
    }
}

SINGLE_BATTLE_TEST("Geode Heart makes the next Rock attack target Sp. Def after a special hit", s16 damage)
{
    bool32 specialHit;

    PARAMETRIZE { specialHit = FALSE; }
    PARAMETRIZE { specialHit = TRUE; }

    GIVEN {
        PLAYER(SPECIES_GIGALITH) {
            HP(1000); MaxHP(1000); Attack(100); Speed(1);
            Ability(ABILITY_STURDY); UniqueAbility(ABILITY_GEODE_HEART);
            Moves(MOVE_SMACK_DOWN);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            HP(1000); MaxHP(1000); Defense(200); SpDefense(50); Speed(100);
            Ability(ABILITY_SHADOW_TAG);
            Moves(MOVE_CELEBRATE, MOVE_WATER_GUN);
        }
    } WHEN {
        TURN { MOVE(opponent, specialHit ? MOVE_WATER_GUN : MOVE_CELEBRATE); MOVE(player, MOVE_SMACK_DOWN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GEODE_HEART);
        if (specialHit)
        {
            ABILITY_POPUP(player, ABILITY_GEODE_HEART);
            MESSAGE("Gigalith's Geode Heart stored special energy!");
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (specialHit)
        {
            ABILITY_POPUP(player, ABILITY_GEODE_HEART);
            MESSAGE("Gigalith's Geode Heart released stored energy!");
        }
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

DOUBLE_BATTLE_TEST("Geode Heart charges each target damaged by a spread special move")
{
    GIVEN {
        PLAYER(SPECIES_ROGGENROLA) {
            HP(1000); MaxHP(1000); Speed(1);
            Ability(ABILITY_STURDY); UniqueAbility(ABILITY_GEODE_HEART);
            Moves(MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_BOLDORE) {
            HP(1000); MaxHP(1000); Speed(1);
            Ability(ABILITY_STURDY); UniqueAbility(ABILITY_GEODE_HEART);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            SpAttack(100); Speed(100);
            Ability(ABILITY_SHADOW_TAG);
            Moves(MOVE_SURF);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Speed(1);
            Ability(ABILITY_SHADOW_TAG);
            Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SURF);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Geode Heart consumes its special-damage charge on the next Rock attack")
{
    s16 damage[2];

    GIVEN {
        PLAYER(SPECIES_GIGALITH) {
            HP(1000); MaxHP(1000); Attack(100); Speed(1);
            Ability(ABILITY_STURDY); UniqueAbility(ABILITY_GEODE_HEART);
            Moves(MOVE_SMACK_DOWN);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            HP(1000); MaxHP(1000); Defense(200); SpDefense(50); Speed(100);
            Ability(ABILITY_SHADOW_TAG);
            Moves(MOVE_CELEBRATE, MOVE_WATER_GUN);
        }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_SMACK_DOWN); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SMACK_DOWN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GEODE_HEART);
        ABILITY_POPUP(player, ABILITY_GEODE_HEART);
        MESSAGE("Gigalith's Geode Heart stored special energy!");
        HP_BAR(opponent, captureDamage: &damage[0]);
        ABILITY_POPUP(player, ABILITY_GEODE_HEART);
        MESSAGE("Gigalith's Geode Heart released stored energy!");
        HP_BAR(opponent, captureDamage: &damage[1]);
    } THEN {
        EXPECT_GT(damage[0], damage[1]);
    }
}
