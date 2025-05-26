#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Gluttony boosts highest stat when eating a berry at half HP")
{
  GIVEN
  {
    PLAYER(SPECIES_SNORLAX)
    {
      Ability(ABILITY_GLUTTONY);
      Item(ITEM_AGUAV_BERRY);
      HP(300);
    };
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
    ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, opponent);
    HP_BAR(player);
    ABILITY_POPUP(player, ABILITY_GLUTTONY);
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    MESSAGE("Snorlax's Gluttony raised its Attack!");  // Assume Attack is highest
  }
  THEN
  {
    EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
  }
}
