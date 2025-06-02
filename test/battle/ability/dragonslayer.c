#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Dragonslayer increases damage vs. Dragon-type", s16 damage)
{
  u16 ability;
  PARAMETRIZE
  {
    ability = ABILITY_DRAGONSLAYER;
  }
  PARAMETRIZE
  {
    ability = ABILITY_JUSTIFIED;
  }
  GIVEN
  {
    PLAYER(SPECIES_COBALION)
    {
      Ability(ability);
    }
    OPPONENT(SPECIES_DRAGONITE);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_SACRED_SWORD);
    }
  }
  SCENE
  {
    HP_BAR(opponent, captureDamage : &results[i].damage);
  }
  FINALLY
  {
    EXPECT_GT(results[0].damage, results[1].damage);
  }
}
