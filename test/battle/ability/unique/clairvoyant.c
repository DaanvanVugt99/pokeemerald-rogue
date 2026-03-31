#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CELEBRATE].power == 0);
    ASSUME(gBattleMoves[MOVE_SPLASH].power == 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_POUND].power == gBattleMoves[MOVE_TACKLE].power);
    ASSUME(gBattleMoves[MOVE_FLAMETHROWER].power > gBattleMoves[MOVE_TACKLE].power);
}

SINGLE_BATTLE_TEST("Clairvoyant disables the opponent's lowest Base Power move on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ABRA) { Ability(ABILITY_SYNCHRONIZE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].disabledMove, MOVE_TACKLE);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].disableTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Clairvoyant coexists with Frisk on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ORAN_BERRY); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KADABRA) { Ability(ABILITY_FRISK); UniqueAbility(ABILITY_CLAIRVOYANT); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].disabledMove, MOVE_TACKLE);
    }
}

SINGLE_BATTLE_TEST("Clairvoyant fails if the target has no offensive moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ABRA) { Ability(ABILITY_SYNCHRONIZE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].disabledMove, MOVE_NONE);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].disableTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Clairvoyant randomly disables one of tied lowest Base Power offensive moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_POUND, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_POUND, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_ABRA) { Ability(ABILITY_SYNCHRONIZE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_TACKLE); }
    } THEN {
        u16 disabledMove = gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].disabledMove;
        EXPECT(disabledMove == MOVE_TACKLE || disabledMove == MOVE_POUND);
    }
}
