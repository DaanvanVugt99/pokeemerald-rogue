#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Drag Below lowers Speed when this Pokemon traps a target")
{
    GIVEN {
        PLAYER(SPECIES_GRAPPLOCT) { Speed(100); Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_DRAG_BELOW); Moves(MOVE_OCTOLOCK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_OCTOLOCK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DRAG_BELOW);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Drag Below does not trigger if trapping fails")
{
    GIVEN {
        PLAYER(SPECIES_GRAPPLOCT) { Speed(100); Ability(ABILITY_LIMBER); UniqueAbility(ABILITY_DRAG_BELOW); Moves(MOVE_OCTOLOCK); }
        OPPONENT(SPECIES_GASTLY) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_OCTOLOCK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_DRAG_BELOW);
    } THEN {
        EXPECT_EQ(opponent->status2 & STATUS2_ESCAPE_PREVENTION, 0);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
