#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_CONFUSION].type == TYPE_PSYCHIC);
}

SINGLE_BATTLE_TEST("Brain Surf sets Psychic Terrain and raises Sp. Atk after an Electric move")
{
    GIVEN {
        PLAYER(SPECIES_RAICHU_ALOLAN) { Speed(100); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BRAIN_SURF);
        MESSAGE("The battlefield got weird!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Brain Surf sets Electric Terrain and raises Sp. Atk after a Psychic move")
{
    GIVEN {
        PLAYER(SPECIES_RAICHU_ALOLAN) { Speed(100); Moves(MOVE_CONFUSION); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSION); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BRAIN_SURF);
        MESSAGE("An electric current runs across the battlefield!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Brain Surf triggers only once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_RAICHU_ALOLAN) { Speed(100); Moves(MOVE_THUNDERBOLT, MOVE_CONFUSION); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CONFUSION); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BRAIN_SURF);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BRAIN_SURF);
        }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Brain Surf does not trigger when the opposite terrain is already active")
{
    GIVEN {
        PLAYER(SPECIES_RAICHU_ALOLAN) { Speed(100); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_PSYCHIC_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PSYCHIC_TERRAIN); MOVE(player, MOVE_THUNDERBOLT); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BRAIN_SURF);
        }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}
