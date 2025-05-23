
#include "global.h"
#include "test/battle.h"

// TODO:  FIX TESTS WTF

DOUBLE_BATTLE_TEST("Plus heals when partnered with Plus at end of turn")
{
  GIVEN
  {
    PLAYER(SPECIES_PLUSLE)
    {
      Ability(ABILITY_PLUS);
      HP(80);
    }
    PLAYER(SPECIES_PLUSLE)
    {
      Ability(ABILITY_PLUS);
    }
    OPPONENT(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN;
  }
  SCENE
  {
    ABILITY_POPUP(playerLeft, ABILITY_PLUS);
  }
}

DOUBLE_BATTLE_TEST("Plus gives priority to status moves with Minus on field")
{
  GIVEN
  {
    PLAYER(SPECIES_PLUSLE)
    {
      Ability(ABILITY_PLUS);
      Speed(10);
    }
    PLAYER(SPECIES_MINUN)
    {
      Ability(ABILITY_MINUS);
      Speed(30);
    }
    OPPONENT(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(playerLeft, MOVE_GROWL);
      MOVE(playerRight, MOVE_CELEBRATE);
    }
  }
  SCENE
  {
    MESSAGE("Plusle used Growl!");
    MESSAGE("Minun used Celebrate!");
  }
}

DOUBLE_BATTLE_TEST("Minus ignores defensive stat boosts when partnered with Minus", s16 damage)
{
  u32 move;
  PARAMETRIZE
  {
    move = MOVE_IRON_DEFENSE;
  }
  PARAMETRIZE
  {
    move = MOVE_NONE;
  }

  GIVEN
  {
    PLAYER(SPECIES_MINUN)
    {
      Ability(ABILITY_MINUS);
    }
    PLAYER(SPECIES_MINUN)
    {
      Ability(ABILITY_MINUS);
    }
    OPPONENT(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_WOBBUFFET);
  }

  WHEN
  {
    TURN
    {
      MOVE(playerLeft, MOVE_TACKLE, target : opponentLeft);
    }
  }

  SCENE
  {
    if (move == MOVE_IRON_DEFENSE)
    {
      MESSAGE("Foe Wobbuffet's Defense rose sharply!");
    }
    MESSAGE("Minun used Tackle!");
    HP_BAR(opponentLeft, captureDamage : &results[i].damage);
  }

  FINALLY
  {
    EXPECT_EQ(results[1].damage, results[0].damage);
  }
}
