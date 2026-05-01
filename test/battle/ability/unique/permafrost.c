#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_KARATE_CHOP].type == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_METAL_CLAW].type == TYPE_STEEL);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Permafrost halves damage from Fighting-, Rock-, and Steel-type moves", s16 damage)
{
    u16 move;
    bool32 hasPermafrost;

    PARAMETRIZE { move = MOVE_KARATE_CHOP; hasPermafrost = FALSE; }
    PARAMETRIZE { move = MOVE_KARATE_CHOP; hasPermafrost = TRUE; }
    PARAMETRIZE { move = MOVE_ROCK_THROW; hasPermafrost = FALSE; }
    PARAMETRIZE { move = MOVE_ROCK_THROW; hasPermafrost = TRUE; }
    PARAMETRIZE { move = MOVE_METAL_CLAW; hasPermafrost = FALSE; }
    PARAMETRIZE { move = MOVE_METAL_CLAW; hasPermafrost = TRUE; }

    GIVEN {
        if (hasPermafrost)
            PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_PERMAFROST); }
        else
            PLAYER(SPECIES_WOBBUFFET) { }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(0.5), results[3].damage);
        EXPECT_MUL_EQ(results[4].damage, UQ_4_12(0.5), results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Permafrost does not halve other move types", s16 damage)
{
    bool32 hasPermafrost;

    PARAMETRIZE { hasPermafrost = FALSE; }
    PARAMETRIZE { hasPermafrost = TRUE; }

    GIVEN {
        if (hasPermafrost)
            PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_PERMAFROST); }
        else
            PLAYER(SPECIES_WOBBUFFET) { }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
