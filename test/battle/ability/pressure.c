#include "global.h"
#include "test/battle.h"

// Test 1: Pressure reduces 2 PP from the used move and 1 PP from all other moves
SINGLE_BATTLE_TEST("Pressure reduces 2 PP from the used move and 1 PP from all others when opponent uses a move")
{
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET)
    {
      MovesWithPP({MOVE_POUND, 35}, {MOVE_SPLASH, 40}, {MOVE_TACKLE, 35});
    }
    OPPONENT(SPECIES_WOBBUFFET)
    {
      Ability(ABILITY_PRESSURE);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_POUND);
    }
  }
  THEN
  {
    EXPECT_EQ(player->pp[0], 33);  // Pound (2 PP deducted)
    EXPECT_EQ(player->pp[1], 39);  // Splash (1 PP deducted)
    EXPECT_EQ(player->pp[2], 34);  // Tackle (1 PP deducted)
  }
}

// Test 2: Pressure stacks with multiple Pokémon
DOUBLE_BATTLE_TEST("Pressure reduces 2 PP from the used move and 1 PP from all other moves per Pressure user")
{
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET)
    {
      MovesWithPP({MOVE_SWIFT, 20}, {MOVE_SPLASH, 40});
    }
    PLAYER(SPECIES_WYNAUT);
    OPPONENT(SPECIES_WOBBUFFET)
    {
      Ability(ABILITY_PRESSURE);
    }
    OPPONENT(SPECIES_WYNAUT)
    {
      Ability(ABILITY_PRESSURE);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(playerLeft, MOVE_SWIFT);
    }
  }
  THEN
  {
    EXPECT_EQ(playerLeft->pp[0], 16);  // Swift (4 PP deducted)
    EXPECT_EQ(playerLeft->pp[1], 38);  // Splash (2 PP deducted)
  }
}

// Test 3: Pressure reduces PP for Imprison and Snatch
SINGLE_BATTLE_TEST("Pressure reduces 2 PP from the used move and 1 PP from all others including Imprison and Snatch")
{
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET)
    {
      MovesWithPP({MOVE_IMPRISON, 10}, {MOVE_SNATCH, 10}, {MOVE_TACKLE, 35});
    }
    OPPONENT(SPECIES_WOBBUFFET)
    {
      Ability(ABILITY_PRESSURE);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_IMPRISON);
    }
    TURN
    {
      MOVE(player, MOVE_SNATCH);
    }
  }
  THEN
  {
    EXPECT_EQ(player->pp[0], 8);  // Imprison (2 PP deducted)
    EXPECT_EQ(player->pp[1], 8);  // Snatch (2 PP deducted)
    EXPECT_EQ(player->pp[2], 33);  // Tackle (1 PP deducted per move)
  }
}

// Test 4: Pressure reduces PP for entry hazard moves
SINGLE_BATTLE_TEST(
    "Pressure reduces 2 PP from the used move and 1 PP from all others including Spikes, Stealth Rock, and Toxic "
    "Spikes")
{
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET)
    {
      MovesWithPP({MOVE_SPIKES, 20}, {MOVE_STEALTH_ROCK, 20}, {MOVE_TOXIC_SPIKES, 20});
    }
    OPPONENT(SPECIES_WOBBUFFET)
    {
      Ability(ABILITY_PRESSURE);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_SPIKES);
    }
    TURN
    {
      MOVE(player, MOVE_STEALTH_ROCK);
    }
    TURN
    {
      MOVE(player, MOVE_TOXIC_SPIKES);
    }
  }
  THEN
  {
    EXPECT_EQ(player->pp[0], 17);  // Spikes
    EXPECT_EQ(player->pp[1], 17);  // Stealth Rock
    EXPECT_EQ(player->pp[2], 17);  // Toxic Spikes
  }
}

// Test 5: Pressure does not affect non-attacking moves like Sticky Web
SINGLE_BATTLE_TEST("Pressure does not reduce PP from non-attacking moves like Sticky Web")
{
  GIVEN
  {
    PLAYER(SPECIES_WOBBUFFET)
    {
      MovesWithPP({MOVE_STICKY_WEB, 20}, {MOVE_TACKLE, 35});
    }
    OPPONENT(SPECIES_WOBBUFFET)
    {
      Ability(ABILITY_PRESSURE);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_STICKY_WEB);
    }
  }
  THEN
  {
    EXPECT_EQ(player->pp[0], 19);  // Sticky Web (no additional reduction)
    EXPECT_EQ(player->pp[1], 34);  // Tackle
  }
}
