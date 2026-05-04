#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Suction Cups traps foes for 2 or 3 turns on contact")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].makesContact == TRUE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CRADILY) { Ability(ABILITY_SUCTION_CUPS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SUCTION_CUPS);
        MESSAGE("Wobbuffet was WRAPPED by Foe Cradily!");
    } THEN {
        u32 playerBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        u32 opponentBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        EXPECT_NE(player->status2 & STATUS2_WRAPPED, 0);
        EXPECT_EQ(gBattleStruct->wrappedMove[playerBattler], MOVE_WRAP);
        EXPECT_EQ(gBattleStruct->wrappedBy[playerBattler], opponentBattler);
        EXPECT(gDisableStructs[playerBattler].wrapTurns == 2 || gDisableStructs[playerBattler].wrapTurns == 3);
    }
}

SINGLE_BATTLE_TEST("Suction Cups does not trap on non-contact moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWIFT].makesContact == FALSE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_CRADILY) { Ability(ABILITY_SUCTION_CUPS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->status2 & STATUS2_WRAPPED, 0);
    }
}
