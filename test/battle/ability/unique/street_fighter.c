#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GROWL].effect == EFFECT_ATTACK_DOWN);
    ASSUME(gBattleMoves[MOVE_CLOSE_COMBAT].effect == EFFECT_CLOSE_COMBAT);
}

SINGLE_BATTLE_TEST("Street Fighter raises Attack and Defense when a foe lowers its stats")
{
    GIVEN {
        PLAYER(SPECIES_SCRAFTY) { Ability(ABILITY_CHEAP_TRICK); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CHEAP_TRICK);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Street Fighter does not trigger from self-inflicted stat drops")
{
    GIVEN {
        PLAYER(SPECIES_SCRAFTY) { Ability(ABILITY_CHEAP_TRICK); Moves(MOVE_CLOSE_COMBAT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CLOSE_COMBAT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CHEAP_TRICK);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

DOUBLE_BATTLE_TEST("Street Fighter does not trigger from allied stat drops")
{
    GIVEN {
        PLAYER(SPECIES_SCRAFTY) { Ability(ABILITY_CHEAP_TRICK); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_GROWL, target: playerLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_CHEAP_TRICK);
        }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
