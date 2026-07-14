#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(gBattleMoves[MOVE_HIGH_JUMP_KICK].kickingMove);
}

SINGLE_BATTLE_TEST("Freestyle boosts punching and kicking moves by 1.3x", s16 damage)
{
    u16 move;
    u16 uniqueAbility;

    PARAMETRIZE { move = MOVE_MEGA_PUNCH;      uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_MEGA_PUNCH;      uniqueAbility = ABILITY_FREESTYLE; }
    PARAMETRIZE { move = MOVE_HIGH_JUMP_KICK; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_HIGH_JUMP_KICK; uniqueAbility = ABILITY_FREESTYLE; }

    GIVEN {
        PLAYER(SPECIES_WURMPLE) { Attack(120); UniqueAbility(uniqueAbility); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.3), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Freestyle does not boost moves that are neither punching nor kicking", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_FREESTYLE; }

    GIVEN {
        ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
        ASSUME(!gBattleMoves[MOVE_TACKLE].kickingMove);
        PLAYER(SPECIES_WURMPLE) { Attack(120); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
