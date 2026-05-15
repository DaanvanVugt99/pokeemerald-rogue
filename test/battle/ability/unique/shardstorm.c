#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_ROCK_TOMB].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_ROCK_TOMB].accuracy < 100);
}

SINGLE_BATTLE_TEST("Shardstorm uses 40 BP Rock Tomb after a resisted slicing move")
{
    s16 rockTombHit;

    PASSES_RANDOMLY(gBattleMoves[MOVE_ROCK_TOMB].accuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_KLEAVOR) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_VENUSAUR) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SHARDSTORM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_TOMB, player);
        HP_BAR(opponent, captureDamage: &rockTombHit);
    } THEN {
        EXPECT_EQ(gCalledMove, MOVE_ROCK_TOMB);
        EXPECT_GT(rockTombHit, 0);
    }
}
