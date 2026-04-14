#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
}

SINGLE_BATTLE_TEST("Exhumed makes the first slicing move after a Rock move also use Rock Tomb")
{
    GIVEN {
        PLAYER(SPECIES_KABUTOPS) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_EXHUMED); Moves(MOVE_ROCK_THROW, MOVE_TACKLE, MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_TOMB, player);
    }
}

SINGLE_BATTLE_TEST("Exhumed does not trigger if no Rock-type move was used first")
{
    GIVEN {
        PLAYER(SPECIES_KABUTOPS) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_EXHUMED); Moves(MOVE_TACKLE, MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
    }
}

SINGLE_BATTLE_TEST("Exhumed is consumed after one slicing move")
{
    PASSES_RANDOMLY(gBattleMoves[MOVE_ROCK_TOMB].accuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_KABUTOPS) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_EXHUMED); Moves(MOVE_ROCK_THROW, MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_TOMB, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
    }
}
