#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Unspeakable lowers attacker's Attack and Sp. Atk when Disguise breaks")
{
    GIVEN {
        PLAYER(SPECIES_MIMIKYU_DISGUISED) { Ability(ABILITY_DISGUISE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ASTONISH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ASTONISH); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Unspeakable only triggers when Disguise breaks")
{
    GIVEN {
        PLAYER(SPECIES_MIMIKYU_DISGUISED) { Ability(ABILITY_DISGUISE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ASTONISH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ASTONISH); }
        TURN { MOVE(opponent, MOVE_ASTONISH); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}
