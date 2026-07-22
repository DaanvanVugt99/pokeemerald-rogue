#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Blast Shield blocks 120 BP moves for every Minior Meteor colour")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_MINIOR_METEOR_RED; }
    PARAMETRIZE { species = SPECIES_MINIOR_METEOR_ORANGE; }
    PARAMETRIZE { species = SPECIES_MINIOR_METEOR_YELLOW; }
    PARAMETRIZE { species = SPECIES_MINIOR_METEOR_GREEN; }
    PARAMETRIZE { species = SPECIES_MINIOR_METEOR_BLUE; }
    PARAMETRIZE { species = SPECIES_MINIOR_METEOR_INDIGO; }
    PARAMETRIZE { species = SPECIES_MINIOR_METEOR_VIOLET; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_CLOSE_COMBAT].power == BLAST_SHIELD_MIN_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CLOSE_COMBAT); }
        OPPONENT(species) { HP(100); MaxHP(100); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CLOSE_COMBAT); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_BLAST_SHIELD);
    } THEN {
        EXPECT_EQ(opponent->hp, 100);
    }
}

SINGLE_BATTLE_TEST("Blast Shield only blocks damaging moves at or above 120 listed BP")
{
    u16 move;
    bool32 blocked;

    PARAMETRIZE { move = MOVE_PSYCHIC; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_CLOSE_COMBAT; blocked = TRUE; }
    PARAMETRIZE { move = MOVE_BOOMBURST; blocked = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_PSYCHIC].power < BLAST_SHIELD_MIN_POWER);
        ASSUME(gBattleMoves[MOVE_CLOSE_COMBAT].power == BLAST_SHIELD_MIN_POWER);
        ASSUME(gBattleMoves[MOVE_BOOMBURST].power > BLAST_SHIELD_MIN_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(SPECIES_MINIOR_METEOR_RED) { HP(1000); MaxHP(1000); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        if (blocked)
            ABILITY_POPUP(opponent, ABILITY_BLAST_SHIELD);
        else
            NOT ABILITY_POPUP(opponent, ABILITY_BLAST_SHIELD);
    } THEN {
        if (blocked)
            EXPECT_EQ(opponent->hp, 1000);
        else
            EXPECT_LT(opponent->hp, 1000);
    }
}

SINGLE_BATTLE_TEST("Flak Shield blocks 40 BP moves for every Minior Core colour")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_MINIOR_CORE_RED; }
    PARAMETRIZE { species = SPECIES_MINIOR_CORE_ORANGE; }
    PARAMETRIZE { species = SPECIES_MINIOR_CORE_YELLOW; }
    PARAMETRIZE { species = SPECIES_MINIOR_CORE_GREEN; }
    PARAMETRIZE { species = SPECIES_MINIOR_CORE_BLUE; }
    PARAMETRIZE { species = SPECIES_MINIOR_CORE_INDIGO; }
    PARAMETRIZE { species = SPECIES_MINIOR_CORE_VIOLET; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power == FLAK_SHIELD_MAX_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(species) { HP(400); MaxHP(1000); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_FLAK_SHIELD);
    } THEN {
        EXPECT_EQ(opponent->hp, 400);
    }
}

SINGLE_BATTLE_TEST("Flak Shield only blocks damaging moves at or below 40 listed BP")
{
    u16 move;
    bool32 blocked;

    PARAMETRIZE { move = MOVE_TACKLE; blocked = TRUE; }
    PARAMETRIZE { move = MOVE_SWIFT; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_GROWL; blocked = FALSE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power == FLAK_SHIELD_MAX_POWER);
        ASSUME(gBattleMoves[MOVE_SWIFT].power > FLAK_SHIELD_MAX_POWER);
        ASSUME(gBattleMoves[MOVE_GROWL].power == 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(SPECIES_MINIOR_CORE_RED) { HP(400); MaxHP(1000); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        if (blocked)
            ABILITY_POPUP(opponent, ABILITY_FLAK_SHIELD);
        else
            NOT ABILITY_POPUP(opponent, ABILITY_FLAK_SHIELD);
    } THEN {
        if (blocked || move == MOVE_GROWL)
            EXPECT_EQ(opponent->hp, 400);
        else
            EXPECT_LT(opponent->hp, 400);
    }
}

SINGLE_BATTLE_TEST("Minior shields resolve Max Move power instead of the power sentinel")
{
    u16 species;
    u16 move;
    u16 ability;
    bool32 blocked;

    PARAMETRIZE { species = SPECIES_MINIOR_METEOR_RED; move = MOVE_BOOMBURST; ability = ABILITY_BLAST_SHIELD; blocked = TRUE; }
    PARAMETRIZE { species = SPECIES_MINIOR_CORE_RED; move = MOVE_TACKLE; ability = ABILITY_FLAK_SHIELD; blocked = FALSE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_MAX_STRIKE].power == 1);
        ASSUME(GetMaxMovePower(MOVE_BOOMBURST) >= BLAST_SHIELD_MIN_POWER);
        ASSUME(GetMaxMovePower(MOVE_TACKLE) > FLAK_SHIELD_MAX_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(species) { HP(1000); MaxHP(1000); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, move, dynamax: TRUE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        if (blocked)
            ABILITY_POPUP(opponent, ability);
        else
            NOT ABILITY_POPUP(opponent, ability);
    } THEN {
        if (blocked)
            EXPECT_EQ(opponent->hp, 1000);
        else
            EXPECT_LT(opponent->hp, 1000);
    }
}

SINGLE_BATTLE_TEST("Flak Shield does not treat variable move power as 1 BP")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_LOW_KICK].power == 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_LOW_KICK); }
        OPPONENT(SPECIES_MINIOR_CORE_RED) { HP(1000); MaxHP(1000); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_LOW_KICK); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_FLAK_SHIELD);
    } THEN {
        EXPECT_LT(opponent->hp, 1000);
    }
}

SINGLE_BATTLE_TEST("Mold Breaker bypasses Blast Shield and Flak Shield")
{
    u16 species;
    u16 move;
    u16 hp;

    PARAMETRIZE { species = SPECIES_MINIOR_METEOR_RED; move = MOVE_CLOSE_COMBAT; hp = 1000; }
    PARAMETRIZE { species = SPECIES_MINIOR_CORE_RED; move = MOVE_TACKLE; hp = 400; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_CLOSE_COMBAT].power == BLAST_SHIELD_MIN_POWER);
        ASSUME(gBattleMoves[MOVE_TACKLE].power == FLAK_SHIELD_MAX_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_MOLD_BREAKER); Moves(move); }
        OPPONENT(species) { HP(hp); MaxHP(1000); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_BLAST_SHIELD);
            ABILITY_POPUP(opponent, ABILITY_FLAK_SHIELD);
        }
    } THEN {
        EXPECT_LT(opponent->hp, hp);
    }
}
