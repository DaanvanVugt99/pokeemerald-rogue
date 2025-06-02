#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Inflatable boosts Defense and Sp. Def when hit by Fire or Flying move")
{
  u32 move;
  PARAMETRIZE
  {
    move = MOVE_FLAMETHROWER;
  }  // Fire-type move
  PARAMETRIZE
  {
    move = MOVE_AERIAL_ACE;
  }  // Flying-type move
  PARAMETRIZE
  {
    move = MOVE_TACKLE;
  }  // Neutral test: not Fire or Flying

  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_DRIFBLIM)
    {
      Ability(ABILITY_INFLATABLE);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, move);
    }
  }
  SCENE
  {
    HP_BAR(opponent);
    if (move == MOVE_FLAMETHROWER || move == MOVE_AERIAL_ACE)
    {
      ABILITY_POPUP(opponent, ABILITY_INFLATABLE);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
      MESSAGE("Drifblim's Inflatable raised its Defense!");
      MESSAGE("Drifblim's Inflatable raised its Sp. Def!");
    }
  }
  THEN
  {
    if (move == MOVE_FLAMETHROWER || move == MOVE_AERIAL_ACE)
    {
      EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
      EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    } else
    {
      EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
      EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
  }
}
