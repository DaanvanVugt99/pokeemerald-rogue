#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_SWIFT].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].split == SPLIT_STATUS);
}

SINGLE_BATTLE_TEST("Toxic Technique raises Sp. Atk after Physical moves")
{
    GIVEN {
        PLAYER(SPECIES_TOXICROAK) { Ability(ABILITY_DRY_SKIN); UniqueAbility(ABILITY_TOXIC_TECHNIQUE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOXIC_TECHNIQUE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Toxic Technique raises Attack after Special moves")
{
    GIVEN {
        PLAYER(SPECIES_TOXICROAK) { Ability(ABILITY_DRY_SKIN); UniqueAbility(ABILITY_TOXIC_TECHNIQUE); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOXIC_TECHNIQUE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Toxic Technique can trigger repeatedly")
{
    GIVEN {
        PLAYER(SPECIES_TOXICROAK) { Ability(ABILITY_DRY_SKIN); UniqueAbility(ABILITY_TOXIC_TECHNIQUE); Moves(MOVE_TACKLE, MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Toxic Technique does not trigger on Status moves")
{
    GIVEN {
        PLAYER(SPECIES_TOXICROAK) { Ability(ABILITY_DRY_SKIN); UniqueAbility(ABILITY_TOXIC_TECHNIQUE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TOXIC_TECHNIQUE);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Toxic Technique does not trigger if the target stat is maxed")
{
    GIVEN {
        PLAYER(SPECIES_TOXICROAK) { Speed(100); Ability(ABILITY_DRY_SKIN); UniqueAbility(ABILITY_TOXIC_TECHNIQUE); Moves(MOVE_NASTY_PLOT, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_NASTY_PLOT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_NASTY_PLOT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_NASTY_PLOT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
