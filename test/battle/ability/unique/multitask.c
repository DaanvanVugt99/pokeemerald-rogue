#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_CONFUSION].power > 30);
}

SINGLE_BATTLE_TEST("Multitask uses 30 BP Confusion after status moves")
{
    s16 damage;

    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_MULTITASK); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_MULTITASK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSION, player);
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_GT(damage, 0);
    }
}

SINGLE_BATTLE_TEST("Multitask does not trigger after non-status moves")
{
    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_MULTITASK); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSION, player);
            ABILITY_POPUP(player, ABILITY_MULTITASK);
        }
    }
}
