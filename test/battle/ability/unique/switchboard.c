#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_NUZZLE].effect == EFFECT_PARALYZE_HIT);
    ASSUME(gBattleMoves[MOVE_ENCORE].effect == EFFECT_ENCORE);
}

SINGLE_BATTLE_TEST("Switchboard uses Encore after the first Nuzzle each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DEDENNE) { Speed(1); Ability(ABILITY_CHEEK_POUCH); UniqueAbility(ABILITY_SWITCHBOARD); Moves(MOVE_NUZZLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Speed(100); Ability(ABILITY_BIG_PECKS); Moves(MOVE_CELEBRATE, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_NUZZLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); FORCED_MOVE(opponent); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NUZZLE, player);
        ABILITY_POPUP(player, ABILITY_SWITCHBOARD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ENCORE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Switchboard only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DEDENNE) { Speed(1); Ability(ABILITY_CHEEK_POUCH); UniqueAbility(ABILITY_SWITCHBOARD); Moves(MOVE_NUZZLE); }
        OPPONENT(SPECIES_PIKACHU) { Speed(100); Ability(ABILITY_BIG_PECKS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_NUZZLE); }
        TURN { FORCED_MOVE(opponent); MOVE(player, MOVE_NUZZLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SWITCHBOARD);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SWITCHBOARD);
        }
    }
}
