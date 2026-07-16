#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_HEAD_SMASH].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_HEAD_SMASH].effect == EFFECT_RECOIL_50);
    ASSUME(gBattleMoves[MOVE_SURF].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_RAPID_SPIN].effect == EFFECT_RAPID_SPIN);
}

SINGLE_BATTLE_TEST("Fossil Drill does not use Rapid Spin if its user faints from recoil")
{
    GIVEN {
        PLAYER(SPECIES_ARMALDO) { HP(1); Attack(1); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_FOSSIL_DRILL); Moves(MOVE_HEAD_SMASH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(999); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HEAD_SMASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAD_SMASH, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FOSSIL_DRILL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}

SINGLE_BATTLE_TEST("Fossil Drill triggers only on the first Rock-type move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ARMALDO) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_FOSSIL_DRILL); Moves(MOVE_ROCK_THROW); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); MOVE(player, MOVE_ROCK_THROW); }
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOSSIL_DRILL);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FOSSIL_DRILL);
        }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Fossil Drill does not consume on non-Rock moves")
{
    GIVEN {
        PLAYER(SPECIES_ARMALDO) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_FOSSIL_DRILL); Moves(MOVE_SURF, MOVE_ROCK_THROW); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); MOVE(player, MOVE_SURF); }
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOSSIL_DRILL);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Fossil Drill refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_ARMALDO) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_FOSSIL_DRILL); Moves(MOVE_ROCK_THROW); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); MOVE(player, MOVE_ROCK_THROW); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOSSIL_DRILL);
        ABILITY_POPUP(player, ABILITY_FOSSIL_DRILL);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
    }
}
