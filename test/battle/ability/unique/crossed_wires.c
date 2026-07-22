#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].makesContact == FALSE);
    ASSUME(gBattleMoves[MOVE_TACKLE].effect != EFFECT_CONFUSE_HIT);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].effect != EFFECT_CONFUSE_HIT);
}

SINGLE_BATTLE_TEST("Crossed Wires confuses attackers on contact but not on non-contact")
{
    u16 move;
    bool8 shouldConfuse;

    PARAMETRIZE { move = MOVE_TACKLE; shouldConfuse = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; shouldConfuse = FALSE; }

    GIVEN {
        PLAYER(SPECIES_STUNFISK) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_CROSSED_WIRES); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(!!(opponent->status2 & STATUS2_CONFUSION), shouldConfuse);
    }
}
