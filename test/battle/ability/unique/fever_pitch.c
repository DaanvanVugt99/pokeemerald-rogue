#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_BULK_UP));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Fever Pitch switches the user out after its next successful damaging move")
{
    GIVEN {
        PLAYER(SPECIES_VIGOROTH) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_FEVER_PITCH); Moves(MOVE_BULK_UP, MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULK_UP); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULK_UP, player);
        ABILITY_POPUP(player, ABILITY_FEVER_PITCH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_FEVER_PITCH);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Fever Pitch remains primed when a damaging move is protected against")
{
    GIVEN {
        PLAYER(SPECIES_VIGOROTH) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_FEVER_PITCH); Moves(MOVE_BULK_UP, MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULK_UP); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_PROTECT); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FEVER_PITCH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FEVER_PITCH);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_FEVER_PITCH);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}
