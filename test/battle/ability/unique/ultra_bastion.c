#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
}

SINGLE_BATTLE_TEST("Ultra Bastion sets Stealth Rock on first switch-in if it is the only Ultra Beast")
{
    GIVEN {
        PLAYER(SPECIES_STAKATAKA) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ULTRA_BASTION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}

SINGLE_BATTLE_TEST("Ultra Bastion does not set Stealth Rock if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_STAKATAKA) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_BASTION);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Ultra Bastion only triggers on the first switch-in each battle")
{
    GIVEN {
        PLAYER(SPECIES_STAKATAKA) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DEFOG, MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_DEFOG, target: player); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ULTRA_BASTION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEFOG, opponent);
        MESSAGE("The pointed stones disappeared from the opposing side!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ULTRA_BASTION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
        }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK));
    }
}
