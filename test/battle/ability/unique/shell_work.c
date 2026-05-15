#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_IRON_DEFENSE].effect == EFFECT_DEFENSE_UP_2);
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
    ASSUME(gBattleMoves[MOVE_ROCK_TOMB].effect == EFFECT_SPEED_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_WIDE_GUARD].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Shell Work uses a random construction move when Defense rises")
{
    GIVEN {
        PLAYER(SPECIES_CRUSTLE) { Speed(100); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SHELL_WORK); Moves(MOVE_IRON_DEFENSE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE, WITH_RNG(RNG_ROGUE_SHELL_WORK, MOVE_STEALTH_ROCK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        ABILITY_POPUP(player, ABILITY_SHELL_WORK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}

SINGLE_BATTLE_TEST("Shell Work can choose an attacking construction move when Defense rises")
{
    GIVEN {
        PLAYER(SPECIES_CRUSTLE) { Speed(100); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SHELL_WORK); Moves(MOVE_IRON_DEFENSE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE, WITH_RNG(RNG_ROGUE_SHELL_WORK, MOVE_ROCK_TOMB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        ABILITY_POPUP(player, ABILITY_SHELL_WORK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_TOMB, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT_EQ(gCalledMove, MOVE_ROCK_TOMB);
    }
}

SINGLE_BATTLE_TEST("Shell Work can choose every construction move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_STEALTH_ROCK,
        MOVE_ROCK_TOMB,
        MOVE_SPIKES,
        MOVE_WIDE_GUARD,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_SHELL_WORK);

    GIVEN {
        PLAYER(SPECIES_CRUSTLE) { Speed(100); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SHELL_WORK); Moves(MOVE_IRON_DEFENSE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        ABILITY_POPUP(player, ABILITY_SHELL_WORK);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}

SINGLE_BATTLE_TEST("Shell Work does not trigger when a different stat rises")
{
    GIVEN {
        PLAYER(SPECIES_CRUSTLE) { Speed(100); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SHELL_WORK); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE, WITH_RNG(RNG_ROGUE_SHELL_WORK, MOVE_STEALTH_ROCK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
        NOT ABILITY_POPUP(player, ABILITY_SHELL_WORK);
    }
}

SINGLE_BATTLE_TEST("Shell Work does not trigger when Defense cannot rise")
{
    GIVEN {
        PLAYER(SPECIES_CRUSTLE) { Speed(100); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SHELL_WORK); Moves(MOVE_IRON_DEFENSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE, WITH_RNG(RNG_ROGUE_SHELL_WORK, MOVE_WIDE_GUARD)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE, WITH_RNG(RNG_ROGUE_SHELL_WORK, MOVE_WIDE_GUARD)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE, WITH_RNG(RNG_ROGUE_SHELL_WORK, MOVE_WIDE_GUARD)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE, WITH_RNG(RNG_ROGUE_SHELL_WORK, MOVE_STEALTH_ROCK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHELL_WORK);
        ABILITY_POPUP(player, ABILITY_SHELL_WORK);
        ABILITY_POPUP(player, ABILITY_SHELL_WORK);
        NOT ABILITY_POPUP(player, ABILITY_SHELL_WORK);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], MAX_STAT_STAGE);
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK));
    }
}
