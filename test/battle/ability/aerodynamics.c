#include "global.h"
#include "test/battle.h"

// Test 1: Aerodynamics boosts Speed by 1 stage when hit by Flying-type moves
SINGLE_BATTLE_TEST("Aerodynamics boosts Speed by 1 stage when hit by Flying-type moves")
{
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
    GIVEN
    {
        PLAYER(SPECIES_SWELLOW)
        {
            Ability(ABILITY_AERODYNAMICS);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_AERIAL_ACE); }
    }
    SCENE
    {
        ABILITY_POPUP(player, ABILITY_AERODYNAMICS);
        MESSAGE("Swellow's Speed rose!");
    }
}

// Test 2: Aerodynamics does not activate if protected
SINGLE_BATTLE_TEST("Aerodynamics does not activate if protected")
{
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
    GIVEN
    {
        PLAYER(SPECIES_SWELLOW)
        {
            Ability(ABILITY_AERODYNAMICS);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN
        {
            MOVE(player, MOVE_PROTECT);
            MOVE(opponent, MOVE_AERIAL_ACE);
        }
    }
    SCENE
    {
        NONE_OF
        {
            ABILITY_POPUP(player, ABILITY_AERODYNAMICS);
            MESSAGE("Swellow's Speed rose!");
        }
    }
}

// Test 3: Aerodynamics activates from status moves
SINGLE_BATTLE_TEST("Aerodynamics activates from status moves")
{
    ASSUME(gBattleMoves[MOVE_DEFOG].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_DEFOG].split == SPLIT_STATUS);
    GIVEN
    {
        PLAYER(SPECIES_SWELLOW)
        {
            Ability(ABILITY_AERODYNAMICS);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_DEFOG); }
    }
    SCENE
    {
        ABILITY_POPUP(player, ABILITY_AERODYNAMICS);
        MESSAGE("Swellow's Speed rose!");
    }
}

// Test 4: Aerodynamics is only triggered once on multi-strike moves
SINGLE_BATTLE_TEST("Aerodynamics is only triggered once on multi-strike moves")
{
    ASSUME(gBattleMoves[MOVE_DUAL_WINGBEAT].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_DUAL_WINGBEAT].flags2 == FLAG_TWO_STRIKES);
    GIVEN
    {
        PLAYER(SPECIES_SWELLOW)
        {
            Ability(ABILITY_AERODYNAMICS);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_DUAL_WINGBEAT); }
    }
    SCENE
    {
        ABILITY_POPUP(player, ABILITY_AERODYNAMICS);
        MESSAGE("Swellow's Speed rose!");
    }
}