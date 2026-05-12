#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_POISON_GAS].effect == EFFECT_POISON);
    ASSUME(gBattleMoves[MOVE_TOXIC].effect == EFFECT_TOXIC);
}

SINGLE_BATTLE_TEST("Ultra Venin raises Speed when poisoning a target")
{
    u32 move;
    u32 status;
    PARAMETRIZE { move = MOVE_POISON_GAS; status = STATUS1_POISON; }
    PARAMETRIZE { move = MOVE_TOXIC; status = STATUS1_TOXIC_POISON; }

    GIVEN {
        PLAYER(SPECIES_POIPOLE) { Ability(ABILITY_BEAST_BOOST); Moves(move); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move, hit: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ULTRA_VENIN);
    } THEN {
        EXPECT(opponent->status1 & status);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Ultra Venin does not raise Speed if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_POIPOLE) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_POISON_GAS); }
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_GAS, hit: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_VENIN);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Ultra Venin does not raise Speed when poison fails")
{
    GIVEN {
        PLAYER(SPECIES_POIPOLE) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_POISON_GAS); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_GAS, hit: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_VENIN);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
