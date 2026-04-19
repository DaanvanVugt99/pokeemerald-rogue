#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Moonveil makes the first Dark-type move each switch-in fail")
{
    GIVEN {
        PLAYER(SPECIES_GARDEVOIR) { Ability(ABILITY_TRACE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MIGHTYENA) { Moves(MOVE_BITE, MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOONVEIL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Moonveil is not consumed by non-Dark moves")
{
    GIVEN {
        PLAYER(SPECIES_GARDEVOIR) { Ability(ABILITY_TRACE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MIGHTYENA) { Moves(MOVE_TACKLE, MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_MOONVEIL);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Moonveil refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_GARDEVOIR) { Ability(ABILITY_TRACE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MIGHTYENA) { Moves(MOVE_BITE, MOVE_CELEBRATE, MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOONVEIL);
        ABILITY_POPUP(player, ABILITY_MOONVEIL);
    }
}

SINGLE_BATTLE_TEST("Moonveil also blocks the first Dark-type status move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_GARDEVOIR) { Ability(ABILITY_TRACE); Moves(MOVE_SPORE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TAUNT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TAUNT); }
        TURN { MOVE(player, MOVE_SPORE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_SLEEP);
    }
}
