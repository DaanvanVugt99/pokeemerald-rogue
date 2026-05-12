#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].makesContact == TRUE);
    ASSUME(gBattleMoves[MOVE_ROAR].effect == EFFECT_ROAR);
}

SINGLE_BATTLE_TEST("Crackling Shrine prevents contact effects in Electric Terrain")
{
    GIVEN {
        PLAYER(SPECIES_TAPU_KOKO) { Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_GARCHOMP) { Ability(ABILITY_ROUGH_SKIN); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_ROUGH_SKIN);
            MESSAGE("Tapu Koko was hurt!");
        }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Oracle Shrine makes status moves ignore accuracy in Psychic Terrain")
{
    GIVEN {
        PLAYER(SPECIES_TAPU_LELE) { Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE(opponent->status1 & STATUS1_SLEEP, 0);
    }
}

SINGLE_BATTLE_TEST("Rooted Shrine prevents forced switching in Grassy Terrain")
{
    GIVEN {
        PLAYER(SPECIES_TAPU_BULU);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ROAR); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ROAR); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROOTED_SHRINE);
        MESSAGE("Tapu Bulu anchors itself with Rooted Shrine!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_TAPU_BULU);
    }
}

SINGLE_BATTLE_TEST("Mistbound Shrine blocks secondary effects from damaging moves in Misty Terrain")
{
    GIVEN {
        PLAYER(SPECIES_TAPU_FINI);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_MYSTICAL_FIRE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MYSTICAL_FIRE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}
