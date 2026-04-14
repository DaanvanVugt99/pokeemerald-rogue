#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
    ASSUME(gBattleMoves[MOVE_HELPING_HAND].effect == EFFECT_HELPING_HAND);
}

DOUBLE_BATTLE_TEST("Maternal Instinct uses Helping Hand after Protect in doubles")
{
    GIVEN {
        PLAYER(SPECIES_KANGASKHAN) { Speed(100); Ability(ABILITY_EARLY_BIRD); UniqueAbility(ABILITY_MATERNAL_INSTINCT); Moves(MOVE_PROTECT); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(40); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_MATERNAL_INSTINCT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, playerLeft);
    }
}

SINGLE_BATTLE_TEST("Maternal Instinct does not trigger in singles")
{
    GIVEN {
        PLAYER(SPECIES_KANGASKHAN) { Speed(100); Ability(ABILITY_EARLY_BIRD); UniqueAbility(ABILITY_MATERNAL_INSTINCT); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MATERNAL_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, player);
        }
    }
}
