#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STATIC_BURST].effect == EFFECT_STATIC_BURST);
    ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].effect == EFFECT_PARALYZE);
    ASSUME(gBattleMoves[MOVE_CHARGE].effect == EFFECT_CHARGE);
    ASSUME(gBattleMoves[MOVE_MAGNET_RISE].effect == EFFECT_MAGNET_RISE);
}

SINGLE_BATTLE_TEST("Static Burst damages the target, then uses a random static move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Moves(MOVE_STATIC_BURST); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STATIC_BURST, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_THUNDER_WAVE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STATIC_BURST, player);
        HP_BAR(opponent);
        MESSAGE("Wobbuffet used Thunder Wave!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_WAVE, player);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Static Burst can use an opposing-target static move against the selected target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Moves(MOVE_STATIC_BURST); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STATIC_BURST, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_EERIE_IMPULSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Eerie Impulse!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EERIE_IMPULSE, player);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Static Burst can use a self-targeting static move on the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Moves(MOVE_STATIC_BURST); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STATIC_BURST, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_CHARGE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Charge!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP);
    }
}

SINGLE_BATTLE_TEST("Static Burst does not use a static move if the initial hit has no effect")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_STATIC_BURST); }
        OPPONENT(SPECIES_DIGLETT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STATIC_BURST, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_THUNDER_WAVE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("Wobbuffet used Thunder Wave!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_WAVE, player);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Static Burst does not crash if the random static move fails after the hit")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Moves(MOVE_STATIC_BURST); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STATIC_BURST, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_MAGNET_RISE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_STATIC_BURST, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_MAGNET_RISE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Magnet Rise!");
        MESSAGE("Wobbuffet used Magnet Rise!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_MAGNET_RISE);
    }
}
