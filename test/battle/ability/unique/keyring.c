#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_DISABLE].effect == EFFECT_DISABLE);
    ASSUME(gBattleMoves[MOVE_TORMENT].effect == EFFECT_TORMENT);
    ASSUME(gBattleMoves[MOVE_ENCORE].effect == EFFECT_ENCORE);
    ASSUME(gBattleMoves[MOVE_MAGIC_ROOM].effect == EFFECT_MAGIC_ROOM);
    ASSUME(gBattleMoves[MOVE_REFLECT].effect == EFFECT_REFLECT);
    ASSUME(gBattleMoves[MOVE_LIGHT_SCREEN].effect == EFFECT_LIGHT_SCREEN);
    ASSUME(gBattleMoves[MOVE_METAL_SOUND].effect == EFFECT_SPECIAL_DEFENSE_DOWN_2);
    ASSUME(gBattleMoves[MOVE_LOCK_ON].effect == EFFECT_LOCK_ON);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
}

SINGLE_BATTLE_TEST("Keyring uses a random key trick after the first status move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Speed(100); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_KEYRING); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_KEYRING, MOVE_REFLECT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_KEYRING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
    }
}

SINGLE_BATTLE_TEST("Keyring only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Speed(100); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_KEYRING); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_KEYRING, MOVE_REFLECT)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_KEYRING, MOVE_LIGHT_SCREEN)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KEYRING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
        NOT ABILITY_POPUP(player, ABILITY_KEYRING);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN));
    }
}

SINGLE_BATTLE_TEST("Keyring refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Speed(100); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_KEYRING); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_PIKACHU) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_KEYRING, MOVE_REFLECT)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_KEYRING, MOVE_LIGHT_SCREEN)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KEYRING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
        ABILITY_POPUP(player, ABILITY_KEYRING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN);
    }
}

SINGLE_BATTLE_TEST("Keyring chooses from every key trick")
{
    static const u16 expectedMoves[] =
    {
        MOVE_DISABLE,
        MOVE_TORMENT,
        MOVE_ENCORE,
        MOVE_MAGIC_ROOM,
        MOVE_REFLECT,
        MOVE_LIGHT_SCREEN,
        MOVE_METAL_SOUND,
        MOVE_LOCK_ON,
        MOVE_SPIKES,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_KEYRING);

    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Speed(1); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_KEYRING); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KEYRING);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}

SINGLE_BATTLE_TEST("Keyring does not repeat the status move that triggered it")
{
    static const u16 expectedMoves[] =
    {
        MOVE_DISABLE,
        MOVE_TORMENT,
        MOVE_ENCORE,
        MOVE_MAGIC_ROOM,
        MOVE_REFLECT,
        MOVE_METAL_SOUND,
        MOVE_LOCK_ON,
        MOVE_SPIKES,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_KEYRING);

    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Speed(1); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_KEYRING); Moves(MOVE_LIGHT_SCREEN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_LIGHT_SCREEN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KEYRING);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
