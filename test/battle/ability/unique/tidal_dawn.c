#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Tidal Dawn sets sunlight on switch-in if all Pokemon on the team share a type with Walking Wake")
{
    bool32 partySharesType;

    PARAMETRIZE { partySharesType = TRUE; }
    PARAMETRIZE { partySharesType = FALSE; }

    GIVEN {
        PLAYER(SPECIES_WALKING_WAKE) { Speed(100); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DEINO) { Speed(50); Moves(MOVE_CELEBRATE); }
        if (partySharesType)
            PLAYER(SPECIES_SQUIRTLE) { Speed(50); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_PIKACHU) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (partySharesType)
            ABILITY_POPUP(player, ABILITY_TIDAL_DAWN);
        else
            NOT ABILITY_POPUP(player, ABILITY_TIDAL_DAWN);
    } THEN {
        if (partySharesType)
        {
            EXPECT(gBattleWeather & B_WEATHER_SUN);
            EXPECT(!(gBattleWeather & B_WEATHER_SUN_PERMANENT));
        }
        else
        {
            EXPECT(!(gBattleWeather & B_WEATHER_SUN));
        }
    }
}

SINGLE_BATTLE_TEST("Tidal Dawn sets sunlight when Walking Wake switches in later")
{
    GIVEN {
        PLAYER(SPECIES_DEINO) { Speed(50); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WALKING_WAKE) { Speed(100); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SQUIRTLE) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TIDAL_DAWN);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(!(gBattleWeather & B_WEATHER_SUN_PERMANENT));
    }
}
