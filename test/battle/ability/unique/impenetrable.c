#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_SPECIAL(MOVE_WATER_GUN));
    ASSUME(IS_MOVE_PHYSICAL(MOVE_BRICK_BREAK));
}

SINGLE_BATTLE_TEST("Impenetrable makes super effective special moves deal neutral damage", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_KEEN_EYE; }
    PARAMETRIZE { uniqueAbility = ABILITY_IMPENETRABLE; }

    GIVEN {
        PLAYER(SPECIES_BASTIODON) { Ability(ABILITY_STURDY); UniqueAbility(uniqueAbility); MaxHP(500); HP(500); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
        if (uniqueAbility == ABILITY_IMPENETRABLE)
        {
            NONE_OF { MESSAGE("It's super effective!"); }
        }
        else
        {
            MESSAGE("It's super effective!");
        }
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Impenetrable does not weaken super effective physical moves", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_KEEN_EYE; }
    PARAMETRIZE { uniqueAbility = ABILITY_IMPENETRABLE; }

    GIVEN {
        PLAYER(SPECIES_BASTIODON) { Ability(ABILITY_STURDY); UniqueAbility(uniqueAbility); MaxHP(500); HP(500); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_BRICK_BREAK); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BRICK_BREAK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
        MESSAGE("It's super effective!");
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Impenetrable shows a popup when it softens a special super effective move")
{
    GIVEN {
        PLAYER(SPECIES_BASTIODON) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_IMPENETRABLE); MaxHP(500); HP(500); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_IMPENETRABLE);
        MESSAGE("Bastiodon's Impenetrable softened the attack!");
    }
}
