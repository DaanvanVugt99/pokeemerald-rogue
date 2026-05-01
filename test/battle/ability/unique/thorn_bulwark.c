#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact == TRUE);
    ASSUME(gBattleMoves[MOVE_SWIFT].makesContact == FALSE);
}

SINGLE_BATTLE_TEST("Thorn Bulwark lowers Attack when hit by a contact move during terrain")
{
    GIVEN {
        PLAYER(SPECIES_CHESNAUGHT) { Speed(50); Ability(ABILITY_OVERGROW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GRASSY_TERRAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_THORN_BULWARK);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Thorn Bulwark does not trigger without terrain")
{
    GIVEN {
        PLAYER(SPECIES_CHESNAUGHT) { Speed(50); Ability(ABILITY_OVERGROW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_THORN_BULWARK);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Thorn Bulwark does not trigger on non-contact moves during terrain")
{
    GIVEN {
        PLAYER(SPECIES_CHESNAUGHT) { Speed(50); Ability(ABILITY_OVERGROW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GRASSY_TERRAIN, MOVE_SWIFT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_SWIFT); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_THORN_BULWARK);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Thorn Bulwark respects Attack stat drop immunity")
{
    GIVEN {
        PLAYER(SPECIES_CHESNAUGHT) { Speed(50); Ability(ABILITY_OVERGROW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_CLEAR_BODY); Moves(MOVE_GRASSY_TERRAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_THORN_BULWARK);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
