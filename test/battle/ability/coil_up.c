#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Coil up makes the first biting move gain priority")
{
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_BITE].flags & FLAG_BITING_BASED);
        PLAYER(SPECIES_EKANS)
        {
            Ability(ABILITY_COIL_UP);
            Speed(10);
        }
        OPPONENT(SPECIES_WOBBUFFET)
        {
            Speed(100);
        }
    }
    WHEN
    {
        TURN
        {
            MOVE(opponent, MOVE_CELEBRATE);
            MOVE(player, MOVE_BITE);
        }
        TURN
        {
            MOVE(opponent, MOVE_CELEBRATE);
            MOVE(player, MOVE_BITE);
        }
    }
    SCENE
    {
        MESSAGE("Ekans used Bite!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Ekans used Bite!");
    }
}
