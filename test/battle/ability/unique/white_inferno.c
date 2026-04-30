#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FIRE_PLEDGE].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
}

SINGLE_BATTLE_TEST("White Inferno burns a boosted target on the first landed Fire move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_RESHIRAM) { Ability(ABILITY_TURBOBLAZE); UniqueAbility(ABILITY_WHITE_INFERNO); Speed(100); Moves(MOVE_FIRE_PLEDGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(200); Speed(50); Moves(MOVE_DEFENSE_CURL, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_PLEDGE); MOVE(opponent, MOVE_DEFENSE_CURL); }
        TURN { MOVE(player, MOVE_FIRE_PLEDGE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_FIRE_PLEDGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WHITE_INFERNO);
        STATUS_ICON(opponent, burn: TRUE);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WHITE_INFERNO);
        }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BURN);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("White Inferno only triggers on Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_RESHIRAM) { Ability(ABILITY_TURBOBLAZE); UniqueAbility(ABILITY_WHITE_INFERNO); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DEFENSE_CURL); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_DEFENSE_CURL); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WHITE_INFERNO);
            STATUS_ICON(opponent, burn: TRUE);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
