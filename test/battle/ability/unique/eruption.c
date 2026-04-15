#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
}

SINGLE_BATTLE_TEST("Eruption uses Smokescreen on the foe after the user's first Fire move")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_ERUPTION); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Eruption does not trigger on non-Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_ERUPTION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Eruption only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_ERUPTION); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}

DOUBLE_BATTLE_TEST("Eruption uses Smokescreen on the target of the Fire move")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_ERUPTION); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_WOBBUFFET)    { Moves(MOVE_CELEBRATE); }

        OPPONENT(SPECIES_WOBBUFFET)  { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET)  { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_EMBER, target: opponentRight); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_RIGHT].statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}
