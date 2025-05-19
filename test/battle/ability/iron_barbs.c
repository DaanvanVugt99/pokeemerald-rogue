#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Iron Barbs inflicts damage on contact moves", s16 damage)
{
  u32 move;
  u16 ability;

  // Moves with different types to test Iron Barbs damage calculation
  PARAMETRIZE
  {
    move = MOVE_BULLET_PUNCH;
    ability = ABILITY_IRON_BARBS;
  }
  PARAMETRIZE
  {
    move = MOVE_ACCELEROCK;  // Steel-type contact move
    ability = ABILITY_IRON_BARBS;
  }
  PARAMETRIZE
  {
    move = MOVE_DRILL_RUN;
    ability = ABILITY_IRON_BARBS;
  }
  PARAMETRIZE
  {
    move = MOVE_TACKLE;
    ability = ABILITY_IRON_BARBS;
  }
  PARAMETRIZE
  {
    move = MOVE_ROCK_THROW;  // Non-contact move
    ability = ABILITY_IRON_BARBS;
  }
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_FERROTHORN)
    {
      Ability(ability);
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
      ABILITY_POPUP(opponent, ABILITY_IRON_BARBS);
      HP_BAR(player, captureDamage : &results[i].damage);
    }
  }
  FINALLY
  {
    EXPECT_EQ(results[0].damage, results[1].damage);  // Bullet Punch and Accelerock (both 1/8)
    EXPECT_EQ(results[0].damage, results[2].damage);  // Bullet Punch and Accelerock (both 1/8)
    EXPECT_LT(results[3].damage, results[0].damage);  // Tackle should not deal more damage (1/8)
    EXPECT_EQ(results[4].damage, 0);  // Swift does no damage from Iron Barbs
  }
}
