#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TAILWIND].effect == EFFECT_TAILWIND);
}

SINGLE_BATTLE_TEST("Strong Winds sets Tailwind on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_PIDGEY);
    } WHEN {
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].tailwindTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Strong Winds coexists with a primary switch-in ability")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_STRONG_WINDS); }
    } WHEN {
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND);
    }
}
