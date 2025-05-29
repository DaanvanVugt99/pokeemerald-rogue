#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
  ASSUME(gBattleMoves[MOVE_TOXIC].effect == EFFECT_TOXIC);
}

SINGLE_BATTLE_TEST("Merciless crits when target is poisoned")
{
  GIVEN
  {
    PLAYER(SPECIES_TOXAPEX)
    {
      Ability(ABILITY_MERCILESS);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_TOXIC);
    }
    TURN
    {
      MOVE(player, MOVE_TACKLE);
    }
  }
  SCENE
  {
    ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, player);
    ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, opponent);
    STATUS_ICON(opponent, badPoison : TRUE);
  }
}

SINGLE_BATTLE_TEST("Merciless crits when target has lowered speed")
{
  GIVEN
  {
    PLAYER(SPECIES_TOXAPEX)
    {
      Ability(ABILITY_MERCILESS);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_SCARY_FACE);
    }
    TURN
    {
      MOVE(player, MOVE_TACKLE);
    }
  }
  SCENE
  {
    ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, player);
  }
}
