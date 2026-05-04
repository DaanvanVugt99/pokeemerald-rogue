#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Starlock disables the target's last used move for 2 turns after Recover")
{
    GIVEN {
        PLAYER(SPECIES_STARMIE) { Speed(100); Ability(ABILITY_NATURAL_CURE); UniqueAbility(ABILITY_STARLOCK); Moves(MOVE_RECOVER, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_TACKLE);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 2);
    }
}

SINGLE_BATTLE_TEST("Starlock fails if the target has no last used move")
{
    GIVEN {
        PLAYER(SPECIES_STARMIE) { HP(50); MaxHP(100); Speed(100); Ability(ABILITY_NATURAL_CURE); UniqueAbility(ABILITY_STARLOCK); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_NONE);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 0);
    }
}
