#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Flash Firestorm sets sun on the first time this Pokemon takes damage after switching in")
{
    GIVEN {
        PLAYER(SPECIES_BLAZIKEN) { Ability(ABILITY_BLAZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Tackle!");
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_FLASH_FIRESTORM);
        MESSAGE("Foe Wobbuffet used Tackle!");
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FLASH_FIRESTORM);
        }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
    }
}

SINGLE_BATTLE_TEST("Flash Firestorm refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_BLAZIKEN) { Ability(ABILITY_BLAZE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_RAIN_DANCE, MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLASH_FIRESTORM);
        ABILITY_POPUP(player, ABILITY_FLASH_FIRESTORM);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
    }
}

SINGLE_BATTLE_TEST("Flash Firestorm is not consumed if sun is already active")
{
    GIVEN {
        PLAYER(SPECIES_BLAZIKEN) { Ability(ABILITY_BLAZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DROUGHT); Moves(MOVE_TACKLE, MOVE_CELEBRATE, MOVE_RAIN_DANCE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Tackle!");
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        MESSAGE("Foe Wobbuffet used Rain Dance!");
        ABILITY_POPUP(player, ABILITY_FLASH_FIRESTORM);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
    }
}
