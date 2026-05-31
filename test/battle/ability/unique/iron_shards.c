#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Iron Shards sets Stealth Rock when the user takes 25 percent or less damage from a move")
{
    GIVEN {
        PLAYER(SPECIES_AGGRON) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_IRON_SHARDS); MaxHP(200); HP(200); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}

SINGLE_BATTLE_TEST("Iron Shards does not set Stealth Rock when the user takes more than 25 percent damage from a move")
{
    GIVEN {
        PLAYER(SPECIES_AGGRON) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_IRON_SHARDS); MaxHP(100); HP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK));
    }
}

DOUBLE_BATTLE_TEST("Iron Shards sets Stealth Rock on the opposing side when hit by an ally")
{
    GIVEN {
        PLAYER(SPECIES_AGGRON) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_IRON_SHARDS); MaxHP(200); HP(200); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_DRAGON_RAGE, target: playerLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK));
    }
}

DOUBLE_BATTLE_TEST("Iron Shards can trigger from an ally hit when the user's side already has Stealth Rock")
{
    GIVEN {
        PLAYER(SPECIES_AGGRON) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_IRON_SHARDS); MaxHP(200); HP(200); Moves(MOVE_CELEBRATE); Speed(5); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); Speed(5); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_STEALTH_ROCK, MOVE_CELEBRATE); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(5); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_STEALTH_ROCK);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_DRAGON_RAGE, target: playerLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK);
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}
