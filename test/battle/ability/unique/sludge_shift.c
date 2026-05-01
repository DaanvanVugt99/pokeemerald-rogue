#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sludge Shift uses Psycho Shift at end of turn if the user is statused")
{
    GIVEN {
        PLAYER(SPECIES_GOODRA) { Speed(50); Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_SLUDGE_SHIFT); Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Sludge Shift does not trigger if the user has no status condition")
{
    GIVEN {
        PLAYER(SPECIES_GOODRA) { Speed(50); Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_SLUDGE_SHIFT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
