#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STRING_SHOT].effect == EFFECT_SPEED_DOWN_2 || gBattleMoves[MOVE_STRING_SHOT].effect == EFFECT_SPEED_DOWN);
}

SINGLE_BATTLE_TEST("Silken Thread uses String Shot on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WURMPLE) { Ability(ABILITY_RUN_AWAY); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SILKEN_THREAD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRING_SHOT, opponent);
    } THEN {
        EXPECT_LT(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
