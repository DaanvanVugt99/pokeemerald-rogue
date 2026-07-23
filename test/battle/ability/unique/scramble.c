#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split != SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
}

SINGLE_BATTLE_TEST("Scramble sets Psychic Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SPOINK) { Ability(ABILITY_THICK_FAT); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SCRAMBLE);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Scramble gives damaging moves a 30 percent chance to confuse in Psychic Terrain")
{
    PASSES_RANDOMLY(30, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_SPOINK) { Ability(ABILITY_THICK_FAT); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SCRAMBLE);
        MESSAGE("Foe Wobbuffet became confused!");
    } THEN {
        EXPECT_NE(opponent->status2 & STATUS2_CONFUSION, 0);
    }
}

DOUBLE_BATTLE_TEST("Scramble affects other battlers' damaging moves while its user is on the field")
{
    PASSES_RANDOMLY(30, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_SPOINK) { Ability(ABILITY_THICK_FAT); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_SCRAMBLE);
    } THEN {
        EXPECT_NE(opponentLeft->status2 & STATUS2_CONFUSION, 0);
    }
}

SINGLE_BATTLE_TEST("Scramble field lookup preserves opposing move-end ability popup ownership")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); Speed(100); UniqueAbility(ABILITY_SERENE_VOICE); Moves(MOVE_HYPER_VOICE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HYPER_VOICE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SERENE_VOICE);
    }
}
