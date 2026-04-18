#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BLAZE_KICK].kickingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].kickingMove);
}

SINGLE_BATTLE_TEST("Breakdance lowers Attack with kicking moves when moving before the target")
{
    GIVEN {
        PLAYER(SPECIES_HITMONTOP) { Speed(100); Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_BREAKDANCE); Moves(MOVE_BLAZE_KICK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BLAZE_KICK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Breakdance does not lower Attack if the target moved first")
{
    GIVEN {
        PLAYER(SPECIES_HITMONTOP) { Speed(50); Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_BREAKDANCE); Moves(MOVE_BLAZE_KICK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BLAZE_KICK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
