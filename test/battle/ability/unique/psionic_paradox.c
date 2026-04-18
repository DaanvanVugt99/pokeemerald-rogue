#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
}

SINGLE_BATTLE_TEST("Psionic Paradox sets Plain Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_STANTLER) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_PSIONIC_PARADOX); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_PSIONIC_PARADOX);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Psionic Paradox uses Trick Room on switch-in if Plain Terrain is already active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_PLAIN_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_STANTLER) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_PSIONIC_PARADOX); }
    } WHEN {
        TURN { MOVE(player, MOVE_PLAIN_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Plain Terrain!");
        ABILITY_POPUP(opponent, ABILITY_PSIONIC_PARADOX);
        MESSAGE("Foe Stantler used Trick Room!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
    }
}
