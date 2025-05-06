#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Blitz Boxer only grants priority at full HP")
{
    u16 hp;
    PARAMETRIZE { hp = 100; }
    PARAMETRIZE { hp = 99; }

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_DRAIN_PUNCH].flags & FLAG_PUNCHING_BASED);
        PLAYER(SPECIES_HITMONCHAN)
        {
            Ability(ABILITY_BLITZ_BOXER);
            HP(hp);
            MaxHP(100);
            Speed(1);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_DRAIN_PUNCH); }
    }
    SCENE
    {
        if (hp == 100)
        {
            MESSAGE("Hitmonchan used Drain Punch!");
            MESSAGE("Foe Wobbuffet used Celebrate!");
        }
        else
        {
            MESSAGE("Foe Wobbuffet used Celebrate!");
            MESSAGE("Hitmonchan used Drain Punch!");
        }
    }
}

SINGLE_BATTLE_TEST("Blitz Boxer only grants priority to punching moves")
{
    u32 move;
    PARAMETRIZE { move = MOVE_DRAIN_PUNCH; }
    PARAMETRIZE { move = MOVE_TACKLE; }

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_DRAIN_PUNCH].flags & FLAG_PUNCHING_BASED);
        ASSUME(!(gBattleMoves[MOVE_TACKLE].flags & FLAG_PUNCHING_BASED));
        PLAYER(SPECIES_HITMONCHAN)
        {
            Ability(ABILITY_BLITZ_BOXER);
            HP(100);
            MaxHP(100);
            Speed(1);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    }
    WHEN
    {
        TURN { MOVE(player, move); }
    }
    SCENE
    {
        if (move == MOVE_DRAIN_PUNCH)
        {
            MESSAGE("Hitmonchan used Drain Punch!");
            MESSAGE("Foe Wobbuffet used Celebrate!");
        }
        else
        {
            MESSAGE("Foe Wobbuffet used Celebrate!");
            MESSAGE("Hitmonchan used Tackle!");
        }
    }
}