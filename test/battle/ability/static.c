#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Static inflicts paralysis on defensive contact")
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
    ASSUME(gBattleMoves[MOVE_TACKLE].flags & FLAG_MAKES_CONTACT);
    ASSUME(!(gBattleMoves[MOVE_SWIFT].flags & FLAG_MAKES_CONTACT));
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_PIKACHU)
    {
      Ability(ABILITY_STATIC);
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
    if (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
    {
      ABILITY_POPUP(opponent, ABILITY_STATIC);
      ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
      MESSAGE("Foe Pikachu's Static paralyzed Wobbuffet! It may be unable to move!");
      STATUS_ICON(player, paralysis : TRUE);
    } else
    {
      NONE_OF
      {
        ABILITY_POPUP(opponent, ABILITY_STATIC);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
        MESSAGE("Foe Pikachu's Static paralyzed Wobbuffet! It may be unable to move!");
        STATUS_ICON(player, paralysis : TRUE);
      }
    }
  }
}

SINGLE_BATTLE_TEST("Static inflicts paralysis on attacker's contact move")
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
    ASSUME(gBattleMoves[MOVE_TACKLE].flags & FLAG_MAKES_CONTACT);
    ASSUME(!(gBattleMoves[MOVE_SWIFT].flags & FLAG_MAKES_CONTACT));
    PLAYER(SPECIES_PIKACHU)
    {
      Ability(ABILITY_STATIC);
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
    if (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
    {
      ABILITY_POPUP(player, ABILITY_STATIC);
      ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, opponent);
      STATUS_ICON(opponent, paralysis : TRUE);
    } else
    {
      NONE_OF
      {
        ABILITY_POPUP(player, ABILITY_STATIC);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, opponent);
        STATUS_ICON(opponent, paralysis : TRUE);
      }
    }
  }
}
