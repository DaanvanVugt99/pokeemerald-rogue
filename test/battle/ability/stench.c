#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Stench has a ~20% chance to force switch on contact hit")
{
  GIVEN
  {
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(IsMoveMakingContact(MOVE_TACKLE, SPECIES_RATTATA));
    PLAYER(SPECIES_RATTATA)
    {
      Moves(MOVE_TACKLE);
    }
    PLAYER(SPECIES_SENTRET);  // backup for switching
    OPPONENT(SPECIES_GRIMER)
    {
      Ability(ABILITY_STENCH);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_TACKLE);
      SEND_OUT(player, 1);
    }
  }
  SCENE
  {
    ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    ABILITY_POPUP(opponent, ABILITY_STENCH);
  }
}

SINGLE_BATTLE_TEST("Stench does not activate if move fails due to type immunity")
{
  GIVEN
  {
    PLAYER(SPECIES_GASTLY)
    {
      Moves(MOVE_TACKLE);
    }
    PLAYER(SPECIES_SENTRET);
    OPPONENT(SPECIES_GRIMER)
    {
      Ability(ABILITY_STENCH);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_TACKLE);
    }
  }
  SCENE
  {
    MESSAGE("It doesn't affect Grimer...");
    NOT ABILITY_POPUP(opponent, ABILITY_STENCH);
  }
}

SINGLE_BATTLE_TEST("Stench does not force switch if attacker has no backup")
{
  GIVEN
  {
    PLAYER(SPECIES_RATTATA)
    {
      Moves(MOVE_TACKLE);
    }  // Only one mon
    OPPONENT(SPECIES_GRIMER)
    {
      Ability(ABILITY_STENCH);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_TACKLE);
    }
  }
  SCENE
  {
    ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    NOT ABILITY_POPUP(opponent, ABILITY_STENCH);
  }
}

SINGLE_BATTLE_TEST("Stench activates only once on multi-hit contact move")
{
  GIVEN
  {
    PLAYER(SPECIES_RATTATA)
    {
      Moves(MOVE_DOUBLE_SLAP);
    }
    PLAYER(SPECIES_SENTRET);
    OPPONENT(SPECIES_GRIMER)
    {
      Ability(ABILITY_STENCH);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_DOUBLE_SLAP);
      SEND_OUT(player, 1);
    }
  }
  SCENE
  {
    ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_SLAP, player);
    ABILITY_POPUP(opponent, ABILITY_STENCH);
  }
}
