#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Storm Command sets Electric Terrain after a KO with Flying/Electric allies in party")
{
    GIVEN {
        PLAYER(SPECIES_THUNDURUS) { Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_STORM_COMMAND); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_ZAPDOS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(400); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STORM_COMMAND);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Storm Command does not trigger without at least one other Flying ally")
{
    GIVEN {
        PLAYER(SPECIES_THUNDURUS) { Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_STORM_COMMAND); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_PIKACHU);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(400); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_STORM_COMMAND);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN));
    }
}

SINGLE_BATTLE_TEST("Storm Command does not trigger if the move does not knock out the target")
{
    GIVEN {
        PLAYER(SPECIES_THUNDURUS) { Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_STORM_COMMAND); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_ZAPDOS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_STORM_COMMAND);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN));
    }
}
