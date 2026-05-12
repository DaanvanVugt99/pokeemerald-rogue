#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Ultra Conductor sets Electric Terrain after a successful Electric-type move if it is the only Ultra Beast")
{
    GIVEN {
        PLAYER(SPECIES_XURKITREE) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_THUNDER_SHOCK); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
        ABILITY_POPUP(player, ABILITY_ULTRA_CONDUCTOR);
        MESSAGE("An electric current runs across the battlefield!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Ultra Conductor does not set Electric Terrain after a non-Electric move")
{
    GIVEN {
        PLAYER(SPECIES_XURKITREE) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_CONDUCTOR);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN));
    }
}

SINGLE_BATTLE_TEST("Ultra Conductor does not set Electric Terrain if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_XURKITREE) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_THUNDER_SHOCK); }
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_CONDUCTOR);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN));
    }
}
