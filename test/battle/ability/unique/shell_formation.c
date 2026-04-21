#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_WITHDRAW].effect == EFFECT_DEFENSE_UP);
}

SINGLE_BATTLE_TEST("Shell Formation triggers on every Water-type move")
{
    GIVEN {
        PLAYER(SPECIES_BLASTOISE) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_SHELL_FORMATION); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(800); HP(800); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHELL_FORMATION);
        ABILITY_POPUP(player, ABILITY_SHELL_FORMATION);
    }
}

SINGLE_BATTLE_TEST("Shell Formation still triggers after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_BLASTOISE) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_SHELL_FORMATION); Moves(MOVE_WATER_GUN); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(800); HP(800); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHELL_FORMATION);
        ABILITY_POPUP(player, ABILITY_SHELL_FORMATION);
    }
}
