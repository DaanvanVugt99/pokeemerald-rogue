#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_BULK_UP));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_HAIL].effect == EFFECT_HAIL);
    ASSUME(gBattleMoves[MOVE_SNOWSCAPE].effect == EFFECT_SNOWSCAPE);
    ASSUME(gBattleMoves[MOVE_LEER].target == MOVE_TARGET_BOTH);
}

SINGLE_BATTLE_TEST("Frostbite Ritual uses Hail after status moves")
{
    GIVEN {
        PLAYER(SPECIES_CRABOMINABLE) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_FROSTBITE_RITUAL); Moves(MOVE_BULK_UP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULK_UP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULK_UP, player);
        ABILITY_POPUP(player, ABILITY_FROSTBITE_RITUAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAIL, player);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_HAIL);
    }
}

SINGLE_BATTLE_TEST("Frostbite Ritual does not trigger if Hail is already active")
{
    GIVEN {
        PLAYER(SPECIES_CRABOMINABLE) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_FROSTBITE_RITUAL); Moves(MOVE_HAIL, MOVE_BULK_UP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HAIL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BULK_UP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAIL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULK_UP, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FROSTBITE_RITUAL);
        }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_HAIL);
    }
}

SINGLE_BATTLE_TEST("Frostbite Ritual replaces Snow with Hail")
{
    GIVEN {
        PLAYER(SPECIES_CRABOMINABLE) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_FROSTBITE_RITUAL); Moves(MOVE_CELEBRATE, MOVE_BULK_UP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SNOWSCAPE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SNOWSCAPE); }
        TURN { MOVE(player, MOVE_BULK_UP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNOWSCAPE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULK_UP, player);
        ABILITY_POPUP(player, ABILITY_FROSTBITE_RITUAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAIL, player);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_HAIL);
        EXPECT(!(gBattleWeather & B_WEATHER_SNOW));
    }
}

SINGLE_BATTLE_TEST("Frostbite Ritual does not trigger after damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_CRABOMINABLE) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_FROSTBITE_RITUAL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FROSTBITE_RITUAL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HAIL, player);
        }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_HAIL));
    }
}

DOUBLE_BATTLE_TEST("Frostbite Ritual triggers after Leer affects both foes")
{
    GIVEN {
        PLAYER(SPECIES_CRABOMINABLE) { Speed(100); Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_FROSTBITE_RITUAL); Moves(MOVE_LEER); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_LEER);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEER, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_FROSTBITE_RITUAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAIL, playerLeft);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT(gBattleWeather & B_WEATHER_HAIL);
    }
}
