#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_FORESTS_CURSE].effect == EFFECT_THIRD_TYPE);
    ASSUME(gBattleMoves[MOVE_FORESTS_CURSE].argument == TYPE_GRASS);
}

SINGLE_BATTLE_TEST("Type effectiveness messages print the extremely effective message for 4x hits")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_GEODUDE) { HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        MESSAGE("It's extremely effective!");
    }
}

SINGLE_BATTLE_TEST("Type effectiveness messages keep the super effective message for 2x hits")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_CHARMANDER) { HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Type effectiveness messages print the extremely effective message for 8x hits")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FORESTS_CURSE, MOVE_EMBER); }
        OPPONENT(SPECIES_SCIZOR) { HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_FORESTS_CURSE); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        MESSAGE("It's extremely effective!");
    }
}

SINGLE_BATTLE_TEST("Type effectiveness messages keep the not very effective message for 0.5x hits")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_SQUIRTLE) { HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        MESSAGE("It's not very effective…");
    }
}

SINGLE_BATTLE_TEST("Type effectiveness messages print the mostly ineffective message for 0.25x hits")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_OMANYTE) { HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        MESSAGE("It's mostly ineffective...");
    }
}

SINGLE_BATTLE_TEST("Type effectiveness messages keep the no effect message for 0x hits")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GASTLY) { HP(1000); MaxHP(1000); Defense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("It doesn't affect Foe Gastly…");
    }
}

SINGLE_BATTLE_TEST("Type effectiveness messages print the mostly ineffective message for 0.125x hits")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FORESTS_CURSE, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_KINGDRA) { HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_FORESTS_CURSE); }
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        MESSAGE("It's mostly ineffective...");
    }
}
