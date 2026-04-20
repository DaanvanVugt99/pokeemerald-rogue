#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_KARATE_CHOP].type == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIGHTING);
}

SINGLE_BATTLE_TEST("Blubber halves Fire- and Fighting-type damage", s16 damage)
{
    u16 move;
    bool32 reduced;

    PARAMETRIZE { move = MOVE_EMBER; reduced = TRUE; }
    PARAMETRIZE { move = MOVE_KARATE_CHOP; reduced = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; reduced = FALSE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_BLUBBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        if (reduced)
            EXPECT_MUL_EQ(results[2].damage, UQ_4_12(0.5), results[i].damage);
    }
}
