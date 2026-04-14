#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ROCK);
}

SINGLE_BATTLE_TEST("Abyssal lowers Sp. Def by 1 when using a Rock-type move")
{
    u16 move;
    bool8 shouldDrop;
    PARAMETRIZE { move = MOVE_ROCK_THROW; shouldDrop = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; shouldDrop = FALSE; }

    PASSES_RANDOMLY(gBattleMoves[move].accuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_OMASTAR) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_ABYSSAL); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - (shouldDrop ? 1 : 0));
    }
}
