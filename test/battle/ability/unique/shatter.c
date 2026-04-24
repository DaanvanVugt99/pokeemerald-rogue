#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Shatter sets Stealth Rock after knocking out a target")
{
    GIVEN {
        PLAYER(SPECIES_RHYPERIOR) { Ability(ABILITY_SOLID_ROCK); UniqueAbility(ABILITY_SHATTER); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_METAPOD) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}

SINGLE_BATTLE_TEST("Shatter does not set Stealth Rock if the target was the last opposing Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_RHYPERIOR) { Ability(ABILITY_SOLID_ROCK); UniqueAbility(ABILITY_SHATTER); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_METAPOD) { HP(0); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK));
    }
}

DOUBLE_BATTLE_TEST("Shatter still sets Stealth Rock if the knocked out target has no replacement but its partner is still alive")
{
    GIVEN {
        PLAYER(SPECIES_RHYPERIOR) { Ability(ABILITY_SOLID_ROCK); UniqueAbility(ABILITY_SHATTER); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_METAPOD) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, playerLeft);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}
