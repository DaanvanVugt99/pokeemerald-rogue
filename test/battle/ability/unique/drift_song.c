#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].effect == EFFECT_WEATHER_BALL);
    ASSUME(gBattleMoves[MOVE_GROWL].soundMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_WATER);
}

SINGLE_BATTLE_TEST("Drift Song heals 1/8 max HP at end of turn after the user uses a Water-type move")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_DRIFT_SONG); HP(300); MaxHP(400); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DRIFT_SONG);
        MESSAGE("Phione's Drift Song restored its HP a little!");
        HP_BAR(player, damage: -50);
    }
}

SINGLE_BATTLE_TEST("Drift Song heals 1/8 max HP at end of turn after the user uses a sound move")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_DRIFT_SONG); HP(300); MaxHP(400); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DRIFT_SONG);
        MESSAGE("Phione's Drift Song restored its HP a little!");
        HP_BAR(player, damage: -50);
    }
}

SINGLE_BATTLE_TEST("Drift Song heals 1/8 max HP at end of turn after the user uses a dynamically Water-type move")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Speed(100); Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_DRIFT_SONG); HP(300); MaxHP(400); Moves(MOVE_CELEBRATE, MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_RAIN_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DRIFT_SONG);
        MESSAGE("Phione's Drift Song restored its HP a little!");
        HP_BAR(player, damage: -50);
    } THEN {
        EXPECT_EQ(player->hp, 350);
    }
}

SINGLE_BATTLE_TEST("Drift Song does not heal after a non-Water non-sound move")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_DRIFT_SONG); HP(300); MaxHP(400); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DRIFT_SONG);
            MESSAGE("Phione's Drift Song restored its HP a little!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 300);
    }
}

SINGLE_BATTLE_TEST("Drift Song does not heal again on a later turn where the user does not move")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Speed(100); Ability(ABILITY_TRUANT); UniqueAbility(ABILITY_DRIFT_SONG); HP(300); MaxHP(400); Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Attack(200); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DRIFT_SONG);
        MESSAGE("Phione's Drift Song restored its HP a little!");
        HP_BAR(player, damage: -50);
        MESSAGE("Phione is loafing around!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NOT ABILITY_POPUP(player, ABILITY_DRIFT_SONG);
    } THEN {
        EXPECT_LT(player->hp, 350);
    }
}
