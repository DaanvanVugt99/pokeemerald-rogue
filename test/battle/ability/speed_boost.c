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
      Speed(99);
    }
    OPPONENT(SPECIES_HYPNO)
    {
      Speed(100);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(opponent, MOVE_HYPNOSIS);
    }  // Hypno uses Hypnosis to put Xatu to sleep
    TURN;  // Skip to simulate sleep turns
    TURN;  // Sleep turn 2 (Early Bird wakes up here)
    TURN
    {
      MOVE(player, MOVE_TACKLE);
    }  // Xatu wakes up and uses Tackle
  }
  SCENE
  {
    MESSAGE("Hypno used Hypnosis!");
    MESSAGE("Xatu fell asleep!");
    ABILITY_POPUP(player, ABILITY_EARLY_BIRD);
    MESSAGE("Xatu's Early Bird raised it's speed!");
  }
}
