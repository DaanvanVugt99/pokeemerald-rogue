#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Cold Rebound retaliates with Icy Wind on contact")
{
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_DEWGONG)
    {
      Ability(ABILITY_COLD_REBOUND);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_TACKLE);
    }
  }
  SCENE
  {
    ABILITY_POPUP(opponent, ABILITY_COLD_REBOUND);
  }
}
