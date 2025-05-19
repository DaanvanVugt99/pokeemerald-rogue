#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Levitate gives STAB to Flying-type moves", s16 damage)
{
  u32 move;
  u16 ability;

  ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);  // Ensure move is Electric-type
  PARAMETRIZE
  {
    move = MOVE_AERIAL_ACE;
    ability = ABILITY_LEVITATE;
  }
  PARAMETRIZE
  {
    move = MOVE_AERIAL_ACE;
    ability = ABILITY_HEAVY_METAL;
  }
  GIVEN
  {
    PLAYER(SPECIES_BRONZONG)
    {
      Ability(ability);
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
    HP_BAR(opponent, captureDamage : &results[i].damage);
  }
  FINALLY
  {
    EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);  // Levitate grants STAB
  }
}
