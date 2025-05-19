#include "global.h"
#include "test/battle.h"

// Test: Run Away sharply raises opponent's Speed after Intimidate
DOUBLE_BATTLE_TEST("Run Away sharply raises opponent's Speed after Intimidate")
{
  u32 abilityLeft, abilityRight;

  PARAMETRIZE
  {
    abilityLeft = ABILITY_VITAL_SPIRIT;
    abilityRight = ABILITY_VITAL_SPIRIT;
  }
  PARAMETRIZE
  {
    abilityLeft = ABILITY_VITAL_SPIRIT;
    abilityRight = ABILITY_RUN_AWAY;
  }
  PARAMETRIZE
  {
    abilityLeft = ABILITY_RUN_AWAY;
    abilityRight = ABILITY_VITAL_SPIRIT;
  }
  PARAMETRIZE
  {
    abilityLeft = ABILITY_RUN_AWAY;
    abilityRight = ABILITY_RUN_AWAY;
  }

  GIVEN
  {
    OPPONENT(SPECIES_RATTATA)
    {
      Ability(abilityLeft);
    }
    OPPONENT(SPECIES_RATICATE)
    {
      Ability(abilityRight);
    }
    PLAYER(SPECIES_GYARADOS)
    {
      Ability(ABILITY_INTIMIDATE);
    }
    PLAYER(SPECIES_ARBOK)
    {
      Ability(ABILITY_INTIMIDATE);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(opponentLeft, MOVE_TACKLE, target : playerLeft);
      MOVE(opponentRight, MOVE_TACKLE, target : playerRight);
    }
  }
  SCENE
  {
    // 1st mon Intimidate
    ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
    MESSAGE("Gyarados's Intimidate cuts Foe Rattata's attack!");
    if (abilityLeft == ABILITY_RUN_AWAY)
    {
      ABILITY_POPUP(opponentLeft, ABILITY_RUN_AWAY);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
      MESSAGE("Foe Rattata's Speed sharply rose!");
    }
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
    MESSAGE("Gyarados's Intimidate cuts Foe Raticate's attack!");
    if (abilityRight == ABILITY_RUN_AWAY)
    {
      ABILITY_POPUP(opponentRight, ABILITY_RUN_AWAY);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
      MESSAGE("Foe Raticate's Speed sharply rose!");
    }

    // 2nd mon Intimidate
    ABILITY_POPUP(playerRight, ABILITY_INTIMIDATE);
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
    MESSAGE("Arbok's Intimidate cuts Foe Rattata's attack!");
    if (abilityLeft == ABILITY_RUN_AWAY)
    {
      ABILITY_POPUP(opponentLeft, ABILITY_RUN_AWAY);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
      MESSAGE("Foe Rattata's Speed sharply rose!");
    }
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
    MESSAGE("Arbok's Intimidate cuts Foe Raticate's attack!");
    if (abilityRight == ABILITY_RUN_AWAY)
    {
      ABILITY_POPUP(opponentRight, ABILITY_RUN_AWAY);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
      MESSAGE("Foe Raticate's Speed sharply rose!");
    }
  }
  FINALLY
  {
    // Intimidate lowers Attack by 1 stage, Run Away increases Speed by 2 stages
    EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    EXPECT_EQ(opponentLeft->statStages[STAT_SPEED],
              (abilityLeft == ABILITY_RUN_AWAY) ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE);
    EXPECT_EQ(opponentRight->statStages[STAT_SPEED],
              (abilityRight == ABILITY_RUN_AWAY) ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE);
  }
}

#include "global.h"
#include "test/battle.h"

// Test: Run Away sharply raises opponent's Speed after Intimidate
DOUBLE_BATTLE_TEST("Run Away sharply raises opponent's Speed after Intimidate")
{
  u32 abilityLeft, abilityRight;

  PARAMETRIZE
  {
    abilityLeft = ABILITY_VITAL_SPIRIT;
    abilityRight = ABILITY_VITAL_SPIRIT;
  }
  PARAMETRIZE
  {
    abilityLeft = ABILITY_VITAL_SPIRIT;
    abilityRight = ABILITY_RUN_AWAY;
  }
  PARAMETRIZE
  {
    abilityLeft = ABILITY_RUN_AWAY;
    abilityRight = ABILITY_VITAL_SPIRIT;
  }
  PARAMETRIZE
  {
    abilityLeft = ABILITY_RUN_AWAY;
    abilityRight = ABILITY_RUN_AWAY;
  }

  GIVEN
  {
    OPPONENT(SPECIES_RATTATA)
    {
      Ability(abilityLeft);
    }
    OPPONENT(SPECIES_RATICATE)
    {
      Ability(abilityRight);
    }
    PLAYER(SPECIES_GYARADOS)
    {
      Ability(ABILITY_INTIMIDATE);
    }
    PLAYER(SPECIES_ARBOK)
    {
      Ability(ABILITY_INTIMIDATE);
    }
  }
  WHEN
  {
    TURN
    {
      MOVE(opponentLeft, MOVE_TACKLE, target : playerLeft);
      MOVE(opponentRight, MOVE_TACKLE, target : playerRight);
    }
  }
  SCENE
  {
    // 1st mon Intimidate
    ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
    MESSAGE("Gyarados's Intimidate cuts Foe Rattata's attack!");
    if (abilityLeft == ABILITY_RUN_AWAY)
    {
      ABILITY_POPUP(opponentLeft, ABILITY_RUN_AWAY);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
      MESSAGE("Foe Rattata's Speed sharply rose!");
    }
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
    MESSAGE("Gyarados's Intimidate cuts Foe Raticate's attack!");
    if (abilityRight == ABILITY_RUN_AWAY)
    {
      ABILITY_POPUP(opponentRight, ABILITY_RUN_AWAY);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
      MESSAGE("Foe Raticate's Speed sharply rose!");
    }

    // 2nd mon Intimidate
    ABILITY_POPUP(playerRight, ABILITY_INTIMIDATE);
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
    MESSAGE("Arbok's Intimidate cuts Foe Rattata's attack!");
    if (abilityLeft == ABILITY_RUN_AWAY)
    {
      ABILITY_POPUP(opponentLeft, ABILITY_RUN_AWAY);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
      MESSAGE("Foe Rattata's Speed sharply rose!");
    }
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
    MESSAGE("Arbok's Intimidate cuts Foe Raticate's attack!");
    if (abilityRight == ABILITY_RUN_AWAY)
    {
      ABILITY_POPUP(opponentRight, ABILITY_RUN_AWAY);
      ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
      MESSAGE("Foe Raticate's Speed sharply rose!");
    }
  }
  FINALLY
  {
    EXPECT_EQ(opponentLeft->statStages[STAT_SPEED],
              (abilityLeft == ABILITY_RUN_AWAY) ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE);
    EXPECT_EQ(opponentRight->statStages[STAT_SPEED],
              (abilityRight == ABILITY_RUN_AWAY) ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE);
  }
}

#include "global.h"
#include "test/battle.h"

// Test: Run Away activates for opponent after Sticky Web lowers Speed
SINGLE_BATTLE_TEST("Run Away activates for opponent after Sticky Web lowers Speed")
{
  GIVEN
  {
    OPPONENT(SPECIES_WOBBUFFET);
    OPPONENT(SPECIES_RATTATA)
    {
      Ability(ABILITY_RUN_AWAY);
    }
    PLAYER(SPECIES_GALVANTULA);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_STICKY_WEB);  // Player sets up Sticky Web
    }
    TURN
    {
      SWITCH(opponent, 1);  // Opponent switches in Rattata
    }
  }
  SCENE
  {
    ABILITY_POPUP(opponent, ABILITY_RUN_AWAY);
    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    MESSAGE("Foe Rattata's Speed sharply rose!");
  }
}
