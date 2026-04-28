#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SNAP_TRAP].effect == EFFECT_TRAP);
}

SINGLE_BATTLE_TEST("Trapper uses Snap Trap on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CARNIVINE) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_TRAPPER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRAPPER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNAP_TRAP, player);
    } THEN {
        EXPECT_NE(opponent->status2 & STATUS2_WRAPPED, 0);
    }
}

SINGLE_BATTLE_TEST("Trapper does not use Snap Trap if the user cannot use an extra move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CARNIVINE) { Status1(STATUS1_SLEEP); Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_TRAPPER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->status2 & STATUS2_WRAPPED, 0);
    }
}
