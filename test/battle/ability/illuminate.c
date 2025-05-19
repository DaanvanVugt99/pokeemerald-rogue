#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Illuminate (opponent) lowers player's accuracy after switch in", s16 damage)
{
  u32 ability;
  PARAMETRIZE
  {
    ability = ABILITY_ILLUMINATE;
  }
  PARAMETRIZE
  {
    ability = ABILITY_SHED_SKIN;
  }
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_LANTURN)
    {
      Ability(ability);
    }
  }
  WHEN
  {
    TURN
    {
      SWITCH(opponent, 1);
    }
    TURN
    {
      MOVE(player, MOVE_TACKLE);
    }
  }
  SCENE
  {
    if (ability == ABILITY_ILLUMINATE)
    {
      ABILITY_POPUP(opponent, ABILITY_ILLUMINATE);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
      MESSAGE("Foe Lanturn's Illuminate lowers Wobbuffet's accuracy!");
    }
  }
}

DOUBLE_BATTLE_TEST("Illuminate doesn't activate on an empty field in a double battle")
{
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    PLAYER(SPECIES_WOBBUFFET)
    {
      HP(1);
    }
    PLAYER(SPECIES_LANTURN)
    {
      Ability(ABILITY_ILLUMINATE);
    }
    PLAYER(SPECIES_ABRA);
    OPPONENT(SPECIES_WOBBUFFET)
    {
      HP(1);
    }
    OPPONENT(SPECIES_WOBBUFFET)
    {
      HP(1);
    }
    OPPONENT(SPECIES_ARBOK);
    OPPONENT(SPECIES_WYNAUT);
  }
  WHEN
  {
    TURN
    {
      MOVE(playerLeft, MOVE_EXPLOSION);
      SEND_OUT(playerLeft, 2);
      SEND_OUT(opponentLeft, 2);
      SEND_OUT(playerRight, 3);
      SEND_OUT(opponentRight, 3);
    }
    TURN
    {
      MOVE(playerLeft, MOVE_CELEBRATE);
    }
  }
  SCENE
  {
    HP_BAR(playerLeft, hp : 0);
    ANIMATION(ANIM_TYPE_MOVE, MOVE_EXPLOSION, playerLeft);

    MESSAGE("Go! Lanturn!");
    MESSAGE("Go! Arbok!");
    MESSAGE("Go! Abra!");
    MESSAGE("Go! Wynaut!");

    NONE_OF
    {
      ABILITY_POPUP(playerLeft, ABILITY_ILLUMINATE);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
      MESSAGE("Lanturn's Illuminate lowers Foe Arbok's accuracy!");
    }
  }
}
