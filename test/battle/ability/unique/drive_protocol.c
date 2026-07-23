#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRAVITY].effect == EFFECT_GRAVITY);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
}

SINGLE_BATTLE_TEST("Drive Protocol clears hazards and sets Gravity on switch-in if Iron Treads is the only Paradox")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_IRON_TREADS) { HP(200); MaxHP(200); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_STICKY_WEB, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DRIVE_PROTOCOL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STICKY_WEB));
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Drive Protocol sets Gravity on switch-in even when there are no hazards")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_IRON_TREADS) { HP(200); MaxHP(200); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DRIVE_PROTOCOL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Drive Protocol does not activate if Iron Treads has another Paradox ally")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_IRON_TREADS) { HP(200); MaxHP(200); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SANDY_SHOCKS) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_STEALTH_ROCK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DRIVE_PROTOCOL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK);
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_GRAVITY));
    }
}
