#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WILD_GROWTH].effect == EFFECT_WILD_GROWTH);
    ASSUME(gBattleMoves[MOVE_INGRAIN].effect == EFFECT_INGRAIN);
    ASSUME(gBattleMoves[MOVE_LEECH_SEED].effect == EFFECT_LEECH_SEED);
    ASSUME(gBattleMoves[MOVE_WORRY_SEED].effect == EFFECT_WORRY_SEED);
    ASSUME(gBattleMoves[MOVE_GROWTH].effect == EFFECT_GROWTH);
}

SINGLE_BATTLE_TEST("Wild Growth heals the user, then uses a random overgrowth move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(80); MaxHP(160); Moves(MOVE_WILD_GROWTH); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WILD_GROWTH, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_LEECH_SEED)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WILD_GROWTH, player);
        HP_BAR(player);
        MESSAGE("Wobbuffet used Leech Seed!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
    } THEN {
        EXPECT_GT(player->hp, 100);
        EXPECT(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED);
    }
}

SINGLE_BATTLE_TEST("Wild Growth can use a target-dependent overgrowth move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(80); MaxHP(160); Moves(MOVE_WILD_GROWTH); }
        OPPONENT(SPECIES_WYNAUT) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WILD_GROWTH, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_WORRY_SEED)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player);
        MESSAGE("Wobbuffet used Worry Seed!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WORRY_SEED, player);
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(opponent->ability, ABILITY_INSOMNIA);
    }
}

SINGLE_BATTLE_TEST("Wild Growth can use a self-targeting overgrowth move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(80); MaxHP(160); Moves(MOVE_WILD_GROWTH); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WILD_GROWTH, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_INGRAIN)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player);
        MESSAGE("Wobbuffet used Ingrain!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
    } THEN {
        EXPECT_EQ(player->hp, 110);
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_ROOTED);
    }
}

SINGLE_BATTLE_TEST("Wild Growth does not use an overgrowth move at full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(160); MaxHP(160); Moves(MOVE_WILD_GROWTH); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WILD_GROWTH, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_GROWTH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWTH, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Wild Growth does not crash if the random overgrowth move fails after healing")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(80); MaxHP(160); Moves(MOVE_WILD_GROWTH); }
        OPPONENT(SPECIES_ODDISH) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WILD_GROWTH, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_LEECH_SEED)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player);
        MESSAGE("Wobbuffet used Leech Seed!");
        MESSAGE("It doesn't affect Foe Oddish…");
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED));
    }
}
