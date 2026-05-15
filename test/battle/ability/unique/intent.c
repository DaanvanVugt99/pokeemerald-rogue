#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_GUNK_SHOT].accuracy < 100);
}

SINGLE_BATTLE_TEST("Intent makes the next move hit after a resisted hit")
{
    GIVEN {
        PLAYER(SPECIES_URSHIFU_SINGLE_STRIKE_STYLE) { Ability(ABILITY_INNER_FOCUS); Moves(MOVE_EMBER, MOVE_GUNK_SHOT); }
        OPPONENT(SPECIES_CHARMANDER) { MaxHP(200); HP(200); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GUNK_SHOT, hit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_INTENT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUNK_SHOT, player);
        HP_BAR(opponent);
    }
}
