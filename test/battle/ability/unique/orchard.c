#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Orchard sets Grassy Terrain on switch-in with another Grass ally and another Dragon ally")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_APPLETUN) { Ability(ABILITY_RIPEN); UniqueAbility(ABILITY_ORCHARD); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BULBASAUR) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DRATINI) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ORCHARD);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Orchard does not set Grassy Terrain on switch-in without both ally types")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_APPLETUN) { Ability(ABILITY_RIPEN); UniqueAbility(ABILITY_ORCHARD); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BULBASAUR) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ORCHARD);
        }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN));
    }
}
