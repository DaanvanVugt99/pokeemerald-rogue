#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPICY_EXTRACT].effect == EFFECT_SPICY_EXTRACT);
}

SINGLE_BATTLE_TEST("Spicy Extract raises the target's Attack and lowers its Defense by two stages")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPICY_EXTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPICY_EXTRACT, player);
        MESSAGE("Foe Wobbuffet's Attack sharply rose!");
        MESSAGE("Foe Wobbuffet's Defense harshly fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Spicy Extract has both stat changes reversed by Contrary")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SNIVY) { Ability(ABILITY_CONTRARY); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPICY_EXTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPICY_EXTRACT, player);
        MESSAGE("Foe Snivy's Attack harshly fell!");
        MESSAGE("Foe Snivy's Defense sharply rose!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Spicy Extract has only its Defense drop prevented by Big Pecks")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_PIDGEY) { Ability(ABILITY_BIG_PECKS); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPICY_EXTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPICY_EXTRACT, player);
        MESSAGE("Foe Pidgey's Attack sharply rose!");
        ABILITY_POPUP(opponent, ABILITY_BIG_PECKS);
        MESSAGE("Foe Pidgey's Big Pecks prevents Defense loss!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Spicy Extract fails against a substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_SPICY_EXTRACT, 15}); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(200); MaxHP(200); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_SPICY_EXTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        MESSAGE("Wobbuffet used Spicy Extract!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SPICY_EXTRACT, player);
    } THEN {
        EXPECT_EQ(player->pp[0], 14);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Spicy Extract consumes PP when both affected stats are capped")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_SPICY_EXTRACT, 15}, {MOVE_CELEBRATE, 40}); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SHELL_SMASH, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SHELL_SMASH); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SHELL_SMASH); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SHELL_SMASH); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SHELL_SMASH); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SHELL_SMASH); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SHELL_SMASH); }
        TURN { MOVE(player, MOVE_SPICY_EXTRACT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Spicy Extract!");
        MESSAGE("But it failed!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SPICY_EXTRACT, player);
    } THEN {
        EXPECT_EQ(player->pp[0], 14);
    }
}

AI_DOUBLE_BATTLE_TEST("Spicy Extract AI favors boosting a physical partner protected by Clear Amulet")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_CLEAR_AMULET); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_SPICY_EXTRACT); }
    } WHEN {
        TURN {
            SCORE_GT(opponentRight, MOVE_SPICY_EXTRACT, MOVE_SCRATCH, target: opponentLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Spicy Extract AI avoids using it on a partner with only special attacks")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_CLEAR_AMULET); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_SPICY_EXTRACT); }
    } WHEN {
        TURN {
            SCORE_LT_VAL(opponentRight, MOVE_SPICY_EXTRACT, AI_SCORE_DEFAULT, target: opponentLeft);
        }
    }
}
