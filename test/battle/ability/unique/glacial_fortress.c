#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power == gBattleMoves[MOVE_WATER_GUN].power);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Glacial Fortress uses Defense against special attacks", s16 damage)
{
    u16 move;
    bool32 hasGlacialFortress;

    PARAMETRIZE { move = MOVE_TACKLE; hasGlacialFortress = FALSE; }
    PARAMETRIZE { move = MOVE_TACKLE; hasGlacialFortress = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; hasGlacialFortress = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; hasGlacialFortress = FALSE; }

    GIVEN {
        if (hasGlacialFortress)
        {
            PLAYER(SPECIES_WOBBUFFET) {
                HP(1000); MaxHP(1000); Defense(200); SpDefense(50);
                UniqueAbility(ABILITY_GLACIAL_FORTRESS);
            }
        }
        else
        {
            PLAYER(SPECIES_WOBBUFFET) {
                HP(1000); MaxHP(1000); Defense(200); SpDefense(50);
            }
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Attack(100); SpAttack(100);
            Moves(move);
        }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_GT(results[3].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Mold Breaker bypasses Glacial Fortress", s16 damage)
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            HP(1000); MaxHP(1000); Defense(200); SpDefense(50);
            UniqueAbility(ABILITY_GLACIAL_FORTRESS);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            SpAttack(100);
            Ability(ability);
            Moves(MOVE_WATER_GUN);
        }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}
