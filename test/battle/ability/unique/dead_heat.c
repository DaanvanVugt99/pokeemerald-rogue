#include "global.h"
#include "battle_main.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Dead Heat matches the fastest unmodified Speed on the field")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); UniqueAbility(ABILITY_DEAD_HEAT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetBattlerTotalSpeedStat(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)),
                  GetBattlerTotalSpeedStat(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)));
    }
}

SINGLE_BATTLE_TEST("Dead Heat applies the user's Speed boosts after matching")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); UniqueAbility(ABILITY_DEAD_HEAT); Moves(MOVE_AGILITY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_GT(GetBattlerTotalSpeedStat(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)),
                  GetBattlerTotalSpeedStat(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)));
    }
}

SINGLE_BATTLE_TEST("Dead Heat does not copy Choice Scarf's modifier")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); UniqueAbility(ABILITY_DEAD_HEAT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Item(ITEM_CHOICE_SCARF); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_LT(GetBattlerTotalSpeedStat(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)),
                  GetBattlerTotalSpeedStat(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)));
    }
}

DOUBLE_BATTLE_TEST("Dead Heat can match the raw Speed of its partner")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); UniqueAbility(ABILITY_DEAD_HEAT); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetBattlerTotalSpeedStat(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)),
                  GetBattlerTotalSpeedStat(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)));
    }
}
