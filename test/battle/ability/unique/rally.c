#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
}

SINGLE_BATTLE_TEST("Rally heals and cures status the first time it drops below half HP each battle")
{
    GIVEN {
        PLAYER(SPECIES_PAWMOT) { HP(100); MaxHP(160); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RALLY);
        MESSAGE("Pawmot's status returned to normal!");
    } THEN {
        EXPECT_EQ(player->hp, 60);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}
