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

SINGLE_BATTLE_TEST("Psionic Paradox uses Trick Room on switch-in if any terrain is already active")
{
    u32 terrainMove;
    u32 terrainStatus;

    PARAMETRIZE { terrainMove = MOVE_PLAIN_TERRAIN; terrainStatus = STATUS_FIELD_PLAIN_TERRAIN; }
    PARAMETRIZE { terrainMove = MOVE_ELECTRIC_TERRAIN; terrainStatus = STATUS_FIELD_ELECTRIC_TERRAIN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_PLAIN_TERRAIN, MOVE_ELECTRIC_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_STANTLER) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_PSIONIC_PARADOX); }
    } WHEN {
        TURN { MOVE(player, terrainMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_PSIONIC_PARADOX);
        MESSAGE("Foe Stantler used Trick Room!");
    } THEN {
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY, terrainStatus);
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
    }
}
