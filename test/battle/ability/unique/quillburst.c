#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
}

SINGLE_BATTLE_TEST("Quillburst uses Pin Missile after being hit by a contact move")
{
    GIVEN {
        PLAYER(SPECIES_TOGEDEMARU) { Ability(ABILITY_IRON_BARBS); UniqueAbility(ABILITY_QUILLBURST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_QUILLBURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PIN_MISSILE, player);
    }
}

SINGLE_BATTLE_TEST("Quillburst does not trigger after a non-contact move")
{
    GIVEN {
        PLAYER(SPECIES_TOGEDEMARU) { Ability(ABILITY_IRON_BARBS); UniqueAbility(ABILITY_QUILLBURST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_QUILLBURST);
    }
}
