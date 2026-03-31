#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].accuracy == 60);
    ASSUME(gBattleMoves[MOVE_SING].accuracy == 55);
}

SINGLE_BATTLE_TEST("Spiral Gaze makes Hypnosis perfectly accurate on first turn")
{
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_SPIRAL_GAZE); Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        NONE_OF { MESSAGE("Poliwag's attack missed!"); }
    }
}

SINGLE_BATTLE_TEST("Spiral Gaze only affects Hypnosis on first turn")
{
    u16 move;
    u32 expectedAccuracy;
    PARAMETRIZE { move = MOVE_HYPNOSIS; expectedAccuracy = 100; }
    PARAMETRIZE { move = MOVE_SING; expectedAccuracy = gBattleMoves[MOVE_SING].accuracy; }
    PASSES_RANDOMLY(expectedAccuracy, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_SPIRAL_GAZE); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
}

SINGLE_BATTLE_TEST("Spiral Gaze does not boost Hypnosis after first turn")
{
    PASSES_RANDOMLY(gBattleMoves[MOVE_HYPNOSIS].accuracy, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_SPIRAL_GAZE); Moves(MOVE_CELEBRATE, MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYPNOSIS); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
    }
}
