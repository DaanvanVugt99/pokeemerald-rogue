#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_YAWN));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
}

SINGLE_BATTLE_TEST("Deep Sleep heals 1/4 max HP after using Yawn")
{
    GIVEN {
        PLAYER(SPECIES_SNORLAX) { HP(50); MaxHP(100); Ability(ABILITY_IMMUNITY); UniqueAbility(ABILITY_DEEP_SLEEP); Moves(MOVE_YAWN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_YAWN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DEEP_SLEEP);
        HP_BAR(player, damage: -25);
    }
}

SINGLE_BATTLE_TEST("Deep Sleep does not trigger after non-Yawn moves")
{
    GIVEN {
        PLAYER(SPECIES_SNORLAX) { HP(50); MaxHP(100); Ability(ABILITY_IMMUNITY); UniqueAbility(ABILITY_DEEP_SLEEP); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DEEP_SLEEP);
            HP_BAR(player);
        }
    }
}
