#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_PSYCHIC_TERRAIN].effect == EFFECT_PSYCHIC_TERRAIN);
    ASSUME(gBattleMoves[MOVE_PLAIN_TERRAIN].effect == EFFECT_PLAIN_TERRAIN);
}

SINGLE_BATTLE_TEST("Monkey Mind reduces special damage and is stronger in Psychic Terrain", s16 damage)
{
    u32 ability;
    u32 terrainMove;
    u32 species;

    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; terrainMove = MOVE_NONE; }
    PARAMETRIZE { species = SPECIES_ORANGURU; ability = ABILITY_MONKEY_MIND; terrainMove = MOVE_NONE; }
    PARAMETRIZE { species = SPECIES_ORANGURU; ability = ABILITY_MONKEY_MIND; terrainMove = MOVE_PSYCHIC_TERRAIN; }
    PARAMETRIZE { species = SPECIES_ORANGURU; ability = ABILITY_MONKEY_MIND; terrainMove = MOVE_PLAIN_TERRAIN; }

    GIVEN {
        PLAYER(species) { Ability(ability); SpDefense(100); Moves(MOVE_PSYCHIC_TERRAIN, MOVE_PLAIN_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); }
    } WHEN {
        if (terrainMove != MOVE_NONE)
            TURN { MOVE(player, terrainMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.9), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.75), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.75), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Monkey Mind does not reduce physical damage", s16 damage)
{
    u32 ability;
    u32 species;

    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_ORANGURU; ability = ABILITY_MONKEY_MIND; }

    GIVEN {
        PLAYER(species) { Ability(ability); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
