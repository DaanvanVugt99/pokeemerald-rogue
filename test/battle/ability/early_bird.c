#include "global.h"
#include "test/battle.h"

// Test: Early Bird grants a Speed boost when the Pokémon wakes up
SINGLE_BATTLE_TEST("Early Bird grants a Speed boost when the Pokémon wakes up")
{
  GIVEN
  {
    PLAYER(SPECIES_XATU)
    {
      Ability(ABILITY_EARLY_BIRD);
      MovesWithPP({MOVE_TACKLE, 35}, {MOVE_REST, 10});
    }
    OPPONENT(SPECIES_HYPNO)
    {
      MovesWithPP({MOVE_HYPNOSIS, 20});
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(opponent, MOVE_HYPNOSIS);
      MOVE(player, MOVE_REST);
    }
    TURN
    {
      MOVE(player, MOVE_TACKLE);  // Wake up and act
    }
  }
}
