#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Vital Spirit prevents Superpower stat drops")
{
  s16 turnOneHit;
  s16 turnTwoHit;

  GIVEN
  {
    PLAYER(SPECIES_MANKEY)
    {
      Ability(ABILITY_VITAL_SPIRIT);
    };
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_SUPERPOWER);
    }
    TURN
    {
      MOVE(player, MOVE_SUPERPOWER);
    }
  }
  SCENE
  {
    HP_BAR(player, captureDamage : &turnOneHit);
    NONE_OF
    {
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    }
    HP_BAR(player, captureDamage : &turnTwoHit);
  }
  THEN
  {
    EXPECT_EQ(turnOneHit, turnTwoHit);
  }
}

SINGLE_BATTLE_TEST("Vital Spirit prevents Hammer Arm stat drops")
{
  GIVEN
  {
    PLAYER(SPECIES_MANKEY)
    {
      Ability(ABILITY_VITAL_SPIRIT);
    };
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_HAMMER_ARM);
    }
  }
  SCENE
  {
    NONE_OF
    {
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    }
  }
}
