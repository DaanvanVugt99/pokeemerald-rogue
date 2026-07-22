#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PLAIN_TERRAIN].effect == EFFECT_PLAIN_TERRAIN);
    ASSUME(gItems[ITEM_PLAIN_SEED].holdEffect == HOLD_EFFECT_SEEDS);
    ASSUME(gItems[ITEM_PLAIN_SEED].holdEffectParam == HOLD_EFFECT_PARAM_PLAIN_TERRAIN);
    ASSUME(gSpeciesInfo[SPECIES_GASTLY].types[0] == TYPE_GHOST || gSpeciesInfo[SPECIES_GASTLY].types[1] == TYPE_GHOST);
    ASSUME(gSpeciesInfo[SPECIES_GEODUDE].types[0] == TYPE_ROCK || gSpeciesInfo[SPECIES_GEODUDE].types[1] == TYPE_ROCK);
    ASSUME(gSpeciesInfo[SPECIES_REGISTEEL].types[0] == TYPE_STEEL || gSpeciesInfo[SPECIES_REGISTEEL].types[1] == TYPE_STEEL);
}

SINGLE_BATTLE_TEST("Plain Terrain activates Plain Seed and raises accuracy")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PLAIN_SEED); Moves(MOVE_PLAIN_TERRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PLAIN_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Using Plain Seed, the accuracy of Wobbuffet rose!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Plain Terrain activates Mimicry and changes battlers to Normal type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_STUNFISK_GALARIAN) { Ability(ABILITY_MIMICRY); UniqueAbility(ABILITY_IMPACT); }
    } WHEN {
        TURN { MOVE(player, MOVE_PLAIN_TERRAIN); }
    } SCENE {
        ABILITY_POPUP(opponent);
        MESSAGE("Foe Stunfisk's type changed to Normal!");
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].type1, TYPE_NORMAL);
    }
}

SINGLE_BATTLE_TEST("Plain Terrain increases power of Normal-type moves by 30/50 percent", s16 damage)
{
    bool32 terrain;

    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_PLAIN_TERRAIN); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Wobbuffet used Tackle!");
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        if (B_TERRAIN_TYPE_BOOST >= GEN_8)
            EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage);
        else
            EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Normal-type moves do not affect Ghost-types without Plain Terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GASTLY);
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Plain Terrain lets Normal-type moves hit Ghost-types")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GASTLY);
    } WHEN {
        TURN { MOVE(player, MOVE_PLAIN_TERRAIN); }
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Plain Terrain makes Normal-type moves neutral against Rock and Steel", s16 damage)
{
    bool32 terrain;
    u16 species;

    PARAMETRIZE { terrain = FALSE; species = SPECIES_GEODUDE; }
    PARAMETRIZE { terrain = TRUE; species = SPECIES_GEODUDE; }
    PARAMETRIZE { terrain = FALSE; species = SPECIES_REGISTEEL; }
    PARAMETRIZE { terrain = TRUE; species = SPECIES_REGISTEEL; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species);
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_PLAIN_TERRAIN); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Wobbuffet used Tackle!");
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        if (B_TERRAIN_TYPE_BOOST >= GEN_8)
        {
            EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.6), results[1].damage);
            EXPECT_MUL_EQ(results[2].damage, Q_4_12(2.6), results[3].damage);
        }
        else
        {
            EXPECT_MUL_EQ(results[0].damage, Q_4_12(3.0), results[1].damage);
            EXPECT_MUL_EQ(results[2].damage, Q_4_12(3.0), results[3].damage);
        }
    }
}
