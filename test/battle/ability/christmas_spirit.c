#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Christmas Spirit takes less damage when in Hail or Snow", s16 damage)
{
  u32 move;
  PARAMETRIZE
  {
    move = MOVE_HAIL;
  }
  PARAMETRIZE
  {
    move = MOVE_SNOWSCAPE;
  }
  PARAMETRIZE
  {
    move = MOVE_CELEBRATE;
  }
  GIVEN
  {
    PLAYER(SPECIES_DELIBIRD)
    {
      Ability(ABILITY_CHRISTMAS_SPIRIT);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, move);
      MOVE(opponent, MOVE_EMBER);
    }
  }
  SCENE
  {
    HP_BAR(player, captureDamage : &results[i].damage);
  }
  FINALLY
  {
    EXPECT_GT(results[0].damage, results[2].damage);
    EXPECT_GT(results[1].damage, results[2].damage);
  }
}
