#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Scavenger heals 1/3 HP when a Pokémon faints")
{
    GIVEN
    {
        PLAYER(SPECIES_WOBBUFFET)
        {
            Ability(ABILITY_SCAVENGER);
            HP(50);
            MaxHP(150);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    }
    WHEN
    {
        TURN
        {
            MOVE(player, MOVE_TACKLE);
            SEND_OUT(opponent, 1);
        }
    }
    SCENE
    {
        MESSAGE("Foe Magikarp fainted!");
        ABILITY_POPUP(player, ABILITY_SCAVENGER);
        MESSAGE("Wobbuffet's Scavenger restored its HP a little!");
        HP_BAR(player, damage : -50);
    }
}

DOUBLE_BATTLE_TEST("Scavenger heals 1/3 HP per fainted Pokémon when multiple faint from one move")
{
    GIVEN
    {
        PLAYER(SPECIES_WOBBUFFET)
        {
            Ability(ABILITY_SCAVENGER);
            HP(50);
            MaxHP(150);
        }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_MAGIKARP) { HP(1); }
        OPPONENT(SPECIES_FEEBAS) { HP(1); }
    }
    WHEN
    {
        TURN
        {
            MOVE(playerLeft, MOVE_SURF);
        }
    }
    SCENE
    {
        MESSAGE("Foe Magikarp fainted!");
        MESSAGE("Foe Feebas fainted!");
        ABILITY_POPUP(playerLeft, ABILITY_SCAVENGER);
        MESSAGE("Wobbuffet's Scavenger restored its HP a little!");
        HP_BAR(playerLeft, damage : -100); // Heals 1/3 HP for each fainted Pokémon (50+50)
    }
}