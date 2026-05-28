#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Grafitti Tag makes a tagged foe leave Toxic Spikes when switching out")
{
    GIVEN {
        PLAYER(SPECIES_GRAFAIAI) { Ability(ABILITY_UNBURDEN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARIZARD) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GRAFITTI_TAG);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_GRAFITTI_TAG, opponent);
        MESSAGE("Grafaiai tagged\nthe opposing Pokemon!");
        MESSAGE("Poison Spikes were scattered all around the opposing team's feet!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TOXIC_SPIKES);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount, 1);
    }
}

SINGLE_BATTLE_TEST("Grafitti Tag is consumed after the tagged foe switches out")
{
    GIVEN {
        PLAYER(SPECIES_GRAFAIAI) { Ability(ABILITY_UNBURDEN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARIZARD) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 0); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GRAFITTI_TAG);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_GRAFITTI_TAG, opponent);
        MESSAGE("Poison Spikes were scattered all around the opposing team's feet!");
        NONE_OF {
            MESSAGE("Poison Spikes were scattered all around the opposing team's feet!");
        }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount, 1);
    }
}

SINGLE_BATTLE_TEST("Grafitti Tag makes a tagged foe leave Toxic Spikes when fainting")
{
    GIVEN {
        PLAYER(SPECIES_GRAFAIAI) { Ability(ABILITY_UNBURDEN); Attack(200); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Defense(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GRAFITTI_TAG);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_GRAFITTI_TAG, opponent);
        MESSAGE("Grafaiai tagged\nthe opposing Pokemon!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        MESSAGE("Foe Wobbuffet fainted!");
        MESSAGE("Poison Spikes were scattered all around the opposing team's feet!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TOXIC_SPIKES);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount, 1);
    }
}

DOUBLE_BATTLE_TEST("Grafitti Tag only tags the directly opposing foe in doubles")
{
    GIVEN {
        PLAYER(SPECIES_GRAFAIAI) { Ability(ABILITY_UNBURDEN); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARIZARD) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_GRAFITTI_TAG);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_GRAFITTI_TAG, opponentLeft);
        MESSAGE("Grafaiai tagged\nthe opposing Pokemon!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_GRAFITTI_TAG, opponentRight);
        }
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].grafittiTagged);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)].grafittiTagged);
    }
}
