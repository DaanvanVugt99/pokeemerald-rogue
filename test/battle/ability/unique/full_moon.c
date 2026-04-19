#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_POUND].accuracy == 100);
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Full Moon gives 20 percent evasion")
{
    PASSES_RANDOMLY(1, 5, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_MIGHTYENA) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_FULL_MOON); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_POUND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_POUND); }
    } SCENE {
        NONE_OF {
            HP_BAR(player);
        }
    }
}

SINGLE_BATTLE_TEST("Full Moon lowers the target's Speed by 1 when attacking in Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_MIGHTYENA) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_FULL_MOON); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ECLIPSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_ECLIPSE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Full Moon does not lower Speed outside Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_MIGHTYENA) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_FULL_MOON); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
