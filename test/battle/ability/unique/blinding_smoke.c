#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_SMOKESCREEN].effect == EFFECT_ACCURACY_DOWN);
}

SINGLE_BATTLE_TEST("Blinding Smoke does not trigger on the first turn out")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_BLINDING_SMOKE); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BLINDING_SMOKE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SMOKESCREEN, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Blinding Smoke uses Smokescreen after turn 1 out when using Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_BLINDING_SMOKE); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BLINDING_SMOKE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SMOKESCREEN, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Blinding Smoke does not trigger on non-Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_BLINDING_SMOKE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BLINDING_SMOKE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SMOKESCREEN, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Blinding Smoke can trigger repeatedly after turn 1")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_BLINDING_SMOKE); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BLINDING_SMOKE);
        ABILITY_POPUP(player, ABILITY_BLINDING_SMOKE);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 2);
    }
}

DOUBLE_BATTLE_TEST("Blinding Smoke targets the foe targeted by the Fire move")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_BLINDING_SMOKE); Moves(MOVE_CELEBRATE, MOVE_EMBER); }
        PLAYER(SPECIES_WOBBUFFET)  { Moves(MOVE_CELEBRATE); }

        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_EMBER, target: opponentRight); }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}
