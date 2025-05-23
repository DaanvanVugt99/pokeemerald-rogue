#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Pickup clears hazards on switch-in")
{
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    PLAYER(SPECIES_ZIGZAGOON)
    {
      Ability(ABILITY_PICKUP);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(opponent, MOVE_STEALTH_ROCK);
    }
    TURN
    {
      SWITCH(player, 1);
    }
  }
  SCENE
  {
    MESSAGE("Pointed stones float in the air around your team!");
    ABILITY_POPUP(player, ABILITY_PICKUP);
    MESSAGE("Zigzagoon's Pickup removed the hazards on its side!");
  }
}
