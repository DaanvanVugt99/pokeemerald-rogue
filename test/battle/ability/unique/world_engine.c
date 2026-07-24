#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("World Engine can begin at every phase and advances after the turn")
{
    u32 phase;
    u32 expectedWeather;
    u32 expectedTerrain;

    PARAMETRIZE { phase = 0; expectedWeather = B_WEATHER_RAIN;      expectedTerrain = STATUS_FIELD_ELECTRIC_TERRAIN; }
    PARAMETRIZE { phase = 1; expectedWeather = B_WEATHER_SNOW;      expectedTerrain = STATUS_FIELD_MISTY_TERRAIN; }
    PARAMETRIZE { phase = 2; expectedWeather = B_WEATHER_SANDSTORM; expectedTerrain = STATUS_FIELD_PSYCHIC_TERRAIN; }
    PARAMETRIZE { phase = 3; expectedWeather = B_WEATHER_ACID_RAIN; expectedTerrain = STATUS_FIELD_INFESTED_TERRAIN; }
    PARAMETRIZE { phase = 4; expectedWeather = B_WEATHER_ECLIPSE;   expectedTerrain = STATUS_FIELD_PLAIN_TERRAIN; }
    PARAMETRIZE { phase = 5; expectedWeather = B_WEATHER_SUN;       expectedTerrain = STATUS_FIELD_GRASSY_TERRAIN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_KECLEON) { UniqueAbility(ABILITY_WORLD_ENGINE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_WORLD_ENGINE, phase)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WORLD_ENGINE);
        ABILITY_POPUP(player, ABILITY_WORLD_ENGINE);
    } THEN {
        EXPECT(gBattleWeather & expectedWeather);
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY, expectedTerrain);
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_TURNS);
        EXPECT_EQ(gFieldTimers.terrainTimer, TERRAIN_DURATION_TURNS);
    }
}

SINGLE_BATTLE_TEST("World Engine pauses its cycle while its holder is absent")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_KECLEON) { UniqueAbility(ABILITY_WORLD_ENGINE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DITTO) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_WORLD_ENGINE, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 2); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WORLD_ENGINE);
        ABILITY_POPUP(player, ABILITY_WORLD_ENGINE);
        NOT ABILITY_POPUP(player, ABILITY_WORLD_ENGINE);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY, STATUS_FIELD_ELECTRIC_TERRAIN);
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_TURNS - 1);
        EXPECT_EQ(gFieldTimers.terrainTimer, TERRAIN_DURATION_TURNS - 1);
    }
}

SINGLE_BATTLE_TEST("World Engine advances immediately when its holder returns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_KECLEON) { UniqueAbility(ABILITY_WORLD_ENGINE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DITTO) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_WORLD_ENGINE, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 2); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        // Sun -> Rain at turn end, pause, Snow on return, then Sandstorm.
        EXPECT(gBattleWeather & B_WEATHER_SANDSTORM);
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY, STATUS_FIELD_PSYCHIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("World Engine protects its holder from damaging phase weather")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_KECLEON) { HP(100); MaxHP(100); UniqueAbility(ABILITY_WORLD_ENGINE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_WORLD_ENGINE, 3)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT HP_BAR(player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_LT(opponent->hp, 100);
    }
}
