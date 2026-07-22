#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_RECOVER));
    ASSUME(gBattleMoves[MOVE_ELECTRIC_TERRAIN].effect == EFFECT_ELECTRIC_TERRAIN);
}

SINGLE_BATTLE_TEST("Calyrex Reigns establish their component fields for three turns")
{
    u32 ability;
    bool32 expectGrassy;
    u32 expectedWeather;

    PARAMETRIZE { ability = ABILITY_BOUNTIFUL_REIGN; expectGrassy = TRUE;  expectedWeather = 0; }
    PARAMETRIZE { ability = ABILITY_FROZEN_REIGN;    expectGrassy = FALSE; expectedWeather = B_WEATHER_SNOW; }
    PARAMETRIZE { ability = ABILITY_DREAD_REIGN;     expectGrassy = FALSE; expectedWeather = B_WEATHER_ECLIPSE; }
    PARAMETRIZE { ability = ABILITY_TUNDRA_REIGN;    expectGrassy = TRUE;  expectedWeather = B_WEATHER_SNOW; }
    PARAMETRIZE { ability = ABILITY_ASTRAL_REIGN;    expectGrassy = TRUE;  expectedWeather = B_WEATHER_ECLIPSE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_TELEPATHY); UniqueAbility(ability); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ability);
    } THEN {
        if (expectGrassy)
        {
            EXPECT(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN);
            EXPECT_EQ(gFieldTimers.terrainTimer, REIGN_FIELD_DURATION_TURNS - 1);
        }
        else
        {
            EXPECT(!(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN));
        }

        if (expectedWeather != 0)
        {
            EXPECT(gBattleWeather & expectedWeather);
            EXPECT_EQ(gWishFutureKnock.weatherDuration, REIGN_FIELD_DURATION_TURNS - 1);
        }
        else
        {
            EXPECT_EQ(gBattleWeather, 0);
        }
    }
}

SINGLE_BATTLE_TEST("Calyrex Reigns are not consumed by a failed status move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_TELEPATHY); UniqueAbility(ABILITY_BOUNTIFUL_REIGN); Moves(MOVE_RECOVER, MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet's HP is full!");
        ABILITY_POPUP(player, ABILITY_BOUNTIFUL_REIGN);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Calyrex Reigns can only establish their fields once per battle")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_TELEPATHY); UniqueAbility(ABILITY_BOUNTIFUL_REIGN); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_TELEPATHY); Moves(MOVE_ELECTRIC_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BOUNTIFUL_REIGN);
        NOT ABILITY_POPUP(player, ABILITY_BOUNTIFUL_REIGN);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN));
    }
}
