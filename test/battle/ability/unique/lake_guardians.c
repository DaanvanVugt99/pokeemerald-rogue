#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_FIRE_BLAST].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_HEAT_WAVE].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_HEAT_WAVE].target == MOVE_TARGET_BOTH);
    ASSUME(gBattleMoves[MOVE_RAZOR_LEAF].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_RAZOR_LEAF].target == MOVE_TARGET_BOTH);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_GRASS);
}

SINGLE_BATTLE_TEST("Burning Heart scorches the opposing field when the user uses a Fire-type move")
{
    GIVEN {
        PLAYER(SPECIES_MESPRIT) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_BURNING_HEART); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BURNING_HEART);
        MESSAGE("A sea of fire enveloped the opposing side!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SEA_OF_FIRE);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SEA_OF_FIRE));
    }
}

SINGLE_BATTLE_TEST("Burning Heart only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MESPRIT) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_BURNING_HEART); Moves(MOVE_EMBER, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BURNING_HEART);
        MESSAGE("The sea of fire around the opposing side disappeared!");
        NOT ABILITY_POPUP(player, ABILITY_BURNING_HEART);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SEA_OF_FIRE));
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Burning Heart is not consumed by a missed Fire-type move")
{
    GIVEN {
        PLAYER(SPECIES_MESPRIT) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_BURNING_HEART); Moves(MOVE_FIRE_BLAST); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_BLAST, hit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SEA_OF_FIRE));
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

DOUBLE_BATTLE_TEST("Burning Heart scorches the opposing field if a spread Fire-type move hits any target")
{
    GIVEN {
        PLAYER(SPECIES_MESPRIT) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_BURNING_HEART); Moves(MOVE_HEAT_WAVE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_PROTECT); MOVE(playerLeft, MOVE_HEAT_WAVE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_BURNING_HEART);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SEA_OF_FIRE);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Burning Heart does not trigger for another Pokemon's Fire-type move")
{
    GIVEN {
        PLAYER(SPECIES_MESPRIT) { Speed(50); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_BURNING_HEART); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_EMBER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_EMBER); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_BURNING_HEART);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SEA_OF_FIRE));
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SEA_OF_FIRE));
    }
}

SINGLE_BATTLE_TEST("Iron Will creates a Swamp on the opposing field when the user uses a Grass-type move")
{
    GIVEN {
        PLAYER(SPECIES_AZELF) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_IRON_WILL); Moves(MOVE_RAZOR_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAZOR_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_IRON_WILL);
        MESSAGE("A swamp enveloped the opposing side!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SWAMP);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SWAMP));
    }
}

SINGLE_BATTLE_TEST("Iron Will only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_AZELF) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_IRON_WILL); Moves(MOVE_RAZOR_LEAF, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAZOR_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RAZOR_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_IRON_WILL);
        MESSAGE("The swamp around the opposing side disappeared!");
        NOT ABILITY_POPUP(player, ABILITY_IRON_WILL);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SWAMP));
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Iron Will is not consumed by a missed Grass-type move")
{
    GIVEN {
        PLAYER(SPECIES_AZELF) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_IRON_WILL); Moves(MOVE_RAZOR_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAZOR_LEAF, hit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SWAMP));
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

DOUBLE_BATTLE_TEST("Iron Will creates a Swamp if a spread Grass-type move hits any target")
{
    GIVEN {
        PLAYER(SPECIES_AZELF) { Speed(100); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_IRON_WILL); Moves(MOVE_RAZOR_LEAF); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(200); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_PROTECT); MOVE(playerLeft, MOVE_RAZOR_LEAF); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_IRON_WILL);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SWAMP);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Iron Will does not trigger for another Pokemon's Grass-type move")
{
    GIVEN {
        PLAYER(SPECIES_AZELF) { Speed(50); Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_IRON_WILL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_RAZOR_LEAF); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAZOR_LEAF); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_IRON_WILL);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SWAMP));
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SWAMP));
    }
}
