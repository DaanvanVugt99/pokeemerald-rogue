#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ICY_WIND].effect == EFFECT_SPEED_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_AQUA_JET].priority > 0);
    ASSUME(gBattleMoves[MOVE_ENDURE].effect == EFFECT_ENDURE);
    ASSUME(gBattleMoves[MOVE_HAIL].effect == EFFECT_HAIL);
    ASSUME(gBattleMoves[MOVE_AVALANCHE].effect == EFFECT_REVENGE);
}

SINGLE_BATTLE_TEST("Ice Floe uses a polar move the first time the user takes damage each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_BEARTIC) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_ICE_FLOE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_ICE_FLOE, MOVE_AQUA_JET)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_ICE_FLOE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AQUA_JET, player);
    }
}

SINGLE_BATTLE_TEST("Ice Floe can choose self-targeted polar moves")
{
    GIVEN {
        PLAYER(SPECIES_BEARTIC) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_ICE_FLOE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_ICE_FLOE, MOVE_HAIL)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_ICE_FLOE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAIL, player);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_HAIL);
    }
}

SINGLE_BATTLE_TEST("Ice Floe only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_BEARTIC) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_ICE_FLOE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_ICE_FLOE, MOVE_AQUA_JET)); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_ICE_FLOE, MOVE_HAIL)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ICE_FLOE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AQUA_JET, player);
        NOT ABILITY_POPUP(player, ABILITY_ICE_FLOE);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_HAIL));
    }
}

SINGLE_BATTLE_TEST("Ice Floe does not trigger if the user takes no damage")
{
    GIVEN {
        PLAYER(SPECIES_BEARTIC) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_ICE_FLOE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_ICE_FLOE, MOVE_AQUA_JET)); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ICE_FLOE);
    }
}

SINGLE_BATTLE_TEST("Ice Floe chooses from every polar move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_ICY_WIND,
        MOVE_AQUA_JET,
        MOVE_ENDURE,
        MOVE_HAIL,
        MOVE_AVALANCHE,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_ICE_FLOE);

    GIVEN {
        PLAYER(SPECIES_BEARTIC) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_ICE_FLOE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ICE_FLOE);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
