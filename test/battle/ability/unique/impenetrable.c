#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_SPECIAL(MOVE_WATER_GUN));
    ASSUME(IS_MOVE_PHYSICAL(MOVE_BRICK_BREAK));
}

SINGLE_BATTLE_TEST("Impenetrable blocks super effective special moves")
{
    u16 move;
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_EARTH_POWER; }

    GIVEN {
        PLAYER(SPECIES_BASTIODON) { Ability(ABILITY_SOLID_ROCK); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_IMPENETRABLE);
        NONE_OF { HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(CalcTypeEffectivenessMultiplierForUI(move, gBattleMoves[move].type,
            B_POSITION_OPPONENT_LEFT, B_POSITION_PLAYER_LEFT, ABILITY_SOLID_ROCK, FALSE), UQ_4_12(0.0));
        EXPECT_EQ(CalcPartyMonTypeEffectivenessMultiplier(move, SPECIES_BASTIODON, 0,
            ABILITY_SOLID_ROCK, ABILITY_IMPENETRABLE), UQ_4_12(0.0));
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
        MESSAGE("It's extremely effective!");
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Impenetrable leaves neutral and resisted special moves unchanged", s16 damage)
{
    u16 uniqueAbility, move;
    PARAMETRIZE { uniqueAbility = ABILITY_KEEN_EYE; move = MOVE_FLAMETHROWER; }
    PARAMETRIZE { uniqueAbility = ABILITY_IMPENETRABLE; move = MOVE_FLAMETHROWER; }
    PARAMETRIZE { uniqueAbility = ABILITY_KEEN_EYE; move = MOVE_PSYCHIC; }
    PARAMETRIZE { uniqueAbility = ABILITY_IMPENETRABLE; move = MOVE_PSYCHIC; }

    GIVEN {
        PLAYER(SPECIES_BASTIODON) { Ability(ABILITY_STURDY); UniqueAbility(uniqueAbility); MaxHP(500); HP(500); SpDefense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        NONE_OF { ABILITY_POPUP(player, ABILITY_IMPENETRABLE); }
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Impenetrable can be bypassed by Mold Breaker")
{
    GIVEN {
        PLAYER(SPECIES_BASTIODON) { Ability(ABILITY_SOLID_ROCK); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MOLD_BREAKER); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        NONE_OF { ABILITY_POPUP(player, ABILITY_IMPENETRABLE); }
        HP_BAR(player);
        MESSAGE("It's super effective!");
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}
