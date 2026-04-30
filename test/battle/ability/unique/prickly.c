#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
}

SINGLE_BATTLE_TEST("Prickly uses Twineedle after being hit by a contact move")
{
    GIVEN {
        PLAYER(SPECIES_MARACTUS) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_PRICKLY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PRICKLY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TWINEEDLE, player);
    }
}

SINGLE_BATTLE_TEST("Prickly does not trigger after a non-contact move")
{
    GIVEN {
        PLAYER(SPECIES_MARACTUS) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_PRICKLY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_PRICKLY);
    }
}
