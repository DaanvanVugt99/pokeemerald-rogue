#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SUNNY_DAY].effect == EFFECT_SUNNY_DAY);
    ASSUME(gBattleMoves[MOVE_ELECTRIC_TERRAIN].effect == EFFECT_ELECTRIC_TERRAIN);
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
}

SINGLE_BATTLE_TEST("Emerald Accord restores weather, terrain, side conditions, and stat changes to neutral")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SUNNY_DAY, MOVE_REFLECT, MOVE_TAILWIND, MOVE_CELEBRATE); }
        PLAYER(SPECIES_RAYQUAZA) { Speed(75); Ability(ABILITY_AIR_LOCK); UniqueAbility(ABILITY_EMERALD_ACCORD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DRAGON_DANCE, MOVE_STEALTH_ROCK, MOVE_ELECTRIC_TERRAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_DRAGON_DANCE); }
        TURN { MOVE(player, MOVE_REFLECT); MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_TAILWIND); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_EMERALD_ACCORD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAZE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEFOG, player);
        MESSAGE("Rayquaza restored the battlefield to neutral!");
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ(gFieldStatuses, 0);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & (SIDE_STATUS_SCREEN_ANY | SIDE_STATUS_HAZARDS_ANY | SIDE_STATUS_TAILWIND)));
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & (SIDE_STATUS_SCREEN_ANY | SIDE_STATUS_HAZARDS_ANY | SIDE_STATUS_TAILWIND)));
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[1]);
    }
}

SINGLE_BATTLE_TEST("Emerald Accord ends room effects")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TRICK_ROOM); }
        PLAYER(SPECIES_RAYQUAZA) { Ability(ABILITY_AIR_LOCK); UniqueAbility(ABILITY_EMERALD_ACCORD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK_ROOM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gFieldStatuses & (STATUS_FIELD_TRICK_ROOM | STATUS_FIELD_WONDER_ROOM | STATUS_FIELD_MAGIC_ROOM)));
        EXPECT_EQ(gFieldTimers.trickRoomTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Emerald Accord waits for a non-neutral switch-in before being consumed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_RAYQUAZA) { Ability(ABILITY_AIR_LOCK); UniqueAbility(ABILITY_EMERALD_ACCORD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_SUNNY_DAY); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_EMERALD_ACCORD);
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[1]);
    }
}

SINGLE_BATTLE_TEST("Emerald Accord activates only once per battle")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
        PLAYER(SPECIES_RAYQUAZA) { Ability(ABILITY_AIR_LOCK); UniqueAbility(ABILITY_EMERALD_ACCORD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_SUNNY_DAY); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_EMERALD_ACCORD);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_EMERALD_ACCORD);
        }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[1]);
    }
}

SINGLE_BATTLE_TEST("Emerald Accord lets Mega Rayquaza start Delta Stream after the reset")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY); }
        PLAYER(SPECIES_RAYQUAZA_MEGA) { Ability(ABILITY_DELTA_STREAM); UniqueAbility(ABILITY_EMERALD_ACCORD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_EMERALD_ACCORD);
        ABILITY_POPUP(player, ABILITY_DELTA_STREAM);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_STRONG_WINDS);
    }
}
