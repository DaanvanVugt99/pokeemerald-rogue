#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHADOW_BALL].type == TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_WILL_O_WISP].effect == EFFECT_WILL_O_WISP);
}

SINGLE_BATTLE_TEST("Guiding Flames does not trigger on the first turn out")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION_HISUIAN) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_GUIDING_FLAMES); Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_WOBBUFFET)         { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BALL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_GUIDING_FLAMES);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, player);
        }
    } THEN {
        EXPECT(!(opponent->status1 & STATUS1_BURN));
    }
}

SINGLE_BATTLE_TEST("Guiding Flames uses Will-O-Wisp after turn 1 out when using Ghost moves")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION_HISUIAN) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_GUIDING_FLAMES); Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_WOBBUFFET)         { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BALL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SHADOW_BALL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GUIDING_FLAMES);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, player);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Guiding Flames does not trigger on non-Ghost moves")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION_HISUIAN) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_GUIDING_FLAMES); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET)         { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_GUIDING_FLAMES);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, player);
        }
    } THEN {
        EXPECT(!(opponent->status1 & STATUS1_BURN));
    }
}

DOUBLE_BATTLE_TEST("Guiding Flames targets the foe targeted by the Ghost move")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION_HISUIAN) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_GUIDING_FLAMES); Moves(MOVE_CELEBRATE, MOVE_SHADOW_BALL); }
        PLAYER(SPECIES_WOBBUFFET)          { Moves(MOVE_CELEBRATE); }

        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_SHADOW_BALL, target: opponentRight); }
    } THEN {
        EXPECT(!(opponentLeft->status1 & STATUS1_BURN));
        EXPECT(opponentRight->status1 & STATUS1_BURN);
    }
}
