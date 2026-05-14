#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_SHURIKEN].effect == EFFECT_MULTI_HIT);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_SMOKESCREEN].effect == EFFECT_ACCURACY_DOWN);
    ASSUME(gBattleMoves[MOVE_DOUBLE_TEAM].effect == EFFECT_EVASION_UP);
    ASSUME(gBattleMoves[MOVE_MUD_SLAP].effect == EFFECT_ACCURACY_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_AQUA_JET].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_SHADOW_SNEAK].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_TOXIC_SPIKES].effect == EFFECT_TOXIC_SPIKES);
    ASSUME(gBattleMoves[MOVE_TAUNT].effect == EFFECT_TAUNT);
    ASSUME(gBattleMoves[MOVE_DETECT].effect == EFFECT_PROTECT);
    ASSUME(gBattleMoves[MOVE_FEINT].effect == EFFECT_FEINT);
    ASSUME(gBattleMoves[MOVE_MAT_BLOCK].effect == EFFECT_MAT_BLOCK);
}

SINGLE_BATTLE_TEST("Ninja Tools uses a random ninja tool after Water Shuriken")
{
    GIVEN {
        PLAYER(SPECIES_GRENINJA) { Speed(100); Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_NINJA_TOOLS); Moves(MOVE_WATER_SHURIKEN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_SHURIKEN, WITH_RNG(RNG_ROGUE_NINJA_TOOLS, MOVE_QUICK_ATTACK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_SHURIKEN, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_NINJA_TOOLS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Ninja Tools can choose status ninja tools")
{
    GIVEN {
        PLAYER(SPECIES_GRENINJA) { Speed(100); Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_NINJA_TOOLS); Moves(MOVE_WATER_SHURIKEN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_SHURIKEN, WITH_RNG(RNG_ROGUE_NINJA_TOOLS, MOVE_TAUNT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_SHURIKEN, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_NINJA_TOOLS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer != 0);
    }
}

SINGLE_BATTLE_TEST("Ninja Tools does not trigger after other Water moves")
{
    GIVEN {
        PLAYER(SPECIES_GRENINJA) { Speed(100); Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_NINJA_TOOLS); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_ROGUE_NINJA_TOOLS, MOVE_QUICK_ATTACK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_NINJA_TOOLS);
    }
}

SINGLE_BATTLE_TEST("Ninja Tools chooses from every ninja tool")
{
    static const u16 expectedMoves[] =
    {
        MOVE_SMOKESCREEN,
        MOVE_DOUBLE_TEAM,
        MOVE_MUD_SLAP,
        MOVE_QUICK_ATTACK,
        MOVE_AQUA_JET,
        MOVE_SHADOW_SNEAK,
        MOVE_SPIKES,
        MOVE_TOXIC_SPIKES,
        MOVE_TAUNT,
        MOVE_DETECT,
        MOVE_FEINT,
        MOVE_MAT_BLOCK,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_NINJA_TOOLS);

    GIVEN {
        PLAYER(SPECIES_GRENINJA) { Speed(100); Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_NINJA_TOOLS); Moves(MOVE_WATER_SHURIKEN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_SHURIKEN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NINJA_TOOLS);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
