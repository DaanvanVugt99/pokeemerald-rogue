#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRAVITY].effect == EFFECT_GRAVITY);
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
}

SINGLE_BATTLE_TEST("Proto Orbit uses Gravity and Trick Room on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_COSMOG) { Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_PROTO_ORBIT); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
    }
}
