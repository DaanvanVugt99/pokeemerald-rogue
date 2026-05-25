#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TOXIC_SPIKES].effect == EFFECT_TOXIC_SPIKES);
    ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_ACID_SPRAY].effect == EFFECT_SPECIAL_DEFENSE_DOWN_HIT_2);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Caustic Bloom consumes one Toxic Spikes layer from the target side to use Acid Spray")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Speed(100); Ability(ABILITY_TOXIC_DEBRIS); UniqueAbility(ABILITY_CAUSTIC_BLOOM); Moves(MOVE_TOXIC_SPIKES, MOVE_ROCK_THROW); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_THROW, player);
        ABILITY_POPUP(player, ABILITY_CAUSTIC_BLOOM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, player);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TOXIC_SPIKES));
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount, 0);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Caustic Bloom consumes only one Toxic Spikes layer")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Speed(100); Ability(ABILITY_TOXIC_DEBRIS); UniqueAbility(ABILITY_CAUSTIC_BLOOM); Moves(MOVE_TOXIC_SPIKES, MOVE_ROCK_THROW); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CAUSTIC_BLOOM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TOXIC_SPIKES);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount, 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Caustic Bloom does not trigger without Toxic Spikes on the target side")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Speed(100); Ability(ABILITY_TOXIC_DEBRIS); UniqueAbility(ABILITY_CAUSTIC_BLOOM); Moves(MOVE_ROCK_THROW); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CAUSTIC_BLOOM);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Caustic Bloom does not consume Toxic Spikes if the Rock move fails")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Speed(1); Ability(ABILITY_TOXIC_DEBRIS); UniqueAbility(ABILITY_CAUSTIC_BLOOM); Moves(MOVE_TOXIC_SPIKES, MOVE_ROCK_THROW); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_ROCK_THROW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CAUSTIC_BLOOM);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, player);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TOXIC_SPIKES);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount, 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Caustic Bloom does not trigger from non-Rock moves")
{
    GIVEN {
        PLAYER(SPECIES_GLIMMORA) { Speed(100); Ability(ABILITY_TOXIC_DEBRIS); UniqueAbility(ABILITY_CAUSTIC_BLOOM); Moves(MOVE_TOXIC_SPIKES, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CAUSTIC_BLOOM);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, player);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TOXIC_SPIKES);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount, 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
