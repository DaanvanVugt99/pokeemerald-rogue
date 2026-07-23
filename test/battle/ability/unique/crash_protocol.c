#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRAVITY].effect == EFFECT_GRAVITY);
    ASSUME(gBattleMoves[MOVE_STONE_EDGE].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_STONE_EDGE].power > 0);
    ASSUME(gBattleMoves[MOVE_STONE_EDGE].accuracy < 100);
    ASSUME(gBattleMoves[MOVE_PSYCHO_CUT].type != TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_PSYCHO_CUT].power > 0);
    ASSUME(gBattleMoves[MOVE_FLY].effect == EFFECT_SEMI_INVULNERABLE);
}

SINGLE_BATTLE_TEST("Crash Protocol sets Gravity before Iron Boulder's first Rock attack")
{
    GIVEN {
        PLAYER(SPECIES_IRON_BOULDER) { Attack(100); Speed(100); Ability(ABILITY_QUARK_DRIVE); Moves(MOVE_STONE_EDGE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(100); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STONE_EDGE, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CRASH_PROTOCOL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        MESSAGE("Gravity intensified!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STONE_EDGE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Crash Protocol remains inactive with another Paradox ally")
{
    GIVEN {
        PLAYER(SPECIES_IRON_BOULDER) { Speed(100); Ability(ABILITY_QUARK_DRIVE); Moves(MOVE_STONE_EDGE); }
        PLAYER(SPECIES_GREAT_TUSK) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STONE_EDGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CRASH_PROTOCOL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_GRAVITY));
        EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]));
    }
}

SINGLE_BATTLE_TEST("Crash Protocol waits for a damaging Rock-type move")
{
    GIVEN {
        PLAYER(SPECIES_IRON_BOULDER) { Speed(100); Ability(ABILITY_QUARK_DRIVE); Moves(MOVE_PSYCHO_CUT, MOVE_STONE_EDGE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHO_CUT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_STONE_EDGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHO_CUT, player);
        ABILITY_POPUP(player, ABILITY_CRASH_PROTOCOL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Crash Protocol does not consume its activation while Gravity is already active")
{
    GIVEN {
        PLAYER(SPECIES_IRON_BOULDER) { Speed(50); Ability(ABILITY_QUARK_DRIVE); Moves(MOVE_STONE_EDGE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_GRAVITY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRAVITY); MOVE(player, MOVE_STONE_EDGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CRASH_PROTOCOL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]));
    }
}

SINGLE_BATTLE_TEST("Crash Protocol grounds airborne targets before the Rock attack")
{
    GIVEN {
        PLAYER(SPECIES_IRON_BOULDER) { Attack(100); Speed(50); Ability(ABILITY_QUARK_DRIVE); Moves(MOVE_STONE_EDGE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(100); Speed(100); Moves(MOVE_FLY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FLY); MOVE(player, MOVE_STONE_EDGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLY, opponent);
        ABILITY_POPUP(player, ABILITY_CRASH_PROTOCOL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STONE_EDGE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_ON_AIR));
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Crash Protocol is not consumed when Iron Boulder cannot act")
{
    GIVEN {
        PLAYER(SPECIES_IRON_BOULDER) { Speed(100); Ability(ABILITY_QUARK_DRIVE); Status1(STATUS1_SLEEP_TURN(2)); Moves(MOVE_STONE_EDGE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STONE_EDGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CRASH_PROTOCOL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_GRAVITY));
        EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]));
    }
}

SINGLE_BATTLE_TEST("Crash Protocol sets Gravity before a Rock attack is blocked by Protect")
{
    GIVEN {
        PLAYER(SPECIES_IRON_BOULDER) { Speed(50); Ability(ABILITY_QUARK_DRIVE); Moves(MOVE_STONE_EDGE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_STONE_EDGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        ABILITY_POPUP(player, ABILITY_CRASH_PROTOCOL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        MESSAGE("Foe Wobbuffet protected itself!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
