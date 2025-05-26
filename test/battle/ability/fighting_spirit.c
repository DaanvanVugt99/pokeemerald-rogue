#include "global.h"
#include "test/battle.h"

// Test: Fighting Spirit changes Fighting moves to Ghost type and boosts damage by 1.3x
SINGLE_BATTLE_TEST("Fighting Spirit changes Fighting move to Ghost and boosts damage", s16 damage)
{
  u16 ability;
  PARAMETRIZE
  {
    ability = ABILITY_FIGHTING_SPIRIT;
  }
  PARAMETRIZE
  {
    ability = ABILITY_STEADFAST;
  }
  GIVEN
  {
    PLAYER(SPECIES_LUCARIO)
    {
      Ability(ability);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_CLOSE_COMBAT);
    }
  }
  SCENE
  {
    HP_BAR(opponent, captureDamage : &results[i].damage);
  }
  FINALLY
  {
    EXPECT_GT(results[1].damage, results[0].damage);
  }
}
