#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_HYDRO_PUMP].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_MUDDY_WATER].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_MUDDY_WATER].target == MOVE_TARGET_BOTH);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_WATER);
}

SINGLE_BATTLE_TEST("Still Mind creates a Rainbow when the user uses a Water-type move")
{
    GIVEN {
        PLAYER(SPECIES_UXIE) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_STILL_MIND); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STILL_MIND);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW);
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_RAINBOW));
    }
}

SINGLE_BATTLE_TEST("Still Mind does not create a Rainbow when another Pokemon uses a Water-type move")
{
    GIVEN {
        PLAYER(SPECIES_UXIE) { Speed(50); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_STILL_MIND); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_STILL_MIND);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW));
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_RAINBOW));
    }
}

SINGLE_BATTLE_TEST("Still Mind does not create a Rainbow for non-Water moves")
{
    GIVEN {
        PLAYER(SPECIES_UXIE) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_STILL_MIND); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_STILL_MIND);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW));
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_RAINBOW));
    }
}

SINGLE_BATTLE_TEST("Still Mind only creates a Rainbow once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_UXIE) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_STILL_MIND); Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STILL_MIND);
        MESSAGE("The rainbow on your side disappeared!");
        NOT ABILITY_POPUP(player, ABILITY_STILL_MIND);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW));
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Still Mind is not consumed by a missed Water-type move")
{
    GIVEN {
        PLAYER(SPECIES_UXIE) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_STILL_MIND); Moves(MOVE_HYDRO_PUMP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYDRO_PUMP, hit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW));
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

DOUBLE_BATTLE_TEST("Still Mind creates a Rainbow if a spread Water-type move hits any target")
{
    GIVEN {
        PLAYER(SPECIES_UXIE) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_STILL_MIND); Moves(MOVE_MUDDY_WATER); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_PROTECT); MOVE(playerLeft, MOVE_MUDDY_WATER); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_STILL_MIND);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
