#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
}

SINGLE_BATTLE_TEST("Branch Swing multiplies Speed by 1.5x in Grassy Terrain even while ungrounded")
{
    GIVEN {
        PLAYER(SPECIES_PASSIMIAN) { Speed(80); Ability(ABILITY_DEFIANT); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GRASSY_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Branch Swing makes the user ungrounded in Grassy Terrain")
{
    GIVEN {
        PLAYER(SPECIES_PASSIMIAN) { Ability(ABILITY_DEFIANT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GRASSY_TERRAIN, MOVE_EARTHQUAKE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, opponent);
        MESSAGE("It doesn't affect Passimian…");
    }
}
