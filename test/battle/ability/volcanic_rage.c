#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Volcanic Rage triggers after using a Fire-type move")
{
    GIVEN
    {
        PLAYER(SPECIES_TYPHLOSION) { Ability(ABILITY_VOLCANIC_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_EMBER); }
    }
    SCENE
    {
        ABILITY_POPUP(player, ABILITY_VOLCANIC_RAGE);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Volcanic Rage does not trigger after using a non-Fire-type move")
{
    GIVEN
    {
        PLAYER(SPECIES_TYPHLOSION) { Ability(ABILITY_VOLCANIC_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_TACKLE); }
    }
    SCENE
    {
        NOT ABILITY_POPUP(player, ABILITY_VOLCANIC_RAGE);
    }
}

DOUBLE_BATTLE_TEST("Volcanic Rage triggers after Fire-type move in double battle")
{
    GIVEN
    {
        PLAYER(SPECIES_TYPHLOSION) { Ability(ABILITY_VOLCANIC_RAGE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN
        {
            MOVE(playerLeft, MOVE_EMBER);
        }
    }
    SCENE
    {
        ABILITY_POPUP(playerLeft, ABILITY_VOLCANIC_RAGE);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
    }
}

SINGLE_BATTLE_TEST("Volcanic Rage does not trigger if Fire-type move fails")
{
    GIVEN
    {
        PLAYER(SPECIES_TYPHLOSION) { Ability(ABILITY_VOLCANIC_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_FLASH_FIRE); }
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_EMBER); }
    }
    SCENE
    {
        NOT ABILITY_POPUP(player, ABILITY_VOLCANIC_RAGE);
    }
}

SINGLE_BATTLE_TEST("Volcanic Rage deals the same damage at low health")
{
    GIVEN
    {
        PLAYER(SPECIES_TYPHLOSION)
        {
            Ability(ABILITY_VOLCANIC_RAGE);
            MaxHP(100);
            HP(10);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_EMBER); }
    }
    SCENE
    {
        ABILITY_POPUP(player, ABILITY_VOLCANIC_RAGE);
    }
}