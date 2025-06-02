#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
  ASSUME(gBattleMoves[MOVE_TACKLE].flags & FLAG_MAKES_CONTACT);
  ASSUME(gBattleMoves[MOVE_BUBBLE].type == TYPE_WATER);
  ASSUME(gBattleMoves[MOVE_TACKLE].flags & FLAG_MAKES_CONTACT);
  ASSUME(gBattleMoves[MOVE_WATERFALL].flags & FLAG_MAKES_CONTACT);
  ASSUME(gBattleMoves[MOVE_WATERFALL].type == TYPE_WATER);
}

SINGLE_BATTLE_TEST("Liquified halves damage taken from moves that make direct contact", s16 damage)
{
  u32 ability;
  PARAMETRIZE
  {
    ability = ABILITY_STICKY_HOLD;
  }
  PARAMETRIZE
  {
    ability = ABILITY_LIQUIFIED;
  }
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_MUK)
    {
      Ability(ability);
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
    MESSAGE("Wobbuffet used Tackle!");
    HP_BAR(opponent, captureDamage : &results[i].damage);
  }
  FINALLY
  {
    EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
  }
}

SINGLE_BATTLE_TEST("Liquified doubles damage taken from water type moves", s16 damage)
{
  u32 ability;
  PARAMETRIZE
  {
    ability = ABILITY_STICKY_HOLD;
  }
  PARAMETRIZE
  {
    ability = ABILITY_LIQUIFIED;
  }
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_MUK)
    {
      Ability(ability);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_BUBBLE);
    }
  }
  SCENE
  {
    MESSAGE("Wobbuffet used Bubble!");
    HP_BAR(opponent, captureDamage : &results[i].damage);
  }
  FINALLY
  {
    EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
  }
}

SINGLE_BATTLE_TEST("Liquified does not alter damage of water-type moves that make direct contact", s16 damage)
{
  u32 ability;
  PARAMETRIZE
  {
    ability = ABILITY_STICKY_HOLD;
  }
  PARAMETRIZE
  {
    ability = ABILITY_LIQUIFIED;
  }
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_MUK)
    {
      Ability(ability);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_WATERFALL);
    }
  }
  SCENE
  {
    MESSAGE("Wobbuffet used Waterfall!");
    HP_BAR(opponent, captureDamage : &results[i].damage);
  }
  FINALLY
  {
    EXPECT_EQ(results[0].damage, results[1].damage);
  }
}
