#include "global.h"
#include "test/battle.h"

// Test: Rock Head changes Normal-type recoil move to Rock-type and prevents recoil
SINGLE_BATTLE_TEST("Rock Head changes Normal recoil move to Rock-type and prevents recoil", s16 damage)
{
  u32 ability;

  PARAMETRIZE
  {
    ability = ABILITY_ROCK_HEAD;
  }
  PARAMETRIZE
  {
    ability = ABILITY_STURDY;
  }

  GIVEN
  {
    ASSUME(gBattleMoves[MOVE_DOUBLE_EDGE].type == TYPE_NORMAL);
    PLAYER(SPECIES_GOLEM)
    {
      Ability(ability);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_DOUBLE_EDGE);
    }
  }
  SCENE
  {
    HP_BAR(opponent, captureDamage : &results[i].damage);
    if (ability == ABILITY_ROCK_HEAD)
    {
      NONE_OF
      {
        HP_BAR(player);
        MESSAGE("Golem is hurt by recoil!");
      }
    }
  }
  FINALLY
  {
    EXPECT_GT(results[0].damage, results[1].damage);  // Gains STAB
  }
}
