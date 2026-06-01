#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STAGE_FRIGHT].effect == EFFECT_STAGE_FRIGHT);
    ASSUME(gBattleMoves[MOVE_SCARY_FACE].effect == EFFECT_SPEED_DOWN_2);
    ASSUME(gBattleMoves[MOVE_SCREECH].effect == EFFECT_DEFENSE_DOWN_2);
    ASSUME(gBattleMoves[MOVE_MEAN_LOOK].effect == EFFECT_MEAN_LOOK);
    ASSUME(gBattleMoves[MOVE_ASTONISH].effect == EFFECT_FLINCH_HIT);
}

SINGLE_BATTLE_TEST("Stage Fright lowers Sp. Atk, then uses a random fear move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_STAGE_FRIGHT); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STAGE_FRIGHT, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_SCARY_FACE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STAGE_FRIGHT, player);
        MESSAGE("Wobbuffet used Scary Face!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Stage Fright's random fear move does not spend extra PP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_STAGE_FRIGHT, 10}); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STAGE_FRIGHT, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_SCARY_FACE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Stage Fright!");
        MESSAGE("Wobbuffet used Scary Face!");
    } THEN {
        EXPECT_EQ(player->pp[0], 9);
    }
}

SINGLE_BATTLE_TEST("Stage Fright can use a damaging fear move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_STAGE_FRIGHT); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STAGE_FRIGHT, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_ASTONISH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Astonish!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ASTONISH, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Stage Fright does not use a fear move if the Sp. Atk drop fails")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_EERIE_IMPULSE, MOVE_STAGE_FRIGHT); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_IMPULSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EERIE_IMPULSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EERIE_IMPULSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_STAGE_FRIGHT, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_ASTONISH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ASTONISH, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPATK], MIN_STAT_STAGE);
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Stage Fright does not crash if the random fear move fails after the Sp. Atk drop")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_STAGE_FRIGHT); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STAGE_FRIGHT, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_MEAN_LOOK)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_STAGE_FRIGHT, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_MEAN_LOOK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Mean Look!");
        MESSAGE("Wobbuffet used Mean Look!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
    }
}
