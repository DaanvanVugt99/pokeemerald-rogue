#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

AI_SINGLE_BATTLE_TEST("Hidden Stash records a coin for each Trainer Pokemon knocked out")
{
    GIVEN {
        PLAYER(SPECIES_GHOLDENGO) { Level(100); Attack(255); Ability(ABILITY_GOOD_AS_GOLD); UniqueAbility(ABILITY_HIDDEN_STASH); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); MaxHP(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); EXPECT_SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gBattleResults.hiddenStashKOs, 2);
    }
}

WILD_BATTLE_TEST("Hidden Stash does not record wild Pokemon knockouts")
{
    GIVEN {
        PLAYER(SPECIES_GHOLDENGO) { Level(100); Attack(255); Ability(ABILITY_GOOD_AS_GOLD); UniqueAbility(ABILITY_HIDDEN_STASH); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gBattleResults.hiddenStashKOs, 0);
    }
}
