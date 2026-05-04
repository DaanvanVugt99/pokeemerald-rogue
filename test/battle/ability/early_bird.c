#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Early Bird raises Speed by 1 when the Pokemon wakes up")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_EARLY_BIRD); Status1(STATUS1_SLEEP_TURN(1)); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet woke up!");
        STATUS_ICON(player, none: TRUE);
        ABILITY_POPUP(player, ABILITY_EARLY_BIRD);
        MESSAGE("Wobbuffet's Speed rose!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}
