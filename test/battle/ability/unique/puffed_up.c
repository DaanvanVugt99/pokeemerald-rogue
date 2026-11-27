#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Puffed Up uses Hone Claws once when both opponents flinch")
{
    GIVEN {
        PLAYER(SPECIES_PURUGLY) { UniqueAbility(ABILITY_PUFFED_UP); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentRight);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_PUFFED_UP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HONE_CLAWS, playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerLeft->statStages[STAT_ACC], DEFAULT_STAT_STAGE + 1);
    }
}
