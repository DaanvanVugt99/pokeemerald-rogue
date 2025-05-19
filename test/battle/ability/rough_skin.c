#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Rough Skin inflicts damage on contact moves", s16 damage)
{
  u32 move;
  u16 ability;

  // Moves with different types to test Rough Skin damage calculation
  PARAMETRIZE
  {
    move = MOVE_TACKLE;
    ability = ABILITY_ROUGH_SKIN;
  }
  PARAMETRIZE
  {
    move = MOVE_KARATE_CHOP;
    ability = ABILITY_ROUGH_SKIN;
  }
  PARAMETRIZE
  {
    move = MOVE_BUG_BITE;
    ability = ABILITY_ROUGH_SKIN;
  }
  PARAMETRIZE
  {
    move = MOVE_WATERFALL;
    ability = ABILITY_ROUGH_SKIN;
  }
  PARAMETRIZE
  {
    move = MOVE_SWIFT;
    ability = ABILITY_ROUGH_SKIN;
  }
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_GARCHOMP)
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
      ABILITY_POPUP(opponent, ABILITY_ROUGH_SKIN);
      HP_BAR(player, captureDamage : &results[i].damage);
    }
  }
  FINALLY
  {
    FINALLY
    {
      EXPECT_EQ(results[0].damage, results[1].damage);  // Tackle and Karate Chop (both 1/6)
      EXPECT_EQ(results[0].damage, results[2].damage);  // Tackle and Leech Life (both 1/6)
      EXPECT_LT(results[3].damage, results[0].damage);  // Waterfall does less than Tackle
      EXPECT_EQ(results[4].damage, 0);  // Swift does no damage from Rough Skin
    }
  }
}
