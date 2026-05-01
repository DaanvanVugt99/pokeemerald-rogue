#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RAIN_DANCE].effect == EFFECT_RAIN_DANCE);
    ASSUME(gBattleMoves[MOVE_ELECTRIC_TERRAIN].effect == EFFECT_ELECTRIC_TERRAIN);
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gBattleMoves[MOVE_PSYCHIC_TERRAIN].effect == EFFECT_PSYCHIC_TERRAIN);
    ASSUME(gBattleMoves[MOVE_SANDSTORM].effect == EFFECT_SANDSTORM);
    ASSUME(gBattleMoves[MOVE_SPORE].effect == EFFECT_SLEEP);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].effect == EFFECT_EARTHQUAKE);
}

SINGLE_BATTLE_TEST("Clueless is unaffected by rain damage modifiers", s16 damage)
{
    bool32 rain;
    u16 uniqueAbility;

    PARAMETRIZE { rain = FALSE; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { rain = TRUE;  uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { rain = TRUE;  uniqueAbility = ABILITY_CLUELESS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); SpAttack(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpDefense(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_RAIN_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, rain ? MOVE_RAIN_DANCE : MOVE_CELEBRATE); MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Clueless ignores Electric Terrain sleep prevention")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_ELECTRIC_TERRAIN, MOVE_SPORE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_CLUELESS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRIC_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SPORE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
        STATUS_ICON(opponent, sleep: TRUE);
    } THEN {
        EXPECT_NE(opponent->status1 & STATUS1_SLEEP, 0);
    }
}

SINGLE_BATTLE_TEST("Clueless ignores Psychic Terrain priority protection")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_PSYCHIC_TERRAIN, MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_CLUELESS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC_TERRAIN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Clueless is unaffected by Grassy Terrain reducing Earthquake damage", s16 damage)
{
    bool32 terrain;
    u16 uniqueAbility;

    PARAMETRIZE { terrain = FALSE; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { terrain = TRUE;  uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { terrain = TRUE;  uniqueAbility = ABILITY_CLUELESS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Attack(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Defense(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_GRASSY_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, terrain ? MOVE_GRASSY_TERRAIN : MOVE_CELEBRATE); MOVE(player, MOVE_EARTHQUAKE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Clueless is unaffected by residual sandstorm damage")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Speed(100); Moves(MOVE_SANDSTORM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_CLUELESS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SANDSTORM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SANDSTORM, player);
        NONE_OF {
            MESSAGE("Foe Wobbuffet is buffeted by the sandstorm!");
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
