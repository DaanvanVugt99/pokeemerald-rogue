#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BUG_BITE].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_TERRAIN_PULSE].effect == EFFECT_TERRAIN_PULSE);
    ASSUME(gBattleMoves[MOVE_INFESTED_TERRAIN].effect == EFFECT_INFESTED_TERRAIN);
    ASSUME(gSpeciesInfo[SPECIES_GEODUDE].types[0] == TYPE_ROCK || gSpeciesInfo[SPECIES_GEODUDE].types[1] == TYPE_ROCK);
    ASSUME(gSpeciesInfo[SPECIES_GEODUDE].types[0] == TYPE_GROUND || gSpeciesInfo[SPECIES_GEODUDE].types[1] == TYPE_GROUND);
}

SINGLE_BATTLE_TEST("Mantis Mimicry makes Bug-type moves Grass type and boosts them", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_MANTIS_MIMICRY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); Ability(ability); Moves(MOVE_BUG_BITE); }
        OPPONENT(SPECIES_GEODUDE) { HP(1000); MaxHP(1000); Defense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_BUG_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage * 5);
    }
}

SINGLE_BATTLE_TEST("Mantis Mimicry does not boost non-Bug moves", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_MANTIS_MIMICRY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); Ability(ability); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GEODUDE) { HP(1000); MaxHP(1000); Defense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mantis Mimicry converts dynamically Bug-type moves to Grass type", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_MANTIS_MIMICRY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ability); Moves(MOVE_INFESTED_TERRAIN, MOVE_TERRAIN_PULSE); }
        OPPONENT(SPECIES_GEODUDE) { HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TERRAIN_PULSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INFESTED_TERRAIN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TERRAIN_PULSE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage * 5);
    }
}
