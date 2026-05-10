#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(gBattleMoves[MOVE_SEED_BOMB].ballisticMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].ballisticMove);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
}

SINGLE_BATTLE_TEST("Hot Shells uses Ember after sound-based moves")
{
    GIVEN {
        PLAYER(SPECIES_TOUCANNON) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_HOT_SHELLS); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_HOT_SHELLS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Hot Shells uses Ember after ball or bomb moves")
{
    GIVEN {
        PLAYER(SPECIES_TOUCANNON) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_HOT_SHELLS); Moves(MOVE_SEED_BOMB); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEED_BOMB); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEED_BOMB, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_HOT_SHELLS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Hot Shells does not trigger after non-sound non-ball moves")
{
    GIVEN {
        PLAYER(SPECIES_TOUCANNON) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_HOT_SHELLS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HOT_SHELLS);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        }
    }
}
