#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_COSMIC_POWER].effect == EFFECT_COSMIC_POWER);
}

SINGLE_BATTLE_TEST("Ancient Idol uses Defense for physical attacks, so Cosmic Power boosts physical damage", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ANCIENT_IDOL; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Defense(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE, MOVE_COSMIC_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_COSMIC_POWER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COSMIC_POWER, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Ancient Idol uses Sp. Def for special attacks, so Cosmic Power boosts special damage", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ANCIENT_IDOL; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); SpDefense(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_WATER_GUN, MOVE_COSMIC_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_COSMIC_POWER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COSMIC_POWER, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Ancient Idol physical attacks are not reduced by burn", s16 damage)
{
    u16 uniqueAbility;
    bool32 burned;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; burned = FALSE; }
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; burned = TRUE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ANCIENT_IDOL; burned = FALSE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ANCIENT_IDOL; burned = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            Attack(100); Defense(160); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Status1(burned ? STATUS1_BURN : STATUS1_NONE); Moves(MOVE_TACKLE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}
