#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Levitate gives Flying-type moves STAB", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_LEVITATE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
        ASSUME(gBattleMoves[MOVE_AERIAL_ACE].power > 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Levitate does not give non-Flying moves STAB", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_LEVITATE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
        ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
