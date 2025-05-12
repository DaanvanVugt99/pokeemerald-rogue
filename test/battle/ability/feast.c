#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Feast heals 1/2 HP when the Pokémon knocks out an opponent")
{
    GIVEN
    {
        PLAYER(SPECIES_WOBBUFFET)
        {
            Ability(ABILITY_FEAST);
            HP(50);
            MaxHP(100);
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
        ABILITY_POPUP(player, ABILITY_FEAST);
        MESSAGE("Wobbuffet's Feast restored its HP a little!");
        HP_BAR(player, damage : -50); // Expect healing by 1/2 of max HP
    }
}