#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ICE_SHARD].type == TYPE_ICE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ICE);
}

SINGLE_BATTLE_TEST("Absolute Zero freezes a boosted target on the first landed Ice move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_KYUREM) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_ABSOLUTE_ZERO); Speed(100); Moves(MOVE_ICE_SHARD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(200); Speed(50); Moves(MOVE_DEFENSE_CURL, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ICE_SHARD); MOVE(opponent, MOVE_DEFENSE_CURL); }
        TURN { MOVE(player, MOVE_ICE_SHARD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ABSOLUTE_ZERO);
        STATUS_ICON(opponent, freeze: TRUE);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Absolute Zero only triggers on Ice moves")
{
    GIVEN {
        PLAYER(SPECIES_KYUREM) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_ABSOLUTE_ZERO); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DEFENSE_CURL); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_DEFENSE_CURL); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ABSOLUTE_ZERO);
            STATUS_ICON(opponent, freeze: TRUE);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
