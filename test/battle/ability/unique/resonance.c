#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SNARL].soundMove);
    ASSUME(gBattleMoves[MOVE_SNARL].type == TYPE_DARK);
}

SINGLE_BATTLE_TEST("Resonance makes sound moves super effective against Rock-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_NOIVERN) { Ability(ABILITY_INFILTRATOR); UniqueAbility(ABILITY_RESONANCE); Moves(MOVE_SNARL); }
        OPPONENT(SPECIES_GEODUDE);
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL); }
    } SCENE {
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Resonance makes sound moves super effective against Steel-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_NOIVERN) { Ability(ABILITY_INFILTRATOR); UniqueAbility(ABILITY_RESONANCE); Moves(MOVE_SNARL); }
        OPPONENT(SPECIES_MAGNEMITE);
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL); }
    } SCENE {
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Resonance makes sound moves super effective against Ice-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_NOIVERN) { Ability(ABILITY_INFILTRATOR); UniqueAbility(ABILITY_RESONANCE); Moves(MOVE_SNARL); }
        OPPONENT(SPECIES_SPHEAL);
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL); }
    } SCENE {
        MESSAGE("It's super effective!");
    }
}
