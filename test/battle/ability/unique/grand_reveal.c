#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].priority == 1);
}

SINGLE_BATTLE_TEST("Grand Reveal guarantees critical hits for the turn Illusion breaks")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK) { Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_GRAND_REVEAL); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BLISSEY) { Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_QUICK_ATTACK, criticalHit: FALSE); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Without Grand Reveal, Illusion breaking does not force a critical hit")
{
    GIVEN {
        PLAYER(SPECIES_DITTO) { Ability(ABILITY_ILLUSION); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BLISSEY) { Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_QUICK_ATTACK, criticalHit: FALSE); MOVE(player, MOVE_SCRATCH, criticalHit: FALSE); }
    } SCENE {
        NONE_OF {
            MESSAGE("A critical hit!");
        }
    }
}
