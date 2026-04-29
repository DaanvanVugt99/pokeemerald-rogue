#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
}

SINGLE_BATTLE_TEST("Temporal Lock uses Trick Room on switch-in if all Pokemon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DRATINI) { Ability(ABILITY_SHED_SKIN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DIALGA) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_TEMPORAL_LOCK); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_TEMPORAL_LOCK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, opponent);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
        EXPECT_EQ(gFieldTimers.trickRoomTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Temporal Lock does not use Trick Room on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DIALGA) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_TEMPORAL_LOCK); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_TEMPORAL_LOCK);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM));
    }
}
