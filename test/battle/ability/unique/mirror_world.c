#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mirror World reverses weaknesses and resistances")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_MIRROR_WORLD);
            Moves(MOVE_EMBER);
        }
        OPPONENT(SPECIES_BULBASAUR);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MIRROR_WORLD);
        HP_BAR(opponent);
        MESSAGE("It's not very effective…");
    }
}

SINGLE_BATTLE_TEST("Mirror World turns type immunities into weaknesses")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_MIRROR_WORLD);
            Moves(MOVE_TACKLE);
        }
        OPPONENT(SPECIES_SHUPPET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MIRROR_WORLD);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Mirror World lets Ground moves hit Flying Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_MIRROR_WORLD);
            Moves(MOVE_MUD_SHOT);
        }
        OPPONENT(SPECIES_PIDGEY);
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_SHOT); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MIRROR_WORLD);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Mirror World ends as soon as its holder leaves the field")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_MIRROR_WORLD);
            Moves(MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_BULBASAUR) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EMBER); }
    } WHEN {
        TURN {
            SWITCH(player, 1);
            MOVE(opponent, MOVE_EMBER);
        }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MIRROR_WORLD);
        HP_BAR(player);
        MESSAGE("It's super effective!");
    }
}
