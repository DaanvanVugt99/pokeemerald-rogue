#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mystic Power gives STAB to off-type moves", s16 damage)
{
    u16 move;

    PARAMETRIZE { move = MOVE_THUNDERBOLT; }
    PARAMETRIZE { move = MOVE_PSYCHIC; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_THUNDERBOLT].power == gBattleMoves[MOVE_PSYCHIC].power);
        ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);
        ASSUME(gBattleMoves[MOVE_PSYCHIC].type == TYPE_PSYCHIC);

        PLAYER(SPECIES_UNOWN) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_MYSTIC_POWER); Moves(move); }
        OPPONENT(SPECIES_EEVEE) { Ability(ABILITY_RUN_AWAY); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
