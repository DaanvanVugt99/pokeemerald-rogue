#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Breakdance clears hazards and lowers the Attack of a foe flinched by an ally")
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; }
    PARAMETRIZE { ability = ABILITY_INNER_FOCUS; }
    GIVEN {
        PLAYER(SPECIES_HITMONTOP) { Ability(ABILITY_TECHNICIAN); UniqueAbility(ABILITY_BREAKDANCE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Speed(20); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); }
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentLeft);
            MOVE(opponentRight, MOVE_SPIKES);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(playerLeft, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - (ability == ABILITY_NONE));
        EXPECT_EQ(!!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES), ability == ABILITY_INNER_FOCUS);
    }
}
