#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Water Veil grants Aqua Ring status on switch-in")
{
  GIVEN
  {
    PLAYER(SPECIES_LAPRAS)
    {
      Ability(ABILITY_WATER_VEIL);
      Speed(50);
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
    }
  }
  SCENE
  {
    MESSAGE("Lapras enveloped itself in a veil made of water!");
  }
}
