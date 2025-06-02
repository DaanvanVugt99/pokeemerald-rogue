#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Chlorofumes lowers attack with Grass-type moves")
{
  u16 ability;

  ASSUME(gBattleMoves[MOVE_VINE_WHIP].type == TYPE_GRASS);
  PARAMETRIZE
  {
    ability = ABILITY_CHLOROFUMES;
  }
  PARAMETRIZE
  {
    ability = ABILITY_THICK_FAT;
  }
  GIVEN
  {
    PLAYER(SPECIES_VENUSAUR)
    {
      Ability(ability);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(player, MOVE_VINE_WHIP);
    }
  }
}

SINGLE_BATTLE_TEST("Chlorofumes lowers attack and poisons with Grass-type moves in sun")
{
  u16 ability;

  ASSUME(gBattleMoves[MOVE_VINE_WHIP].type == TYPE_GRASS);
  PARAMETRIZE
  {
    ability = ABILITY_CHLOROFUMES;
  }
  PARAMETRIZE
  {
    ability = ABILITY_THICK_FAT;
  }
  GIVEN
  {
    PLAYER(SPECIES_VENUSAUR)
    {
      Ability(ability);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(opponent, MOVE_SUNNY_DAY);
      MOVE(player, MOVE_VINE_WHIP);
    }
  }
}
