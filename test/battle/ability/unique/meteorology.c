#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Meteorology sets weather from the held rock and Forecast transforms Castform")
{
    u16 item;
    u32 weather;
    u32 species;

    PARAMETRIZE { item = ITEM_HEAT_ROCK; weather = B_WEATHER_SUN; species = SPECIES_CASTFORM_SUNNY; }
    PARAMETRIZE { item = ITEM_DAMP_ROCK; weather = B_WEATHER_RAIN; species = SPECIES_CASTFORM_RAINY; }
    PARAMETRIZE {
        item = ITEM_ICY_ROCK;
        weather = (B_SNOW_WARNING >= GEN_9) ? B_WEATHER_SNOW : B_WEATHER_HAIL;
        species = SPECIES_CASTFORM_SNOWY;
    }
    PARAMETRIZE { item = ITEM_SMOOTH_ROCK; weather = B_WEATHER_SANDSTORM; species = SPECIES_CASTFORM_SANDY; }
    PARAMETRIZE { item = ITEM_ACID_ROCK; weather = B_WEATHER_ACID_RAIN; species = SPECIES_CASTFORM_ACID_RAIN; }
    PARAMETRIZE { item = ITEM_DIM_ROCK; weather = B_WEATHER_ECLIPSE; species = SPECIES_CASTFORM_ECLIPSE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CASTFORM_NORMAL) { Ability(ABILITY_FORECAST); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_METEOROLOGY);
        ABILITY_POPUP(player, ABILITY_FORECAST);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
        MESSAGE("Castform transformed!");
    } THEN {
        EXPECT(gBattleWeather & weather);
        EXPECT_EQ(player->species, species);
    }
}

SINGLE_BATTLE_TEST("Meteorology does nothing without a weather rock")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CASTFORM_NORMAL) { Ability(ABILITY_FORECAST); Item(ITEM_LEFTOVERS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ(player->species, SPECIES_CASTFORM_NORMAL);
    }
}
