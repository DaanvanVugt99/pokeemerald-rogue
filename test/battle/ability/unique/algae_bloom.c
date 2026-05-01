#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ACID].type == TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_SMOG].type == TYPE_POISON);
}

SINGLE_BATTLE_TEST("Algae Bloom uses Smog after Poison-type moves")
{
    GIVEN {
        PLAYER(SPECIES_DRAGALGE) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_ALGAE_BLOOM); Moves(MOVE_ACID); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ACID); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_ALGAE_BLOOM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SMOG, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Algae Bloom does not trigger after non-Poison moves")
{
    GIVEN {
        PLAYER(SPECIES_DRAGALGE) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_ALGAE_BLOOM); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ALGAE_BLOOM);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SMOG, player);
        }
    }
}
