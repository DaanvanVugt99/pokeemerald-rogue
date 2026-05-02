#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_HELIOLISK].types[0] == TYPE_ELECTRIC);
    ASSUME(gSpeciesInfo[SPECIES_HELIOLISK].types[1] == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
}

SINGLE_BATTLE_TEST("Forecast Frill changes secondary type to match active weather")
{
    u32 move;
    u32 expectedType;

    PARAMETRIZE { move = MOVE_SUNNY_DAY;  expectedType = TYPE_FIRE; }
    PARAMETRIZE { move = MOVE_RAIN_DANCE; expectedType = TYPE_WATER; }
    PARAMETRIZE { move = MOVE_SANDSTORM;  expectedType = TYPE_GROUND; }
    PARAMETRIZE { move = MOVE_HAIL;       expectedType = TYPE_ICE; }
    PARAMETRIZE { move = MOVE_SNOWSCAPE;  expectedType = TYPE_ICE; }
    PARAMETRIZE { move = MOVE_ACID_RAIN;  expectedType = TYPE_POISON; }
    PARAMETRIZE { move = MOVE_ECLIPSE;    expectedType = TYPE_DARK; }

    GIVEN {
        PLAYER(SPECIES_HELIOLISK) { Ability(ABILITY_DRY_SKIN); UniqueAbility(ABILITY_FORECAST_FRILL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FORECAST_FRILL);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_ELECTRIC);
        EXPECT_EQ(player->type2, expectedType);
        EXPECT_EQ(player->type3, TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Forecast Frill updates secondary type when weather changes")
{
    GIVEN {
        PLAYER(SPECIES_HELIOLISK) { Ability(ABILITY_DRY_SKIN); UniqueAbility(ABILITY_FORECAST_FRILL); Moves(MOVE_SUNNY_DAY, MOVE_RAIN_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); }
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FORECAST_FRILL);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
        ABILITY_POPUP(player, ABILITY_FORECAST_FRILL);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_ELECTRIC);
        EXPECT_EQ(player->type2, TYPE_WATER);
        EXPECT_EQ(player->type3, TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Forecast Frill restores secondary type when weather expires")
{
    u32 turns;

    GIVEN {
        PLAYER(SPECIES_HELIOLISK) { Ability(ABILITY_DRY_SKIN); UniqueAbility(ABILITY_FORECAST_FRILL); Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); }
        for (turns = 0; turns < WEATHER_DURATION_TURNS; turns++)
            TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ANY));
        EXPECT_EQ(player->type1, TYPE_ELECTRIC);
        EXPECT_EQ(player->type2, TYPE_NORMAL);
        EXPECT_EQ(player->type3, TYPE_MYSTERY);
    }
}
