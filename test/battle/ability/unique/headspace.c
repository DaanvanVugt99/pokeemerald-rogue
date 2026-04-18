#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
}

SINGLE_BATTLE_TEST("Headspace sets Psychic Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GIRAFARIG) { Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_HEADSPACE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_HEADSPACE);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Headspace uses Trick Room on switch-in if Psychic Terrain is already active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_PSYCHIC_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GIRAFARIG) { Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_HEADSPACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Psychic Terrain!");
        ABILITY_POPUP(opponent, ABILITY_HEADSPACE);
        MESSAGE("Foe Girafarig used Trick Room!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN);
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
    }
}
