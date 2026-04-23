#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHIMMER].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_SHIMMER].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_SHIMMER].split == SPLIT_SPECIAL);
    ASSUME(gSpeciesInfo[SPECIES_CLEFAIRY].types[0] == TYPE_FAIRY || gSpeciesInfo[SPECIES_CLEFAIRY].types[1] == TYPE_FAIRY);
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].types[0] == TYPE_NORMAL && gSpeciesInfo[SPECIES_SNORLAX].types[1] == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Shimmer is super effective against Fairy-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SHIMMER); }
        OPPONENT(SPECIES_CLEFAIRY);
    } WHEN {
        TURN { MOVE(player, MOVE_SHIMMER); }
    } SCENE {
        MESSAGE("Wobbuffet used Shimmer!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHIMMER, player);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Shimmer behaves normally against non-Fairy Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SHIMMER); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(player, MOVE_SHIMMER); }
    } SCENE {
        MESSAGE("Wobbuffet used Shimmer!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHIMMER, player);
        NOT MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Shimmer deals more damage to Fairy-type targets than neutral targets", s16 damage)
{
    u16 species;
    PARAMETRIZE { species = SPECIES_CLEFAIRY; }
    PARAMETRIZE { species = SPECIES_SNORLAX; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SHIMMER); SpAttack(100); }
        OPPONENT(species) { HP(200); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHIMMER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}
