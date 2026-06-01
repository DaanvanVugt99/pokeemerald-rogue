#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Supersweet Syrup activates every time the user switches in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DIPPLIN) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 0); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SUPERSWEET_SYRUP);
        ABILITY_POPUP(opponent, ABILITY_SUPERSWEET_SYRUP);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 2);
    }
}
