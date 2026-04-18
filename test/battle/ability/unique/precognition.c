#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
}

SINGLE_BATTLE_TEST("Precognition uses Future Sight after an opposing status move")
{
    GIVEN {
        PLAYER(SPECIES_ESPEON) { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ABILITY_PRECOGNITION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, opponent);
    } THEN {
        EXPECT_EQ(gWishFutureKnock.futureSightMove[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)], MOVE_FUTURE_SIGHT);
        EXPECT_EQ(gWishFutureKnock.futureSightAttacker[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)], GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
    }
}

SINGLE_BATTLE_TEST("Precognition does not trigger after an opposing damaging move")
{
    GIVEN {
        PLAYER(SPECIES_ESPEON) { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ABILITY_PRECOGNITION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PRECOGNITION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FUTURE_SIGHT, player);
        }
    } THEN {
        EXPECT_EQ(gWishFutureKnock.futureSightMove[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)], MOVE_NONE);
    }
}
