#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
    ASSUME(gBattleMoves[MOVE_SALT_CURE].effect == EFFECT_SALT_CURE);
}

SINGLE_BATTLE_TEST("Barnacle Wall salt cures attackers after contact but not non-contact moves")
{
    u16 move;
    bool8 shouldSaltCure;
    PARAMETRIZE { move = MOVE_TACKLE; shouldSaltCure = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; shouldSaltCure = FALSE; }

    GIVEN {
        PLAYER(SPECIES_BARBARACLE) { Ability(ABILITY_TOUGH_CLAWS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(!!(gStatuses4[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] & STATUS4_SALT_CURE), shouldSaltCure);
    }
}
