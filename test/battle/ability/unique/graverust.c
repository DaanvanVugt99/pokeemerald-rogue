#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Graverust lowers opposing Speed at end of turn during Eclipse when the foe is below half HP")
{
    GIVEN {
        PLAYER(SPECIES_DHELMISE) { Speed(100); Ability(ABILITY_STEELWORKER); UniqueAbility(ABILITY_GRAVERUST); Moves(MOVE_ECLIPSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); MaxHP(200); HP(99); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ECLIPSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Graverust does not lower Speed outside Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_DHELMISE) { Ability(ABILITY_STEELWORKER); UniqueAbility(ABILITY_GRAVERUST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(200); HP(99); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Graverust does not lower Speed during Eclipse when the foe is above half HP")
{
    GIVEN {
        PLAYER(SPECIES_DHELMISE) { Speed(100); Ability(ABILITY_STEELWORKER); UniqueAbility(ABILITY_GRAVERUST); Moves(MOVE_ECLIPSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); MaxHP(200); HP(101); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ECLIPSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Graverust checks each opposing battler independently")
{
    GIVEN {
        PLAYER(SPECIES_DHELMISE) { Speed(100); Ability(ABILITY_STEELWORKER); UniqueAbility(ABILITY_GRAVERUST); Moves(MOVE_ECLIPSE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); MaxHP(200); HP(99); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); MaxHP(200); HP(101); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ECLIPSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
