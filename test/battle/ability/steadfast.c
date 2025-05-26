#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Steadfast boosts Defense and Sp. Def when HP drops below 50%")
{
  GIVEN
  {
    PLAYER(SPECIES_LUCARIO)
    {
      Ability(ABILITY_STEADFAST);
      HP(150);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(opponent, MOVE_SUPER_FANG);
    }
  }
  SCENE
  {
    ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_BEAM, opponent);
    HP_BAR(player);
    ABILITY_POPUP(player, ABILITY_STEADFAST);
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    MESSAGE("Lucario's Steadfast raised its Defense!");
    MESSAGE("Lucario's Steadfast raised its Sp. Def!");
  }
  THEN
  {
    EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
  }
}
