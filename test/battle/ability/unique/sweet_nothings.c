#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAINING_KISS].kissingMove);
    ASSUME(gBattleMoves[MOVE_SWEET_KISS].kissingMove);
    ASSUME(gBattleMoves[MOVE_LOVELY_KISS].kissingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].kissingMove);
}

SINGLE_BATTLE_TEST("Sweet Nothings doubles Draining Kiss damage", s16 damage)
{
    u16 opponentAbility;
    PARAMETRIZE { opponentAbility = ABILITY_NEUTRALIZING_GAS; }
    PARAMETRIZE { opponentAbility = ABILITY_SHADOW_TAG; }

    GIVEN {
        PLAYER(SPECIES_JYNX) { Ability(ABILITY_OBLIVIOUS); UniqueAbility(ABILITY_SWEET_NOTHINGS); Moves(MOVE_DRAINING_KISS); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(opponentAbility); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAINING_KISS); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        s16 lower = results[0].damage < results[1].damage ? results[0].damage : results[1].damage;
        s16 higher = results[0].damage < results[1].damage ? results[1].damage : results[0].damage;
        EXPECT_GT(higher, lower * 16 / 10);
    }
}

SINGLE_BATTLE_TEST("Sweet Nothings lowers Attack after kissing moves")
{
    u16 move;
    PARAMETRIZE { move = MOVE_DRAINING_KISS; }
    PARAMETRIZE { move = MOVE_SWEET_KISS; }
    PARAMETRIZE { move = MOVE_LOVELY_KISS; }

    PASSES_RANDOMLY(gBattleMoves[move].accuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_JYNX) { Ability(ABILITY_OBLIVIOUS); UniqueAbility(ABILITY_SWEET_NOTHINGS); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SWEET_NOTHINGS);
    } THEN {
        EXPECT_LT(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
