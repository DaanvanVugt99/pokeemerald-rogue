#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Anticipation grants +1 Evasiveness on activation")
{
    GIVEN
    {
        PLAYER(SPECIES_WORMADAM) { Ability(ABILITY_ANTICIPATION); }
        OPPONENT(SPECIES_CHARMANDER);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_EMBER); }
    }
    SCENE
    {
        MESSAGE("Wormadam's evasiveness rose!");
    }
}

DOUBLE_BATTLE_TEST("Anticipation grants +1 Evasiveness on activation in a double battle")
{
    GIVEN
    {
        PLAYER(SPECIES_WORMADAM) { Ability(ABILITY_ANTICIPATION); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARMANDER);
        OPPONENT(SPECIES_PIDGEY);
    }
    WHEN
    {
        TURN
        {
            MOVE(opponentLeft, MOVE_EMBER, target : playerRight);
            MOVE(opponentRight, MOVE_GUST, target : playerRight);
        }
    }
    SCENE
    {
        MESSAGE("Wormadam's evasiveness rose!");
    }
}
