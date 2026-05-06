#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Shed Skin cures status at the end of turn")
{
    GIVEN {
        PLAYER(SPECIES_EKANS) { Ability(ABILITY_SHED_SKIN); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHED_SKIN);
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}
