#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHOCK_WAVE].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Black Thunder paralyzes a boosted target on the first landed Electric move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ZEKROM) { Ability(ABILITY_TERAVOLT); UniqueAbility(ABILITY_BLACK_THUNDER); Speed(100); Moves(MOVE_SHOCK_WAVE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(200); Speed(50); Moves(MOVE_DEFENSE_CURL, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE); MOVE(opponent, MOVE_DEFENSE_CURL); }
        TURN { MOVE(player, MOVE_SHOCK_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SHOCK_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BLACK_THUNDER);
        STATUS_ICON(opponent, paralysis: TRUE);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BLACK_THUNDER);
        }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_PARALYSIS);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Black Thunder only triggers on Electric moves")
{
    GIVEN {
        PLAYER(SPECIES_ZEKROM) { Ability(ABILITY_TERAVOLT); UniqueAbility(ABILITY_BLACK_THUNDER); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DEFENSE_CURL); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_DEFENSE_CURL); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BLACK_THUNDER);
            STATUS_ICON(opponent, paralysis: TRUE);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
