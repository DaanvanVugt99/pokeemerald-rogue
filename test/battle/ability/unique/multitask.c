#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_BARRAGE].effect == EFFECT_MULTI_HIT);
}

SINGLE_BATTLE_TEST("Multitask uses Barrage after the first status move each battle")
{
    s16 damage;

    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_MULTITASK); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_MULTITASK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BARRAGE, player);
        HP_BAR(opponent, captureDamage: &damage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MULTITASK);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BARRAGE, player);
        }
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
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BARRAGE, player);
            ABILITY_POPUP(player, ABILITY_MULTITASK);
        }
    }
}
