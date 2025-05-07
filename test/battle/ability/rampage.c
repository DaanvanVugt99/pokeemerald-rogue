#include "global.h"
#include "test/battle.h"

#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Rampage allows recharge moves to act next turn")
{
    u32 move;
    ASSUME(gBattleMoves[MOVE_HYPER_BEAM].effect == EFFECT_RECHARGE);
    ASSUME(gBattleMoves[MOVE_HYDRO_CANNON].effect == EFFECT_RECHARGE);

    PARAMETRIZE { move = MOVE_HYPER_BEAM; }
    PARAMETRIZE { move = MOVE_HYDRO_CANNON; }

    GIVEN
    {
        PLAYER(SPECIES_GYARADOS) { Ability(ABILITY_RAMPAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    }

    WHEN
    {
        TURN
        {
            MOVE(player, move);
            SEND_OUT(opponent, 1);
        }
        TURN
        {
            MOVE(player, MOVE_TACKLE);
        }
    }
    SCENE
    {
        HP_BAR(opponent);
        NONE_OF { MESSAGE("Gyarados must recharge!"); }
        MESSAGE("Gyarados used Tackle!");
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Rampage does not prevent recharge if no KO")
{
    GIVEN
    {
        PLAYER(SPECIES_GYARADOS) { Ability(ABILITY_RAMPAGE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    }

    WHEN
    {
        TURN
        {
            MOVE(player, MOVE_HYPER_BEAM); // Gyarados uses Hyper Beam
        }
        TURN
        {
            SKIP_TURN(player);              // Explicitly state Gyarados cannot move
            MOVE(opponent, MOVE_CELEBRATE); // Opponent moves while Gyarados is recharging
        }
        TURN
        {
            MOVE(player, MOVE_TACKLE); // Gyarados tries to move after recharge
        }
    }
    SCENE
    {
        HP_BAR(opponent);                         // Confirm Wobbuffet survived
        MESSAGE("Gyarados must recharge!");       // Check that recharge happens
        MESSAGE("Foe Wobbuffet used Celebrate!"); // Validate opponent's move
        MESSAGE("Gyarados used Tackle!");         // Ensure Gyarados moves after recharge
        HP_BAR(opponent);
    }
}