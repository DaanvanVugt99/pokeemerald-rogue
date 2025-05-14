#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Low Blow uses feint attack on switch in")
{
    GIVEN
    {
        PLAYER(SPECIES_SNEASEL) { Ability(ABILITY_LOW_BLOW); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN {}
    }
    SCENE
    {
        ABILITY_POPUP(player, ABILITY_LOW_BLOW);
    }
}

DOUBLE_BATTLE_TEST("Low Blow targets correct opponent in double battle")
{
    GIVEN
    {
        PLAYER(SPECIES_SNEASEL) { Ability(ABILITY_LOW_BLOW); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN {}
    }
    SCENE
    {
        ABILITY_POPUP(playerLeft, ABILITY_LOW_BLOW);
        MESSAGE("Sneasel used Feint Attack!");
        HP_BAR(opponentLeft); // Should target the left opponent (Wobbuffet) by default
    }
}