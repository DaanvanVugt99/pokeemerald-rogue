#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_INFESTED_TERRAIN].effect == EFFECT_INFESTED_TERRAIN);
}

SINGLE_BATTLE_TEST("Burrowing Horns lowers the target's Speed by 1 after contact moves")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_BURROWING_HORNS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Burrowing Horns does not lower Speed after non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_BURROWING_HORNS); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Burrowing Horns traps targets after contact moves in Infested Terrain")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Speed(50); Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_BURROWING_HORNS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_INFESTED_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_INFESTED_TERRAIN); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
    }
}

SINGLE_BATTLE_TEST("Burrowing Horns does not trap targets outside Infested Terrain")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Speed(50); Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_BURROWING_HORNS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(!!(opponent->status2 & STATUS2_ESCAPE_PREVENTION), FALSE);
    }
}
