#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WHIRLWIND].effect == EFFECT_ROAR);
}

SINGLE_BATTLE_TEST("Scrap Draft sets Spikes when Skarmory uses Whirlwind")
{
    GIVEN {
        PLAYER(SPECIES_SKARMORY) { Ability(ABILITY_STURDY); Moves(MOVE_WHIRLWIND); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WHIRLWIND); }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}

SINGLE_BATTLE_TEST("Scrap Draft does not set more than three Spikes layers")
{
    GIVEN {
        PLAYER(SPECIES_SKARMORY) { Ability(ABILITY_STURDY); Moves(MOVE_WHIRLWIND); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SQUIRTLE) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WHIRLWIND); }
        TURN { MOVE(player, MOVE_WHIRLWIND); }
        TURN { MOVE(player, MOVE_WHIRLWIND); }
        TURN { MOVE(player, MOVE_WHIRLWIND); }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 3);
    }
}

SINGLE_BATTLE_TEST("Scrap Draft preserves Whirlwind's chosen switch target")
{
    PASSES_RANDOMLY(1, 2, RNG_FORCE_RANDOM_SWITCH);
    GIVEN {
        PLAYER(SPECIES_SKARMORY) { Ability(ABILITY_STURDY); Moves(MOVE_WHIRLWIND); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SQUIRTLE) { HP(0); }
    } WHEN {
        TURN { MOVE(player, MOVE_WHIRLWIND); }
    } SCENE {
        MESSAGE("Foe Bulbasaur was dragged out!");
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}
