#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ICE_PUNCH].type == TYPE_ICE);
    ASSUME(gBattleMoves[MOVE_IRON_HEAD].type == TYPE_STEEL);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_RAPID_SPIN].effect == EFFECT_RAPID_SPIN);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
}

SINGLE_BATTLE_TEST("Snowplow sets Snow and uses Rapid Spin after an Ice move")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH_ALOLAN) { Speed(1); Ability(ABILITY_SLUSH_RUSH); Moves(MOVE_ICE_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPIKES); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); MOVE(player, MOVE_ICE_PUNCH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SNOWPLOW);
        MESSAGE("It started to snow!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Snowplow sets Snow and uses Rapid Spin after a Steel move")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH_ALOLAN) { Speed(1); Ability(ABILITY_SLUSH_RUSH); Moves(MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPIKES); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); MOVE(player, MOVE_IRON_HEAD); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SNOWPLOW);
        MESSAGE("It started to snow!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Snowplow triggers only once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH_ALOLAN) { Speed(100); Ability(ABILITY_SLUSH_RUSH); Moves(MOVE_ICE_PUNCH, MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ICE_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_HEAD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SNOWPLOW);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SNOWPLOW);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
        }
    }
}

SINGLE_BATTLE_TEST("Snowplow does not consume on non-Ice or Steel moves")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH_ALOLAN) { Speed(100); Ability(ABILITY_SLUSH_RUSH); Moves(MOVE_TACKLE, MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_HEAD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SNOWPLOW);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAPID_SPIN, player);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
    }
}
