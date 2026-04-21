#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_CHANSEY].types[0] == TYPE_NORMAL);
    ASSUME(gSpeciesInfo[SPECIES_RELICANTH].types[0] != TYPE_NORMAL);
    ASSUME(gSpeciesInfo[SPECIES_RELICANTH].types[1] != TYPE_NORMAL);
}

DOUBLE_BATTLE_TEST("Vendetta boosts damage against opposing Normal-type targets over non-Normal-type targets")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_VENDETTA); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHANSEY) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentRight); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        s16 normalDamage = opponentLeft->maxHP - opponentLeft->hp;
        s16 nonNormalDamage = opponentRight->maxHP - opponentRight->hp;
        EXPECT_GT(normalDamage * 2, nonNormalDamage * 3 - 2);
        EXPECT_LT(normalDamage * 2, nonNormalDamage * 3 + 2);
    }
}

SINGLE_BATTLE_TEST("Vendetta does not boost damage against non-Normal-type targets", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_VENDETTA; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_RELICANTH) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Vendetta raises Speed by 1 after KOing a Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_SEVIPER) { Ability(ABILITY_SHED_SKIN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_VENDETTA);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Vendetta does not boost damage against allied Normal-type targets")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_VENDETTA); Moves(MOVE_EARTHQUAKE); }
        PLAYER(SPECIES_CHANSEY) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHANSEY) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_EARTHQUAKE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        s16 allyDamage = playerRight->maxHP - playerRight->hp;
        s16 foeDamage = opponentLeft->maxHP - opponentLeft->hp;
        EXPECT_GT(foeDamage * 2, allyDamage * 3 - 2);
        EXPECT_LT(foeDamage * 2, allyDamage * 3 + 2);
    }
}
