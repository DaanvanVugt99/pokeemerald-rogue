#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Forewarn disables move on switch-in")
{
  GIVEN
  {
    PLAYER(SPECIES_XATU)
    {
      Ability(ABILITY_FOREWARN);
    }
    OPPONENT(SPECIES_CHARMANDER)
    {
      Moves(MOVE_EMBER, MOVE_FLAMETHROWER, MOVE_SMOKESCREEN, MOVE_GROWL);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(opponent, MOVE_EMBER);
    }
  }
  SCENE
  {
    ABILITY_POPUP(player, ABILITY_FOREWARN);
  }
}
