#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Effect Spore inflicts a status on defensive contact")
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
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_BRELOOM)
    {
      Ability(ABILITY_EFFECT_SPORE);
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
      PASSES_RANDOMLY(1, 9);
      ABILITY_POPUP(opponent, ABILITY_EFFECT_SPORE);
      ONE_OF
      {
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
        STATUS_ICON(player, paralysis : TRUE);

        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
        STATUS_ICON(player, sleep : TRUE);

        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, player);
        STATUS_ICON(player, poison : TRUE);
      }
    } else
    {
      NONE_OF
      {
        ABILITY_POPUP(opponent, ABILITY_EFFECT_SPORE);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, player);
        STATUS_ICON(player, paralysis : TRUE);
        STATUS_ICON(player, sleep : TRUE);
        STATUS_ICON(player, poison : TRUE);
      }
    }
  }
}

SINGLE_BATTLE_TEST("Effect Spore inflicts a status on attacker's contact move")
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
    PLAYER(SPECIES_BRELOOM)
    {
      Ability(ABILITY_EFFECT_SPORE);
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
      PASSES_RANDOMLY(1, 9);
      ABILITY_POPUP(player, ABILITY_EFFECT_SPORE);
      ONE_OF
      {
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
        STATUS_ICON(player, paralysis : TRUE);

        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
        STATUS_ICON(player, sleep : TRUE);

        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, player);
        STATUS_ICON(player, poison : TRUE);
      }

    } else
    {
      NONE_OF
      {
        ABILITY_POPUP(player, ABILITY_EFFECT_SPORE);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, opponent);
        STATUS_ICON(opponent, paralysis : TRUE);
        STATUS_ICON(opponent, sleep : TRUE);
        STATUS_ICON(opponent, burn : TRUE);
      }
    }
  }
}
