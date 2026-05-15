#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MUD_SHOT].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_RAPID_SPIN].effect == EFFECT_RAPID_SPIN);
    ASSUME(gBattleMoves[MOVE_MUD_SLAP].effect == EFFECT_ACCURACY_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_SAND_ATTACK].effect == EFFECT_ACCURACY_DOWN);
    ASSUME(gBattleMoves[MOVE_ROCK_TOMB].effect == EFFECT_SPEED_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_ROTOTILLER].effect == EFFECT_ROTOTILLER);
    ASSUME(gBattleMoves[MOVE_WORK_UP].effect == EFFECT_ATTACK_SPATK_UP);
    ASSUME(gBattleMoves[MOVE_HELPING_HAND].effect == EFFECT_HELPING_HAND);
}

SINGLE_BATTLE_TEST("Work Crew uses a random work move after the first Ground-type move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DIGGERSBY) { Speed(100); Ability(ABILITY_HUGE_POWER); UniqueAbility(ABILITY_WORK_CREW); Moves(MOVE_MUD_SHOT); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_SHOT, WITH_RNG(RNG_ROGUE_WORK_CREW, MOVE_ROCK_TOMB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MUD_SHOT, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_WORK_CREW);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_TOMB, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Work Crew only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DIGGERSBY) { Speed(100); Ability(ABILITY_HUGE_POWER); UniqueAbility(ABILITY_WORK_CREW); Moves(MOVE_MUD_SHOT); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_SHOT, WITH_RNG(RNG_ROGUE_WORK_CREW, MOVE_MUD_SLAP)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MUD_SHOT, WITH_RNG(RNG_ROGUE_WORK_CREW, MOVE_ROCK_TOMB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WORK_CREW);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WORK_CREW);
        }
    }
}

DOUBLE_BATTLE_TEST("Work Crew chooses from every work move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_RAPID_SPIN,
        MOVE_MUD_SLAP,
        MOVE_SAND_ATTACK,
        MOVE_ROCK_TOMB,
        MOVE_SPIKES,
        MOVE_ROTOTILLER,
        MOVE_WORK_UP,
        MOVE_HELPING_HAND,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_WORK_CREW);

    GIVEN {
        PLAYER(SPECIES_DIGGERSBY) { Speed(100); Ability(ABILITY_HUGE_POWER); UniqueAbility(ABILITY_WORK_CREW); Moves(MOVE_MUD_SHOT); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_MUD_SHOT, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_WORK_CREW);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
