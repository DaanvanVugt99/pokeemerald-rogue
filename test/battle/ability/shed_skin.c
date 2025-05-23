#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Shed Skin caps damage at half HP")
{
  GIVEN
  {
    PLAYER(SPECIES_EKANS)
    {
      Ability(ABILITY_SHED_SKIN);
      MaxHP(100);
      HP(100);
    }
    OPPONENT(SPECIES_WOBBUFFET);
  }
  WHEN
  {
    TURN
    {
      MOVE(opponent, MOVE_SPORE);
    }
    TURN
    {
      MOVE(opponent, MOVE_SEISMIC_TOSS);
    }
  }
  SCENE
  {
    ANIMATION(ANIM_TYPE_MOVE, MOVE_SEISMIC_TOSS, opponent);
    HP_BAR(player, hp : 50);
    ABILITY_POPUP(player, ABILITY_STURDY);
    MESSAGE("Ekans shed its skin and recovered!");
  }
}
