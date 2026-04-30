#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
}

SINGLE_BATTLE_TEST("Carrion Hour reduces special damage to 0.9x", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_CARRION_HOUR; }
    PARAMETRIZE { uniqueAbility = ABILITY_BREAK_FORM; }

    GIVEN {
        PLAYER(SPECIES_VULLABY) { Ability(ABILITY_OVERCOAT); UniqueAbility(uniqueAbility); SpDefense(100); HP(300); MaxHP(300); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(0.9), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Carrion Hour does not reduce physical damage", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_CARRION_HOUR; }
    PARAMETRIZE { uniqueAbility = ABILITY_BREAK_FORM; }

    GIVEN {
        PLAYER(SPECIES_VULLABY) { Ability(ABILITY_OVERCOAT); UniqueAbility(uniqueAbility); Defense(100); HP(300); MaxHP(300); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Carrion Hour heals 1/8 max HP at end of turn during Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_VULLABY) { Ability(ABILITY_OVERCOAT); UniqueAbility(ABILITY_CARRION_HOUR); HP(120); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_ECLIPSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ECLIPSE); }
    } THEN {
        EXPECT_EQ(player->hp, 140);
    }
}
