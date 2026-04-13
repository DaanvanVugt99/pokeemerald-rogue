#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_IRON_DEFENSE].effect == EFFECT_DEFENSE_UP_2);
}

SINGLE_BATTLE_TEST("Shell Formation triggers only on the first Water-type move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_BLASTOISE) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_SHELL_FORMATION); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHELL_FORMATION);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SHELL_FORMATION);
        }
    }
}

SINGLE_BATTLE_TEST("Shell Formation refreshes after the user switches out and back in")
{
    GIVEN {
        PLAYER(SPECIES_BLASTOISE) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_SHELL_FORMATION); Moves(MOVE_WATER_GUN); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
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
