#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RAIN_DANCE].effect == EFFECT_RAIN_DANCE);
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].effect == EFFECT_WEATHER_BALL);
    ASSUME(gBattleMoves[MOVE_TERRAIN_PULSE].effect == EFFECT_TERRAIN_PULSE);
    ASSUME(gBattleMoves[MOVE_SECRET_POWER].effect == EFFECT_SECRET_POWER);
    ASSUME(gBattleMoves[MOVE_NATURE_POWER].effect == EFFECT_NATURE_POWER);
    ASSUME(gBattleMoves[MOVE_HIDDEN_POWER].effect == EFFECT_HIDDEN_POWER);
}

SINGLE_BATTLE_TEST("Adaptive Force uses a random adaptive rune on switch-in during weather")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_RAIN_DANCE); }
        PLAYER(SPECIES_SIGILYPH) { Speed(100); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_ADAPTIVE_FORCE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_ADAPTIVE_FORCE, MOVE_WEATHER_BALL)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_FORCE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
    } THEN {
        EXPECT(opponent->hp < opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Adaptive Force uses a random adaptive rune on switch-in during terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_GRASSY_TERRAIN); }
        PLAYER(SPECIES_SIGILYPH) { Speed(100); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_ADAPTIVE_FORCE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_ADAPTIVE_FORCE, MOVE_TERRAIN_PULSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_FORCE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TERRAIN_PULSE, player);
    } THEN {
        EXPECT(opponent->hp < opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Adaptive Force does not trigger on switch-in without weather or terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SIGILYPH) { Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_ADAPTIVE_FORCE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_ADAPTIVE_FORCE, MOVE_WEATHER_BALL)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ADAPTIVE_FORCE);
    }
}

SINGLE_BATTLE_TEST("Adaptive Force chooses from every adaptive rune")
{
    static const u16 expectedMoves[] =
    {
        MOVE_WEATHER_BALL,
        MOVE_TERRAIN_PULSE,
        MOVE_SECRET_POWER,
        MOVE_NATURE_POWER,
        MOVE_HIDDEN_POWER,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_ADAPTIVE_FORCE);

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_RAIN_DANCE); }
        PLAYER(SPECIES_SIGILYPH) { Speed(100); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_ADAPTIVE_FORCE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_FORCE);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
