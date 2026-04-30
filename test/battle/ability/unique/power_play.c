#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SUCKER_PUNCH].effect == EFFECT_SUCKER_PUNCH);
    ASSUME(gBattleMoves[MOVE_TORMENT].effect == EFFECT_TORMENT);
}

SINGLE_BATTLE_TEST("Power Play uses Torment when Sucker Punch fails")
{
    GIVEN {
        PLAYER(SPECIES_KINGAMBIT) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_POWER_PLAY); Moves(MOVE_SUCKER_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUCKER_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_POWER_PLAY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TORMENT, player);
    }
}

SINGLE_BATTLE_TEST("Power Play does not use Torment when Sucker Punch succeeds")
{
    GIVEN {
        PLAYER(SPECIES_KINGAMBIT) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_POWER_PLAY); Moves(MOVE_SUCKER_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUCKER_PUNCH); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_POWER_PLAY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TORMENT, player);
        }
    }
}
