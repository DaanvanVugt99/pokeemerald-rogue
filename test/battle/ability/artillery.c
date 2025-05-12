#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Artillery makes launcher-based moves hit both enemies")
{
    GIVEN
    {
        PLAYER(SPECIES_BLASTOISE) { Ability(ABILITY_ARTILLERY); }
        PLAYER(SPECIES_MAGIKARP);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    }
    WHEN
    {
        TURN
        {
            MOVE(playerLeft, MOVE_AURA_SPHERE, target : opponentLeft);
        }
    }
    SCENE
    {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
    }
}
