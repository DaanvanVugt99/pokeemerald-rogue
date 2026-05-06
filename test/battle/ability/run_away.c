#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Run Away ignores move-based trapping")
{
    GIVEN {
        PLAYER(SPECIES_RATTATA) { Ability(ABILITY_RUN_AWAY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_MEAN_LOOK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_MEAN_LOOK); }
    } THEN {
        EXPECT_EQ(CanBattlerEscape(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), TRUE);
    }
}

SINGLE_BATTLE_TEST("Run Away ignores ability-based trapping")
{
    GIVEN {
        PLAYER(SPECIES_RATTATA) { Ability(ABILITY_RUN_AWAY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(IsAbilityPreventingEscape(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), 0);
    }
}

SINGLE_BATTLE_TEST("Run Away does not ignore non-trapping switch locks")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_INGRAIN].effect == EFFECT_INGRAIN);
        PLAYER(SPECIES_RATTATA) { Ability(ABILITY_RUN_AWAY); Moves(MOVE_INGRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_INGRAIN); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(CanBattlerEscape(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), FALSE);
    }
}
