#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Branch Swing increases Speed by 50% on Grassy Terrain")
{
    GIVEN
    {
        PLAYER(SPECIES_MANKEY)
        {
            Ability(ABILITY_BRANCH_SWING);
            Speed(100);
        }
        OPPONENT(SPECIES_WOBBUFFET)
        {
            Speed(120);
        }
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); }
        TURN
        {
            MOVE(player, MOVE_TACKLE);
            MOVE(opponent, MOVE_TACKLE);
        }
    }
    SCENE
    {
        MESSAGE("Mankey used Tackle!"); // Mankey should move first
        MESSAGE("Foe Pidgey used Tackle!");
    }
}

SINGLE_BATTLE_TEST("Branch Swing makes the Pokémon ungrounded on Grassy Terrain")
{
    GIVEN
    {
        PLAYER(SPECIES_MANKEY)
        {
            Ability(ABILITY_BRANCH_SWING);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); }
        TURN
        {
            MOVE(opponent, MOVE_EARTHQUAKE);
        }
    }
    SCENE
    {
        MESSAGE("Foe Wobbuffet used Earthquake!");
        MESSAGE("But it failed!");
    }
}
