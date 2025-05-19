#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Suction Cups traps opponent on contact move")
{
  u32 move;
  PARAMETRIZE
  {
    move = MOVE_TACKLE;
  }
  PARAMETRIZE
  {
    move = MOVE_SWIFT;
  }
  GIVEN
  {
    ASSUME(gBattleMoves[MOVE_TACKLE].flags == FLAG_MAKES_CONTACT);
    ASSUME(!(gBattleMoves[MOVE_SWIFT].flags == FLAG_MAKES_CONTACT));
    PLAYER(SPECIES_OCTILLERY)
    {
      Ability(ABILITY_SUCTION_CUPS);
    }
    OPPONENT(SPECIES_WOBBUFFET);
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
    if (gBattleMoves[move].flags == FLAG_MAKES_CONTACT)
    {
      ABILITY_POPUP(player, ABILITY_SUCTION_CUPS);
    } else
    {
      NONE_OF
      {
        ABILITY_POPUP(player, ABILITY_SUCTION_CUPS);
      }
    }
  }
}
