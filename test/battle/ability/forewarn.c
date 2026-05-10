#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_FLAMETHROWER].power > gBattleMoves[MOVE_TACKLE].power);
    ASSUME(gBattleMoves[MOVE_FISSURE].effect == EFFECT_OHKO);
}

SINGLE_BATTLE_TEST("Forewarn reveals and disables the foe's strongest move")
{
    GIVEN {
        PLAYER(SPECIES_DROWZEE) { Speed(100); Ability(ABILITY_FOREWARN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_FLAMETHROWER, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOREWARN);
        MESSAGE("Forewarn alerted Drowzee to the Foe Wobbuffet's Flamethrower!");
        MESSAGE("Foe Wobbuffet's Flamethrower was disabled!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_FLAMETHROWER);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Forewarn treats OHKO moves as strongest moves")
{
    GIVEN {
        PLAYER(SPECIES_DROWZEE) { Speed(100); Ability(ABILITY_FOREWARN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_FISSURE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOREWARN);
        MESSAGE("Forewarn alerted Drowzee to the Foe Wobbuffet's Fissure!");
        MESSAGE("Foe Wobbuffet's Fissure was disabled!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_FISSURE);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Forewarn skips zero PP moves")
{
    GIVEN {
        PLAYER(SPECIES_DROWZEE) { Speed(100); Ability(ABILITY_FOREWARN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); MovesWithPP({MOVE_FLAMETHROWER, 0}, {MOVE_TACKLE, 35}, {MOVE_CELEBRATE, 35}); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOREWARN);
        MESSAGE("Forewarn alerted Drowzee to the Foe Wobbuffet's Tackle!");
        MESSAGE("Foe Wobbuffet's Tackle was disabled!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_TACKLE);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Forewarn does not bypass Aroma Veil")
{
    GIVEN {
        PLAYER(SPECIES_DROWZEE) { Speed(100); Ability(ABILITY_FOREWARN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_AROMATISSE) { Speed(50); Ability(ABILITY_AROMA_VEIL); Moves(MOVE_FLAMETHROWER, MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FOREWARN);
            MESSAGE("Forewarn alerted Drowzee to the Foe Aromatisse's Flamethrower!");
            MESSAGE("Foe Aromatisse's Flamethrower was disabled!");
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_NONE);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Forewarn does not overwrite an existing disabled move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DISABLE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_DROWZEE) { Speed(100); Ability(ABILITY_FOREWARN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE, MOVE_FLAMETHROWER, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_DISABLE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet's Tackle was disabled!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FOREWARN);
            MESSAGE("Forewarn alerted Drowzee to the Foe Wobbuffet's Flamethrower!");
            MESSAGE("Foe Wobbuffet's Flamethrower was disabled!");
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_TACKLE);
    }
}
