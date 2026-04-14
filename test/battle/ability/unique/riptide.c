#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
}

SINGLE_BATTLE_TEST("Riptide lowers Defense when a slicing move deals 50% or less")
{
    GIVEN {
        PLAYER(SPECIES_KABUTOPS) { Level(1); Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_RIPTIDE); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); HP(999); MaxHP(999); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Riptide does not lower Defense when slicing damage is above 50%")
{
    GIVEN {
        PLAYER(SPECIES_KABUTOPS) { Level(100); Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_RIPTIDE); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(1); Ability(ABILITY_STURDY); HP(100); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
