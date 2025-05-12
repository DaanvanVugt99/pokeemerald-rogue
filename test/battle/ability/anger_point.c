#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Anger Point maximizes Attack on a critical hit")
{
    GIVEN
    {
        PLAYER(SPECIES_PRIMEAPE) { Ability(ABILITY_ANGER_POINT); }
        OPPONENT(SPECIES_CHARMANDER);
    }
    WHEN
    {
        TURN
        {
            MOVE(opponent, MOVE_SLASH, criticalHit : 1);
        }
    }
    SCENE
    {
        MESSAGE("Primeape's Anger Point maxed its Attack!"); // Stat maxed out message
    }
}

SINGLE_BATTLE_TEST("Anger Point gives +1 Attack on a non-critical physical hit")
{
    GIVEN
    {
        PLAYER(SPECIES_PRIMEAPE) { Ability(ABILITY_ANGER_POINT); }
        OPPONENT(SPECIES_CHARMANDER);
    }
    WHEN
    {
        TURN
        {
            MOVE(opponent, MOVE_TACKLE);
        }
    }
    SCENE
    {
        MESSAGE("Primeape's Attack rose!");
    }
}

SINGLE_BATTLE_TEST("Anger Point does not trigger on a special move")
{
    GIVEN
    {
        PLAYER(SPECIES_PRIMEAPE) { Ability(ABILITY_ANGER_POINT); }
        OPPONENT(SPECIES_CHARMANDER);
    }
    WHEN
    {
        TURN
        {
            MOVE(opponent, MOVE_EMBER);
        }
    }
    SCENE
    {
        NONE_OF { MESSAGE("Primeape's Attack rose!"); }
    }
}
